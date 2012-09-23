/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2001 by Andreas Zehender
    email                : zehender@kde.org
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


#include "pmtreeviewitem.h"
#include "pmobject.h"
#include <kiconloader.h>

#include "pmtreeview.h"
#include "pmfactory.h"
#include "pmtexturemap.h"

PMTreeViewItem::PMTreeViewItem( PMObject* object, QListView* parent )
      : QListViewItem( parent )
{
   m_pObject = object;
   setDescriptions( );
   initSelection( );
}

PMTreeViewItem::PMTreeViewItem( PMObject* object, QListViewItem* parent )
      : QListViewItem( parent )
{
   m_pObject = object;
   setDescriptions( );
   initSelection( );
}

PMTreeViewItem::PMTreeViewItem( PMObject* object, QListView* parent,
                                QListViewItem* after )
      : QListViewItem( parent, after )
{
   m_pObject = object;
   setDescriptions( );
   initSelection( );
}

PMTreeViewItem::PMTreeViewItem( PMObject* object, QListViewItem* parent,
                   QListViewItem* after )
      : QListViewItem( parent, after )
{
   m_pObject = object;
   setDescriptions( );
   initSelection( );
}

void PMTreeViewItem::setDescriptions( )
{
   QString text;
   setPixmap( 0, SmallIcon( m_pObject->pixmap( ), PMFactory::instance( ) ) );
   
   if( m_pObject->canHaveName( ) )
   {
      text = m_pObject->name( );
      if( text.isEmpty( ) )
         text = m_pObject->description( );      
   }
   else
      text = m_pObject->description( );
   
   if( m_pObject->parent( ) )
   {
      if( m_pObject->parent( )->isA( "TextureMapBase" ) )
      {
         PMTextureMapBase* tm = ( PMTextureMapBase* ) m_pObject->parent( );
         if( m_pObject->type( ) == tm->mapType( ) )
            text = QString( "[%1] " ).arg( tm->mapValue( m_pObject ), 4, 'f', 2 ) + text;
      }
   }
   setText( 0, text );
}

QString PMTreeViewItem::key( int, bool ) const
{
   QString result;
   if( m_pObject->parent( ) )
      result.sprintf( "%06i", m_pObject->parent( )->findChild( m_pObject ) );
   else
      result = "000000";
   return result;
}

void PMTreeViewItem::setSelected( bool select )
{
   bool ws = isSelected( );
   PMTreeView* treeview = ( PMTreeView* ) listView( );

   // ignore selections during a move event
   if( treeview->acceptSelect( ) )
   {
      QListViewItem::setSelected( select );

      if( ws != isSelected( ) )
         treeview->itemSelected( this, isSelected( ) );
   }
}

void PMTreeViewItem::initSelection( )
{
   QListViewItem::setSelected( m_pObject->isSelected( ) );
//   if( m_pObject->isSelected( ) )
//      repaint( );
}
