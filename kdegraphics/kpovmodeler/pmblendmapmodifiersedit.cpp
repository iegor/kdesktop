/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2001 by Luis Carvalho
    email                : lpassos@mail.telepac.pt
    copyright            : (C) 2001 by Andreas Zehender
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


#include "pmblendmapmodifiersedit.h"
#include "pmblendmapmodifiers.h"
#include "pmvectoredit.h"
#include "pmlineedits.h"
#include "pmvector.h"

#include <qwidget.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <ktabctl.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdialog.h>
#include <kfiledialog.h>

PMBlendMapModifiersEdit::PMBlendMapModifiersEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMBlendMapModifiersEdit::createTopWidgets( )
{
   QHBoxLayout* hl;

   Base::createTopWidgets( );

   hl = new QHBoxLayout( topLayout( ) );
   m_pEnableFrequencyEdit = new QCheckBox( i18n( "Frequency:" ), this );
   m_pFrequencyEdit = new PMFloatEdit( this );
   hl->addWidget( m_pEnableFrequencyEdit );
   hl->addWidget( m_pFrequencyEdit );
   hl->addStretch( 1 );

   hl = new QHBoxLayout( topLayout( ) );
   m_pEnablePhaseEdit = new QCheckBox( i18n( "Phase:" ), this );
   m_pPhaseEdit = new PMFloatEdit( this );
   hl->addWidget( m_pEnablePhaseEdit );
   hl->addWidget( m_pPhaseEdit );
   hl->addStretch( 1 );

   hl = new QHBoxLayout( topLayout( ) );
   m_pEnableWaveFormEdit = new QCheckBox( i18n( "Wave form:" ), this );
   m_pWaveTypeCombo = new QComboBox( this );
   m_pWaveTypeCombo->insertItem( i18n( "Ramp" ) );
   m_pWaveTypeCombo->insertItem( i18n( "Triangle" ) );
   m_pWaveTypeCombo->insertItem( i18n( "Sine" ) );
   m_pWaveTypeCombo->insertItem( i18n( "Scallop" ) );
   m_pWaveTypeCombo->insertItem( i18n( "Cubic" ) );
   m_pWaveTypeCombo->insertItem( i18n( "Poly" ) );
   hl->addWidget( m_pEnableWaveFormEdit );
   hl->addWidget( m_pWaveTypeCombo );
   hl->addStretch( 1 );

   hl = new QHBoxLayout( topLayout( ) );
   m_pWaveExponentLabel = new QLabel( i18n( "Exponent:" ), this );
   m_pWaveExponentEdit = new PMFloatEdit( this );
   hl->addWidget( m_pWaveExponentLabel );
   hl->addWidget( m_pWaveExponentEdit );
   hl->addStretch( 1 );

   connect( m_pEnableFrequencyEdit, SIGNAL( clicked( ) ), SLOT( slotFrequencyClicked( ) ) );
   connect( m_pFrequencyEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pEnablePhaseEdit, SIGNAL( clicked( ) ), SLOT( slotPhaseClicked( ) ) );
   connect( m_pPhaseEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pEnableWaveFormEdit, SIGNAL( clicked( ) ), SLOT( slotWaveFormClicked( ) ) );
   connect( m_pWaveExponentEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pWaveTypeCombo, SIGNAL( activated( int ) ), SLOT( slotTypeComboChanged( int ) ) );
}

