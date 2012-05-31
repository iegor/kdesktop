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


#include "pmaddcommand.h"
#include "pmcommandmanager.h"
#include "pmpart.h"
#include "pmdeclare.h"
#include "pmerrorflags.h"
#include "pmrecursiveobjectiterator.h"
#include "pmmemento.h"

#include <klocale.h>

PMAddCommand::PMAddCommand( PMObject* obj, PMObject* parent, PMObject* after )
      : PMCommand( i18n( "Add New %1" ).arg( obj->description( ) ) )
{
   m_objects.append( obj );
   m_pParent = parent;
   m_pAfter = after;
   m_executed = false;
   m_firstExecution = true;
   m_linksCreated = false;
   m_pParentChangeMemento = 0;
}

PMAddCommand::PMAddCommand( const PMObjectList& list, PMObject* parent,
                            PMObject* after )
      : PMCommand( i18n( "Add Objects" ) )
{
   m_objects = list;
   m_pParent = parent;
   m_pAfter = after;
   m_executed = false;
   m_firstExecution = true;
   m_linksCreated = false;
   m_pParentChangeMemento = 0;
}

PMAddCommand::~PMAddCommand( )
{
   if( !m_executed )
   {
      m_objects.setAutoDelete( true );
      m_objects.clear( );
   }

   m_insertErrors.setAutoDelete( true );
   m_insertErrors.clear( );
}

void PMAddCommand::execute( PMCommandManager* theManager )
{
   if( !m_executed )
   {
      PMObjectListIterator it( m_objects );
      PMObject* prev = m_pAfter;
      PMObjectList errors;
      PMObject* current;
      bool error = false;

      if( m_firstExecution )
         if( m_pParent->dataChangeOnInsertRemove( ) )
            m_pParent->createMemento( );
      
      for( ; it.current( ); ++it )
      {
         current = it.current( );
         if( !prev )
         {
            if( m_pParent->canInsert( current, 0 ) )
            {
               m_pParent->insertChild( current, 0 );
               prev = current;
               theManager->cmdObjectChanged( current, PMCAdd );
            }
            else
               error = true;
         }
         else
         {
            if( m_pParent->canInsert( current, prev ) )
            {
               m_pParent->insertChildAfter( current, prev );
               prev = current;
               theManager->cmdObjectChanged( current, PMCAdd );
            }
            else
               error = true;
         }
         
         if( error )
         {
            errors.append( current );
            theManager->cmdObjectChanged( current, PMCAdd | PMCInsertError );
            if( current->isA( "Declare" ) )
            {
               // the object, that couldn't be inserted was a declare,
               // remove all links
               PMObjectListIterator links =
                  ( ( PMDeclare* ) current )->linkedObjects( );
               for( ; links.current( ); ++links )
               {
                  PMObject* l = links.current( );
                  if( l->parent( ) )
                     l->parent( )->takeChild( l );
                  else
                     m_objects.removeRef( l );
                  m_insertErrors.append( l );
               }
            }
            if( current->linkedObject( ) )
               current->linkedObject( )->removeLinkedObject( current );
            error = false;
         }
      }

      if( m_pParent->mementoCreated( ) )
         m_pParentChangeMemento = m_pParent->takeMemento( );

      if( m_pParentChangeMemento )
      {
         PMObjectChangeListIterator c = m_pParentChangeMemento->changedObjects( );
         for( ; c.current( ); ++c )
            theManager->cmdObjectChanged( c.current( )->object( ),
                                          c.current( )->mode( ) );
      }

      if( m_linksCreated )
      {
         PMObjectListIterator rit( m_links );
         for( ; rit.current( ); ++rit )
            rit.current( )->linkedObject( )->addLinkedObject( rit.current( ) );
         PMObjectListIterator dit( m_linkedDeclares );
         for( ; dit.current( ); ++dit )
            theManager->cmdObjectChanged( dit.current( ), PMCData );
      }
      
      PMObjectListIterator errorit( errors );
      for( ; errorit.current( ); ++errorit )
      {
         m_objects.removeRef( errorit.current( ) );
         m_insertErrors.append( errorit.current( ) );

         PMRecursiveObjectIterator lit( errorit.current( ) );
         for( ; lit.current( ); ++lit )
            if( lit.current( )->linkedObject( ) )
               lit.current( )->linkedObject( )->removeLinkedObject( lit.current( ) );
      }
      
      m_executed = true;
      m_firstExecution = false;
   }
}

void PMAddCommand::undo( PMCommandManager* theManager )
{
   if( m_executed )
   {
      PMObjectListIterator it( m_objects );
      PMObject* obj;
      PMDeclare* decl;

      if( !m_linksCreated )
      {
         for( ; it.current( ); ++it )
         {
            PMRecursiveObjectIterator lit( it.current( ) );
            for( ; lit.current( ); ++lit )
            {
               decl = lit.current( )->linkedObject( );
               if( decl )
               {
                  m_links.append( lit.current( ) );
                  if( !m_linkedDeclares.containsRef( decl ) )
                     m_linkedDeclares.append( decl );
               }
            }
         }
         m_linksCreated = true;
      }

      PMObjectListIterator rit( m_links );
      for( ; rit.current( ); ++rit )
         rit.current( )->linkedObject( )->removeLinkedObject( rit.current( ) );
      
      for( it.toLast( ) ; it.current( ); --it )
      {
         obj = it.current( );
         // signal has to be emitted before the item is removed
         theManager->cmdObjectChanged( obj, PMCRemove );

         if( obj->parent( ) )
            obj->parent( )->takeChild( obj );
      }

      if( m_pParentChangeMemento )
      {
         m_pParent->restoreMemento( m_pParentChangeMemento );
         PMObjectChangeListIterator c = m_pParentChangeMemento->changedObjects( );
         for( ; c.current( ); ++c )
         {
            theManager->cmdObjectChanged( c.current( )->object( ),
                                          c.current( )->mode( ) );
         }
      }
      
      PMObjectListIterator dit( m_linkedDeclares );
      for( ; dit.current( ); ++dit )
         theManager->cmdObjectChanged( dit.current( ), PMCData );
      
      m_executed = false;
   }
}

int PMAddCommand::errorFlags( PMPart* )
{
   return PMENone;
}
