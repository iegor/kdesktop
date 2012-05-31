/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2001 by Luis Carvalho
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


#include "pmwarpedit.h"
#include "pmwarp.h"
#include "pmvectoredit.h"
#include "pmlineedits.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <klocale.h>
#include <kdialog.h>


PMWarpEdit::PMWarpEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMWarpEdit::createTopWidgets( )
{
   QHBoxLayout* hl;
   QVBoxLayout* vl;
   QGridLayout* gl;

   Base::createTopWidgets( );

   QLabel* label = new QLabel( i18n( "Warp type:" ), this );
   m_pWarpTypeEdit = new QComboBox( false, this );
   m_pWarpTypeEdit->insertItem( i18n( "Repeat" ) );
   m_pWarpTypeEdit->insertItem( i18n( "Black Hole" ) );
   m_pWarpTypeEdit->insertItem( i18n( "Turbulence" ) );
   m_pWarpTypeEdit->insertItem( i18n( "Cylindrical" ) );
   m_pWarpTypeEdit->insertItem( i18n( "Spherical" ) );
   m_pWarpTypeEdit->insertItem( i18n( "Toroidal" ) );
   m_pWarpTypeEdit->insertItem( i18n( "Planar" ) );
   hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( label );
   hl->addWidget( m_pWarpTypeEdit );
   hl->addStretch( 1 );
   
   /* Repeat Warp Objects */
   m_pRepeatWidget = new QWidget( this );
   vl = new QVBoxLayout( m_pRepeatWidget, 0, KDialog::spacingHint( ) );
   gl = new QGridLayout( vl, 3, 2 );
   m_pDirectionLabel = new QLabel( i18n( "Direction:" ), m_pRepeatWidget );
   m_pDirectionEdit = new PMVectorEdit( "x", "y", "z", m_pRepeatWidget );
   gl->addWidget( m_pDirectionLabel, 0, 0 );
   gl->addWidget( m_pDirectionEdit, 0, 1 );
   m_pOffsetLabel = new QLabel( i18n( "Offset:" ), m_pRepeatWidget );
   m_pOffsetEdit = new PMVectorEdit( "x", "y", "z", m_pRepeatWidget );
   gl->addWidget( m_pOffsetLabel, 1, 0 );
   gl->addWidget( m_pOffsetEdit, 1, 1 );
   m_pFlipLabel = new QLabel( i18n( "Flip:" ), m_pRepeatWidget );
   m_pFlipEdit = new PMVectorEdit( "x", "y", "z", m_pRepeatWidget );
   gl->addWidget( m_pFlipLabel, 2, 0 );
   gl->addWidget( m_pFlipEdit, 2, 1 );
   
   /* Black Hole Warp Objects */
   m_pBlackHoleWidget = new QWidget( this );
   vl = new QVBoxLayout( m_pBlackHoleWidget, 0, KDialog::spacingHint( ) );
   m_pLocationLabel = new QLabel( i18n( "Location:" ), m_pBlackHoleWidget );
   m_pLocationEdit = new PMVectorEdit( "x", "y", "z", m_pBlackHoleWidget );
   hl = new QHBoxLayout( vl );
   hl->addWidget( m_pLocationLabel );
   hl->addWidget( m_pLocationEdit );
   m_pRadiusLabel = new QLabel( i18n( "Radius:" ), m_pBlackHoleWidget );
   m_pRadiusEdit = new PMFloatEdit( m_pBlackHoleWidget );
   m_pStrengthLabel = new QLabel( i18n( "Strength:" ), m_pBlackHoleWidget );
   m_pStrengthEdit = new PMFloatEdit( m_pBlackHoleWidget );
   m_pFalloffLabel = new QLabel( i18n( "Falloff:" ), m_pBlackHoleWidget );
   m_pFalloffEdit = new PMFloatEdit( m_pBlackHoleWidget );
   hl = new QHBoxLayout( vl );
   gl = new QGridLayout( hl, 3, 2 );
   gl->addWidget( m_pRadiusLabel, 0, 0 );
   gl->addWidget( m_pRadiusEdit, 0, 1 );
   gl->addWidget( m_pStrengthLabel, 1, 0 );
   gl->addWidget( m_pStrengthEdit, 1, 1 );
   gl->addWidget( m_pFalloffLabel, 2, 0 );
   gl->addWidget( m_pFalloffEdit, 2, 1 );
   hl->addStretch( 1 );
   
   m_pRepeatLabel = new QLabel( i18n( "Repeat:" ), m_pBlackHoleWidget );
   m_pRepeatEdit = new PMVectorEdit( "x", "y", "z", m_pBlackHoleWidget );
   hl = new QHBoxLayout( vl );
   hl->addWidget( m_pRepeatLabel );
   hl->addWidget( m_pRepeatEdit );
   m_pTurbulenceLabel = new QLabel( i18n( "Turbulence:" ), m_pBlackHoleWidget );
   m_pTurbulenceEdit = new PMVectorEdit( "x", "y", "z", m_pBlackHoleWidget );
   hl = new QHBoxLayout( vl );
   hl->addWidget( m_pTurbulenceLabel );
   hl->addWidget( m_pTurbulenceEdit );
   m_pInverseEdit = new QCheckBox( i18n( "Inverse" ), m_pBlackHoleWidget );
   vl->addWidget( m_pInverseEdit );

   /* Turbulence Warp Objects */
   m_pTurbulenceWidget = new QWidget( this );
   vl = new QVBoxLayout( m_pTurbulenceWidget, 0, KDialog::spacingHint( ) );
   m_pValueVectorLabel = new QLabel( i18n( "Value:" ), m_pTurbulenceWidget );
   m_pValueVectorEdit = new PMVectorEdit( "x", "y", "z", m_pTurbulenceWidget );
   hl = new QHBoxLayout( vl );
   hl->addWidget( m_pValueVectorLabel );
   hl->addWidget( m_pValueVectorEdit );

   hl = new QHBoxLayout( vl );
   gl = new QGridLayout( hl, 3, 2 );
   m_pOctavesLabel = new QLabel( i18n( "Octaves:" ), m_pTurbulenceWidget );
   m_pOctavesEdit = new PMIntEdit( m_pTurbulenceWidget );
   gl->addWidget( m_pOctavesLabel, 0, 0 );
   gl->addWidget( m_pOctavesEdit, 0, 1 );
   m_pOmegaLabel = new QLabel( i18n( "Omega:" ), m_pTurbulenceWidget );
   m_pOmegaEdit = new PMFloatEdit( m_pTurbulenceWidget );
   gl->addWidget( m_pOmegaLabel, 1, 0 );
   gl->addWidget( m_pOmegaEdit, 1, 1 );
   m_pLambdaLabel = new QLabel( i18n( "Lambda:" ), m_pTurbulenceWidget );
   m_pLambdaEdit = new PMFloatEdit( m_pTurbulenceWidget );
   gl->addWidget( m_pLambdaLabel, 2, 0 );
   gl->addWidget( m_pLambdaEdit, 2, 1 );
   hl->addStretch( 1 );

   /* Mapping Warp Objects */
   m_pMappingWidget = new QWidget( this );
   vl = new QVBoxLayout( m_pMappingWidget, 0, KDialog::spacingHint( ) );
   label = new QLabel( i18n( "Orientation:" ), m_pMappingWidget );
   m_pOrientationEdit = new PMVectorEdit( "x", "y", "z", m_pMappingWidget );
   hl = new QHBoxLayout( vl );
   hl->addWidget( label );
   hl->addWidget( m_pOrientationEdit );

   gl = new QGridLayout( vl, 2, 2 );
   label = new QLabel( i18n( "Distance exponent:" ), m_pMappingWidget );
   m_pDistExpEdit = new PMFloatEdit( m_pMappingWidget );
   gl->addWidget( label, 0, 0 );
   gl->addWidget( m_pDistExpEdit, 0, 1 );
   m_pMajorRadiusLabel = new QLabel( i18n( "Major radius:" ), m_pMappingWidget );
   m_pMajorRadiusEdit = new PMFloatEdit( m_pMappingWidget );
   gl->addWidget( m_pMajorRadiusLabel, 1, 0 );
   gl->addWidget( m_pMajorRadiusEdit, 1, 1 );

   vl = new QVBoxLayout( topLayout( ) );
   vl->addSpacing( 0 );
   vl->addWidget( m_pRepeatWidget );
   vl->addWidget( m_pBlackHoleWidget );
   vl->addWidget( m_pTurbulenceWidget );
   vl->addWidget( m_pMappingWidget );
   
   connect( m_pWarpTypeEdit, SIGNAL( activated( int ) ), SLOT( slotComboChanged( int ) ) );
   connect( m_pDirectionEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pOffsetEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pFlipEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pLocationEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pRadiusEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pStrengthEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pFalloffEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pInverseEdit, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pRepeatEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pTurbulenceEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pValueVectorEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pOctavesEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pOmegaEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pLambdaEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pOrientationEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pDistExpEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pMajorRadiusEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
}

void PMWarpEdit::displayObject( PMObject* o )
{
   if( o->isA( "Warp" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_pDisplayedObject = ( PMWarp* ) o;

      switch( m_pDisplayedObject->warpType( ) )
      {
         case PMWarp::Repeat:
            m_pWarpTypeEdit->setCurrentItem( 0 );
            slotComboChanged( 0 );
            break;
         case PMWarp::BlackHole:
            m_pWarpTypeEdit->setCurrentItem( 1 );
            slotComboChanged( 1 );
            break;
         case PMWarp::Turbulence:
            m_pWarpTypeEdit->setCurrentItem( 2 );
            slotComboChanged( 2 );
            break;
         case PMWarp::Cylindrical:
            m_pWarpTypeEdit->setCurrentItem( 3 );
            slotComboChanged( 3 );
            break;
         case PMWarp::Spherical:
            m_pWarpTypeEdit->setCurrentItem( 4 );
            slotComboChanged( 4 );
            break;
         case PMWarp::Toroidal:
            m_pWarpTypeEdit->setCurrentItem( 5 );
            slotComboChanged( 5 );
            break;
         case PMWarp::Planar:
            m_pWarpTypeEdit->setCurrentItem( 6 );
            slotComboChanged( 6 );
            break;
      }
      m_pDirectionEdit->setVector( m_pDisplayedObject->direction( ) );
      m_pDirectionEdit->setReadOnly( readOnly );
      m_pOffsetEdit->setVector( m_pDisplayedObject->offset( ) );
      m_pOffsetEdit->setReadOnly( readOnly );
      m_pFlipEdit->setVector( m_pDisplayedObject->flip( ) );
      m_pFlipEdit->setReadOnly( readOnly );
      m_pLocationEdit->setVector( m_pDisplayedObject->location( ) );
      m_pLocationEdit->setReadOnly( readOnly );
      m_pRadiusEdit->setValue( m_pDisplayedObject->radius( ) );
      m_pRadiusEdit->setReadOnly( readOnly );
      m_pStrengthEdit->setValue( m_pDisplayedObject->strength( ) );
      m_pStrengthEdit->setReadOnly( readOnly );
      m_pFalloffEdit->setValue( m_pDisplayedObject->falloff( ) );
      m_pFalloffEdit->setReadOnly( readOnly );
      m_pInverseEdit->setChecked( m_pDisplayedObject->inverse( ) );
      m_pInverseEdit->setEnabled( !readOnly );
      m_pRepeatEdit->setVector( m_pDisplayedObject->repeat( ) );
      m_pRepeatEdit->setReadOnly( readOnly );
      m_pTurbulenceEdit->setVector( m_pDisplayedObject->turbulence( ) );
      m_pTurbulenceEdit->setReadOnly( readOnly );
      m_pValueVectorEdit->setVector( m_pDisplayedObject->valueVector( ) );
      m_pValueVectorEdit->setReadOnly( readOnly );
      m_pOctavesEdit->setValue( m_pDisplayedObject->octaves( ) );
      m_pOctavesEdit->setReadOnly( readOnly );
      m_pOmegaEdit->setValue( m_pDisplayedObject->omega( ) );
      m_pOmegaEdit->setReadOnly( readOnly );
      m_pLambdaEdit->setValue( m_pDisplayedObject->lambda( ) );
      m_pLambdaEdit->setReadOnly( readOnly );
      m_pOrientationEdit->setVector( m_pDisplayedObject->orientation( ) );
      m_pOrientationEdit->setReadOnly( readOnly );
      m_pDistExpEdit->setValue( m_pDisplayedObject->distExp( ) );
      m_pDistExpEdit->setReadOnly( readOnly );
      m_pMajorRadiusEdit->setValue( m_pDisplayedObject->majorRadius( ) );
      m_pMajorRadiusEdit->setReadOnly( readOnly );

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMWarpEdit: Can't display object\n";
}

void PMWarpEdit::saveContents( )
{
   bool mapping = false;

   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      switch( m_pWarpTypeEdit->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setWarpType( PMWarp::Repeat );
            m_pDisplayedObject->setDirection( m_pDirectionEdit->vector( ) );
            m_pDisplayedObject->setOffset( m_pOffsetEdit->vector( ) );
            m_pDisplayedObject->setFlip( m_pOffsetEdit->vector( ) );
            break;
         case 1:
            m_pDisplayedObject->setWarpType( PMWarp::BlackHole );
            m_pDisplayedObject->setLocation( m_pLocationEdit->vector( ) );
            m_pDisplayedObject->setRadius( m_pRadiusEdit->value( ) );
            m_pDisplayedObject->setStrength( m_pStrengthEdit->value( ) );
            m_pDisplayedObject->setFalloff( m_pFalloffEdit->value( ) );
            m_pDisplayedObject->setInverse( m_pInverseEdit->isChecked( ) );
            m_pDisplayedObject->setRepeat( m_pRepeatEdit->vector( ) );
            m_pDisplayedObject->setTurbulence( m_pTurbulenceEdit->vector( ) );
            break;
         case 2:
            m_pDisplayedObject->setWarpType( PMWarp::Turbulence );
            m_pDisplayedObject->setValueVector( m_pValueVectorEdit->vector( ) );
            m_pDisplayedObject->setOctaves( m_pOctavesEdit->value( ) );
            m_pDisplayedObject->setOmega( m_pOmegaEdit->value( ) );
            m_pDisplayedObject->setLambda( m_pLambdaEdit->value( ) );
            break;
         case 3:
            m_pDisplayedObject->setWarpType( PMWarp::Cylindrical );
            mapping = true;
            break;
         case 4:
            m_pDisplayedObject->setWarpType( PMWarp::Spherical );
            mapping = true;
            break;
         case 5:
            m_pDisplayedObject->setWarpType( PMWarp::Toroidal );
            m_pDisplayedObject->setMajorRadius( m_pMajorRadiusEdit->value( ) );
            mapping = true;
            break;
         case 6:
            m_pDisplayedObject->setWarpType( PMWarp::Planar );
            mapping = true;
            break;
      }

      if( mapping )
      {
         m_pDisplayedObject->setOrientation( m_pOrientationEdit->vector( ) );
         m_pDisplayedObject->setDistExp( m_pDistExpEdit->value( ) );
      }
   }
}

bool PMWarpEdit::isDataValid( )
{
   double x,y,z;

   switch( m_pWarpTypeEdit->currentItem( ) )
   {
      case 0:
         if( !m_pDirectionEdit->isDataValid( ) ||
             !m_pOffsetEdit->isDataValid( ) ||
             !m_pFlipEdit->isDataValid( ) )
            return false;
         //  The direction vector can only have one non-zero component
         x = m_pDirectionEdit->vector( ).x( );
         y = m_pDirectionEdit->vector( ).y( );
         z = m_pDirectionEdit->vector( ).z( );
         if( ( x && ( y || z ) ) || ( y && ( x || z  )) || ( z && ( x || y ) ) )
            return false;
         break;
      case 1:
         break;
      case 2:
         if( !m_pOctavesEdit->isDataValid( ) ||
              m_pOctavesEdit->value( ) < 1 ||
              m_pOctavesEdit->value( ) > 10 )
            return false;
         break;
      case 3:
      case 4:
      case 5:
      case 6:
         if( !m_pOrientationEdit->isDataValid( ) ||
             !m_pDistExpEdit->isDataValid( ) ||
             !m_pMajorRadiusEdit->isDataValid( ) )
            return false;
         break;
   }
   return Base::isDataValid( );
}

void PMWarpEdit::slotComboChanged( int c )
{
   switch ( c )
   {
      case 0:
         m_pRepeatWidget->show( );
         m_pBlackHoleWidget->hide( );
         m_pTurbulenceWidget->hide( );
         m_pMappingWidget->hide( );
         break;
      case 1:
         m_pRepeatWidget->hide( );
         m_pBlackHoleWidget->show( );
         m_pTurbulenceWidget->hide( );
         m_pMappingWidget->hide( );
         break;
      case 2:
         m_pRepeatWidget->hide( );
         m_pBlackHoleWidget->hide( );
         m_pTurbulenceWidget->show( );
         m_pMappingWidget->hide( );
         break;
      case 3:
      case 4:
      case 6:
         m_pRepeatWidget->hide( );
         m_pBlackHoleWidget->hide( );
         m_pTurbulenceWidget->hide( );
         m_pMappingWidget->show( );
         m_pMajorRadiusLabel->hide( );
         m_pMajorRadiusEdit->hide( );
         break;
      case 5:
         m_pRepeatWidget->hide( );
         m_pBlackHoleWidget->hide( );
         m_pTurbulenceWidget->hide( );
         m_pMappingWidget->show( );
         m_pMajorRadiusLabel->show( );
         m_pMajorRadiusEdit->show( );
         break;
   }
   emit dataChanged( );
   emit sizeChanged( );
}

#include "pmwarpedit.moc"
