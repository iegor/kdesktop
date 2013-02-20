/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2002 by Andreas Zehender
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

#include "pmobjectselect.h"
#include "pmfactory.h"
#include <klocale.h>
#include <kiconloader.h>

PMListBoxObject::PMListBoxObject( QListBox* listbox, PMObject* obj,
                                  QListBoxItem* after )
      : QListBoxPixmap( listbox,
                      SmallIcon( obj->pixmap( ), PMFactory::instance( ) ),
                      checkName( obj->name( ) ), after )
{
   m_pObject = obj;
}

PMListBoxObject::PMListBoxObject( QListBox* listbox, PMObject* obj )
      : QListBoxPixmap( listbox,
                      SmallIcon( obj->pixmap( ), PMFactory::instance( ) ),
                      checkName( obj->name( ) ) )
{
   m_pObject = obj;
}

PMListBoxObject::PMListBoxObject( PMObject* obj )
      : QListBoxPixmap( SmallIcon( obj->pixmap( ), PMFactory::instance( ) ),
                      checkName( obj->name( ) ) )
{
   m_pObject = obj;
}

PMListBoxObject::PMListBoxObject( QListBox* listbox, PMObject* obj,
                                  const QString& text, QListBoxItem* after )
      : QListBoxPixmap( listbox,
                      SmallIcon( obj->pixmap( ), PMFactory::instance( ) ),
                      text, after )
{
   m_pObject = obj;
}

PMListBoxObject::PMListBoxObject( QListBox* listbox, PMObject* obj,
                                  const QString& text )
      : QListBoxPixmap( listbox,
                      SmallIcon( obj->pixmap( ), PMFactory::instance( ) ),
                      text )
{
   m_pObject = obj;
}

PMListBoxObject::PMListBoxObject( PMObject* obj, const QString& text )
      : QListBoxPixmap( SmallIcon( obj->pixmap( ), PMFactory::instance( ) ),
                      text )
{
   m_pObject = obj;
}

QString PMListBoxObject::checkName( const QString& text )
{
   if( text.isEmpty( ) )
      return i18n( "(unnamed)" );
   return text;
}

PMListBoxObject::~PMListBoxObject( )
{
}


QSize PMObjectSelect::s_size = QSize( 200, 300 );


PMObjectSelect::PMObjectSelect( QWidget* parent, const char* name, bool modal )
      : KDialogBase( parent, name, modal, i18n( "Choose Object" ), Ok | Cancel )
{
   m_pSelectedObject = 0;
   m_pListBox = new QListBox( this );
   setMainWidget( m_pListBox );
   setInitialSize( s_size );

   connect( m_pListBox, SIGNAL( highlighted( QListBoxItem* ) ),
            SLOT( slotHighlighted( QListBoxItem* ) ) );
   connect( m_pListBox, SIGNAL( selected( QListBoxItem* ) ),
            SLOT( slotSelected( QListBoxItem* ) ) );
   enableButtonOK( false );
}

PMObjectSelect::~PMObjectSelect( )
{
   s_size = size( );
}

void PMObjectSelect::addObject( PMObject* obj )
{
   m_pListBox->insertItem( new PMListBoxObject( obj ) );
}

int PMObjectSelect::selectObject( PMObject* link, const QString& t,
                                  PMObject* & obj, QWidget* parent )
{
   PMObject* last = link;
   PMObject* scene;
   bool stop = false;
   bool found = false;
   
   do
   {
      scene = last->parent( );
      if( scene )
      {
         if( scene->type( ) == "Scene" )
         {
            last = last->prevSibling( );
            stop = true;
            found = true;
         }
         else
            last = last->parent( );
      }
      else
         stop = true;
   }
   while( !stop );
   
   if( found )
   {
      PMObjectSelect s( parent );

      PMObject* o = scene->firstChild( );
      bool l = false;
      
      while( o && !l && last )
      {
         if( o->type( ) == t )
            s.m_pListBox->insertItem( new PMListBoxObject( o ) );
         
         if( o == last )
            l = true;
         else
            o = o->nextSibling( );
      }
   
      int result = s.exec( );
      if( result == Accepted )
         obj = s.selectedObject( );
      
      return result;
   }
   else
      kdError( PMArea ) << "PMObjectSelect: Link does not seem to be correctly inserted in the scene.\n";
   return Rejected;
}

