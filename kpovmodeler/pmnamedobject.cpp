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


#include "pmnamedobject.h"
#include "pmxmlhelper.h"
#include "pmmemento.h"

PMDefinePropertyClass( PMNamedObject, PMNamedObjectProperty );

PMMetaObject* PMNamedObject::s_pMetaObject = 0;

PMNamedObject::PMNamedObject( PMPart* part )
      : Base( part )
{
}

PMNamedObject::PMNamedObject( const PMNamedObject& o )
      : Base( o )
{
   m_name = o.m_name;
}

PMNamedObject::~PMNamedObject( )
{
}

PMMetaObject* PMNamedObject::metaObject( ) const
{
   if( !s_pMetaObject )
   {
      s_pMetaObject = new PMMetaObject( "NamedObject", Base::metaObject( ) );
      s_pMetaObject->addProperty(
         new PMNamedObjectProperty( "name", &PMNamedObject::setName, &PMNamedObject::name ) );
   }
   return s_pMetaObject;
}

void PMNamedObject::cleanUp( ) const
{
   if( s_pMetaObject )
   {
      delete s_pMetaObject;
      s_pMetaObject = 0;
   }
   Base::cleanUp( );
}

void PMNamedObject::serialize( QDomElement& e, QDomDocument& doc ) const
{
   e.setAttribute( "name", m_name );
   Base::serialize( e, doc );
}

void PMNamedObject::readAttributes( const PMXMLHelper& h )
{
   m_name = h.stringAttribute( "name", "" );
   Base::readAttributes( h );
}

void PMNamedObject::setName( const QString& newName )
{
   if( newName != m_name )
   {
      if( m_pMemento )
      {
         m_pMemento->addData( s_pMetaObject, PMNameID, m_name );
         m_pMemento->setDescriptionChanged( );
      }

      m_name = newName;
   }
}

void PMNamedObject::restoreMemento( PMMemento* s )
{
   PMMementoDataIterator it( s );
   PMMementoData* data;

   for( ; it.current( ); ++it )
   {
      data = it.current( );
      if( data->objectType( ) == s_pMetaObject )
      {
         switch( data->valueID( ) )
         {
            case PMNameID:
               setName( data->stringData( ) );
               break;
            default:
               kdError( PMArea ) << "Wrong ID in PMNamedObject::restoreMemento\n";
               break;
         }
      }
   }
   Base::restoreMemento( s );
}
