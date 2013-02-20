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

#include "pmserializer.h"
#include "pmerrorflags.h"
#include "pmdebug.h"

#include <klocale.h>

unsigned int PMSerializer::s_maxErrors = 30;
unsigned int PMSerializer::s_maxWarnings = 50;


PMSerializer::PMSerializer( QIODevice* dev )
{
   m_pDev = dev;
   m_errors = 0;
   m_warnings = 0;
   m_bFatalError = false;
}

PMSerializer::~PMSerializer( )
{

}

void PMSerializer::serializeList( const PMObjectList& objects )
{
   PMObjectListIterator it( objects );
   for( ; it.current( ); ++it )
      serialize( it.current( ) );
}

int PMSerializer::errorFlags( ) const
{
   int result = 0;
   if( errors( ) )
      result |= PMEError;
   if( warnings( ) )
      result |= PMEWarning;
   if( fatal( ) )
      result |= PMEFatal;
   return result;
}

void PMSerializer::printMessage( const QString& type, const QString& msg )
{
   m_messages += PMMessage( type + ": " + msg );
}

void PMSerializer::printError( const QString& msg )
{
   if( m_errors < s_maxErrors )
   {
      printMessage( i18n( "Error" ), msg );
      m_errors++;
   }
   else if( m_errors == s_maxErrors )
   {
      m_messages += PMMessage( i18n( "Maximum of %1 errors reached." )
                    .arg( s_maxErrors ) );
      m_errors++;
   }
}

void PMSerializer::printWarning( const QString& msg )
{
   if( m_warnings < s_maxWarnings )
   {
      printMessage( i18n( "Warning" ), msg );
      m_warnings++;
   }
   else if( m_warnings == s_maxWarnings )
   {
      m_messages += PMMessage( i18n( "Maximum of %1 warnings reached." )
                    .arg( s_maxWarnings ) );
      m_warnings++;
   }
}
