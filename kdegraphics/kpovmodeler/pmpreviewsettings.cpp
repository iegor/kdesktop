/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2003 by Andreas Zehender
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

#include "pmpreviewsettings.h"

#include "pmlineedits.h"
#include "pmdialogeditbase.h"
#include "pmdefaults.h"

#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <kcolorbutton.h>
#include <klocale.h>
#include <kmessagebox.h>

PMPreviewSettings::PMPreviewSettings( QWidget* parent, const char* name )
      : PMSettingsDialogPage( parent, name )
{
   QHBoxLayout* hlayout;
   QVBoxLayout* vlayout;
   QVBoxLayout* gvl;
   QGridLayout* grid;
   QGroupBox* gb;

   vlayout = new QVBoxLayout( this, 0, KDialog::spacingHint( ) );
   
   hlayout = new QHBoxLayout( vlayout );
   grid = new QGridLayout( hlayout, 2, 2 );
   grid->addWidget( new QLabel( i18n( "Size:" ), this ), 0, 0 );
   m_pPreviewSize = new PMIntEdit( this );
   m_pPreviewSize->setValidation( true, 10, true, 400 );
   grid->addWidget( m_pPreviewSize, 0, 1 );

   grid->addWidget( new QLabel( i18n( "Gamma:" ), this ), 1, 0 );
   m_pPreviewGamma = new PMFloatEdit( this );
   grid->addWidget( m_pPreviewGamma, 1, 1 );
   hlayout->addStretch( 1 );

   gb = new QGroupBox( i18n( "Rendered Objects" ), this );
   vlayout->addWidget( gb );
   gvl = new QVBoxLayout( gb, KDialog::marginHint( ), KDialog::spacingHint( ) );
   gvl->addSpacing( 10 );
   m_pPreviewSphere = new QCheckBox( i18n( "Sphere" ), gb );
   gvl->addWidget( m_pPreviewSphere );
   m_pPreviewCylinder = new QCheckBox( i18n( "Cylinder" ), gb );
   gvl->addWidget( m_pPreviewCylinder );
   m_pPreviewBox = new QCheckBox( i18n( "Box" ), gb );
   gvl->addWidget( m_pPreviewBox );

   gb = new QGroupBox( i18n( "Wall" ), this );
   vlayout->addWidget( gb );
   gvl = new QVBoxLayout( gb, KDialog::marginHint( ), KDialog::spacingHint( ) );
   gvl->addSpacing( 10 );
   m_pPreviewWall = new QCheckBox( i18n( "Enable wall" ), gb );
   gvl->addWidget( m_pPreviewWall );
   hlayout = new QHBoxLayout( gvl );
   hlayout->addWidget( new QLabel( i18n( "Color 1:" ), gb ) );
   m_pWallColor1 = new KColorButton( gb );
   hlayout->addWidget( m_pWallColor1 );
   hlayout->addWidget( new QLabel( i18n( "Color 2:" ), gb ) );
   m_pWallColor2 = new KColorButton( gb );
   hlayout->addWidget( m_pWallColor2 );
   hlayout->addStretch( 1 );

   gb = new QGroupBox( i18n( "Floor" ), this );
   vlayout->addWidget( gb );
   gvl = new QVBoxLayout( gb, KDialog::marginHint( ), KDialog::spacingHint( ) );
   gvl->addSpacing( 10 );
   m_pPreviewFloor = new QCheckBox( i18n( "Enable floor" ), gb );
   gvl->addWidget( m_pPreviewFloor );
   hlayout = new QHBoxLayout( gvl );
   hlayout->addWidget( new QLabel( i18n( "Color 1:" ), gb ) );
   m_pFloorColor1 = new KColorButton( gb );
   hlayout->addWidget( m_pFloorColor1 );
   hlayout->addWidget( new QLabel( i18n( "Color 2:" ), gb ) );
   m_pFloorColor2 = new KColorButton( gb );
   hlayout->addWidget( m_pFloorColor2 );
   hlayout->addStretch( 1 );

   gb = new QGroupBox( i18n( "Antialiasing" ), this );
   vlayout->addWidget( gb );
   gvl = new QVBoxLayout( gb, KDialog::marginHint( ), KDialog::spacingHint( ) );
   gvl->addSpacing( 10 );
   m_pPreviewAA = new QCheckBox( i18n( "Enable antialiasing" ), gb );
   gvl->addWidget( m_pPreviewAA );
   hlayout = new QHBoxLayout( gvl );
   hlayout->addWidget( new QLabel( i18n( "Depth:" ), gb ) );
   m_pPreviewAALevel = new PMIntEdit( gb );
   m_pPreviewAALevel->setValidation( true, 1, true, 9 );
   hlayout->addWidget( m_pPreviewAALevel );
   hlayout->addWidget( new QLabel( i18n( "Threshold:" ), gb ) );
   m_pPreviewAAThreshold = new PMFloatEdit( gb );
   hlayout->addWidget( m_pPreviewAAThreshold );
   hlayout->addStretch( 1 );
   
   vlayout->addStretch( 1 );
}

