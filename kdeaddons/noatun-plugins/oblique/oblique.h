// Copyright (c) 2003-2005 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#ifndef OBLIQUE_H
#define OBLIQUE_H

#include <noatun/playlist.h>
#include <noatun/plugin.h>
#include "query.h"
#include "kdatacollection.h"

#include <kio/global.h>

class View;
class Tree;
class Base;
class Selector;
class TreeItem;
class DirectoryAdder;

namespace KIO
{
	class ListJob;
	class Job;
}

class Oblique : public Playlist, public Plugin
{
Q_OBJECT
	View *mView;
	Base *mBase;
	Selector *mSelector;
	KDataCollection mSchemaCollection;
	DirectoryAdder *mAdder;
	

public:
	Oblique();
	~Oblique();

	Base *base() { return mBase; }

	QStringList schemaNames() const { return mSchemaCollection.names(); }
	QString loadSchema(Query &q, const QString &name)
	{
		QString t = q.load(mSchemaCollection.file(name));
		if (t.length())
			q.setName(name);
		return t;
	}
	
	void saveSchema(Query &q, const QString &name, const QString &title)
	{
		q.save(title, mSchemaCollection.saveFile(name));
	}
	
	void removeSchema(const QString &name)
	{
		mSchemaCollection.remove(name);
	}

	virtual void reset();
	virtual void clear();
	virtual void addFile(const KURL&, bool play=false);
	virtual PlaylistItem next();
	virtual PlaylistItem previous();
	virtual PlaylistItem current();
	virtual void setCurrent(const PlaylistItem &);
	virtual PlaylistItem getFirst() const;
	virtual PlaylistItem getAfter(const PlaylistItem &item) const;
	virtual bool listVisible() const;
	virtual void showList();
	virtual void hideList();

	virtual Playlist *playlist() { return this; }

public slots:
	void selected(TreeItem *cur);
	void beginDirectoryAdd(const KURL &url);

private slots:
	void loopTypeChange(int i);
	void adderDone();
};

/**
 * loads the database into a Tree
 **/
class Loader : public QObject
{
Q_OBJECT
	// the id of the next file to load
	FileId mDeferredLoaderAt;
	Tree *mTree;
	Base *mBase;

public:
	Loader(Tree *into);

signals:
	void finished();

private slots:
	void loadItemsDeferred();
};

/**
 * Adds a directory to
 * emits @ref done() when finished so you
 * can delete it
 **/
class DirectoryAdder : public QObject
{
	Q_OBJECT
	Oblique *mOblique;
	KURL::List pendingAddDirectories;
	KURL::List::Iterator lastAddedSubDirectory;
	KIO::ListJob *listJob;
	KURL currentJobURL;

public:
	DirectoryAdder(const KURL &dir, Oblique *oblique);
	
	Oblique *oblique() { return mOblique; }

public slots:
	void add(const KURL &dir);

signals:
	void done();

private slots:
	void slotResult(KIO::Job *job);
	void slotEntries(KIO::Job *job, const KIO::UDSEntryList &entries);
	void slotRedirection(KIO::Job *, const KURL & url);

private:
	
	void addNextPending();
};

#endif
