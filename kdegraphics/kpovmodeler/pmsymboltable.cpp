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

#include "pmsymboltable.h"
#include "pmdeclare.h"
#include "pmdebug.h"

#include <string.h>

PMSymbol::PMSymbol( const QString& id, PMDeclare* o )
{
   setId( id );
   m_type = Object;
   m_pObj = o;
   m_pVal = 0;
   m_pRenamedSymbol = 0;
}

PMSymbol::PMSymbol( const QString& id, const PMValue& v )
{
   setId( id );
   m_type = Value;
   m_pObj = 0;
   m_pVal = new PMValue( v );
   m_pRenamedSymbol = 0;
}

PMSymbol::~PMSymbol( )
{
   if( m_pVal )
      delete m_pVal;
}

void PMSymbol::setId( const QString& id )
{
   m_id = id.left( MaxIDLength );
}

PMDeclare* PMSymbol::object( ) const
{
   if( m_type == Object )
      return m_pObj;
   kdError( PMArea ) << "Symbol is not an object\n";
   return 0;
}

PMValue PMSymbol::value( ) const
{
   if( m_type == Value )
      return *m_pVal;
   kdError( PMArea ) << "Symbol is not a value\n";
   return PMValue( );
}


PMSymbolTable::PMSymbolTable( )
      : QDict<PMSymbol>( 1009 ), m_lastID( 47 )
{
   setAutoDelete( true );
   m_lastID.setAutoDelete( true );
}

PMSymbolTable::~PMSymbolTable( )
{
   clear( );
}

QString PMSymbolTable::findNewID( const QString& prefix )
{
   PMSymbol* symbol;
   QString testID;
   unsigned int number;
   
   int* lastNumber = m_lastID.find( prefix );
   if( lastNumber )
      number = *lastNumber + 1;
   else
      number = 0;

   // find next free id
   do
   {
      testID = prefix + QString( "%1" ).arg( number );
      symbol = find( testID );
      if( symbol )
         number++;
   }
   while( symbol );

   if( lastNumber )
      *lastNumber = number;
   else
      m_lastID.insert( prefix, new int( number ) );

   return testID;
}

PMSymbol* PMSymbolTable::findNewID( const QString& prefix, PMDeclare* obj )
{
   QString newID = findNewID( prefix );
   obj->setID( newID );

   PMSymbol* s = new PMSymbol( newID, obj );
//   insert( newID, s );
   
   return s;
}
