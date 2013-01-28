// Copyright (c) 2003 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#ifndef TREE_H
#define TREE_H

#include <qwidget.h>
#include <klistview.h>

#include "base.h"
#include "query.h"
#include "file.h"

class Oblique;
class Tree;

class TreeItem : public KListViewItem
{
	QueryGroup *mGroup;
	File mFile;

	bool mUserOpened:1;
	bool mHidden:1;

public:
	TreeItem(Tree *parent, QueryGroup *group, const File &file, const QString &p=0);
	TreeItem(TreeItem *parent, QueryGroup *group, const File &file, const QString &p=0);
	~TreeItem();

	QueryGroup *group() { return mGroup; }
	const QueryGroup *group() const { return mGroup; }
	void setGroup(QueryGroup *group) { mGroup = group; }

	TreeItem *parent() { return static_cast<TreeItem*>(KListViewItem::parent()); }
	Tree *tree();
	TreeItem *itemBelow() { return static_cast<TreeItem*>(KListViewItem::itemBelow()); }
	TreeItem *firstChild() { return static_cast<TreeItem*>(KListViewItem::firstChild()); }
	TreeItem *nextSibling() { return static_cast<TreeItem*>(KListViewItem::nextSibling()); }

	// for gdb, which sucks.
	QString presentation() const;

	File file() { return mFile; }
	void setFile(File file) { mFile = file; }

	void setOpen(bool o);

	TreeItem *find(File item);

	bool playable() const;

	/**
	 * get the next item that is playable logically.
	 * that is, if it has a File, and its parent hasn't a
	 * File
	 **/
	TreeItem *nextPlayable();

	void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);

	virtual int compare(QListViewItem * i, int col, bool) const;

	/**
	 * open my parents so that I'm visible, if I'm playing or
	 * close my parents if I'm not playing, and the user
	 * didn't open
	 **/
	void autoExpand();
	bool userOpened() const { return mUserOpened; }
	bool hideIfNoMatch(const QString &match);

	void setHidden(bool on);

	virtual void setup();

private:
	TreeItem *next();

	void forceAutoExpand();

};

class FileMenu;
class Loader;

class Tree : public KListView
{
Q_OBJECT
	Oblique *mOblique;

	Query mQuery;
	TreeItem *mCurrent;
	FileMenu *lastMenu;
	Slice *mSlice;
	QString mFileOfQuery;

	friend class TreeItem;
	int mPlayableItemCount; // used by the friendship

	QPtrList<TreeItem> mAutoExpanded;
	unsigned int mAutoExpanding;

	Loader *mLoader;

public:
	Tree(Oblique *oblique, QWidget *parent=0);
	~Tree();
	TreeItem *firstChild();
	TreeItem *find(File item);
	TreeItem *current() { return mCurrent; }
	Query *query() { return &mQuery; }
	Oblique *oblique() { return mOblique; }
	Slice *slice() { return mSlice; }
	QString fileOfQuery() const { return mFileOfQuery; }
	
	void clear();

	int playableItemCount() const { return mPlayableItemCount; }

	void addAutoExpanded(TreeItem *i) { mAutoExpanded.append(i); }
	void removeAutoExpanded(TreeItem *i) { mAutoExpanded.removeRef(i); }
	void resetAutoExpanded() { mAutoExpanded.clear(); }

	void setAutoExpanding(bool e) { mAutoExpanding += e ? 1 : -1; }
	bool autoExpanding() const { return mAutoExpanding; }

	void deleted(TreeItem *item);
	bool setSchema(const QString &name);

protected:
	virtual QDragObject *dragObject();
	void movableDropEvent(QListViewItem* parent, QListViewItem* afterme);

public slots:
	void insert(TreeItem *replace, File file);
	void insert(File file);
	void remove(File file);
	void update(File file);
	void setCurrent(TreeItem *cur);
	void setSlice(Slice *sl);
	
	void checkInsert(Slice*, File);
	void checkRemove(Slice*, File);

	/**
	 * the resulting presentation of this item must contain the string @p text
	 * or it will not be displayed
	 *  (used for Jump)
	 **/
	void setLimit(const QString &text);

private slots:
	void contextMenu(KListView* l, QListViewItem* i, const QPoint& p);
	void play(QListViewItem *item);

	void destroyLoader();
	
	void dropped(QPtrList<QListViewItem> &items, QPtrList<QListViewItem> &, QPtrList<QListViewItem> &afterNow);

signals:
	void selected(TreeItem *);

private:
	/**
	 * check if it fits into the group, and create
	 * the tree nodes for it
	 **/
	TreeItem *collate(TreeItem *fix, QueryGroup *group, const File &file, TreeItem *childOf=0);
	TreeItem *collate(const File &file, TreeItem *childOf=0)
	{
		if (!mQuery.firstChild()) return 0;
		return collate(0, mQuery.firstChild(), file, childOf);
	}

	TreeItem *collate(TreeItem *fix, const File &file, TreeItem *childOf=0)
	{
		if (!mQuery.firstChild()) return 0;
		return collate(fix, mQuery.firstChild(), file, childOf);
	}

	TreeItem *node(TreeItem *fix, QueryGroup *group, const File &file, TreeItem *childOf);

	/**
	 * remove the siblings and children of the treeitem
	 **/
	void remove(TreeItem *, const File &file);

	void limitHide(TreeItem *i, const QString &text);

	void reload();
};


#endif
