// Copyright (c) 2003 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#ifndef SELECTOR_H
#define SELECTOR_H

#include <noatun/playlist.h>
#include "file.h"

class Item : public PlaylistItemData
{
	File mFile;

public:
	Item(const File &file);

	File itemFile() const { return mFile; }

	virtual QString property(const QString &key, const QString &def=0) const;
	virtual void setProperty(const QString &key, const QString &property);
	virtual void clearProperty (const QString &key);
	virtual QStringList properties() const;

	virtual bool isProperty(const QString &key) const;

	virtual bool operator==(const PlaylistItemData &d) const;
	virtual void remove();
};

/**
 * a selector is an object that can get items from the
 * playlist in a certain order
 **/
class Selector
{
public:
	Selector();
	virtual ~Selector();
	virtual Item *next()=0;
	virtual Item *previous()=0;
	virtual Item *current()=0;
	virtual void setCurrent(const Item &item)=0;
};

class Tree;
class TreeItem;

class SequentialSelector : public Selector
{
	Tree *mTree;

public:
	SequentialSelector(Tree *tree);
	virtual ~SequentialSelector();
	virtual Item *next();
	virtual Item *previous();
	virtual Item *current();
	virtual void setCurrent(const Item &item);
	virtual void setCurrent(TreeItem *current);
};

class RandomSelector : public Selector
{
	Tree *mTree;
	TreeItem *mPrevious;
public:
	RandomSelector(Tree *tree);

	virtual Item *next();
	virtual Item *previous();
	virtual Item *current();
	virtual void setCurrent(const Item &item);
	virtual void setCurrent(TreeItem *item, TreeItem *previous);
};

#endif
