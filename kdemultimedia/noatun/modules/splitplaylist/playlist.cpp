#include "playlist.h"
#include "view.h"
#include <noatun/player.h>

#include <kapplication.h>
#include <krandomsequence.h>
#include <kdebug.h>
#include <kwin.h>

#include <kiconloader.h>

SplitPlaylist *SplitPlaylist::Self=0;

SplitPlaylist::SplitPlaylist()
	: Playlist(0, "SplitPlaylist"), Plugin(), mExiting(false)
{
	Self=this;
}

void SplitPlaylist::init()
{
	view=new View(this); // 195
	connect(view->listView(), SIGNAL(executed(QListViewItem*)), SLOT(listItemSelected(QListViewItem*)));
	connect(view, SIGNAL(shown()), SIGNAL(listShown()));
	connect(view, SIGNAL(hidden()), SIGNAL(listHidden()));

	view->init(); // 1000
}

SplitPlaylist::~SplitPlaylist()
{
	mExiting=true;
	delete view;
}

void SplitPlaylist::reset()
{
	SafeListViewItem *i;
	setCurrent(i=static_cast<SafeListViewItem*>(view->listView()->firstChild()), false);
	if (i && !i->isOn())
		next(false);
}

PlaylistItem SplitPlaylist::next()
{
	return next(true);
}

PlaylistItem SplitPlaylist::next(bool play)
{
	PlaylistItem nextItem;

	if (napp->player()->loopStyle() == Player::Random)
	{
		// Ignore all this order stuff and select a random item
		List *lview = view->listView();

		if (lview->childCount())
		{
			SafeListViewItem *slvi = static_cast<SafeListViewItem*>(
					lview->itemAtIndex(KApplication::random() % lview->childCount())
				);
			nextItem = PlaylistItem(slvi);
		}
		else
		{
			nextItem = 0;
		}
	}
	else
	{
		if(!current())
		{
			nextItem = static_cast<SafeListViewItem*>(static_cast<SafeListViewItem*>(getFirst().data()));
		}
		else
		{
			nextItem = static_cast<SafeListViewItem*>(
				static_cast<SafeListViewItem*>(current().data())->itemBelow());
		}
	}

	if (!nextItem) // don't set a null-item as current item
	{
		return 0;
//		nextItem = static_cast<SafeListViewItem*>(static_cast<SafeListViewItem*>(getFirst().data()));
	}

	PlaylistItem oldCurrent = currentItem;
	setCurrent(nextItem, play);

	// Hack for back button on randomized play
	if (oldCurrent)
		randomPrevious = oldCurrent;

	if (currentItem)
		if (!static_cast<SafeListViewItem*>(currentItem.data())->isOn())
			return next(play);

	return currentItem;
}

PlaylistItem SplitPlaylist::current()
{
	return currentItem;
}

PlaylistItem SplitPlaylist::previous()
{
	if (napp->player()->loopStyle() == Player::Random && randomPrevious)
	{
		List *list = view->listView();
		// check if the item still exists (hackitude: 50%)
		bool found=false;
		for (QListViewItem *i = list->firstChild(); i; i = i->nextSibling())
		{
			if (i == static_cast<SafeListViewItem*>(randomPrevious.data()))
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			// setCurrent modified randomPrevious, and setCurrent is pass-by-reference
			PlaylistItem prev = randomPrevious;

			setCurrent(prev);
			return currentItem;
		}
	}
	// there's a possibility that I will fall out to here
	// from the above branch

	PlaylistItem nextItem;
	if(!current())
	{
		nextItem = static_cast<SafeListViewItem*>(static_cast<SafeListViewItem*>(getFirst().data()));
	}
	else
	{
		nextItem = static_cast<SafeListViewItem*>(
			static_cast<SafeListViewItem*>(current().data())->itemAbove());
	}
	if (!nextItem) // don't set a null-item as current item
		return 0;

	setCurrent(nextItem);

	if (currentItem)
		if (!static_cast<SafeListViewItem*>(currentItem.data())->isOn())
			return previous();

	return currentItem;
}

PlaylistItem SplitPlaylist::getFirst() const
{
	return static_cast<SafeListViewItem*>(view->listView()->firstChild());
}

PlaylistItem SplitPlaylist::getAfter(const PlaylistItem &item) const
{
	if (item)
		return static_cast<SafeListViewItem*>(static_cast<const SafeListViewItem*>(item.data())->nextSibling());
	return 0;
}

bool SplitPlaylist::listVisible() const
{
	KWin::WindowInfo info = KWin::windowInfo(view->winId());
	return !(info.hasState(NET::Shaded) || info.hasState(NET::Hidden) || !info.valid() || !info.isOnCurrentDesktop());
}

void SplitPlaylist::showList()
{
	KWin::setOnDesktop(view->winId(), KWin::currentDesktop());
	view->show();
	if (view->isMinimized())
		view->showNormal();
	view->raise();
}

void SplitPlaylist::hideList()
{
	view->hide();
}

void SplitPlaylist::clear()
{
	view->listView()->clear();
}

void SplitPlaylist::addFile(const KURL &file, bool play)
{
	view->addFile(file, play);
}

void SplitPlaylist::setCurrent(const PlaylistItem &i)
{
	setCurrent(i, true);
}

void SplitPlaylist::setCurrent(const PlaylistItem &i, bool emitC)
{
	randomPrevious = PlaylistItem();
	emitC = emitC && currentItem;
	if (!i)
	{
		currentItem=0;
	}
	else
	{
		// remove the old icon
		SafeListViewItem *now=static_cast<SafeListViewItem*>(current().data());
		if (now)
			now->setPixmap(0, QPixmap());

		QRect rect(view->listView()->itemRect(static_cast<SafeListViewItem*>(current().data())));
		rect.setWidth(view->listView()->viewport()->width());
		currentItem=i;
		view->listView()->viewport()->repaint(rect,true);

		view->listView()->ensureItemVisible(static_cast<SafeListViewItem*>(current().data()));
		QRect currentRect= view->listView()->itemRect(static_cast<SafeListViewItem*>(current().data()));
		view->listView()->viewport()->repaint(currentRect);

		now=static_cast<SafeListViewItem*>(current().data());
		if(now)
			now->setPixmap(0, ::SmallIcon("noatunplay"));
	}

	if (emitC && !exiting())
		emit playCurrent();
}

void SplitPlaylist::remove(const PlaylistItem &)
{
//	delete i;
}

void SplitPlaylist::listItemSelected(QListViewItem *i)
{
	setCurrent(PlaylistItem(static_cast<SafeListViewItem*>(i)), false);
	emit playCurrent();
}

void SplitPlaylist::randomize()
{
	// turning off sorting is necessary
	// otherwise, the list will get randomized and promptly sorted again
	view->setSorting(false);
	List *lview = view->listView();
	// eeeeevil :)
	QPtrList<void> list;
	QPtrList<QListViewItem> items;
	for(int i = 0; i < lview->childCount(); i++)
	{
		list.append( (void*) i );
		items.append( lview->itemAtIndex( i ) );
	}

	KRandomSequence seq;
	seq.randomize( &list );

	for(int i = 0; i < lview->childCount(); i++)
	{
		items.take()->moveItem(lview->itemAtIndex((long) list.take()));
	}

	setCurrent(currentItem, false);
}

void SplitPlaylist::sort()
{
	view->setSorting(true);
	setCurrent(currentItem, false);
}

#include "playlist.moc"