void PMBlendMapModifiersEdit::displayObject( PMObject* o )
{
   QString str;

   if( o->isA( "BlendMapModifiers" ) )
   {
      m_pDisplayedObject = ( PMBlendMapModifiers* ) o;

      m_pEnableFrequencyEdit->setChecked( m_pDisplayedObject->isFrequencyEnabled( ) );
      m_pEnablePhaseEdit->setChecked( m_pDisplayedObject->isPhaseEnabled( ) );
      m_pEnableWaveFormEdit->setChecked( m_pDisplayedObject->isWaveFormEnabled( ) );

      m_pFrequencyEdit->setValue( m_pDisplayedObject->frequency( ) );
      m_pPhaseEdit->setValue( m_pDisplayedObject->phase( ) );
      m_pWaveExponentEdit->setValue( m_pDisplayedObject->waveFormExponent( ) );

      switch( m_pDisplayedObject->waveFormType( ) )
      {
         case PMBlendMapModifiers::RampWave:
            m_pWaveTypeCombo->setCurrentItem( 0 );
            break;
         case PMBlendMapModifiers::TriangleWave:
            m_pWaveTypeCombo->setCurrentItem( 1 );
            break;
         case PMBlendMapModifiers::SineWave:
            m_pWaveTypeCombo->setCurrentItem( 2 );
            break;
         case PMBlendMapModifiers::ScallopWave:
            m_pWaveTypeCombo->setCurrentItem( 3 );
            break;
         case PMBlendMapModifiers::CubicWave:
            m_pWaveTypeCombo->setCurrentItem( 4 );
            break;
         case PMBlendMapModifiers::PolyWave:
            m_pWaveTypeCombo->setCurrentItem( 5 );
            break;
      }

      slotFrequencyClicked( );
      slotPhaseClicked( );
      slotWaveFormClicked( );
      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMBlendMapModifiersEdit: Can't display object\n";
}

void PMBlendMapModifiersEdit::setBlendMapModifiersType( int i )
{
   m_pWaveTypeCombo->setCurrentItem( i );
   slotTypeComboChanged( i );
}

void PMBlendMapModifiersEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );

      m_pDisplayedObject->enableFrequency( m_pEnableFrequencyEdit->isChecked( ) );
      m_pDisplayedObject->enablePhase( m_pEnablePhaseEdit->isChecked( ) );
      m_pDisplayedObject->enableWaveForm( m_pEnableWaveFormEdit->isChecked( ) );

      if( m_pEnableFrequencyEdit->isChecked( ) )
         m_pDisplayedObject->setFrequency( m_pFrequencyEdit->value( ) );

      if( m_pEnablePhaseEdit->isChecked( ) )
         m_pDisplayedObject->setPhase( m_pPhaseEdit->value( ) );

      if( m_pEnableWaveFormEdit->isChecked( ) )
      {
         switch( m_pWaveTypeCombo->currentItem( ) )
         {
            case 0: /* Ramp Wave */
               m_pDisplayedObject->setWaveFormType( PMBlendMapModifiers::RampWave );
               break;
            case 1: /* Triangle Wave */
               m_pDisplayedObject->setWaveFormType( PMBlendMapModifiers::TriangleWave );
               break;
            case 2: /* Sine Wave */
              m_pDisplayedObject->setWaveFormType( PMBlendMapModifiers::SineWave );
              break;
            case 3: /* Scallop Wave */
               m_pDisplayedObject->setWaveFormType( PMBlendMapModifiers::ScallopWave );
               break;
            case 4: /* Cubic Wave */
               m_pDisplayedObject->setWaveFormType( PMBlendMapModifiers::CubicWave );
               break;
            case 5: /* Poly Wave */
               m_pDisplayedObject->setWaveFormType( PMBlendMapModifiers::PolyWave );
               m_pDisplayedObject->setWaveFormExponent( m_pWaveExponentEdit->value( ) );
               break;
         }
      }
   }
}

bool PMBlendMapModifiersEdit::isDataValid( )
{
   if( !m_pFrequencyEdit->isDataValid( ) ) return false;
   if( !m_pPhaseEdit->isDataValid( ) ) return false;

   switch( m_pWaveTypeCombo->currentItem( ) )
   {
      case 5: /* Poly Wave */
         if( !m_pWaveExponentEdit->isDataValid( ) ) return false;
         break;
   }

   return Base::isDataValid( );
}

void PMBlendMapModifiersEdit::slotTypeComboChanged( int c )
{
   switch( c )
   {
      case 5: /* Poly Wave */
         m_pWaveExponentLabel->show( );
         m_pWaveExponentEdit->show( );
         break;
      default:
         m_pWaveExponentLabel->hide( );
         m_pWaveExponentEdit->hide( );
         break;
   }  
   emit dataChanged( );
   emit sizeChanged( );
}

void PMBlendMapModifiersEdit::slotFrequencyClicked( )
{
   if( m_pEnableFrequencyEdit->isChecked( ) )
      m_pFrequencyEdit->setEnabled( true );
   else
      m_pFrequencyEdit->setEnabled( false );
   emit dataChanged( );
   emit sizeChanged( );
}

void PMBlendMapModifiersEdit::slotPhaseClicked( )
{
   if(m_pEnablePhaseEdit->isChecked( ) )
      m_pPhaseEdit->setEnabled( true );
   else
      m_pPhaseEdit->setEnabled( false );
   emit dataChanged( );
   emit sizeChanged( );
}

void PMBlendMapModifiersEdit::slotWaveFormClicked( )
{
   if(m_pEnableWaveFormEdit->isChecked( ) )
   {
      m_pWaveTypeCombo->setEnabled( true );
      slotTypeComboChanged( m_pWaveTypeCombo->currentItem( ) );
   }
   else
   {
      m_pWaveTypeCombo->setEnabled( false );
      m_pWaveExponentLabel->hide( );
      m_pWaveExponentEdit->hide( );
   }
   emit dataChanged( );
   emit sizeChanged( );
}

#include "pmblendmapmodifiersedit.moc"
