/***************************************************************************
    begin                : Tue Oct 5 1999
    copyright            : (C) 1999 by Peter Putzer
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

#include <qrect.h>
#include <qscrollbar.h>
#include <qheader.h>

#include <kdebug.h>

#include "ksvdraglist.h"
#include "kdltooltip.h"

KDLToolTip::KDLToolTip (KSVDragList *parent, QToolTipGroup* group)
  : QToolTip(parent, group),
	mParent (parent)
{
}

KDLToolTip::~KDLToolTip()
{
}

void KDLToolTip::maybeTip (const QPoint& p)
{
  if (!mParent->displayToolTips())
	return;

  QString text;
  QRect rect;

  const QRect vert = mParent->verticalScrollBar()->geometry();
  const QRect horiz = mParent->horizontalScrollBar()->geometry();

  if (vert.contains(p))
	{
	  rect = vert;
	  
	  if (!mParent->commonToolTips())
		text = mParent->verticalScrollBarTip();
	  else
		text = mParent->tooltip();
	}
  else if (horiz.contains(p))
	{
	  rect = horiz;
	  if (!mParent->commonToolTips())
		text = mParent->horizontalScrollBarTip();
	  else
		text = mParent->tooltip();
	  
	}
  else
	{ 
	  QPoint rp = mParent->viewport()->mapFromParent (p);
	  QListViewItem* i = mParent->itemAt (rp);
	  KSVItem* item = static_cast<KSVItem*> (i);
	  	
	  rect = mParent->header()->geometry();
      if (rect.contains (p))
        {
		  text = mParent->tooltip();
        }
      else if (item)
		{
		  rect = mParent->itemRect (i);
		  rect.moveTopLeft (mParent->viewport()->mapToParent (rect.topLeft()));

		  text = item->tooltip();
		}
	  else
		{
          rect = mParent->rect();

          QListViewItem* last = mParent->lastItem();          
          if (last)
            rect.setTop (mParent->viewport()->mapToParent (mParent->itemRect(last).bottomRight()).y());
          
		  text = mParent->tooltip();
		}
	}
  
  if (!text.isEmpty())
	tip (rect, text);
}