int PMObjectSelect::selectObject( PMObject* link,
                                  const QStringList& t,
                                  PMObject* & obj, QWidget* parent )
{
   PMObject* last = link;
   PMObject* scene;
   bool stop = false;
   bool found = false;
   
   do
   {
      scene = last->parent( );
      if( scene )
      {
         if( scene->type( ) == "Scene" )
         {
            last = last->prevSibling( );
            stop = true;
            found = true;
         }
         else
            last = last->parent( );
      }
      else
         stop = true;
   }
   while( !stop );
   
   if( found )
   {
      PMObjectSelect s( parent );

      PMObject* o = scene->firstChild( );
      bool l = false;
      
      while( o && !l && last )
      {
         if( t.findIndex( o->type( ) ) >= 0 )
            s.m_pListBox->insertItem( new PMListBoxObject( o ) );
         
         if( o == last )
            l = true;
         else
            o = o->nextSibling( );
      }
   
      int result = s.exec( );
      if( result == Accepted )
         obj = s.selectedObject( );
      
      return result;
   }
   else
      kdError( PMArea ) << "PMObjectSelect: Link does not seem to be correctly inserted in the scene.\n";
   return Rejected;
}

int PMObjectSelect::selectDeclare( PMObject* link, const QString& declareType,
                                   PMObject* & obj, QWidget* parent )
{
   PMObject* last = link;
   PMObject* scene;
   bool stop = false;
   bool found = false;
   
   do
   {
      scene = last->parent( );
      if( scene )
      {
         if( scene->type( ) == "Scene" )
         {
            last = last->prevSibling( );
            stop = true;
            found = true;
         }
         else
            last = last->parent( );
      }
      else
         stop = true;
   }
   while( !stop );
   
   if( found )
   {
      PMObjectSelect s( parent );

      PMObject* o = scene->firstChild( );
      PMDeclare* decl;
      
      bool l = false;
      
      while( o && !l && last )
      {
         if( o->type( ) == "Declare" )
         {
            decl = ( PMDeclare* ) o;
            if( decl->declareType( ) == declareType )
               s.m_pListBox->insertItem( new PMListBoxObject( o ) );
         }
         
         if( o == last )
            l = true;
         else
            o = o->nextSibling( );
      }
   
      int result = s.exec( );
      if( result == Accepted )
         obj = s.selectedObject( );
      
      return result;
   }
   else
      kdError( PMArea ) << "PMObjectSelect: Link does not seem to be correctly inserted in the scene.\n";
   return Rejected;
}

int PMObjectSelect::selectDeclare( PMObject* link, const QStringList& declareTypes,
                                   PMObject* & obj, QWidget* parent )
{
   PMObject* last = link;
   PMObject* scene;
   bool stop = false;
   bool found = false;
   
   do
   {
      scene = last->parent( );
      if( scene )
      {
         if( scene->type( ) == "Scene" )
         {
            last = last->prevSibling( );
            stop = true;
            found = true;
         }
         else
            last = last->parent( );
      }
      else
         stop = true;
   }
   while( !stop );
   
   if( found )
   {
      PMObjectSelect s( parent );

      PMObject* o = scene->firstChild( );
      PMDeclare* decl;
      
      bool l = false;
      
      while( o && !l && last )
      {
         if( o->type( ) == "Declare" )
         {
            decl = ( PMDeclare* ) o;
            if( declareTypes.findIndex( decl->declareType( ) ) >= 0 )
               s.m_pListBox->insertItem( new PMListBoxObject( o ) );
         }
         
         if( o == last )
            l = true;
         else
            o = o->nextSibling( );
      }
   
      int result = s.exec( );
      if( result == Accepted )
         obj = s.selectedObject( );
      
      return result;
   }
   else
      kdError( PMArea ) << "PMObjectSelect: Link does not seem to be correctly inserted in the scene.\n";
   return Rejected;
}

void PMObjectSelect::slotHighlighted( QListBoxItem* lbi )
{
   m_pSelectedObject = ( ( PMListBoxObject* ) lbi )->object( );
   enableButtonOK( true );
}

void PMObjectSelect::slotSelected( QListBoxItem* lbi )
{
   m_pSelectedObject = ( ( PMListBoxObject* ) lbi )->object( );
   enableButtonOK( true );
   accept( );
}
#include "pmobjectselect.moc"
