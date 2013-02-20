// Copyright (c) 2003-2005 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#include "oblique.h"
#include "base.h"
#include "view.h"
#include "file.h"
#include "selector.h"
#include "cmodule.h"

#include <noatun/player.h>

#include <kconfig.h>
#include <kstandarddirs.h>
#include <kio/job.h>
#include <kfileitem.h>

#include <qtimer.h>

extern "C" Plugin *create_plugin()
{
	return new Oblique();
}



Oblique::Oblique()
	: Playlist(0, 0), mSchemaCollection("oblique/schemas")
{
	mView = 0;
	mAdder = 0;
	
	KConfigGroup g(KGlobal::config(), "oblique");

	mBase = new Base(::locate("data", "noatun/")+"/oblique-list");

	mView = new View(this);
	connect(napp->player(), SIGNAL(loopTypeChange(int)), SLOT(loopTypeChange(int)));

	mSelector = new SequentialSelector(mView->tree());

	new Configure(this);

	// psst, window's gone, pass it on!
	connect(mView, SIGNAL(listHidden()), SIGNAL(listHidden()));
	connect(mView, SIGNAL(listShown()), SIGNAL(listShown()));

	loopTypeChange(napp->player()->loopStyle());
}


Oblique::~Oblique()
{
	adderDone();
	delete mView;
	delete mBase;
}

void Oblique::loopTypeChange(int i)
{
	PlaylistItem now = current();

	if (i == Player::Random)
	{
		if (!dynamic_cast<RandomSelector*>(mSelector))
		{
			delete mSelector;
			mSelector = new RandomSelector(mView->tree());
		}
	}
	else
	{
		delete mSelector;
		mSelector = new SequentialSelector(mView->tree());
	}
}

void Oblique::adderDone()
{
	delete mAdder;
	mAdder = 0;
}


void Oblique::reset()
{
	mView->tree()->setCurrent(0);
	loopTypeChange(0);
}

void Oblique::clear()
{
	mBase->clear();
}

void Oblique::addFile(const KURL &url, bool play)
{
	KFileItem fileItem(KFileItem::Unknown, KFileItem::Unknown, url);
	if (fileItem.isDir())
	{
		beginDirectoryAdd(url);
	}
	else
	{
		File f = mBase->add(url.path());
		PlaylistItem p=new Item(f);
		p.data()->added();
		if (play) setCurrent(p);
	}
}


PlaylistItem Oblique::next()
{
	return mSelector->next();
}

PlaylistItem Oblique::previous()
{
	return mSelector->previous();
}


PlaylistItem Oblique::current()
{
	return mSelector->current();
}

void Oblique::setCurrent(const PlaylistItem &item)
{
	if (!item) return;
	mSelector->setCurrent(*static_cast<Item*>(const_cast<PlaylistItemData*>(item.data())));
	emit playCurrent();
}



PlaylistItem Oblique::getFirst() const
{
	FileId first=1;
	File item = mBase->first(first);

	if (!item) return 0;

	return new Item(item);
}


PlaylistItem Oblique::getAfter(const PlaylistItem &item) const
{
	File after = mBase->first(static_cast<const Item*>(item.data())->itemFile().id()+1);
	if (!after) return 0;
	return new Item(after);
}

bool Oblique::listVisible() const
{
	return mView->isVisible();
}

void Oblique::showList()
{
	mView->show();
}

void Oblique::hideList()
{
	mView->hide();
}


void Oblique::selected(TreeItem *cur)
{
	Item *item = new Item(cur->file());
	PlaylistItem pli = item;
	setCurrent(pli);
}

void Oblique::beginDirectoryAdd(const KURL &url)
{
	if (mAdder)
	{
		mAdder->add(url);
	}
	else
	{
		mAdder = new DirectoryAdder(url, this);
		connect(mAdder, SIGNAL(done()), SLOT(adderDone()));
	}
}



