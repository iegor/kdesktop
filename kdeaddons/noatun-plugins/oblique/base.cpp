// Copyright (c) 2003 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#include "base.h"
#include "file.h"

#include "kdbt.h"
#include "kbuffer.h"

#include <qstringlist.h>
#include <qmap.h>
#include <qfile.h>
#include <qdom.h>

#include <cstdlib>
#include <assert.h>
#include <db_cxx.h>



struct Base::Private
{
	Private() : db(0, DB_CXX_NO_EXCEPTIONS) { }
	Db db;
	typedef KDbt<FileId> Key;
	typedef KDbt<QStringList> Data;

	FileId high;

	FileId cachedId;
	mutable QMap<QString,QString> cachedProperties;

	QPtrList<Slice> slices;
	int sliceHigh;
};


Base::Base(const QString &file)
{
	d = new Private;
	d->cachedId = 0;

	QCString filename = QFile::encodeName(file);

	bool create = true;
	if (d->db.open(
#if DB_VERSION_MINOR > 0 && DB_VERSION_MAJOR >= 4
			NULL,
#endif
			filename,
			0, DB_BTREE, DB_NOMMAP, 0
		)==0)
	{ // success
		Private::Data data;
		Private::Key key(0);
		if (d->db.get(0, &key, &data, 0)==0)
		{
			QStringList strs;
			data.get(strs);

			mFormatVersion = strs[0].toUInt(0, 16); // TODO
			d->high = strs[1].toUInt();

			if (strs.count() == 3)
				loadMetaXML(strs[2]);
			else
				loadMetaXML("");

			create=false;
		}
	}
	if (create)
	{ // failure
		QFile(filename).remove();
		d->db.open(
#if DB_VERSION_MINOR > 0 && DB_VERSION_MAJOR >= 4
				NULL,
#endif
				filename,0, DB_BTREE, DB_NOMMAP|DB_CREATE,0
			);

		d->high=0;
		QStringList strs;
		strs << "00010002" << "0" << "";
		resetFormatVersion();
		loadMetaXML("");
		Private::Data data(strs);
		Private::Key key(0);
		// in the stringlist for Key(0), we have the following list:
		// { "version of the file",
		//   "the high extreme (auto-increment counter in SQL terminology)",
		//   "the metaxml"
		// }
		d->db.put(0, &key, &data, 0);
	}
}

void Base::resetFormatVersion()
{
	mFormatVersion = 0x00010002;
}

Base::~Base()
{
	QStringList strs;
	strs << QString::number(mFormatVersion, 16) << QString::number(d->high);
	strs << saveMetaXML();

	Private::Data data(strs);
	Private::Key key(0);
	d->db.put(0, &key, &data, 0);
	d->db.sync(0);
	d->db.close(0);
	delete d;
}

File Base::add(const QString &file)
{
	Private::Key key(++d->high);
	QStringList properties;
	properties << "file" << file;
	Private::Data data(properties);

	unless (d->db.put(0, &key, &data, 0))
	{
		// success !
		File f(this, d->high);
		f.makeCache();
		emit added(f);
		return f;
	}

	return File();
}

File Base::find(FileId id)
{
	if (id == 0) return File();

	Private::Key key(id);
	Private::Data data;

	unless (d->db.get(0, &key, &data, 0))
	{
		// exists
		return File(this, id);
	}
	else
	{
		return File(); // null item
	}
}

void Base::clear()
{
	for (FileId id = high(); id >= 1; id--)
	{
		File f = find(id);
		if (f)
			f.remove();
	}
}


FileId Base::high() const
{
	return d->high;
}

File Base::first(FileId first)
{
	if (first > high()) return File();

	while (!find(first))
	{
		++first;
		if (first > high())
			return File();
	}
	return File(this, first);
}

QString Base::property(FileId id, const QString &property) const
{
	loadIntoCache(id);
	if (!d->cachedProperties.contains(property)) return QString::null;
	QMap<QString,QString>::Iterator i = d->cachedProperties.find(property);
	return i.data();
}

void Base::setProperty(FileId id, const QString &key, const QString &value)
{
	loadIntoCache(id);
	d->cachedProperties.insert(key, value);
	// reinsert it into the DB

	QStringList props;
	for (
			QMap<QString,QString>::Iterator i(d->cachedProperties.begin());
			i != d->cachedProperties.end(); ++i
		)
	{
		props << i.key() << i.data();
	}

	Private::Data data(props);
	Private::Key dbkey(id);
	d->db.put(0, &dbkey, &data, 0);
	d->db.sync(0);

	emit modified(File(this, id));
}

QStringList Base::properties(FileId id) const
{
	loadIntoCache(id);
	QStringList props;
	for (
			QMap<QString,QString>::Iterator i(d->cachedProperties.begin());
			i != d->cachedProperties.end(); ++i
		)
	{
		props << i.key();
	}
	return props;
}

