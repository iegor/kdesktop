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


#include "pmmovecommand.h"
#include "pmobject.h"
#include "pmpart.h"
#include "pmcommandmanager.h"
#include "pmdeclare.h"
#include "pmerrorflags.h"
#include "pmrecursiveobjectiterator.h"
#include "pmmemento.h"
#include "pmdebug.h"

#include <klocale.h>
#include <qptrdict.h>

PMMoveCommand::PMMoveCommand( PMObject* obj, PMObject* parent, PMObject* after )
      : PMCommand( i18n( "Move %1" ).arg( obj->description( ) ) )
{
   m_pParent = parent;
   m_pAfter = after;

   if( obj->parent( ) )
   {
      m_infoList.append( new PMDeleteInfo( obj ) );
   }
   else
   {
      // object has no parent!
      // top level objects can't be moved, move all child items
      PMObject* tmp;
      for( tmp = obj->firstChild( ); tmp; tmp = tmp->nextSibling( ) )
         m_infoList.append( new PMDeleteInfo( tmp ) );
   }

   m_executed = false;
   m_firstExecution = true;
}

PMMoveCommand::PMMoveCommand( const PMObjectList& list, PMObject* parent,
                            PMObject* after )
      : PMCommand( i18n( "Move Objects" ) )
{
   PMObjectListIterator it( list );

   for( ; it.current( ); ++it )
   {
      if( it.current( )->parent( ) )
         m_infoList.append( new PMDeleteInfo( it.current( ) ) );
      else
      {
         // object has no parent!
         // top level objects can't be moved, move all child items
         PMObject* tmp;
         for( tmp = it.current( )->firstChild( ); tmp; tmp = tmp->nextSibling( ) )
            m_infoList.append( new PMDeleteInfo( tmp ) );
      }
   }  

   m_pParent = parent;
   m_pAfter = after;
   m_executed = false;
   m_firstExecution = true;
}

PMMoveCommand::~PMMoveCommand( )
{
   if( m_executed )
   {
      m_insertErrors.setAutoDelete( true );
      m_insertErrors.clear( );
   }
}

void PMMoveCommand::execute( PMCommandManager* theManager )
{
   if( !m_executed )
   {
      // remove objects
      PMDeleteInfoListIterator it( m_infoList );
      PMObject* prev = m_pAfter;
      PMObject* obj;
      bool error = false;
      PMDeleteInfo* info = 0;
      PMObject* parent;
   
      for( it.toLast( ); it.current( ); --it )
      {
         info = it.current( );
         parent = info->parent( );
         // signal has to be emitted before the item is removed
         theManager->cmdObjectChanged( info->deletedObject( ), PMCRemove );
         if( m_firstExecution )
            if( parent->dataChangeOnInsertRemove( )
                && !parent->mementoCreated( ) )
               parent->createMemento( );
         parent->takeChild( info->deletedObject( ) );
      }

      // insert at new position
      if( m_firstExecution )
         if( m_pParent->dataChangeOnInsertRemove( )
             && !m_pParent->mementoCreated( ) )
            m_pParent->createMemento( );
               
      for( it.toFirst( ); it.current( ); ++it )
      {
         if( !it.current( )->insertError( ) )
         {
            obj = it.current( )->deletedObject( );

            if( !prev )
            {
               if( m_pParent->canInsert( obj, 0 ) )
               {
                  m_pParent->insertChild( obj, 0 );
                  prev = obj;
                  theManager->cmdObjectChanged( obj, PMCAdd );
               }
               else
                  error = true;
            }
            else
            {
               if( m_pParent->canInsert( obj, prev ) )
               {
                  m_pParent->insertChildAfter( obj, prev );
                  prev = obj;
                  theManager->cmdObjectChanged( obj, PMCAdd );
               }
               else
                  error = true;
            }
            
            if( error )
            {
               m_insertErrors.append( it.current( )->deletedObject( ) );
               it.current( )->setInsertError( );
               theManager->cmdObjectChanged( obj, PMCAdd | PMCInsertError );
               error = false;
            }
         }
      }

      if( m_firstExecution )
      {
         if( m_pParent->mementoCreated( ) )
            m_dataChanges.append( m_pParent->takeMemento( ) );
         
         for( it.toLast( ); it.current( ); --it )
         {
            info = it.current( );
            parent = info->parent( );

            if( parent->mementoCreated( ) )
               m_dataChanges.append( parent->takeMemento( ) );
         }
      }

      QPtrListIterator<PMMemento> mit( m_dataChanges );
      for( ; mit.current( ); ++mit )
      {
         PMObjectChangeListIterator change = mit.current( )->changedObjects( );
         for( ; change.current( ); ++change )
            theManager->cmdObjectChanged( change.current( )->object( ),
                                          change.current( )->mode( ) );
      }
      
      m_executed = true;
      m_firstExecution = true;
   }
}

