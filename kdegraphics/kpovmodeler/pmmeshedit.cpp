/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2003 by Leon Pennington
    email                : leon@leonscape.co.uk
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


#include "pmmeshedit.h"
#include "pmmesh.h"
#include "pmvectoredit.h"

#include <qlayout.h>
#include <qcheckbox.h>

#include <klocale.h>

PMMeshEdit::PMMeshEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMMeshEdit::createTopWidgets( )
{
   Base::createTopWidgets( );

   QHBoxLayout* layout;
   m_pHierarchy = new QCheckBox( i18n( "Hierarchy" ), this );
   m_pEnableInsideVector = new QCheckBox( i18n( "Inside vector:" ), this );
   m_pInsideVector = new PMVectorEdit( "x", "y", "z", this );
   layout = new QHBoxLayout( topLayout( ) );
   layout->addWidget( m_pHierarchy );
   layout->addStretch( 1 );
   layout = new QHBoxLayout( topLayout( ) );
   layout->addWidget( m_pEnableInsideVector );
   layout->addWidget( m_pInsideVector );
   layout->addStretch( 1 );

   connect( m_pHierarchy, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pEnableInsideVector, SIGNAL( clicked( ) ), SLOT( slotInsideVectorClicked( ) ) );
   connect( m_pInsideVector, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
}

void PMMeshEdit::displayObject( PMObject* o )
{
   if( o->isA( "Mesh" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_pDisplayedObject = ( PMMesh* ) o;

      m_pHierarchy->setChecked( m_pDisplayedObject->hierarchy( ) );
      m_pHierarchy->setEnabled( !readOnly );
      m_pEnableInsideVector->setChecked( m_pDisplayedObject->isInsideVectorEnabled( ) );
      m_pEnableInsideVector->setEnabled( !readOnly );
      m_pInsideVector->setVector( m_pDisplayedObject->insideVector( ) );
      m_pInsideVector->setReadOnly( readOnly );
      slotInsideVectorClicked( );
      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMMeshEdit: Can't display object\n";
}

void PMMeshEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setHierarchy( m_pHierarchy->isChecked( ) );
      m_pDisplayedObject->enableInsideVector( m_pEnableInsideVector->isChecked( ) );
      m_pDisplayedObject->setInsideVector( m_pInsideVector->vector( ) );
   }
}

bool PMMeshEdit::isDataValid( )
{
   return Base::isDataValid( );
}

void PMMeshEdit::slotInsideVectorClicked( )
{
   if( m_pEnableInsideVector->isChecked( ) )
      m_pInsideVector->setEnabled( true );
   else
      m_pInsideVector->setEnabled( false );
   emit dataChanged( );
}

#include "pmmeshedit.moc"
