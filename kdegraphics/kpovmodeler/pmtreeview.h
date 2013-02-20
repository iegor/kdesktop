//-*-C++-*-
/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2003 by Andreas Zehender
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


#ifndef PMTREEVIEW_H
#define PMTREEVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qlistview.h>
#include "pmobject.h"
#include "pmviewbase.h"
#include "pmviewfactory.h"
 
class PMTreeViewItem;
class PMPart;

/**
 * Wrapper class for the treeview/dock widget
 */
class PMTreeViewWidget : public PMViewBase
{
public:
   /**
    * Default constructor
    */
   PMTreeViewWidget( PMPart* pare, QWidget* parent, const char* name = 0 );
   
   /** */
   virtual QString viewType( ) const { return QString( "treeview" ); }
   /** */
   virtual QString description( ) const;
};

/**
 * Widget that displays the scene as tree view
 */
class PMTreeView : public QListView
{
   Q_OBJECT
   friend class PMTreeViewItem;
public:
   /**
    * Creates a PMTreeView with parent and name that displays the
    * document doc
    */
   PMTreeView( PMPart* part, QWidget* parent = 0, const char* name = 0 );
   /**
    * Deletes the PMTreeView
    */
   ~PMTreeView( );

   /**
    * Returns true PMTreeViewItem::setSelected should be accepted
    */
   bool acceptSelect( ) const { return m_acceptSelect; }
   /**
    * Returns the connected part
    */
   PMPart* part( ) const { return m_pPart; }
   
public slots:
   /**
    * Called when an object is changed.
    * @see PMPart::objectChanged( ) */
   void slotObjectChanged( PMObject* obj, const int mode, QObject* sender );
   /**
    * Refreshes the whole csg tree
    */
   void slotRefresh( );
   /**
    * Clears all data
    */
   void slotClear( );
   
signals:
   /**
    * Emitted, when an object is selected or deselected
    */
   void objectChanged( PMObject* obj, const int mode, QObject* sender );
   /**
    * Emitted in the destructor
    */
   void destroyed( PMTreeView* v );
   
protected:
   void contentsMousePressEvent( QMouseEvent * e );
   void contentsMouseMoveEvent( QMouseEvent * e );
   void itemSelected( PMTreeViewItem* item, bool selected );
   
   void viewportMousePressEvent( QMouseEvent * e );
   void viewportMouseReleaseEvent( QMouseEvent* e );
   void viewportMouseMoveEvent( QMouseEvent* e );
   
   void viewportDragMoveEvent( QDragMoveEvent *e );
   void viewportDragEnterEvent( QDragEnterEvent *e );
   void viewportDragLeaveEvent( QDragLeaveEvent* e );
   void viewportDropEvent( QDropEvent* e );

   void focusOutEvent( QFocusEvent* e );
   void focusInEvent( QFocusEvent* e );
   
   void keyPressEvent( QKeyEvent* e );
   
private:
   /**
    * Adds child items of item to the tree view
    */
   void addChildItems( PMTreeViewItem* item );
   /**
    * Returns the ListViewItem connected with the PMObject obj
    */
   PMTreeViewItem* findObject( const PMObject* obj );
   /**
    * Selects the item. Expands the tree if necessary
    */
   void selectItem( QListViewItem* item );
   /**
    * Returns true if the drop target is a tree view for the same part
    */
   bool targetDisplaysPart( QWidget* target );
   
   /**
    * the displayed document
    */
   PMPart* m_pPart;
   
   /**
    * the selected items
    */
//   QPtrList<PMTreeViewItem> m_selectedItems;
   PMTreeViewItem* m_pLastSelected;
   bool m_itemSelected;
   bool m_itemDeselected;
   bool m_selectionCleared;
   bool m_event;
   bool m_acceptSelect;
   bool m_selectOnReleaseEvent;
   
   PMTreeViewItem* m_pDragOverItem;
//   QStringList m_lstDropFormats;

   // for drag and drop, copied from KonqBaseListViewWidget
   bool m_pressed;
   QPoint m_pressedPos;
   PMTreeViewItem* m_pressedItem;
};

/**
 * Factory class for the tree view
 */
class PMTreeViewFactory : public PMViewTypeFactory
{
public:
   PMTreeViewFactory( ) { }
   virtual QString viewType( ) const { return QString( "treeview" ); }
   virtual QString description( ) const;
   virtual QString iconName( ) const { return QString( "pmtreeview" ); }
   virtual PMViewBase* newInstance( QWidget* parent, PMPart* part ) const
   {
      return new PMTreeViewWidget( part, parent );
   }
};

#endif
