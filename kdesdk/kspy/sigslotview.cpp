/***************************************************************************
                          sigslotview.cpp  -  description
                             -------------------
    begin                : Tue May 1 2001
    copyright            : (C) 2001 by Richard Moore
    email                : rich@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qobjectdict.h>
#include <qobjectlist.h>
#include <qmetaobject.h>
#include <qstrlist.h>
#include <qvariant.h>

#include <klocale.h>

#include "sigslotview.h"

SigSlotView::SigSlotView(QWidget *parent, const char *name ) : KListView(parent,name)
{
  addColumn( i18n( "Signals/Slots" ) );

  setRootIsDecorated( true );
  setAllColumnsShowFocus( true );
  setFullWidth( true );
}

SigSlotView::~SigSlotView()
{
}

void SigSlotView::buildList( QObject *o )
{
  QMetaObject *mo = o->metaObject();

  KListViewItem *sigs = new KListViewItem( this, "Signals" );
  QStrList sigList = mo->signalNames( true );
  QStrListIterator sigIt( sigList );
  char *si;
  while ( (si=sigIt.current()) != 0 ) {
    ++sigIt;
    new KListViewItem( sigs, si /*, "someSignal()"*/ );
  }

  KListViewItem *slts = new KListViewItem( this, "Slots" );
  QStrList sltList = mo->slotNames( true );
  QStrListIterator sltIt( sltList );
  char *sl;
  while ( (sl=sltIt.current()) != 0 ) {
    ++sltIt;
    new KListViewItem( slts, sl/*, "someSlot()"*/ );
  }

  setOpen( sigs, false );
  setOpen( slts, false );
}

void SigSlotView::setTarget( QObject *o )
{
  clear();
  buildList( o );
}

#include "sigslotview.moc"