void PMPreviewSettings::displaySettings( )
{
   m_pPreviewSize->setValue( PMDialogEditBase::previewSize( ) );
   m_pPreviewGamma->setValue( PMDialogEditBase::previewGamma( ) );
   m_pPreviewSphere->setChecked( PMDialogEditBase::previewShowSphere( ) );
   m_pPreviewCylinder->setChecked( PMDialogEditBase::previewShowCylinder( ) );
   m_pPreviewBox->setChecked( PMDialogEditBase::previewShowBox( ) );
   m_pPreviewAA->setChecked( PMDialogEditBase::isPreviewAAEnabled( ) );
   m_pPreviewAALevel->setValue( PMDialogEditBase::previewAADepth( ) );
   m_pPreviewAAThreshold->setValue( PMDialogEditBase::previewAAThreshold( ) );
   m_pPreviewWall->setChecked( PMDialogEditBase::previewShowWall( ) );
   m_pPreviewFloor->setChecked( PMDialogEditBase::previewShowFloor( ) );
   m_pFloorColor1->setColor( PMDialogEditBase::previewFloorColor1( ) );
   m_pFloorColor2->setColor( PMDialogEditBase::previewFloorColor2( ) );
   m_pWallColor1->setColor( PMDialogEditBase::previewWallColor1( ) );
   m_pWallColor2->setColor( PMDialogEditBase::previewWallColor2( ) );
}

void PMPreviewSettings::displayDefaults( )
{
   m_pPreviewSize->setValue( c_defaultTPSize );
   m_pPreviewGamma->setValue( c_defaultTPGamma );
   m_pPreviewSphere->setChecked( c_defaultTPShowSphere );
   m_pPreviewCylinder->setChecked( c_defaultTPShowCylinder );
   m_pPreviewBox->setChecked( c_defaultTPShowBox );
   m_pPreviewAA->setChecked( c_defaultTPAA );
   m_pPreviewAALevel->setValue( c_defaultTPAADepth );
   m_pPreviewAAThreshold->setValue( c_defaultTPAAThreshold );
   m_pPreviewWall->setChecked( c_defaultTPShowWall );
   m_pPreviewFloor->setChecked( c_defaultTPShowFloor );
   m_pFloorColor1->setColor( c_defaultTPFloorColor1 );
   m_pFloorColor2->setColor( c_defaultTPFloorColor2 );
   m_pWallColor1->setColor( c_defaultTPWallColor1 );
   m_pWallColor2->setColor( c_defaultTPWallColor2 );
}

bool PMPreviewSettings::validateData( )
{
   if( !m_pPreviewSize->isDataValid( ) )
   {
      emit showMe( );
      m_pPreviewSize->setFocus( );
      return false;
   }
   if( !m_pPreviewGamma->isDataValid( ) )
   {
      emit showMe( );
      m_pPreviewGamma->setFocus( );
      return false;
   }
   if( !m_pPreviewAALevel->isDataValid( ) )
   {
      emit showMe( );
      m_pPreviewAALevel->setFocus( );
      return false;
   }
   if( !m_pPreviewAAThreshold->isDataValid( ) )
   {
      emit showMe( );
      m_pPreviewAAThreshold->setFocus( );
      return false;
   }
   if( !( m_pPreviewSphere->isChecked( ) || m_pPreviewCylinder->isChecked( )
          || m_pPreviewBox->isChecked( ) ) )
   {
      emit showMe( );
      KMessageBox::error( this, i18n( "At least one object has to be selected." ),
                          i18n( "Error" ) );
  
      return false;
   }
   return true;
}

void PMPreviewSettings::applySettings( )
{
   PMDialogEditBase::setPreviewSize( m_pPreviewSize->value( ) );
   PMDialogEditBase::setPreviewGamma( m_pPreviewGamma->value( ) );
   PMDialogEditBase::previewShowSphere( m_pPreviewSphere->isChecked( ) );
   PMDialogEditBase::previewShowCylinder( m_pPreviewCylinder->isChecked( ) );
   PMDialogEditBase::previewShowBox( m_pPreviewBox->isChecked( ) );
   PMDialogEditBase::setPreviewAAEnabled( m_pPreviewAA->isChecked( ) );
   PMDialogEditBase::setPreviewAADepth( m_pPreviewAALevel->value( ) );
   PMDialogEditBase::setPreviewAAThreshold( m_pPreviewAAThreshold->value( ) );
   PMDialogEditBase::previewShowFloor( m_pPreviewFloor->isChecked( ) );
   PMDialogEditBase::previewShowWall( m_pPreviewWall->isChecked( ) );
   PMDialogEditBase::setPreviewWallColor1( m_pWallColor1->color( ) );
   PMDialogEditBase::setPreviewWallColor2( m_pWallColor2->color( ) );
   PMDialogEditBase::setPreviewFloorColor1( m_pFloorColor1->color( ) );
   PMDialogEditBase::setPreviewFloorColor2( m_pFloorColor2->color( ) );
}

#include "pmpreviewsettings.moc"
