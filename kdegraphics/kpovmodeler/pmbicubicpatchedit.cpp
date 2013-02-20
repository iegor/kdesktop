/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2001-2002 by Andreas Zehender
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


#include "pmbicubicpatchedit.h"
#include "pmbicubicpatch.h"
#include "pmvectoredit.h"
#include "pmlineedits.h"
#include "pmvectorlistedit.h"
#include "pmpart.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <klocale.h>

PMBicubicPatchEdit::PMBicubicPatchEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMBicubicPatchEdit::createTopWidgets( )
{
   Base::createTopWidgets( );

   QHBoxLayout* layout;

   m_pType = new QComboBox( false, this );
   m_pType->insertItem( i18n( "Normal (type 0)" ) );
   m_pType->insertItem( i18n( "Preprocessed (type 1)" ) );
   layout = new QHBoxLayout( topLayout( ) );
   layout->addWidget( new QLabel( i18n( "Type:" ), this ) );
   layout->addWidget( m_pType );
   layout->addStretch( 1 );

   m_pUSteps = new PMIntEdit( this );
   m_pUSteps->setValidation( true, 0, false, 0 );
   layout = new QHBoxLayout( topLayout( ) );
   layout->addWidget( new QLabel( i18n( "Steps:" ) + " u", this ) );
   layout->addWidget( m_pUSteps );
   m_pVSteps = new PMIntEdit( this );
   m_pVSteps->setValidation( true, 0, false, 0 );
   layout->addWidget( new QLabel( "v", this ) );
   layout->addWidget( m_pVSteps );

   m_pFlatness = new PMFloatEdit( this );
   m_pFlatness->setValidation( true, 0.0, false, 0.0 );
   layout = new QHBoxLayout( topLayout( ) );
   layout->addWidget( new QLabel( i18n( "Flatness:" ), this ) );
   layout->addWidget( m_pFlatness );
   layout->addStretch( 1 );

   topLayout( )->addWidget( new QLabel( i18n( "Points:" ), this ) );

   m_pPoints = new PMVectorListEdit( "x", "y", "z", this );
   m_pPoints->setSize( 16 );
   topLayout( )->addWidget( m_pPoints );

   m_pUVEnabled = new QCheckBox( i18n( "UV vectors" ), this );
   topLayout( )->addWidget( m_pUVEnabled );
   m_pUVVectors = new PMVectorListEdit( "u", "v", this );
   m_pUVVectors->setSize( 4 );
   topLayout( )->addWidget( m_pUVVectors );

   connect( m_pType, SIGNAL( highlighted( int ) ), SLOT( slotTypeSelected( int ) ) );
   connect( m_pUSteps, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pVSteps, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pFlatness, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pPoints, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pPoints, SIGNAL( selectionChanged( ) ),
            SLOT( slotSelectionChanged( ) ) );
   connect( m_pUVEnabled, SIGNAL( clicked( ) ), SLOT( slotUVEnabledClicked( ) ) );
   connect( m_pUVVectors, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
}

void PMBicubicPatchEdit::displayObject( PMObject* o )
{
   int i;
   if( o->isA( "BicubicPatch" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_pDisplayedObject = ( PMBicubicPatch* ) o;

      m_pType->setCurrentItem( m_pDisplayedObject->patchType( ) );
      m_pType->setEnabled( !readOnly );
      m_pUSteps->setValue( m_pDisplayedObject->uSteps( ) );
      m_pUSteps->setReadOnly( readOnly );
      m_pVSteps->setValue( m_pDisplayedObject->vSteps( ) );
      m_pVSteps->setReadOnly( readOnly );
      m_pFlatness->setValue( m_pDisplayedObject->flatness( ) );
      m_pFlatness->setReadOnly( readOnly );

      for( i = 0; i < 16; i++ )
         m_pPoints->setVector( i, m_pDisplayedObject->controlPoint( i ) );
      m_pPoints->setReadOnly( readOnly );
      updateControlPointSelection( );

      m_pUVEnabled->setChecked( m_pDisplayedObject->isUVEnabled( ) );
      m_pUVEnabled->setEnabled( !readOnly );
      for( i = 0; i < 4; ++i )
         m_pUVVectors->setVector( i, m_pDisplayedObject->uvVector( i ) );
      m_pUVVectors->setReadOnly( readOnly );
      slotUVEnabledClicked( );

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMBicubicPatchEdit: Can't display object\n";
}

void PMBicubicPatchEdit::updateControlPointSelection( )
{
   PMControlPointList cp = part( )->activeControlPoints( );
   PMControlPointListIterator it( cp );
   int s = 0, e = 0;

   m_pPoints->blockSelectionUpdates( true );
   bool sb = m_pPoints->signalsBlocked( );
   m_pPoints->blockSignals( true );

   m_pPoints->clearSelection( );
   while( s < 16 )
   {
      if( ( *it )->selected( ) )
      {
         for( e = s; e < 16 && ( *it )->selected( ); e++ )
            ++it;
         m_pPoints->select( s, e - 1 );
         s = e;
      }
      else
      {
         s++;
         ++it;
      }
   }

   m_pPoints->blockSignals( sb );
   m_pPoints->blockSelectionUpdates( false );
}

void PMBicubicPatchEdit::saveContents( )
{
   int i;

   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setPatchType( m_pType->currentItem( ) );
      m_pDisplayedObject->setUSteps( m_pUSteps->value( ) );
      m_pDisplayedObject->setVSteps( m_pVSteps->value( ) );
      m_pDisplayedObject->setFlatness( m_pFlatness->value( ) );
      for( i = 0; i < 16; i++ )
         m_pDisplayedObject->setControlPoint( i, m_pPoints->vector( i ) );
      m_pDisplayedObject->enableUV( m_pUVEnabled->isChecked( ) );
      for( i = 0; i < 4; ++i )
         m_pDisplayedObject->setUVVector( i, m_pUVVectors->vector( i ) );
   }
}

bool PMBicubicPatchEdit::isDataValid( )
{
   bool ok = false;
   if( m_pUSteps->isDataValid( ) )
      if( m_pVSteps->isDataValid( ) )
         if( m_pFlatness->isDataValid( ) )
            ok = true;
   if( ok )
      ok = m_pPoints->isDataValid( );
   if( ok && m_pUVEnabled->isChecked( ) )
      ok = m_pUVVectors->isDataValid( );
   if( ok )
      return Base::isDataValid( );
   return false;
}

void PMBicubicPatchEdit::slotTypeSelected( int )
{
   emit dataChanged( );
}

void PMBicubicPatchEdit::slotSelectionChanged( )
{
   PMControlPointList cp = part( )->activeControlPoints( );
   PMControlPointListIterator it( cp );
   int i;
   for( i = 0; i < 16; i++, ++it )
      ( *it )->setSelected( m_pPoints->isSelected( i ) );
   emit controlPointSelectionChanged( );
}

void PMBicubicPatchEdit::slotUVEnabledClicked( )
{
   if( m_pUVEnabled->isChecked( ) )
      m_pUVVectors->show( );
   else
      m_pUVVectors->hide( );
   emit dataChanged( );
   emit sizeChanged( );
}

#include "pmbicubicpatchedit.moc"