Loader::Loader(Tree *tree)
	: QObject(tree)
{
	mTree = tree;
	mBase = mTree->oblique()->base();
	mDeferredLoaderAt=1;
	

	QTimer::singleShot(0, this, SLOT(loadItemsDeferred()));
}

void Loader::loadItemsDeferred()
{
	// do/try 16 at a time
	for (int xx=0; xx < 16; xx++)
	{
		if (mDeferredLoaderAt > mBase->high())
		{
			// finished
			mBase->resetFormatVersion();
			emit finished();
			return;
		}

		File f = mBase->find(mDeferredLoaderAt);

		if (f)
		{
			if (mBase->formatVersion() <= 0x00010001)
				f.makeCache();

			if (f.isIn(mTree->slice()))
				mTree->insert(f);
		}
		mDeferredLoaderAt++;
	}

	QTimer::singleShot(0, this, SLOT(loadItemsDeferred()));
}


DirectoryAdder::DirectoryAdder(const KURL &dir, Oblique *oblique)
{
	listJob=0;
	mOblique = oblique;
	
	add(dir);
}

void DirectoryAdder::add(const KURL &dir)
{
	if (dir.upURL().equals(currentJobURL, true))
	{
		// We are a subdir of our currentJobURL and need to get listed next,
		// NOT after all the other dirs that are on the same level as
		// currentJobURL!
		lastAddedSubDirectory = pendingAddDirectories.insert(lastAddedSubDirectory, dir);
		lastAddedSubDirectory++;
	}
	else
	{
		pendingAddDirectories.append(dir);
	}
	addNextPending();
}

void DirectoryAdder::addNextPending()
{
	KURL::List::Iterator pendingIt= pendingAddDirectories.begin();
	if (!listJob &&	(pendingIt!= pendingAddDirectories.end()))
	{
		currentJobURL= *pendingIt;
		listJob = KIO::listDir(currentJobURL, false, false);
		connect(
				listJob, SIGNAL(entries(KIO::Job*, const KIO::UDSEntryList&)),
				SLOT(slotEntries(KIO::Job*, const KIO::UDSEntryList&))
			);
		connect(
				listJob, SIGNAL(result(KIO::Job *)),
				SLOT(slotResult(KIO::Job *))
			);
		connect(
				listJob, SIGNAL(redirection(KIO::Job *, const KURL &)),
				SLOT(slotRedirection(KIO::Job *, const KURL &))
			);
		pendingAddDirectories.remove(pendingIt);
		lastAddedSubDirectory = pendingAddDirectories.begin();
	}
}

void DirectoryAdder::slotResult(KIO::Job *job)
{
	listJob= 0;
	if (job && job->error())
		job->showErrorDialog();
	addNextPending();
	if (!listJob)
		emit done();
}

void DirectoryAdder::slotEntries(KIO::Job *, const KIO::UDSEntryList &entries)
{
	QMap<QString,KURL> __list; // temp list to sort entries

	KIO::UDSEntryListConstIterator it = entries.begin();
	KIO::UDSEntryListConstIterator end = entries.end();

	for (; it != end; ++it)
	{
		KFileItem file(*it, currentJobURL, false /* no mimetype detection */, true);
		// "prudhomm:
		// insert the path + url in the map to sort automatically by path
		// note also that you use audiocd to rip your CDs then it will be sorted the right way
		// now it is an easy fix to have a nice sort BUT it is not the best
		// we should sort based on the tracknumber"
		// - copied over from old kdirlister hack <hans_meine@gmx.de>
		__list.insert(file.url().path(), file.url());
	}
	QMap<QString,KURL>::Iterator __it;
	for( __it = __list.begin(); __it != __list.end(); ++__it )
	{
		oblique()->addFile(__it.data(), false);
	}
}

void DirectoryAdder::slotRedirection(KIO::Job *, const KURL & url)
{
	currentJobURL= url;
}


#include "oblique.moc"
