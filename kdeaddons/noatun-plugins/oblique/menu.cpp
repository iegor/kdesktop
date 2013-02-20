// Copyright (c) 2003,2004 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#include "menu.h"
#include <kiconloader.h>
#include <klocale.h>

#include "tree.h"

FileMenu::FileMenu(QWidget *parent, Oblique *oblique, File file)
	: KPopupMenu(parent)
{
	if (file)
		mFiles.append(file);
	insertItem(
			BarIconSet("delete"), i18n("&Remove From Playlist"),
			this, SLOT(removeFromList())
		);
	insertItem(i18n("&Properties"), this, SLOT(properties()));

	(new SliceListAction(
			i18n("&Slices"), oblique,
			this, SLOT(toggleInSlice(Slice *)),
			mFiles, this
		))->plug(this);
}

static void addTo(QValueList<File> &files, TreeItem *item)
{
	File f = item->file();
	if (f) files.append(f);

	item = item->firstChild();

	while (item)
	{
		addTo(files, item);
		item = item->nextSibling();
	}
}

FileMenu::FileMenu(QWidget *parent, Oblique *oblique, TreeItem *items)
	: KPopupMenu(parent)
{
	addTo(mFiles, items);

	insertItem(
			BarIconSet("delete"), i18n("&Remove From Playlist"),
			this, SLOT(removeFromList())
		);
	insertItem(i18n("&Properties"), this, SLOT(properties()));

	(new SliceListAction(
			i18n("&Slices"), oblique,
			this, SLOT(toggleInSlice(Slice *)),
			mFiles, this
		))->plug(this);
}

void FileMenu::removeFromList()
{
	for (QValueList<File>::Iterator i(mFiles.begin()); i != mFiles.end(); ++i)
	{
		(*i).remove();
	}
}

void FileMenu::properties()
{
	new ObliquePropertiesDialog(mFiles, parentWidget());
}

void FileMenu::toggleInSlice(Slice *slice)
{
	void (File::*task)(Slice*)=0;
	for (QValueList<File>::Iterator i(mFiles.begin()); i != mFiles.end(); ++i)
	{
		if (!task)
		{ // we check with the first one
			if ((*i).isIn(slice))
				task = &File::removeFrom;
			else
				task = &File::addTo;
		}

		((*i).*task)(slice);
	}
}



SliceListAction::SliceListAction(
		const QString &text, Oblique *oblique,
		QObject *reciever, const char *slot,
		const QValueList<File> &files, QObject *parent, const char *name
	) : KActionMenu(text, parent, name)
{
	mFiles = files;
	mOblique = oblique;
	slicesModified();
	if (reciever)
		connect(this, SIGNAL(activated(Slice*)), reciever, slot);
	connect(popupMenu(), SIGNAL(activated(int)), SLOT(hit(int)));
	connect(oblique->base(), SIGNAL(slicesModified()), SLOT(slicesModified()));
}

void SliceListAction::slicesModified()
{
	mIndexToSlices.clear();
	KPopupMenu *menu = popupMenu();
	menu->clear();

	QPtrList<Slice> slices = mOblique->base()->slices();
	int id=1;



	for (QPtrListIterator<Slice> i(slices); *i; ++i)
	{
		Slice *s = *i;
		if (s->id()==0 && mFiles.count())
		{
			continue;
		}

		menu->insertItem(s->name(), id);
		if (mFiles.count())
		{
			menu->setItemChecked(id, mFiles.first().isIn(s));
		}
//		else if (mOblique->slice() == s) // TODO: show the selected one
//		{
//			menu->setItemChecked(id, true);
//		}

		if (mFiles.count() && s->id() == 0)
		{
			menu->setItemEnabled(id, false);
		}

		mIndexToSlices.insert(id, s);
		id++;
	}
}

void SliceListAction::hit(int index)
{
	emit activated(mIndexToSlices[index]);
}



SchemaListAction::SchemaListAction(
		const QString &text,
		QObject *reciever, const char *slot,
		QObject *parent, const char *name
	) : KActionMenu(text, parent, name)
{
	mTree = 0;
	if (reciever)
		connect(this, SIGNAL(activated(const QString&)), reciever, slot);
	connect(popupMenu(), SIGNAL(aboutToShow()), SLOT(prepare()));
	connect(popupMenu(), SIGNAL(activated(int)), SLOT(hit(int)));
}

void SchemaListAction::prepare()
{
	assert(mTree);
	mIndexToSchemas.clear();
	KPopupMenu *menu = popupMenu();
	menu->clear();
	
	if (!mTree) return;

	int id=1;

	QStringList names = mTree->oblique()->schemaNames();

	for (QStringList::Iterator i(names.begin()); i != names.end(); ++i)
	{
		Query q;
		QString title = mTree->oblique()->loadSchema(q, *i);

		menu->insertItem(title, id);

		menu->setItemChecked(id, mTree->fileOfQuery() == *i);

		mIndexToSchemas.insert(id, *i);
		id++;
	}
}

void SchemaListAction::hit(int index)
{
	emit activated(mIndexToSchemas[index]);
}



ObliquePropertiesDialog::ObliquePropertiesDialog(const QValueList<File> &files, QWidget *parent)
	: KPropertiesDialog(makeItems(files), parent), mFiles(files)
{
	connect(this, SIGNAL(propertiesClosed()), SLOT(deleteLater()));
	connect(this, SIGNAL(applied()), SLOT(modified()));

	show();
}

void ObliquePropertiesDialog::modified()
{
	// TODO reload the file's info
	for (QValueList<File>::Iterator i(mFiles.begin()); i != mFiles.end(); ++i)
	{
		(*i).makeCache();
		(*i).base()->notifyChanged(*i);
	}
}

KFileItemList ObliquePropertiesDialog::makeItems(const QValueList<File> &files)
{
	KFileItemList kl;
	for (QValueList<File>::ConstIterator i(files.begin()); i != files.end(); ++i)
	{
		File f = *i;
		kl.append(new KFileItem(f.url(), f.property("mimetype"), KFileItem::Unknown));
	}
	return kl;
}

#include "menu.moc"

