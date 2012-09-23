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


#include "pmscale.h"
#include "pmscaleedit.h"
#include "pmscalecontrolpoint.h"

#include "pmxmlhelper.h"
#include "pmmemento.h"

#include <klocale.h>

const PMVector scaleDefault = PMVector( 0, 0, 0 );

PMDefinePropertyClass( PMScale, PMScaleProperty );

PMMetaObject* PMScale::s_pMetaObject = 0;
PMObject* createNewScale( PMPart* part )
{
   return new PMScale( part );
}

PMScale::PMScale( PMPart* part )
      : Base( part )
{
   m_scale = PMVector( 1.0, 1.0, 1.0 );
}

PMScale::PMScale( const PMScale& s )
      : Base( s )
{
   m_scale = s.m_scale;
}

PMScale::~PMScale( )
{
}

QString PMScale::description( ) const
{
   return i18n( "scale" );
}

void PMScale::serialize( QDomElement& e, QDomDocument& /*d*/ ) const
{
   e.setAttribute( "value", m_scale.serializeXML( ) );
}

void PMScale::readAttributes( const PMXMLHelper& h )
{
   m_scale = h.vectorAttribute( "value", scaleDefault );
}

PMMetaObject* PMScale::metaObject( ) const
{
   if( !s_pMetaObject )
   {
      s_pMetaObject = new PMMetaObject( "Scale", Base::metaObject( ),
                                        createNewScale );
      s_pMetaObject->addProperty(
         new PMScaleProperty( "scale", &PMScale::setScale, &PMScale::scale ) );
   }
   return s_pMetaObject;
}

void PMScale::cleanUp( ) const
{
   if( s_pMetaObject )
   {
      delete s_pMetaObject;
      s_pMetaObject = 0;
   }
   Base::cleanUp( );
}

void PMScale::setScale( const PMVector& p )
{
   if( p != m_scale )
   {
      if( m_pMemento )
      {
         m_pMemento->addData( s_pMetaObject, PMScaleID, m_scale );
         m_pMemento->setViewStructureChanged( );
      }
      m_scale = p;
      m_scale.resize( 3 );
   }
}

PMDialogEditBase* PMScale::editWidget( QWidget* parent ) const
{
   return new PMScaleEdit( parent );
}

void PMScale::restoreMemento( PMMemento* s )
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
            case PMScaleID:
               setScale( data->vectorData( ) );
               break;
            default:
               kdError( PMArea ) << "Wrong ID in PMScale::restoreMemento\n";
               break;
         }
      }
   }
   Base::restoreMemento( s );
}

PMMatrix PMScale::transformationMatrix( ) const
{
   return PMMatrix::scale( m_scale[0], m_scale[1], m_scale[2] );
}

void PMScale::controlPoints( PMControlPointList& list )
{
   list.append( new PMScaleControlPoint( m_scale, PMScaleID ) );
}

void PMScale::controlPointsChanged( PMControlPointList& list )
{
   PMControlPoint* p;

   for( p = list.first( ); p; p = list.next( ) )
   {
      if( p->changed( ) )
      {
         switch( p->id( ) )
         {
            case PMScaleID:
               setScale( ( ( PMScaleControlPoint* ) p )->scale( ) );
               break;
            default:
               kdError( PMArea ) << "Wrong ID in PMScale::controlPointsChanged\n";
               break;
         }
      }
   }
}
