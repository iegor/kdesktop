/***************************************************************************
                          classinfoview.cpp  -  description
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

#include <klocale.h>

#include "classinfoview.h"

ClassInfoView::ClassInfoView(QWidget *parent, const char *name ) : KListView(parent,name)
{
  addColumn( i18n( "Name" ) );
  addColumn( i18n( "Value" ) );

  setRootIsDecorated( true );
  setAllColumnsShowFocus( true );
  setFullWidth( true );
}

ClassInfoView::~ClassInfoView()
{
}

void ClassInfoView::buildList( QObject *o )
{
  QMetaObject *mo = o->metaObject();

  for (int index = 0; index < mo->numClassInfo(true); index++) {
    const QClassInfo * classInfo = mo->classInfo(index, true);
    new KListViewItem( this, classInfo->name, classInfo->value );
  }
}

void ClassInfoView::setTarget( QObject *o )
{
  clear();
  buildList( o );
}

#include "classinfoview.moc"
