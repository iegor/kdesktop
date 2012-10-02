/***************************************************************************
                          navview.cpp  -  description
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

#include <qobjectlist.h>
#include <qobjectdict.h>

#include <klocale.h>

#include "navview.h"
#include "navviewitem.h"

NavView::NavView(QWidget *parent, const char *name ) : KListView(parent,name)
{
  addColumn( i18n( "Name" ) );
  addColumn( i18n( "Type" ) );

  setRootIsDecorated( true );
  setAllColumnsShowFocus( true );
  setFullWidth( true );

  connect( this, SIGNAL( selectionChanged( QListViewItem * ) ),
           this, SLOT( selectItem( QListViewItem * ) ) );
}

NavView::~NavView(){
}

void NavView::buildTree()
{
  const QObjectList *roots = QObject::objectTrees();
  QObjectListIt it( *roots );

  QObject * obj;
  while ( (obj = it.current()) != 0 ) {
    ++it;
    NavViewItem *item = new NavViewItem( this, obj );
    createSubTree( item );
  }
}

void NavView::expandVisibleTree()
{
  QListViewItemIterator it( this, QListViewItemIterator::Visible |
                                  QListViewItemIterator::Expandable );

  while ( it.current() ) {
    setOpen( *it, true );
    ++it;
  }
}

void NavView::selectItem( QListViewItem *item )
{
  NavViewItem *navItem = static_cast<NavViewItem*>( item );

  emit selected( navItem->data );
}

void NavView::createSubTree( NavViewItem *parent )
{
  const QObjectList *kids = parent->data->children();
  if ( !kids )
    return;

  QObject * obj;
  QObjectListIt it( *kids );
  while ( (obj=it.current()) != 0 ) {
    ++it;
    NavViewItem *item = new NavViewItem( parent, obj );
    createSubTree( item );
  }
}

#include "navview.moc"
