/*
** Copyright (C) 2000 Peter Putzer <putzer@kde.org>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

#include <stdlib.h>

#include <qpainter.h>
#include <qdragobject.h>
#include <qdatastream.h>
#include <qheader.h>
#include <qpixmapcache.h>
#include <qbitmap.h>
#include <qcursor.h>
#include <qcolor.h>
#include <qcstring.h>
#include <qlabel.h>

#include <kapplication.h>
#include <kpopupmenu.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

#include "kdltooltip.h"
#include "ksv_core.h"
#include "ksv_conf.h"
#include "ksvdraglist.h"
#include "ksvdrag.h"
#include "ActionList.h"

KSVItem::KSVItem (KListView* view)
  : QListViewItem (view),
	mData (new KSVData()),
    mConfig (KSVConfig::self()),
    mNewNormalColor (mConfig->newNormalColor()),
    mNewSelectedColor (mConfig->newSelectedColor()),
    mChangedNormalColor (mConfig->changedNormalColor()),
    mChangedSelectedColor (mConfig->changedSelectedColor())
{
}

KSVItem::KSVItem (const KSVItem& item)
  : QListViewItem (item.listView()),
	mData (new KSVData(*item.mData)),
    mConfig (KSVConfig::self()),
    mNewNormalColor (mConfig->newNormalColor()),
    mNewSelectedColor (mConfig->newSelectedColor()),
    mChangedNormalColor (mConfig->changedNormalColor()),
    mChangedSelectedColor (mConfig->changedSelectedColor())

{
  setText (SortNumber, mData->numberString());
  setText (ServiceName, label());
  setIcon (*item.pixmap(Icon));
}

KSVItem::KSVItem (KSVDragList* view, const KSVData& data)
  : QListViewItem (view),
	mData (new KSVData (data)),
    mConfig (KSVConfig::self()),
    mNewNormalColor (mConfig->newNormalColor()),
    mNewSelectedColor (mConfig->newSelectedColor()),
    mChangedNormalColor (mConfig->changedNormalColor()),
    mChangedSelectedColor (mConfig->changedSelectedColor())
{
  setText (SortNumber, mData->numberString());
  setText (ServiceName, label());
  setIcon (view->defaultIcon());
}

void KSVItem::copy (const KSVData& item)
{
  *mData = item;

  setText (SortNumber, mData->numberString());
  setText (ServiceName, mData->label());
}

QString KSVItem::key (int, bool) const
{
  return mData->numberString() + mData->label();
}

KSVItem::KSVItem (KListView* view, QString file, QString path, QString label, Q_INT8 nr )
  : QListViewItem (view),
	mData (new KSVData (file, path, label, nr)),
    mConfig (KSVConfig::self()),
    mNewNormalColor (mConfig->newNormalColor()),
    mNewSelectedColor (mConfig->newSelectedColor()),
    mChangedNormalColor (mConfig->changedNormalColor()),
    mChangedSelectedColor (mConfig->changedSelectedColor())
{
  setText (ServiceName, mData->label());
  setText (SortNumber, mData->numberString());
}

KSVItem::~KSVItem()
{
  delete mData;
}

void KSVItem::paintCell (QPainter* p, const QColorGroup& cg, int column, int width, int align)
{
  switch (column)
    {
    case SortNumber:
      p->setFont (mConfig->numberFont());
      break;

    case ServiceName:
      p->setFont (mConfig->serviceFont());
      break;
    }

  QColorGroup colors = cg;

  if (mData->newEntry())
	{
	  colors.setColor (QColorGroup::Text, mNewNormalColor);
	  colors.setColor (QColorGroup::HighlightedText, mNewSelectedColor);
	}
  else if (mData->changed())
	{
	  colors.setColor (QColorGroup::Text, mChangedNormalColor);
	  colors.setColor (QColorGroup::HighlightedText, mChangedSelectedColor);
	}

  QListViewItem::paintCell (p, colors, column, width, align);
}

QString KSVItem::toString() const
{
  return filenameAndPath();
}

QString KSVItem::tooltip () const
{
  QString result;

  if (mConfig->showDescription())
    {
      if (!ksv::getServiceDescription(filename(), result))
        result = toString();
      else
        {
          // split into nice chunks
          result = ksv::breakWords(result.simplifyWhiteSpace(), 50);
        }
    }
  else
    {
      result = toString();
    }

  return result;
}


void KSVItem::setNew ( bool val )
{
  mData->setNewEntry (val);
}


QPixmap KSVItem::paintDragIcon (const QFont& font, const QColorGroup&) const
{
  QFontMetrics metric (font);
  QRect textRect = metric.boundingRect (label());
  const QPixmap& icon = *pixmap (Icon);
  const int margin = listView()->itemMargin();

  const int width = icon.width() + margin * 2 + textRect.width();
  const int height = kMax (icon.height(), textRect.height());

  QPixmap result (width, height);
  result.fill (white);

  QPainter p (&result);
  p.drawPixmap (0, 0, icon);
  p.setFont (font);

  p.drawText (icon.width() + margin, 0,
			  width, height,
			  AlignLeft | AlignVCenter,
			  label());
  p.end();

  QBitmap mask (width, height);
  p.begin (&mask);
  p.setFont (font);

  p.fillRect (0, 0, width, height, color0);

  p.setPen (color1);
  p.drawPixmap (0, 0, icon.createHeuristicMask());

  p.drawText (icon.width() + margin, 0,
			  width, height,
			  AlignLeft | AlignVCenter,
			  label());

  QBrush brush (color0);
  brush.setStyle(Dense5Pattern);
  p.fillRect (0, 0, width, height, brush);

  p.end();

  result.setMask(mask);
  result.setOptimization(QPixmap::BestOptim);
  return result;
}

void KSVItem::setIcon (const QPixmap& icon)
{
  setPixmap (Icon, icon);
}

void KSVItem::setLabel (const QString& label)
{
  mData->setLabel (label);

  setText (ServiceName, label);
}

void KSVItem::setRunlevel (const QString& runlevel)
{
  mData->setRunlevel (runlevel);
}

void KSVItem::setFilename (const QString& file)
{
  mData->setFilename (file);
}

void KSVItem::setNumber (Q_INT8 nr)
{
  mData->setNumber (nr);

  setText(SortNumber, mData->numberString());
}

void KSVItem::setPath (const QString& path)
{
  mData->setPath (path);
}

void KSVItem::setNewNormalColor (const QColor& col)
{
  mNewNormalColor = col;
}

void KSVItem::setNewSelectedColor (const QColor& col)
{
  mNewSelectedColor = col;
}

void KSVItem::setChangedNormalColor (const QColor& col)
{
  mChangedNormalColor = col;
}

void KSVItem::setChangedSelectedColor (const QColor& col)
{
  mChangedSelectedColor = col;
}

void KSVItem::setChanged (bool val)
{
  mData->setChanged (val);
}

void KSVItem::setOriginalRunlevel (const QString& rl)
{
  mData->setOriginalRunlevel (rl);
}



//-----------------------
// KSVDragList
//-----------------------

KSVDragList::KSVDragList ( QWidget* parent, const char* name )
  : KListView (parent, name),
	mItemToDrag (0L),
	mDragMenu (new KPopupMenu (this)),
	mDragCopyMenu (new KPopupMenu (this)),
	mDragMoveMenu (new KPopupMenu (this)),
	mDisplayToolTips (true),
	mCommonToolTips (true)
{
  // add tooltips
  toolTip=new KDLToolTip (this);

  setDragEnabled (true);
  setDragAutoScroll(true);
  setAcceptDrops(true);
  setDropVisualizer (true);

  QHeader* h = header();
  h->setClickEnabled (false);

  addColumn (i18n("No.")); // SortNumber
  //  setColumnWidthMode (KSVItem::SortNumber, Manual);
  addColumn (""); // Icon
//   setColumnWidthMode (KSVItem::Icon, Manual);
  addColumn (i18n("Name")); // ServiceName
//   setColumnWidthMode (KSVItem::ServiceName, Manual);

  h->setResizeEnabled (false, KSVItem::Icon);

  setShowSortIndicator (false);
  setAllColumnsShowFocus (true);

  // multiselection is to complicated due to the sorting numbers
  setSelectionModeExt (Single);

  // init DND menu
  mDragMenu->insertTitle (i18n("Drag Menu"));
  mDragMenu->insertItem ("&Copy", Copy);
  mDragMenu->insertItem ("&Move", Move);

  mDragCopyMenu->insertTitle (i18n("Drag Menu"));
  mDragCopyMenu->insertItem ("&Copy", Copy);

  mDragMoveMenu->insertTitle (i18n("Drag Menu"));
  mDragMoveMenu->insertItem ("&Move", Move);

  mRMList.setAutoDelete(true);

  // catch drops
  connect (this, SIGNAL (dropped (QDropEvent*, QListViewItem*)),
		   this, SLOT (drop (QDropEvent*, QListViewItem*)));
}

KSVDragList::~KSVDragList()
{
  delete mDragMenu;
  delete mDragCopyMenu;
  delete mDragMoveMenu;
  delete toolTip;
}

void KSVDragList::initItem (QString file, QString path, QString name, Q_INT8 nr)
{
  KSVItem* tmp = new KSVItem(this, file, path, name, nr);
  tmp->setRunlevel(QObject::name());
  tmp->setOriginalRunlevel(QObject::name());

  tmp->setIcon (mIcon);

  setUpdatesEnabled(false);

  // marked as new in insert, we don´t want that
  tmp->setNew(false);
  tmp->setChanged (false);

  setUpdatesEnabled(true);
  repaint(false);
}

void KSVDragList::setDefaultIcon (const QPixmap& icon)
{
  mIcon = icon;

  for (QListViewItemIterator it (firstChild()); it.current(); ++it)
    {
	  static_cast <KSVItem*> (it.current())->setIcon (mIcon);
    }
}

void KSVDragList::clear ()
{
  KListView::clear();

  clearRMList();
}

void KSVDragList::setNewNormalColor ( const QColor& col )
{
  mNewNormalColor = col;

  KSVItem* item;
  for (QListViewItemIterator it (firstChild());
	   (item = static_cast<KSVItem*>(it.current()));
	   ++it)
    {
      item->setNewNormalColor (mNewNormalColor);
    }
}

void KSVDragList::setNewSelectedColor (const QColor& col)
{
  mNewSelectedColor = col;

  KSVItem* item;
  for (QListViewItemIterator it (firstChild());
	   (item = static_cast<KSVItem*>(it.current()));
	   ++it)
    {
      item->setNewSelectedColor (mNewSelectedColor);
    }
}

Q_INT8 KSVDragList::generateNumber (const QString& label,
                                    const KSVData* itemBelow, const KSVData* itemAbove) const
{
  Q_INT8 high = itemBelow ? itemBelow->number() : -1;
  Q_INT8 low = itemAbove ? itemAbove->number() : -1;
  Q_INT8 result = generateNumber (high, low);

  if (high == result && result != -1 && label >= itemBelow->label())
    result = -1;

  if (low == result && result != -1 && label <= itemAbove->label())
    result = -1;

  return result;
}

Q_INT8 KSVDragList::generateNumber (Q_INT8 high, Q_INT8 low) const
{
  Q_ASSERT (high >= low || high == -1);

  Q_INT8 result = -1;

  if (low < 0)
	{
	  if (high < 0)
		result = 50;
	  else
		result = high / 2;
	}
  else if (high < 0)
	result = (100 - low) / 2 + low;
  else
	result = (high - low) / 2 + low;

  return result;
}

void KSVDragList::setChangedNormalColor (const QColor& col)
{
  mChangedNormalColor = col;

  KSVItem* item;
  for (QListViewItemIterator it (firstChild());
	   (item = static_cast<KSVItem*> (it.current()));
	   ++it)
    {
      item->setChangedNormalColor (mChangedNormalColor);
    }
}

void KSVDragList::setChangedSelectedColor (const QColor& col)
{
  mChangedSelectedColor = col;

  KSVItem* item;
  for (QListViewItemIterator it (firstChild());
	   (item = static_cast<KSVItem*> (it.current()));
	   ++it)
    {
      item->setChangedSelectedColor (mChangedSelectedColor);
    }
}

KSVItem* KSVDragList::match (const KSVData& data)
{
  KSVItem* res = 0L;

  for (QListViewItemIterator it (this);
	   (res = static_cast<KSVItem*> (it.current()));
	   ++it)
    {
      if (*res->data() == data)
		break;
      else
		res = 0L;
    }

  return res;
}

void KSVDragList::setOrigin (bool val)
{
  mOrigin = val;

  if (mOrigin)
	{
	  emit newOrigin (this);
	  emit newOrigin();
	}
  else
	clearSelection();
}

void KSVDragList::startDrag ()
{
  mItemToDrag = static_cast<KSVItem*> (currentItem());

  KSVDrag* d = dynamic_cast<KSVDrag*> (dragObject());

  if (d)
	{
	  d->setPixmap (mItemToDrag->paintDragIcon (font(), colorGroup()));

	  d->drag();
	}
}

//KSVDrag* KSVDragList::dragObject ()
QDragObject* KSVDragList::dragObject ()
{
  if (mItemToDrag)
	{
	  return new KSVDrag (*mItemToDrag, this);
	}
  else
	return 0L;
}

bool KSVDragList::acceptDrag (QDropEvent* e) const
{
  e->acceptAction ();

  return acceptDrops() && e->provides ("application/x-ksysv");
}

void KSVDragList::focusInEvent (QFocusEvent* e)
{
  KListView::focusInEvent(e);

  if (!currentItem())
	setCurrentItem (firstChild());

  setOrigin(true);
}

void KSVDragList::clearRMList()
{
  mRMList.clear();
}

bool KSVDragList::removeFromRMList (const KSVData& item)
{
  KSVData* res = 0L;

  for (QPtrListIterator<KSVData> it (mRMList);
	   it.current();
	   ++it)
	{
	  res = it.current();

	  if (*res == item)
		break;
	  else
		res = 0L;
	}

  if (res)
	return mRMList.remove (res);
  else
	return false;
}

bool KSVDragList::insert (const KSVData& data, const KSVData* above, const KSVData* below)
{
  Q_INT8 nr = generateNumber (data.label(), below, above);

  if (nr > -1)
	{
	  KSVData real (data);
	  real.setNumber (nr);

	  KSVItem* item = new KSVItem (this, real);
	  item->setNew (true);

	  return true;
	}
  else
	emit cannotGenerateNumber ();

  return false;
}

bool KSVDragList::insert (const KSVData& data, const KSVItem* where, KSVAction*& action)
{
  const KSVData* above = 0L;
  const KSVData* below = 0L;

  if (where)
	{
	  above = where->data();
	  KSVItem* tmp = static_cast<KSVItem*> (where->nextSibling());
	  below = tmp ? tmp->data() : 0L;
	}
  else
    {
      KSVItem* tmp = static_cast<KSVItem*> (firstChild());
      below = tmp ? tmp->data() : 0L;
    }

  bool success = false;
  KSVItem* exists = match (data);
  action = 0L;

  if (exists)
	{
	  if (exists->data() == above || exists->data() == below)
		return false;

	  Q_INT8 nr = generateNumber (exists->label(), below, above);

	  if (nr == -1)
		{
		  emit cannotGenerateNumber();
		}
	  else
		{
		  KSVData oldState = *exists->data();
		  exists->setNumber (nr);
		  sort();

		  action = new ChangeAction (this, &oldState, exists->data());
		  success = true;
		}
	}
  else
	{
	  success = insert (data, above, below);

	  if (success)
		action = new AddAction (this, match (data)->data());
	}

  return success;
}

void KSVDragList::drop (QDropEvent* e, QListViewItem* after)
{
  KSVData data;
  KSVDragList* source = static_cast<KSVDragList*> (e->source());
  QPopupMenu* menu = 0L;

  // hack to empty the cursor stack after DND
  QApplication::restoreOverrideCursor();
  QApplication::restoreOverrideCursor();

  if ((!source) || (!strcmp(source->name(), "Scripts")))
	menu = mDragCopyMenu;
  else if (source == this)
	menu = mDragMoveMenu;
  else
	menu = mDragMenu;

  if (KSVDrag::decodeNative (e, data))
	{
	  int res = -1;

	  if (e->action() == QDropEvent::Copy && source != this)
		res = Copy;
	  else
		res = menu->exec (QCursor::pos(), 1);

	  if (res == -1) // operation cancelled
		return;

	  const bool move = res == Move;

	  KSVItem* tmp = static_cast<KSVItem*> (after);
	  KSVAction* action = 0L;
	  if (insert (data, tmp, action))
		{
		  if (move && source != this)
			{
			  KSVAction* actions[2];
 			  actions [0] = new RemoveAction (source, &data);
 			  actions [1] = action;

			  action = new CompoundAction (actions, 2);
			  delete source->match (data);
			}

		  emit undoAction (action);
		}
	}
}

bool KSVDragList::addToRMList (const KSVData& item)
{
  KSVData* res = 0L;

  for (QPtrListIterator<KSVData> it (mRMList);
	   it.current();
	   ++it)
	{
	  res = it.current();

	  if (*res == item)
		break;
	  else
		res = 0L;
	}

  if (!res)
	{
	  mRMList.append (new KSVData(item));
	  return true;
	}
  else
	  return false;
}

void KSVDragList::setEnabled (bool enable)
{
  if (enable)
    clearWState (WState_ForceDisabled);
  else
    setWState (WState_ForceDisabled);

  if (enable)
    {
      if (testWState (WState_Disabled))
        {
          clearWState (WState_Disabled);
          // setBackgroundFromMode(); // this is private in QWidget...
          // well it doesn't really matter in this case
          enabledChange( TRUE );
        }
    }
  else
    {
      if (!testWState(WState_Disabled))
        {
          if (focusWidget() == this)
            focusNextPrevChild (TRUE);
          setWState (WState_Disabled);
          // setBackgroundFromMode(); // this is private in QWidget...
          // well it doesn't really matter in this case
          enabledChange (FALSE);
        }
    }

  viewport()->setEnabled (enable);
}

// KServiceDragList

KServiceDragList::KServiceDragList (QWidget* parent, const char* name)
  : KSVDragList (parent, name)
{
}

KServiceDragList::~KServiceDragList ()
{
}

void KServiceDragList::startDrag ()
{
  mItemToDrag = static_cast<KSVItem*> (currentItem());

  KSVDrag* d = dynamic_cast<KSVDrag*> (dragObject());

  if (d)
	{
	  d->setPixmap (mItemToDrag->paintDragIcon (font(), colorGroup()));

	  d->dragCopy();
	}
}

#include "ksvdraglist.moc"
