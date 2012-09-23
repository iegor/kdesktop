/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2002 by Andreas Zehender
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


#include "pmsqeedit.h"
#include "pmsqe.h"
#include "pmlineedits.h"

#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>

PMSuperquadricEllipsoidEdit::PMSuperquadricEllipsoidEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMSuperquadricEllipsoidEdit::createTopWidgets( )
{
   Base::createTopWidgets( );
   
   m_pValueE = new PMFloatEdit( this );
   m_pValueE->setValidation( true, 0.01, false, 1.0 );
   m_pValueN = new PMFloatEdit( this );
   m_pValueN->setValidation( true, 0.01, false, 1.0 );
   
   topLayout( )->addWidget( new QLabel( i18n( "Exponents:" ), this ) );
   
   QHBoxLayout* hl = new QHBoxLayout( topLayout( ) );
   QGridLayout* gl = new QGridLayout( hl, 2, 2 );
   gl->addWidget( new QLabel( i18n( "East-west:" ), this ), 0, 0 );
   gl->addWidget( m_pValueE, 0, 1 );
   gl->addWidget( new QLabel( i18n( "North-south:" ), this ), 1, 0 );
   gl->addWidget( m_pValueN, 1, 1 );
   hl->addStretch( 1 );

   connect( m_pValueE, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pValueN, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
}

void PMSuperquadricEllipsoidEdit::displayObject( PMObject* o )
{
   if( o->isA( "SuperquadricEllipsoid" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_pDisplayedObject = ( PMSuperquadricEllipsoid* ) o;

      m_pValueE->setValue( m_pDisplayedObject->eastWestExponent( ) );
      m_pValueN->setValue( m_pDisplayedObject->northSouthExponent( ) );

      m_pValueE->setReadOnly( readOnly );
      m_pValueN->setReadOnly( readOnly );
      
      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMSuperquadricEllipsoidEdit: Can't display object\n";
}

void PMSuperquadricEllipsoidEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setEastWestExponent( m_pValueE->value( ) );
      m_pDisplayedObject->setNorthSouthExponent( m_pValueN->value( ) );
   }
}

bool PMSuperquadricEllipsoidEdit::isDataValid( )
{
   if( m_pValueE->isDataValid( ) )
      if( m_pValueN->isDataValid( ) )
         return Base::isDataValid( );
   return false;
}

#include "pmsqeedit.moc"
