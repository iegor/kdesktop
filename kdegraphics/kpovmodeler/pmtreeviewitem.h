//-*-C++-*-
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


#ifndef PMTREEVIEWITEM_H
#define PMTREEVIEWITEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qlistview.h>
class PMObject;

/**
 * QListViewItem for a @ref PMObject
 *
 * Each PMListViewItem is connected to a PMObject.
 */
class PMTreeViewItem : public QListViewItem
{
public:
   /**
    * Constructs a new top-level list view item in the QListView parent.
    */
   PMTreeViewItem( PMObject* object, QListView* parent );
   /**
    * Constructs a new list view item which is a child of parent and
    * first in the parent's list of children.
    */
   PMTreeViewItem( PMObject* object, QListViewItem* parent );
   /**
    * Constructs a list view item which is a child of parent
    * and is after after in the parent's list of children.
    */
   PMTreeViewItem( PMObject* object, QListView* parent, QListViewItem* after );
   /**
    * Constructs a list view item which is a child of parent
    * and is after after in the parent's list of children.
    */
   PMTreeViewItem( PMObject* object, QListViewItem* parent,
                   QListViewItem* after );
   /**
    * Returns the connected @ref PMObject
    */
   PMObject* object( ) const { return m_pObject; }
   /**
    * Returns a key that can be used for sorting, here the index in the
    * parents list of children
    */
   virtual QString key( int column, bool ascending ) const;
   /**
    * Returns a pointer to the parent item
    */
   PMTreeViewItem* parent( )
   {
      return ( PMTreeViewItem* ) QListViewItem::parent( );
   }
   void setSelected( bool select );
   
   /**
    * Sets the text and pixmap
    */
   void setDescriptions( );
private:
   /**
    * Initializes the selection at creation
    */
   void initSelection( );
   PMObject* m_pObject;
};

#endif
