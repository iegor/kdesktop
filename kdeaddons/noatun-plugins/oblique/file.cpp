// Copyright (c) 2003-2004 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#include "file.h"
#include "selector.h"
#include "query.h"

#include <iostream>

#include <klocale.h>
#include <kfilemetainfo.h>
#include <kmimetype.h>


File::File(Base *base, FileId id)
{
	mBase = base;
	mId = id;

}

File::File(const File &ref)
{
	operator =(ref);
}

File::File()
{
	mBase=0;
	mId = 0;
}

File &File::operator=(const File &ref)
{
	mBase = ref.mBase;
	mId = ref.mId;

	return *this;
}

QString File::file() const
{
	return property("file");
}

KURL File::url() const
{
	KURL url;
	url.setPath(file());
	return url;
}

struct Map { const char *kfmi; const char *noatun; };
static const Map propertyMap[] =
{
	{ "Title", "ob::title_" },
	{ "Artist", "ob::author_" },
	{ "Album", "ob::album_" },
	{ "Genre", "ob::genre_" },
	{ "Tracknumber", "ob::track_" },
	{ "Date", "ob::date_" },
	{ "Comment", "ob::comment_" },
	{ "Location", "ob::location_" },
	{ "Organization", "ob::organization_" },
	{ "Bitrate", "ob::bitrate_" },
	{ "Sample Rate", "ob::samplerate_" },
	{ "Channels", "ob::channels_" },
	{ 0, 0 }
};

QString File::property(const QString &property) const
{
	QString str = base()->property(id(), property);

	if (!str)
	{
		QString mangled = "ob::" + property + "_";
		str = base()->property(id(), mangled);
	}

	return str;
}


void File::makeCache()
{
	setProperty("ob::mimetype_", KMimeType::findByPath(file())->name());
	KFileMetaInfo info(file());

	for (int i=0; propertyMap[i].kfmi; i++)
	{
		QString kname(propertyMap[i].kfmi);
		if (info.isValid() && kname.length())
		{
			QString val = info.item(kname).string(false);
			if (val=="---" || !val.stripWhiteSpace().length())
			{ // grr
				val = "";
			}
			if (val.length())
			{
				setProperty(propertyMap[i].noatun, val);
			}
		}
	}
}


void File::setProperty(const QString &key, const QString &value)
{
	if (property(key) == value) return;
	base()->setProperty(id(), key, value);
	PlaylistItem p=new Item(*this);
	p.data()->modified();
}

void File::clearProperty(const QString &key)
{
	if (property(key).isNull()) return;
	base()->clearProperty(id(), key);
	PlaylistItem p=new Item(*this);
	p.data()->modified();
}

QStringList File::properties() const
{
	QStringList l = base()->properties(id());

	for (int i=0; propertyMap[i].noatun; i++)
	{
		if (property(propertyMap[i].noatun).length())
		{
			l += propertyMap[i].noatun;
		}
	}
	return l;
}

void File::setId(FileId id)
{
	base()->move(mId, id);
	mId = id;
}


void File::setPosition(Query *query, const File &after)
{
	setProperty(
			"Oblique:after_" + query->name() + '_',
			QString::number(after.id())
		);
}

bool File::getPosition(const Query *query, File *after) const
{
	assert(query);
	assert(after);
	QString name = "Oblique:after_" + query->name() + '_';
	if (name.isEmpty()) return false;
	
	QString val = property(name);
	if (val.isEmpty())
		return false;
	*after = File(mBase, val.toUInt());
	return true;
}


void File::remove()
{
	PlaylistItem p=new Item(*this);
	p.data()->removed();
	mBase->remove(*this);
}

void File::addTo(Slice *slice)
{
	QString slices = property("Oblique:slices_");
	slices += "\n" + QString::number(slice->id(), 16);
	setProperty("Oblique:slices_", slices);
	emit mBase->addedTo(slice, *this);
}

void File::removeFrom(Slice *slice)
{
	QString slices = property("Oblique:slices_");
	QStringList sliceList = QStringList::split('\n', slices);
	QString sid = QString::number(slice->id(), 16);
	sliceList.remove(sid);

	slices = sliceList.join("\n");
	setProperty("Oblique:slices_", slices);
	emit mBase->removedFrom(slice, *this);
}

bool File::isIn(const Slice *slice) const
{
	int id = slice->id();
	if (id==0) return true;

	QString slices = property("Oblique:slices_");
	QStringList sliceList = QStringList::split('\n', slices);
	for (QStringList::Iterator i(sliceList.begin()); i!= sliceList.end(); ++i)
	{
		if ((*i).toInt(0, 16) == id) return true;
	}
	return false;
}



Slice::Slice(Base *base, int id, const QString &name)
{
	mId = id;
	mBase = base;
	mName = name;
}

QString Slice::name() const
{
	if (mId == 0) return i18n("Complete Collection");
	return mName;
}

void Slice::setName(const QString &name)
{
	if (mId == 0) return;
	mName = name;
	emit mBase->slicesModified();
}

void Slice::remove()
{
	if (mId == 0) return;
	mBase->removeSlice(this);
	Base *base = mBase;

	for (FileId fi=1; ; fi++)
	{
		File f = base->first(fi);
		f.removeFrom(this);
		fi = f.id();
	}
	emit base->slicesModified();
}