void PMMoveCommand::undo( PMCommandManager* theManager )
{
   if( m_executed )
   {
      PMDeleteInfoListIterator it( m_infoList );
      PMObject* obj;
      for( it.toLast( ) ; it.current( ); --it )
      {
         // signal has to be emitted before the item is removed
         obj = it.current( )->deletedObject( );
         theManager->cmdObjectChanged( obj, PMCRemove );
         if( obj->parent( ) )
            obj->parent( )->takeChild( obj );
      }
      
      for( it.toFirst( ); it.current( ); ++it )
      {
         obj = it.current( )->deletedObject( );
         if( it.current( )->prevSibling( ) )
            it.current( )->parent( )
               ->insertChildAfter( obj, it.current( )->prevSibling( ) );
         else
            it.current( )->parent( )->insertChild( obj, 0 );
         theManager->cmdObjectChanged( obj, PMCAdd );
      }

      QPtrListIterator<PMMemento> mit( m_dataChanges );
      for( ; mit.current( ); ++mit )
      {
         mit.current( )->originator( )->restoreMemento( mit.current( ) );
         PMObjectChangeListIterator change = mit.current( )->changedObjects( );
         for( ; change.current( ); ++change )
            theManager->cmdObjectChanged( change.current( )->object( ),
                                          change.current( )->mode( ) );
      }

      m_executed = false;
   }
}

