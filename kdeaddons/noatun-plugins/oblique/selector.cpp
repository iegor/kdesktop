// Copyright (c) 2003 Charles Samuels <charles@kde.org>
// See the file COPYING for redistribution terms.

#include "tree.h"
#include "selector.h"
#include "oblique.h"

#include <noatun/app.h>
#include <noatun/player.h>

Item::Item(const File &file)
	: mFile(file)
{

}

QString Item::property(const QString &key, const QString &def) const
{
	if (key == "url")
	{
		KURL url;
		url.setPath(property("file"));
		return url.url();
	}

	QString str = mFile.property(key);
	if (str.isNull()) return def;
	return str;
}

void Item::setProperty(const QString &key, const QString &property)
{
	mFile.setProperty(key, property);
}

void Item::clearProperty(const QString &key)
{
	mFile.clearProperty(key);
}

QStringList Item::properties() const
{
	return mFile.properties();
}

bool Item::isProperty(const QString &key) const
{
	return !mFile.property(key).isNull();
}

bool Item::operator==(const PlaylistItemData &d) const
{
	return mFile == static_cast<const Item&>(d).mFile;
}

void Item::remove()
{
	mFile.remove();
}



Selector::Selector()
{
}

Selector::~Selector()
{
}


SequentialSelector::SequentialSelector(Tree *tree)
{
	mTree = tree;
}

SequentialSelector::~SequentialSelector()
{

}

Item *SequentialSelector::next()
{
	TreeItem *current = mTree->current();
	if (current)
	{
		current = current->nextPlayable();
	}
	else
	{
		current = mTree->firstChild();
		if (current && !current->playable())
		{
			current = current->nextPlayable();
		}
	}
	setCurrent(current);
	if (current && current->file())
		return new Item(current->file());
	return 0;
}

Item *SequentialSelector::previous()
{
	TreeItem *back = mTree->firstChild();
	TreeItem *after;
	TreeItem *current = mTree->current();
	// now we just go forward on back until the item after back is me ;)
	// this is terribly unoptimized, but I'm terribly lazy
	while (back and (after = back->nextPlayable()) != current)
	{
		back = after;
	}
	current = back;
	setCurrent(current);
	if (current && current->file())
		return new Item(current->file());
	return 0;
}

Item *SequentialSelector::current()
{
	TreeItem *current = mTree->current();
	if (!current) return next();
	if (current->file())
		return new Item(current->file());
	return 0;
}

void SequentialSelector::setCurrent(const Item &item)
{
	TreeItem *current = mTree->find(item.itemFile());
	setCurrent(current);
}

void SequentialSelector::setCurrent(TreeItem *current)
{
	if (current)
	{
		mTree->setCurrent(current);
	}
}



RandomSelector::RandomSelector(Tree *tree)
{
	mTree = tree;
	mPrevious = 0;
}

static TreeItem *randomItem(int &at, TreeItem *item)
{
	for ( ; item; item = item->nextSibling())
	{
		if (item->playable())
		{
			if (at==0)
				return item;

			at--;
		}
		if (TreeItem *i = randomItem(at, item->firstChild()))
		{
			return i;
		}
	}

	return 0;
}

Item *RandomSelector::next()
{
	TreeItem *previous = mTree->current();
	if (!mTree->playableItemCount())
	{
		return 0;
	}

	for (int tries=15; tries; tries--)
	{
		int randomIndex = KApplication::random() % (mTree->playableItemCount());

		TreeItem *nowCurrent = randomItem(randomIndex, mTree->firstChild());
		if (!nowCurrent) continue;

		setCurrent(nowCurrent, previous);
		return new Item(nowCurrent->file());
	}

	// !!!!
	return 0;
}

Item *RandomSelector::previous()
{
	if (!mPrevious) return 0;
	TreeItem *current = mPrevious;

	mTree->setCurrent(current);
	return new Item(current->file());
}

Item *RandomSelector::current()
{
	TreeItem *current = mTree->current();
	if (!current) return 0;
	return new Item(current->file());
}

void RandomSelector::setCurrent(const Item &item)
{
	setCurrent(mTree->find(item.itemFile()), 0);
}

void RandomSelector::setCurrent(TreeItem *item, TreeItem *previous)
{
	mPrevious = previous;
	mTree->setCurrent(item);

	napp->player()->stop();
	napp->player()->play();
}



