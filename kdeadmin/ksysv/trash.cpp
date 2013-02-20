/***************************************************************************
    begin                : Sun Oct 3 1999
    copyright            : (C) 1997-2000 by Peter Putzer
    email                : putzer@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2.                              *
 *                                                                         *
 ***************************************************************************/

// Trash Can

#include <qtooltip.h>
#include <qlabel.h>
#include <qpainter.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include "ksvdraglist.h"
#include "ksvdrag.h"
#include "ActionList.h"
#include "trash.h"

KSVTrash::KSVTrash (QWidget* parent, const char* name)
  : QFrame (parent, name),
    mKIL (KGlobal::iconLoader()),
    mLabel (new QLabel(this)),
    mOpen (false)
{
  setLineWidth(1);
  setMidLineWidth(0);

  setFrameStyle (QFrame::StyledPanel | QFrame::Sunken);

  mLabel->setPixmap(mKIL->loadIcon("trashcan_empty", KIcon::Desktop));
  mPixmapWidth = mLabel->pixmap()->width();
  mLabel->setGeometry(5, 7, mPixmapWidth, mPixmapWidth);

  QToolTip::add(mLabel, i18n("Drag here to remove services"));
  QToolTip::add(this, i18n("Drag here to remove services"));
    
  setMinimumSize(sizeHint());
  setAcceptDrops(true);

  mLabel->installEventFilter(this);
  mLabel->setAcceptDrops(true);
}

KSVTrash::~KSVTrash()
{
}

void KSVTrash::dropEvent (QDropEvent* e)
{
  KSVData data;
  KSVDragList* list = static_cast<KSVDragList*> (e->source());

  if (list && strcmp (list->name(), "Scripts")  && KSVDrag::decodeNative (e, data))
	{
	  e->accept();
	  
	  emit undoAction (new RemoveAction (list, &data));
	  delete list->match (data);
	}
  else
	e->ignore();

  if (mOpen)
    {
      mLabel->repaint(); 
      mOpen = false;
    }
}

void KSVTrash::dragMoveEvent ( QDragMoveEvent* e )
{
  if (e->provides ("application/x-ksysv") &&
      e->source() && strcmp (e->source()->name(), "Scripts"))
    {
      QPainter p;
	  
      p.begin(mLabel);
      p.drawPixmap( 0, 0, mKIL->loadIcon("trashcan_full", KIcon::Desktop) );
      p.end();
	  
      mOpen = true;
      e->accept();
    }
  else
	e->ignore();
}

void KSVTrash::dragLeaveEvent ( QDragLeaveEvent* )
{
  if (mOpen)
    {
      mLabel->repaint(); 
      mOpen = false;
    }
}

bool KSVTrash::eventFilter( QObject *, QEvent *e )
{
  switch (e->type())
    {
    case QEvent::DragMove:
      dragMoveEvent ( static_cast<QDragMoveEvent*> (e) );
      return true;
      break;
      
    case QEvent::DragLeave:
      dragLeaveEvent ( static_cast<QDragLeaveEvent*> (e) );
      return true;
      break;
      
    case QEvent::Drop:
      dropEvent ( static_cast<QDropEvent*> (e) );
      return true;
      break;

    default:
      return false;
    }
}

QSize KSVTrash::sizeHint() const
{
  static QSize size = QSize (mPixmapWidth + 2 * 5, mPixmapWidth + 2 * 7);

  return size;
}

#include "trash.moc"
