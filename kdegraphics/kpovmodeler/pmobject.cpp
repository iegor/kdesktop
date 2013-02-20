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


#include "pmobject.h"
#include "pmcontrolpoint.h"
#include "pmdialogeditbase.h"
#include "pmmemento.h"
#include "pmprototypemanager.h"
#include "pminsertrulesystem.h"
#include "pmpart.h"

PMDefinePropertyClass( PMObject, PMObjectProperty );

PMMetaObject* PMObject::s_pMetaObject = 0;

PMObject::PMObject( PMPart* part )
{
   m_pParent = 0;
   m_selected = false;
   m_pPrevSibling = 0;
   m_pNextSibling = 0;
   m_pMemento = 0;
   m_readOnly = false;
   m_pPart = part;

   if( !m_pPart )
      kdError( PMArea ) << "PMObject::PMObject: The part may not be null" << endl;
}

PMObject::PMObject( const PMObject& o )
{
   m_pParent = 0;
   m_selected = false;
   m_pPrevSibling = 0;
   m_pNextSibling = 0;
   m_pMemento = 0;
   m_readOnly = false;
   m_pPart = o.m_pPart;
}

PMObject::~PMObject( )
{
}

PMMetaObject* PMObject::metaObject( ) const
{
   if( !s_pMetaObject )
   {
      s_pMetaObject = new PMMetaObject( "Object", 0 );
      s_pMetaObject->addProperty(
         new PMObjectProperty( "readOnly", &PMObject::setReadOnly, &PMObject::isReadOnly ) );
      s_pMetaObject->addProperty(
         new PMObjectProperty( "numberOfChildren", 0, &PMObject::countChildren ) );
   }
   return s_pMetaObject;
}

PMObject* PMObject::newObject( ) const
{
   return metaObject( )->newObject( m_pPart );
}

bool PMObject::insertChildAfter( PMObject*, PMObject* )
{
   kdError( PMArea ) << "Tried to insert object into a non composite object" << "\n";
   return false;
}

bool PMObject::insertChildBefore( PMObject*, PMObject* )
{
   kdError( PMArea ) << "Tried to insert object into a non composite object" << "\n";
   return false;
}

void PMObject::setSelected( bool s )
{
   if( m_selected != s )
   {
      if( s )
      {
         if( isSelectable( ) )
         {
            m_selected = true;
            if( m_pParent )
               m_pParent->adjustSelectedChildren( 1 );
         }
      }
      else
      {
         m_selected = false;
         if( m_pParent )
            m_pParent->adjustSelectedChildren( -1 );
      }
   }
}

bool PMObject::isSelectable( )
{
   bool is = true;
   PMObject* o;

   for( o = m_pParent; o && is; o = o->m_pParent )
      if( o->isSelected( ) )
         is = false;

   return is;
}

bool PMObject::isReadOnly( ) const
{
   if( m_readOnly )
      return true;
   if( m_pParent )
      return m_pParent->isReadOnly( );
   return false;
}

bool PMObject::takeChild( PMObject* )
{
   kdError( PMArea ) << "Tried to remove object out of a non composite object" << "\n";
   return false;
}

bool PMObject::takeChild( uint )
{
   kdError( PMArea ) << "Tried to remove object out of a non composite object" << "\n";
   return false;
}

PMDialogEditBase* PMObject::editWidget( QWidget* parent ) const
{
   return new PMDialogEditBase( parent );
//   return 0;
}

void PMObject::createMemento( )
{
   if( m_pMemento )
      delete m_pMemento;
   m_pMemento = new PMMemento( this );
}

PMMemento* PMObject::takeMemento( )
{
   PMMemento* tmp = m_pMemento;
   m_pMemento = 0;
   return tmp;
}

void PMObject::restoreMemento( PMMemento* /* s */ )
{
   // nothing to be done at the moment
}

PMMatrix PMObject::transformedWith( ) const
{
   PMMatrix result = PMMatrix::identity( );
   const PMObject* o = this;

   if( o->firstChild( ) )
      o = o->firstChild( );
   else if( o->nextSibling( ) )
      o = o->nextSibling( );
   else
      o = o->parent( );

   while( o )
   {
      if( o->hasTransformationMatrix( ) )
         result = o->transformationMatrix( ) * result;

      if( o->nextSibling( ) )
         o = o->nextSibling( );
      else
         o = o->parent( );
   }

   return result;
}

QDomElement PMObject::serialize( QDomDocument& doc ) const
{
   QDomElement e = doc.createElement( className( ).lower( ) );
   serialize( e, doc );
   return e;
}

void PMObject::readAttributes( const PMXMLHelper& )
{
}

void PMObject::cleanUp( ) const
{
   if( s_pMetaObject )
   {
      delete s_pMetaObject;
      s_pMetaObject = 0;
   }
}

bool PMObject::setProperty( const QString& name, const PMVariant& v )
{
   PMPropertyBase* p = metaObject( )->property( name );
   if( !p )
      return false;
   return p->setProperty( this, v );
}

QStringList PMObject::properties( ) const
{
   QStringList lst;
   PMPropertyIterator it = metaObject( )->properties( );
   
   for( ; it.current( ); ++it )
      lst.append( it.current( )->name( ) );

   return lst;
}

PMVariant PMObject::property( const QString& name ) const
{
   PMPropertyBase* p = metaObject( )->property( name );
   if( !p )
      return PMVariant( );
   return p->getProperty( this );
}

bool PMObject::isA( const QString& className ) const
{
   if( !m_pPart )
      return false;
   return m_pPart->prototypeManager( )->isA( metaObject( ), className );
}

QString PMObject::type( ) const
{
   return metaObject( )->className( );
}

bool PMObject::canInsert( const QString& className, const PMObject* after,
                          const PMObjectList* objectsBetween ) const
{
   if( !m_pPart )
      return false;
   return m_pPart->insertRuleSystem( )->canInsert( this, className, after, objectsBetween );
}

bool PMObject::canInsert( const PMObject* o, const PMObject* after,
                          const PMObjectList* objectsBetween ) const
{
   if( !m_pPart )
      return false;
   return m_pPart->insertRuleSystem( )->canInsert( this, o, after, objectsBetween );
}

int PMObject::canInsert( const PMObjectList& list, const PMObject* after ) const
{
   if( !m_pPart )
      return false;
   return m_pPart->insertRuleSystem( )->canInsert( this, list, after );
}

int PMObject::canInsert( const QStringList& classes, const PMObject* after ) const
{
   if( !m_pPart )
      return false;
   return m_pPart->insertRuleSystem( )->canInsert( this, classes, after );
}