int PMMoveCommand::errorFlags( PMPart* )
{
   PMDeleteInfo* info;
   PMDeclare* decl = 0;
   PMObject* obj;
   bool insideSelection;
   bool ok = true;
   bool declareInsertError = false;
   bool error = false;
   bool stop;

   // dictionary of deleted objects
   QPtrDict<bool> deletedObjects( 1009 );
   deletedObjects.setAutoDelete( true );
   QPtrDict<bool> objectsAfterInsertPosition( 1009 );
   objectsAfterInsertPosition.setAutoDelete( true );
   QPtrDict<bool> declaresBeforeInsertPosition( 199 );
   declaresBeforeInsertPosition.setAutoDelete( true );


   PMDeleteInfoListIterator it( m_infoList );
   for( ; it.current( ); ++it )
      deletedObjects.insert( it.current( )->deletedObject( ),
                               new bool( true ) );

   // declares can only be moved, if all linked
   // objects are moved as well or the insert position is before the first
   // linked object and all declares can be inserted

   info = m_infoList.last( );
   
   while( info )
   {
      ok = true;
      declareInsertError = false;
      
      if( info->deletedObject( )->isA( "Declare" ) )
      {
         decl = ( PMDeclare* ) ( info->deletedObject( ) );
         
         if( !m_pParent->canInsert( decl, m_pAfter ) )
         {
            declareInsertError = true;
            ok = false;
         }
         else
         {
            PMObjectListIterator links = decl->linkedObjects( );
            
            for( ; links.current( ) && ok; ++links )
            {
               insideSelection = false;
               for( obj = links.current( ); obj && !insideSelection;
                    obj = obj->parent( ) )
               {
                  if( deletedObjects.find( obj ) )
                     insideSelection = true;
               }

               if( insideSelection )
               {
                  bool stop = false;
                  for( obj = links.current( ); obj && !stop; obj = obj->parent( ) )
                  {
                     if( deletedObjects.find( obj ) )
                        stop = true;
                     else
                        deletedObjects.insert( obj, new bool( true ) );
                  }
               }
               else
               {                  
                  // link will not be moved
                  // check, if after insert position
                  // declares can only be inserted as childs of the scene

                  if( m_pAfter )
                  {
                     // insert point is not the first item
                     // find the top level parent item
                     stop = false;
                     obj = links.current( );
                     do
                     {
                        if( obj->parent( ) )
                        {
                           if( obj->parent( )->type( ) == "Scene" )
                              stop = true;
                           else
                              obj = obj->parent( );
                        }
                        else
                           stop = true; // error
                     }
                     while( !stop );

                     PMObject* topParent = obj;

                     // check if insert point is before the top level
                     // parent object
                     
                     if( !objectsAfterInsertPosition.find( obj ) )
                     {
                        stop = false;
                        obj = obj->prevSibling( );
                        while( obj && !stop )
                        {
                           if( obj == m_pAfter )
                              stop = true;
                           else if( objectsAfterInsertPosition.find( obj ) )
                              stop = true;
                           else
                              obj = obj->prevSibling( );
                        }
                        
                        if( stop )
                           objectsAfterInsertPosition.insert( topParent, new bool( true ) );
                        else
                           ok = false;
                     }
                  }
               }
            }
         }
      }
      
      if( !ok )
      {
         if( declareInsertError )
            m_errors.prepend( i18n( "Can't insert the declare \"%1\" at that point." )
                              .arg( decl->id( ) ) );
         else
            m_errors.prepend( i18n( "The declare \"%1\" can't be moved behind linked objects." )
                              .arg( decl->id( ) ) );
         
         PMDeleteInfo* tmp = info;
         info = m_infoList.prev( );
         deletedObjects.remove( tmp->deletedObject( ) );
         m_infoList.removeRef( tmp );
         
         error = true;
      }
      else
         info = m_infoList.prev( );
   }

   // check if links are moved before the declare object

   // find all declares bevore the insert position
   if( m_pParent->type( ) == "Scene" )
      obj = m_pAfter;
   else
   {
      stop = false;
      obj = m_pParent;
      do
      {
         if( obj->parent( ) )
         {
            if( obj->parent( )->type( ) == "Scene" )
               stop = true;
            else
               obj = obj->parent( );
         }
         else
            stop = true; // error
      }
      while( !stop );
      
      if( obj )
         obj = obj->prevSibling( );
   }
   
   while( obj )
   {
      if( obj->isA( "Declare" ) )
         declaresBeforeInsertPosition.insert( obj, new bool( true ) );
      obj = obj->prevSibling( );
   }
   
   info = m_infoList.first( );
   while( info )
   {
      ok = true;
      PMRecursiveObjectIterator it( info->deletedObject( ) );
      PMObject* linked = 0;
      
      while( it.current( ) && ok )
      {
         linked = it.current( )->linkedObject( );

         if( linked )
         {
            if( !deletedObjects.find( linked ) )
            {
               // Object is linked to a declare and the declare
               // will not be moved.
               // Check, if the insert point is after the declare

               if( !declaresBeforeInsertPosition.find( linked ) )
                  ok = false;
            }
         }

         if( ok )
            ++it;
      }
      
      if( ok )
         info = m_infoList.next( );
      else
      {
         if( linked )
         {
            obj = info->deletedObject( );
            QString name = obj->name( );
            decl = ( PMDeclare* ) linked;
            
            if( name.isEmpty( ) )
               name = i18n( "(unnamed)" );
            m_errors.append( i18n( "The %1 \"%2\" can't be moved because it "
                                   "contains a link to the declare \"%3\" "
                                   "and the insert point is not after "
                                   "the declare." )
                             .arg( obj->description( ) ).arg( name )
                             .arg( decl->id( ) ) );
         }
         else
            kdError( PMArea ) << "linked is 0 in PMMoveCommand::errorFlags\n";
         

         PMDeleteInfo* tmp = info;
         info = m_infoList.next( );
         deletedObjects.remove( tmp->deletedObject( ) );
         m_infoList.removeRef( tmp );
         
         error = true;
      }
   }
   
   if( error )
   {
      if( m_infoList.count( ) == 0 )
         return PMEError | PMEFatal;
      else
         return PMEError;
   }

   return PMENone;
}