void Base::clearProperty(FileId id, const QString &key)
{
	loadIntoCache(id);
	d->cachedProperties.remove(key);
	// reinsert it into the DB

	QStringList props;
	for (
			QMap<QString,QString>::Iterator i(d->cachedProperties.begin());
			i != d->cachedProperties.end(); ++i
		)
	{
		if (i.key() != key)
			props << i.key() << i.data();
	}

	Private::Data data(props);
	Private::Key dbkey(id);
	d->db.put(0, &dbkey, &data, 0);
	d->db.sync(0);

	emit modified(File(this, id));
}

void Base::remove(File file)
{
	Private::Key key(file.id());

	unless (d->db.del(0, &key, 0))
	{
		emit removed(file);
		if (file.id() == d->high)
		{
			d->high--;  // optimization
		}
	}
	d->db.sync(0);
}

void Base::loadIntoCache(FileId id) const
{
	if (d->cachedId == id) return;

	d->cachedId = id;
	d->cachedProperties.clear();

	Private::Key key(id);
	Private::Data data;
	unless (d->db.get(0, &key, &data, 0))
	{
		QStringList props;
		data.get(props);

		if (props.count() % 2)
		{ // corrupt?
			const_cast<Base*>(this)->remove(File(const_cast<Base*>(this), id));
			return;
		}

		for (QStringList::Iterator i(props.begin()); i != props.end(); ++i)
		{
			QString &key = *i;
			QString &value = *++i;
			d->cachedProperties.insert(key, value);
		}
	}
}

QString Base::saveMetaXML()
{
	QDomDocument doc;
	doc.setContent(QString("<meta />"));
	QDomElement doce = doc.documentElement();

	QDomElement e = doc.createElement("slices");
	e.setAttribute("highslice", QString::number(d->sliceHigh));
	doce.appendChild(e);

	for (QPtrListIterator<Slice> i(d->slices); *i; ++i)
	{
		QDomElement slice = doc.createElement("slice");
		slice.setAttribute("id", (*i)->id());
		slice.setAttribute("name", (*i)->name());
		e.appendChild(slice);
	}
	return doc.toString();
}

void Base::move(FileId oldid, FileId newid)
{
	Private::Key key(oldid);
	Private::Data data;
	unless (d->db.get(0, &key, &data, 0))
	{
		QStringList props;
		data.get(props);
		d->db.del(0, &key, 0);

		Private::Key key2(newid);
		d->db.put(0, &key2, &data, 0);
	}
}

void Base::dump()
{
	for (FileId id=1; id <= high(); id++)
	{
		QStringList props = properties(id);
		std::cerr << id << '.';
		for (QStringList::Iterator i(props.begin()); i != props.end(); ++i)
		{
			QString prop = *i;
			std::cerr << ' ' << prop.latin1() << '=' << property(id, prop).latin1();
		}
		std::cerr << std::endl;
	}
}

void Base::notifyChanged(const File &file)
{
	emit modified(file);
}


QPtrList<Slice> Base::slices()
{
	return d->slices;
}

Slice *Base::addSlice(const QString &name)
{
	Slice *sl = new Slice(this, d->sliceHigh++, name);
	d->slices.append(sl);
	slicesModified();
	return sl;
}

Slice *Base::defaultSlice()
{
	for (QPtrListIterator<Slice> i(d->slices); *i; ++i)
	{
		if ((*i)->id() == 0) return *i;
	}

	abort();
	return 0;
}

void Base::removeSlice(Slice *slice)
{
	assert(slice->id() > 0);
	d->slices.removeRef(slice);
	delete slice;
}

Slice *Base::sliceById(int id)
{
	for (QPtrListIterator<Slice> i(d->slices); *i; ++i)
	{
		if ((*i)->id() == id) return *i;
	}
	return 0;
}


void Base::loadMetaXML(const QString &xml)
{
	d->slices.setAutoDelete(true);
	d->slices.clear();
	d->slices.setAutoDelete(false);

	QDomDocument doc;
	doc.setContent(xml);
	QDomElement doce = doc.documentElement();
	bool loadedId0=false;

	for (QDomNode n = doce.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement e = n.toElement();
		if (e.isNull()) continue;

		if (e.tagName().lower() == "slices")
		{
			d->sliceHigh = e.attribute("highslice", "1").toInt();
			for (QDomNode n = e.firstChild(); !n.isNull(); n = n.nextSibling())
			{
				QDomElement e = n.toElement();
				if (e.isNull()) continue;
				if (e.tagName().lower() == "slice")
				{
					int id = e.attribute("id").toInt();
					if (id==0 && loadedId0) break;
					loadedId0=true;
					QString name = e.attribute("name");
					d->slices.append(new Slice(this, id, name));
				}
			}
		}
	}

	if (d->slices.count() == 0)
	{
		// we must have a default
		d->slices.append(new Slice(this, 0, ""));
	}
}

#include "base.moc"
