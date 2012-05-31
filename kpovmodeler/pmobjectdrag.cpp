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


#include "pmobjectdrag.h"
#include "pmobject.h"
#include "pmpart.h"
#include "pmpovrayparser.h"
#include "pmxmlparser.h"
#include "pmxmlhelper.h"
#include "pmdocumentformat.h"
#include "pmiomanager.h"
#include "pmserializer.h"

#include <qbuffer.h>
#include <string.h>

const char* const c_kpmDocumentMimeFormat = "application/x-kpovmodeler";

PMObjectDrag::PMObjectDrag( PMPart* part, PMObject* object, QWidget* dragSource,
                            const char* name )
      : QDragObject( dragSource, name )
{
   QByteArray modelerData;

   QTextStream s2( modelerData, IO_WriteOnly );
   QDomDocument doc( "KPOVMODELER" );
   QDomElement top = doc.createElement( "objects" );
   doc.appendChild( top );
   top.setAttribute( "majorFormat", c_majorDocumentFormat );
   top.setAttribute( "minorFormat", c_minorDocumentFormat );

   if( object->type( ) == "Scene" )
   {
      PMObject* o = object->firstChild( );
      for( ; o; o = o->nextSibling( ) )
      {
         QDomElement e = o->serialize( doc );
         top.appendChild( e );
      }
   }
   else
   {
      QDomElement e = object->serialize( doc );
      top.appendChild( e );
   }
   s2 << doc;

   m_data.push_back( modelerData );
   m_mimeTypes.push_back( c_kpmDocumentMimeFormat );

   const QPtrList<PMIOFormat>& formats = part->ioManager( )->formats( );
   QPtrListIterator<PMIOFormat> it( formats );
   for( ; it.current( ); ++it )
   {
      if( it.current( )->services( ) & PMIOFormat::Export )
      {
         QByteArray data;
         QBuffer buffer( data );
         buffer.open( IO_WriteOnly );

         PMSerializer* ser = it.current( )->newSerializer( &buffer );

         if( ser )
         {
            ser->serialize( object );
            ser->close( );
            delete ser;
            buffer.close( );

            m_data.push_back( data );
            m_mimeTypes.push_back( it.current( )->mimeType( ) );

            kdDebug( PMArea ) << "Added mime type " << it.current( )->mimeType( )
                              << " " << data.size( ) << " bytes" << endl;
         }
         else
            kdError( PMArea ) << "Format claims to support exporting, but doesn't provide a serializer" << endl;

         buffer.close( );
      }
   }
}

PMObjectDrag::PMObjectDrag( PMPart* part, const PMObjectList& objList, QWidget* dragSource,
                            const char* name )
      : QDragObject( dragSource, name )
{
   QByteArray modelerData;

   QTextStream s2( modelerData, IO_WriteOnly );
   QDomDocument doc( "KPOVMODELER" );
   QDomElement top = doc.createElement( "objects" );
   doc.appendChild( top );
   top.setAttribute( "majorFormat", c_majorDocumentFormat );
   top.setAttribute( "minorFormat", c_minorDocumentFormat );

   PMObjectListIterator it( objList );
   for( ; it.current( ); ++it )
   {
      if( it.current( )->type( ) == "Scene" )
      {
         PMObject* o = it.current( )->firstChild( );
         for( ; o; o = o->nextSibling( ) )
         {
            QDomElement e = o->serialize( doc );
            top.appendChild( e );
         }
      }
      else
      {
         QDomElement e = it.current( )->serialize( doc );
         top.appendChild( e );
      }
   }

   s2 << doc;

   m_data.push_back( modelerData );
   m_mimeTypes.push_back( c_kpmDocumentMimeFormat );

   const QPtrList<PMIOFormat>& formats = part->ioManager( )->formats( );
   QPtrListIterator<PMIOFormat> fit( formats );
   for( ; fit.current( ); ++fit )
   {
      if( fit.current( )->services( ) & PMIOFormat::Export )
      {
         QByteArray data;
         QBuffer buffer( data );
         buffer.open( IO_WriteOnly );

         PMSerializer* ser = fit.current( )->newSerializer( &buffer );

         if( ser )
         {
            ser->serializeList( objList );
            ser->close( );
            delete ser;
            buffer.close( );

            m_data.push_back( data );
            m_mimeTypes.push_back( fit.current( )->mimeType( ) );

            kdDebug( PMArea ) << "Added mime type " << fit.current( )->mimeType( )
                              << " " << data.size( ) << " bytes" << endl;
         }
         else
            kdError( PMArea ) << "Format claims to support exporting, but doesn't provide a serializer" << endl;

         buffer.close( );
      }
   }
}

PMObjectDrag::~PMObjectDrag( )
{
}

QByteArray PMObjectDrag::encodedData( const char* format ) const
{
   QValueList<QByteArray>::ConstIterator dit;
   QStringList::ConstIterator sit;

   for( dit = m_data.begin( ), sit = m_mimeTypes.begin( );
        dit != m_data.end( ) && sit != m_mimeTypes.end( ); ++dit, ++sit )
   {
      if( *sit == format )
         return *dit;
   }

   QByteArray empty;
   return empty;
}

const char* PMObjectDrag::format( int i /*=0*/ ) const
{
   if( i >= 0 && i < ( signed ) m_mimeTypes.size( ) )
      return ( *m_mimeTypes.at( i ) ).latin1( );
   return 0;
}

bool PMObjectDrag::canDecode( const QMimeSource* e, PMPart* part )
{
   if( e->provides( c_kpmDocumentMimeFormat ) )
      return true;

   const QPtrList<PMIOFormat>& formats = part->ioManager( )->formats( );
   QPtrListIterator<PMIOFormat> fit( formats );
   for( ; fit.current( ); ++fit )
      if( fit.current( )->services( ) & PMIOFormat::Import &&
          e->provides( fit.current( )->mimeType( ).latin1( ) ) )
         return true;

   return false;
}

PMParser* PMObjectDrag::newParser( const QMimeSource* e, PMPart* part )
{
   if( e->provides( c_kpmDocumentMimeFormat ) )
      return new PMXMLParser( part, e->encodedData( c_kpmDocumentMimeFormat ) );

   const QPtrList<PMIOFormat>& formats = part->ioManager( )->formats( );
   QPtrListIterator<PMIOFormat> fit( formats );
   for( ; fit.current( ); ++fit )
   {
      PMIOFormat* f = fit.current( );
      QString str = f->mimeType( );
      const char* lat = str.latin1( );
      if( f->services( ) & PMIOFormat::Import && e->provides( lat ) )
         return f->newParser( part, e->encodedData( lat ) );
   }

   return 0;
}


