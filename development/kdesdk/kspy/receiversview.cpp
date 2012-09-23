/***************************************************************************
                          receiversview.cpp  -  description
                             -------------------
    begin                : Tue Jan 11 2005
    copyright            : (C) 2005 by Richard Dale
    email                : Richard_Dale@tipitina.demon.co.uk
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
#include <qsignalslotimp.h>

#include <klocale.h>

#include "receiversview.h"

class UnencapsulatedQObject : public QObject {
public:
    QConnectionList *public_receivers(int signal) const { return receivers(signal); }
};

ReceiversView::ReceiversView(QWidget *parent, const char *name ) : KListView(parent,name)
{
  addColumn( i18n( "Object" ) );
  addColumn( i18n( "Type" ) );
  addColumn( i18n( "Member Name" ) );

  setRootIsDecorated( true );
  setAllColumnsShowFocus( true );
  setFullWidth( true );
}

ReceiversView::~ReceiversView()
{
}

void ReceiversView::buildList( QObject *o )
{
  QMetaObject *mo = o->metaObject();
    
  UnencapsulatedQObject * qobject = (UnencapsulatedQObject *) o;
  QStrList signalNames = mo->signalNames(true);
    
  for (int sig = 0; sig < mo->numSignals(true); sig++) {
    QConnectionList * clist = qobject->public_receivers(sig);
    if (clist != 0) {
      KListViewItem *conn = new KListViewItem( this, signalNames.at(sig) );
            
      for (    QConnection * connection = clist->first(); 
               connection != 0; 
               connection = clist->next() ) 
      {
        new KListViewItem( conn,
                           connection->object()->name(), 
                           connection->memberType() == 1 ? "SLOT" : "SIGNAL",
                           connection->memberName() );
      }
    }
  }  
}

void ReceiversView::setTarget( QObject *o )
{
  clear();
  buildList( o );
}

#include "receiversview.moc"
