/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2002 by Luis Carvalho
    email                : lpassos@mail.telepac.pt
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


#include "pmglobalsettingsedit.h"
#include "pmglobalsettings.h"
#include "pmlineedits.h"
#include "pmcoloredit.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <klocale.h>
#include <kdialog.h>
#include <kmessagebox.h>

PMGlobalSettingsEdit::PMGlobalSettingsEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMGlobalSettingsEdit::createTopWidgets( )
{
   QHBoxLayout* hl;
   QLabel* lbl;

   Base::createTopWidgets( );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "Adc bailout:" ), this );
   m_pAdcBailoutEdit = new PMFloatEdit( this );
   hl->addWidget( lbl );
   hl->addWidget( m_pAdcBailoutEdit );
   hl->addStretch( 1 );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "Ambient light:" ), this );
   m_pAmbientLightEdit = new PMColorEdit( false, this );
   topLayout( )->addWidget( lbl );
   topLayout( )->addWidget( m_pAmbientLightEdit );
   hl->addStretch( 1 );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "Assumed gamma:" ), this );
   m_pAssumedGammaEdit = new PMFloatEdit( this );
   hl->addWidget( lbl );
   hl->addWidget( m_pAssumedGammaEdit );
   hl->addStretch( 1 );

   m_pHfGray16Edit = new QCheckBox( i18n( "Hf gray 16" ), this );
   topLayout( )->addWidget( m_pHfGray16Edit );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "Iridiscence wave length:" ), this );
   m_pIridWaveLengthEdit = new PMColorEdit( false, this );
   topLayout( )->addWidget( lbl );
   topLayout( )->addWidget( m_pIridWaveLengthEdit );
   hl->addStretch( 1 );

   hl = new QHBoxLayout( topLayout( ) );
   QGridLayout* layout = new QGridLayout( hl, 4, 2 );
   lbl = new QLabel( i18n( "Maximum intersections:" ), this );
   m_pMaxIntersectionsEdit = new PMIntEdit( this );
   layout->addWidget( lbl, 0, 0 );
   layout->addWidget( m_pMaxIntersectionsEdit, 0, 1 );
   lbl = new QLabel( i18n( "Maximum trace level:" ), this );
   m_pMaxTraceLevelEdit = new PMIntEdit( this );
   layout->addWidget( lbl, 1, 0 );
   layout->addWidget( m_pMaxTraceLevelEdit, 1, 1 );
   lbl = new QLabel( i18n( "Number of waves:" ), this );
   m_pNumberWavesEdit = new PMIntEdit( this );
   layout->addWidget( lbl, 2, 0 );
   layout->addWidget( m_pNumberWavesEdit, 2, 1 );
   lbl = new QLabel( i18n( "Noise generator:" ), this );
   m_pNoiseGeneratorEdit = new QComboBox( false, this );
   m_pNoiseGeneratorEdit->insertItem( i18n( "Original" ) );
   m_pNoiseGeneratorEdit->insertItem( i18n( "Range Corrected" ) );
   m_pNoiseGeneratorEdit->insertItem( i18n( "Perlin" ) );
   layout->addWidget( lbl, 3, 0 );
   layout->addWidget( m_pNoiseGeneratorEdit, 3, 1 );
   hl->addStretch( 1 );

   m_pRadiosityEdit = new QCheckBox( i18n( "Radiosity (Povray 3.1)" ), this );
   topLayout( )->addWidget( m_pRadiosityEdit );

   m_pRadiosityWidget = new QWidget( this );
   hl = new QHBoxLayout( m_pRadiosityWidget, 0, KDialog::spacingHint( ) );
   layout = new QGridLayout( hl, 7, 2 );
   lbl = new QLabel( i18n( "Brightness:" ), m_pRadiosityWidget );
   m_pBrightnessEdit = new PMFloatEdit( m_pRadiosityWidget );
   layout->addWidget( lbl, 0, 0 );
   layout->addWidget( m_pBrightnessEdit, 0, 1 );
   lbl = new QLabel( i18n( "Count:" ), m_pRadiosityWidget );
   m_pCountEdit = new PMIntEdit( m_pRadiosityWidget );
   layout->addWidget( lbl, 1, 0 );
   layout->addWidget( m_pCountEdit, 1, 1 );
   lbl = new QLabel( i18n( "Maximum distance:" ), m_pRadiosityWidget );
   m_pDistanceMaximumEdit = new PMFloatEdit( m_pRadiosityWidget );
   layout->addWidget( lbl, 2, 0 );
   layout->addWidget( m_pDistanceMaximumEdit, 2, 1 );
   lbl = new QLabel( i18n( "Error boundary:" ), m_pRadiosityWidget );
   m_pErrorBoundEdit = new PMFloatEdit( m_pRadiosityWidget );
   layout->addWidget( lbl, 3, 0 );
   layout->addWidget( m_pErrorBoundEdit, 3, 1 );
   lbl = new QLabel( i18n( "Gray threshold:" ), m_pRadiosityWidget );
   m_pGrayThresholdEdit = new PMFloatEdit( m_pRadiosityWidget );
   layout->addWidget( lbl, 4, 0 );
   layout->addWidget( m_pGrayThresholdEdit, 4, 1 );
   lbl = new QLabel( i18n( "Low error factor:" ), m_pRadiosityWidget );
   m_pLowErrorFactorEdit = new PMFloatEdit( m_pRadiosityWidget );
   layout->addWidget( lbl, 5, 0 );
   layout->addWidget( m_pLowErrorFactorEdit, 5, 1 );
   lbl = new QLabel( i18n( "Minimum reuse:" ), m_pRadiosityWidget );
   m_pMinimumReuseEdit = new PMFloatEdit( m_pRadiosityWidget );
   layout->addWidget( lbl, 6, 0 );
   layout->addWidget( m_pMinimumReuseEdit, 6, 1 );
   lbl = new QLabel( i18n( "Nearest count:" ), m_pRadiosityWidget );
   m_pNearestCountEdit = new PMIntEdit( m_pRadiosityWidget );
   layout->addWidget( lbl, 7, 0 );
   layout->addWidget( m_pNearestCountEdit, 7, 1 );
   lbl = new QLabel( i18n( "Recursion limit:" ), m_pRadiosityWidget );
   m_pRecursionLimitEdit = new PMIntEdit( m_pRadiosityWidget );
   layout->addWidget( lbl, 8, 0 );
   layout->addWidget( m_pRecursionLimitEdit, 8, 1 );
   hl->addStretch( 1 );

   topLayout( )->addWidget( m_pRadiosityWidget );

   connect( m_pAdcBailoutEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pAmbientLightEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pAssumedGammaEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pHfGray16Edit, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pIridWaveLengthEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pMaxIntersectionsEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pMaxTraceLevelEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pNumberWavesEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pNoiseGeneratorEdit, SIGNAL( activated( int ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pRadiosityEdit, SIGNAL( clicked( ) ), SLOT( slotRadiosityClicked( ) ) );
   connect( m_pBrightnessEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pCountEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pDistanceMaximumEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pErrorBoundEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pGrayThresholdEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pLowErrorFactorEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pMinimumReuseEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pNearestCountEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pRecursionLimitEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
}

void PMGlobalSettingsEdit::displayObject( PMObject* o )
{
   if( o->isA( "GlobalSettings" ) )
   {
      bool readOnly =  o->isReadOnly( );
      m_pDisplayedObject = ( PMGlobalSettings* ) o;

      m_pAdcBailoutEdit->setValue( m_pDisplayedObject->adcBailout( ) );
      m_pAdcBailoutEdit->setReadOnly( readOnly );
      m_pAmbientLightEdit->setColor( m_pDisplayedObject->ambientLight( ) );
      m_pAmbientLightEdit->setReadOnly( readOnly );
      m_pAssumedGammaEdit->setValue( m_pDisplayedObject->assumedGamma( ) );
      m_pAssumedGammaEdit->setReadOnly( readOnly );
      m_pHfGray16Edit->setChecked( m_pDisplayedObject->hfGray16( ) );
      m_pHfGray16Edit->setEnabled( !readOnly );
      m_pIridWaveLengthEdit->setColor( m_pDisplayedObject->iridWaveLength( ) );
      m_pIridWaveLengthEdit->setReadOnly( readOnly );
      m_pMaxIntersectionsEdit->setValue( m_pDisplayedObject->maxIntersections( ) );
      m_pMaxIntersectionsEdit->setReadOnly( readOnly );
      m_pMaxTraceLevelEdit->setValue( m_pDisplayedObject->maxTraceLevel( ) );
      m_pMaxTraceLevelEdit->setReadOnly( readOnly );
      m_pNumberWavesEdit->setValue( m_pDisplayedObject->numberWaves( ) );
      m_pNumberWavesEdit->setReadOnly( readOnly );
      m_pNoiseGeneratorEdit->setCurrentItem( m_pDisplayedObject->noiseGenerator( ) );
      m_pNoiseGeneratorEdit->setEnabled( !readOnly );
      m_pRadiosityEdit->setChecked( m_pDisplayedObject->isRadiosityEnabled( ) );
      m_pRadiosityEdit->setEnabled( !readOnly );
      m_pBrightnessEdit->setValue( m_pDisplayedObject->brightness( ) );
      m_pBrightnessEdit->setReadOnly( readOnly );
      m_pCountEdit->setValue( m_pDisplayedObject->count( ) );
      m_pCountEdit->setReadOnly( readOnly );
      m_pDistanceMaximumEdit->setValue( m_pDisplayedObject->distanceMaximum( ) );
      m_pDistanceMaximumEdit->setReadOnly( readOnly );
      m_pErrorBoundEdit->setValue( m_pDisplayedObject->errorBound( ) );
      m_pErrorBoundEdit->setReadOnly( readOnly );
      m_pGrayThresholdEdit->setValue( m_pDisplayedObject->grayThreshold( ) );
      m_pGrayThresholdEdit->setReadOnly( readOnly );
      m_pLowErrorFactorEdit->setValue( m_pDisplayedObject->lowErrorFactor( ) );
      m_pLowErrorFactorEdit->setReadOnly( readOnly );
      m_pMinimumReuseEdit->setValue( m_pDisplayedObject->minimumReuse( ) );
      m_pMinimumReuseEdit->setReadOnly( readOnly );
      m_pNearestCountEdit->setValue( m_pDisplayedObject->nearestCount( ) );
      m_pNearestCountEdit->setReadOnly( readOnly );
      m_pRecursionLimitEdit->setValue( m_pDisplayedObject->recursionLimit( ) );
      m_pRecursionLimitEdit->setReadOnly( readOnly );
      slotRadiosityClicked( );

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMGlobalSettingsEdit: Can't display object\n";
}

void PMGlobalSettingsEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setAdcBailout( m_pAdcBailoutEdit->value( ) );
      m_pDisplayedObject->setAmbientLight( m_pAmbientLightEdit->color( ) );
      m_pDisplayedObject->setAssumedGamma( m_pAssumedGammaEdit->value( ) );
      m_pDisplayedObject->setHfGray16( m_pHfGray16Edit->isChecked( ) );
      m_pDisplayedObject->setIridWaveLength( m_pIridWaveLengthEdit->color( ) );
      m_pDisplayedObject->setMaxIntersections( m_pMaxIntersectionsEdit->value( ) );
      m_pDisplayedObject->setMaxTraceLevel( m_pMaxTraceLevelEdit->value( ) );
      m_pDisplayedObject->setNumberWaves( m_pNumberWavesEdit->value( ) );
      m_pDisplayedObject->setNoiseGenerator(
         ( PMGlobalSettings::PMNoiseType ) ( m_pNoiseGeneratorEdit->currentItem( ) ) );
      m_pDisplayedObject->enableRadiosity( m_pRadiosityEdit->isChecked( ) );
      m_pDisplayedObject->setBrightness( m_pBrightnessEdit->value( ) );
      m_pDisplayedObject->setCount( m_pCountEdit->value( ) );
      m_pDisplayedObject->setDistanceMaximum( m_pDistanceMaximumEdit->value( ) );
      m_pDisplayedObject->setErrorBound( m_pErrorBoundEdit->value( ) );
      m_pDisplayedObject->setGrayThreshold( m_pGrayThresholdEdit->value( ) );
      m_pDisplayedObject->setLowErrorFactor( m_pLowErrorFactorEdit->value( ) );
      m_pDisplayedObject->setMinimumReuse( m_pMinimumReuseEdit->value( ) );
      m_pDisplayedObject->setNearestCount( m_pNearestCountEdit->value( ) );
      m_pDisplayedObject->setRecursionLimit( m_pRecursionLimitEdit->value( ) );
   }
}

bool PMGlobalSettingsEdit::isDataValid( )
{
   if( !m_pAdcBailoutEdit->isDataValid( ) ) return false;
   if( !m_pAmbientLightEdit->isDataValid( ) ) return false;
   if( !m_pAssumedGammaEdit->isDataValid( ) ) return false;
   if( !m_pIridWaveLengthEdit->isDataValid( ) ) return false;
   if( !m_pMaxIntersectionsEdit->isDataValid( ) ) return false;
   if( !m_pMaxTraceLevelEdit->isDataValid( ) ) return false;
   if( !m_pNumberWavesEdit->isDataValid( ) ) return false;
   if( !m_pBrightnessEdit->isDataValid( ) ) return false;
   if( !m_pCountEdit->isDataValid( ) ) return false;
   if( !m_pDistanceMaximumEdit->isDataValid( ) ) return false;
   if( !m_pErrorBoundEdit->isDataValid( ) ) return false;
   if( !m_pGrayThresholdEdit->isDataValid( ) ) return false;
   if( !m_pLowErrorFactorEdit->isDataValid( ) ) return false;
   if( !m_pMinimumReuseEdit->isDataValid( ) ) return false;
   if( !m_pNearestCountEdit->isDataValid( ) ) return false;
   if( !m_pRecursionLimitEdit->isDataValid( ) ) return false;

   if( m_pMaxIntersectionsEdit->value( ) < 0 )
   {
      KMessageBox::error( this, i18n( "Maximum intersections must be a positive value." ),
                          i18n( "Error" ) );
      m_pMaxIntersectionsEdit->setFocus( );
      return false;
   }
   if( m_pMaxTraceLevelEdit->value( ) < 0 )
   {
      KMessageBox::error( this, i18n( "Maximum trace level must be a positive value." ),
                          i18n( "Error" ) );
      m_pMaxTraceLevelEdit->setFocus( );
      return false;
   }
   if( m_pNumberWavesEdit->value( ) < 0 )
   {
      KMessageBox::error( this, i18n( "Number of waves must be a positive value." ),
                          i18n( "Error" ) );
      m_pNumberWavesEdit->setFocus( );
      return false;
   }
   if( m_pNearestCountEdit->value( ) < 1 || m_pNearestCountEdit->value( ) > 10 )
   {
      KMessageBox::error( this, i18n( "Nearest count must be between 1 and 10." ),
                          i18n( "Error" ) );
      m_pNearestCountEdit->setFocus( );
      return false;
   }

   if( m_pRecursionLimitEdit->value( ) < 1 || m_pRecursionLimitEdit->value( ) > 2 )
   {
      KMessageBox::error( this, i18n( "Recursion limit must be 1 or 2." ),
                          i18n( "Error" ) );
      m_pRecursionLimitEdit->setFocus( );
      return false;
   }

   return Base::isDataValid( );
}

void PMGlobalSettingsEdit::slotRadiosityClicked( )
{
   if( m_pRadiosityEdit->isChecked( ) ) 
      m_pRadiosityWidget->show( );
   else 
      m_pRadiosityWidget->hide( );

   emit dataChanged( );
   emit sizeChanged( );
}

#include "pmglobalsettingsedit.moc"
