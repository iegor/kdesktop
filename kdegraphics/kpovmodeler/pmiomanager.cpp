/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2003 by Andreas Zehender
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

#include "pmiomanager.h"
#include "pmdebug.h"
#include "pmpovray31format.h"
#include "pmpovray35format.h"

PMIOFormat::PMIOFormat( )
{
}

PMIOFormat::~PMIOFormat( )
{

}

PMIOManager::PMIOManager( PMPart* part )
{
   m_pPart = part;
   m_formats.setAutoDelete( true );

   addFormat( new PMPovray35Format( ) );
   //addFormat( new PMPovray31Format( ) );
}

PMIOManager::~PMIOManager( )
{

}

void PMIOManager::addFormat( PMIOFormat* format )
{
   if( !format )
      return;
   if( !m_formats.containsRef( format ) )
   {
      if( !m_dict.find( format->name( ) ) )
      {
         m_formats.append( format );
         m_dict.insert( format->name( ), format );
      }
      else
         kdError( PMArea ) << "Format " << format->name( ) << "already registered" << endl;
   }
   else
      kdError( PMArea ) << "Format " << format->name( ) << "already registered" << endl;
}

void PMIOManager::removeFormat( const QString& name )
{
   PMIOFormat* pFormat = format( name );
   if( pFormat )
   {
      m_dict.remove( name );
      m_formats.removeRef( pFormat );
   }
}

PMIOFormat* PMIOManager::format( const QString& name ) const
{
   return m_dict.find( name );
}

PMIOFormat* PMIOManager::formatForMimeType( const QString& mime ) const
{
   QPtrListIterator<PMIOFormat> it( m_formats );
   bool found = false;
   PMIOFormat* pFormat = 0;

   while( it.current( ) && !found )
   {
      pFormat = it.current( );
      if( pFormat->mimeType( ) == mime )
         found = true;
      else
         ++it;
   }
   if( found )
      return pFormat;
   return 0;
}
