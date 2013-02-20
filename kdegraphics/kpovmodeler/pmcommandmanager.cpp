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


#include "pmcommandmanager.h"
#include "pmpart.h"

PMCommandManager::PMCommandManager( PMPart* thePart )
{
   m_commands.setAutoDelete( true );
   m_redoCommands.setAutoDelete( true );
   m_maxUndoRedo = 50;
   m_pPart = thePart;
}

PMCommandManager::~PMCommandManager( )
{
   m_commands.clear( );
   m_redoCommands.clear( );
}

void PMCommandManager::execute( PMCommand* cmd )
{
   // some commands (like control point commands) can be executed
   // multiple times.
   cmd->execute( this );
   
   if( m_commands.last( ) != cmd )
      m_commands.append( cmd );
   while( m_commands.count( ) > m_maxUndoRedo )
      m_commands.removeFirst( );
   m_redoCommands.clear( );

   emit updateUndoRedo( cmd->text( ), QString::null );
}

void PMCommandManager::undo( )
{
   if( !m_commands.isEmpty( ) )
   {
      PMCommand* last = m_commands.last( );
      last->undo( this );
      
      m_redoCommands.append( m_commands.take( ) );

      if( m_commands.isEmpty( ) )
         emit updateUndoRedo( QString::null, last->text( ) );
      else
         emit updateUndoRedo( m_commands.last( )->text( ), last->text( ) );
   }
}

void PMCommandManager::redo( )
{
   if( !m_redoCommands.isEmpty( ) )
   {
      PMCommand* last = m_redoCommands.last( );
      last->execute( this );
      
      m_commands.append( m_redoCommands.take( ) );
      
      if( m_redoCommands.isEmpty( ) )
         emit updateUndoRedo( last->text( ), QString::null );
      else
         emit updateUndoRedo( last->text( ), m_redoCommands.last( )->text( ) );
   }
}

void PMCommandManager::clear( )
{
   m_redoCommands.clear( );
   m_commands.clear( );
   emit updateUndoRedo( QString::null, QString::null );
}

void PMCommandManager::cmdObjectChanged( PMObject* obj, const int mode )
{
   emit objectChanged( obj, mode, this );
}

void PMCommandManager::cmdIDChanged( PMObject* obj, const QString& oldID )
{
   emit idChanged( obj, oldID );
}
#include "pmcommandmanager.moc"
