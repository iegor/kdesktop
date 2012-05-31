/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2002 by Luis Passos Carvalho
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


#include "pmbumpmapedit.h"
#include "pmbumpmap.h"
#include "pmvectoredit.h"
#include "pmlineedits.h"
#include "pmpalettevalueedit.h"
#include "pmvector.h"

#include <qwidget.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <ktabctl.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdialog.h>
#include <kfiledialog.h>
#include <kiconloader.h>

PMBumpMapEdit::PMBumpMapEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMBumpMapEdit::createTopWidgets( )
{
   QLabel* lbl;
   QHBoxLayout* hl;

   Base::createTopWidgets( );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "File type:" ), this );
   m_pImageFileTypeEdit = new QComboBox( this );
   m_pImageFileTypeEdit->insertItem( "gif" );
   m_pImageFileTypeEdit->insertItem( "tga" );
   m_pImageFileTypeEdit->insertItem( "iff" );
   m_pImageFileTypeEdit->insertItem( "ppm" );
   m_pImageFileTypeEdit->insertItem( "pgm" );
   m_pImageFileTypeEdit->insertItem( "png" );
   m_pImageFileTypeEdit->insertItem( "jpeg" );
   m_pImageFileTypeEdit->insertItem( "tiff" );
   m_pImageFileTypeEdit->insertItem( "sys" );
   hl->addWidget( lbl );
   hl->addWidget( m_pImageFileTypeEdit );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "File name:" ), this );
   m_pImageFileNameEdit = new QLineEdit( this );
   m_pImageFileNameBrowse = new QPushButton( this );
   m_pImageFileNameBrowse->setPixmap( SmallIcon( "fileopen" ) );
   hl->addWidget( lbl );
   hl->addWidget( m_pImageFileNameEdit );
   hl->addWidget( m_pImageFileNameBrowse );
   hl->addStretch( 1 );

   m_pOnceEdit = new QCheckBox( i18n( "Once" ), this );
   topLayout( )->addWidget( m_pOnceEdit );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "Interpolate:" ), this );
   m_pInterpolateTypeEdit = new QComboBox( this );
   m_pInterpolateTypeEdit->insertItem( i18n( "None" ) );
   m_pInterpolateTypeEdit->insertItem( i18n( "Bilinear" ) );
   m_pInterpolateTypeEdit->insertItem( i18n( "Normalized" ) );
   hl->addWidget( lbl );
   hl->addWidget( m_pInterpolateTypeEdit );
   hl->addStretch( 1 );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "Map type:" ), this );
   m_pMapTypeEdit = new QComboBox( this );
   m_pMapTypeEdit->insertItem( i18n( "Planar" ) );
   m_pMapTypeEdit->insertItem( i18n( "Spherical" ) );
   m_pMapTypeEdit->insertItem( i18n( "Cylindrical" ) );
   m_pMapTypeEdit->insertItem( i18n( "Toroidal" ) );
   hl->addWidget( lbl );
   hl->addWidget( m_pMapTypeEdit );
   hl->addStretch( 1 );

   m_pUseIndexEdit = new QCheckBox( i18n( "Use index" ), this );
   topLayout( )->addWidget( m_pUseIndexEdit );

   hl = new QHBoxLayout( topLayout( ) );
   lbl = new QLabel( i18n( "Bump size:" ), this );
   m_pBumpSizeEdit = new PMFloatEdit( this );
   hl->addWidget( lbl );
   hl->addWidget( m_pBumpSizeEdit );
   hl->addStretch( 1 );

   connect( m_pImageFileTypeEdit, SIGNAL( activated( int ) ), SLOT( slotImageFileTypeChanged( int ) ) );
   connect( m_pMapTypeEdit, SIGNAL( activated( int ) ), SLOT( slotMapTypeChanged( int ) ) );
   connect( m_pInterpolateTypeEdit, SIGNAL( activated( int ) ), SLOT( slotInterpolateTypeChanged( int ) ) );
   connect( m_pImageFileNameBrowse, SIGNAL( clicked( ) ), SLOT( slotImageFileBrowseClicked( ) ) );
   connect( m_pImageFileNameEdit, SIGNAL( textChanged( const QString& ) ), SLOT( slotImageFileNameChanged( const QString& ) ) );
   connect( m_pOnceEdit, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pUseIndexEdit, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pBumpSizeEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
}

void PMBumpMapEdit::displayObject( PMObject* o )
{
   bool readOnly;

   if( o->isA( "BumpMap" ) )
   {
      m_pDisplayedObject = ( PMBumpMap* ) o;
      readOnly = m_pDisplayedObject->isReadOnly( );

      switch( m_pDisplayedObject->bitmapType( ) )
      {
         case PMBumpMap::BitmapGif:
            m_pImageFileTypeEdit->setCurrentItem( 0 );
            break;
         case PMBumpMap::BitmapTga:
            m_pImageFileTypeEdit->setCurrentItem( 1 );
            break;
         case PMBumpMap::BitmapIff:
            m_pImageFileTypeEdit->setCurrentItem( 2 );
            break;
         case PMBumpMap::BitmapPpm:
            m_pImageFileTypeEdit->setCurrentItem( 3 );
            break;
         case PMBumpMap::BitmapPgm:
            m_pImageFileTypeEdit->setCurrentItem( 4 );
            break;
         case PMBumpMap::BitmapPng:
            m_pImageFileTypeEdit->setCurrentItem( 5 );
            break;
         case PMBumpMap::BitmapJpeg:
            m_pImageFileTypeEdit->setCurrentItem( 6 );
            break;
         case PMBumpMap::BitmapTiff:
            m_pImageFileTypeEdit->setCurrentItem( 7 );
            break;
         case PMBumpMap::BitmapSys:
            m_pImageFileTypeEdit->setCurrentItem( 8 );
            break;
      }
      m_pImageFileTypeEdit->setEnabled( !readOnly );

      switch( m_pDisplayedObject->interpolateType( ) )
      {
         case PMBumpMap::InterpolateNone:
            m_pInterpolateTypeEdit->setCurrentItem( 0 );
            break;
         case PMBumpMap::InterpolateBilinear:
            m_pInterpolateTypeEdit->setCurrentItem( 1);
            break;
         case PMBumpMap::InterpolateNormalized:
            m_pInterpolateTypeEdit->setCurrentItem( 2 );
            break;
      }
      m_pInterpolateTypeEdit->setEnabled( !readOnly );

      switch( m_pDisplayedObject->mapType( ) )
      {
         case PMBumpMap::MapPlanar:
            m_pMapTypeEdit->setCurrentItem( 0 );
            break;
         case PMBumpMap::MapSpherical:
            m_pMapTypeEdit->setCurrentItem( 1 );
            break;
         case PMBumpMap::MapCylindrical:
            m_pMapTypeEdit->setCurrentItem( 2 );
            break;
         case PMBumpMap::MapToroidal:
            m_pMapTypeEdit->setCurrentItem( 3 );
            break;
      }
      m_pMapTypeEdit->setEnabled( !readOnly );

      m_pImageFileNameEdit->setText( m_pDisplayedObject->bitmapFile( ) );
      m_pImageFileNameEdit->setEnabled( !readOnly );
      m_pOnceEdit->setChecked( m_pDisplayedObject->isOnceEnabled( ) );
      m_pOnceEdit->setEnabled( !readOnly );
      m_pUseIndexEdit->setChecked( m_pDisplayedObject->isUseIndexEnabled( ) );
      m_pUseIndexEdit->setEnabled( !readOnly );
      m_pBumpSizeEdit->setValue( m_pDisplayedObject->bumpSize( ) );
      m_pBumpSizeEdit->setReadOnly( readOnly );

      Base::displayObject( o );
   }

}

void PMBumpMapEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      switch( m_pImageFileTypeEdit->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setBitmapType( PMBumpMap::BitmapGif );
            break;
         case 1:
            m_pDisplayedObject->setBitmapType( PMBumpMap::BitmapTga );
            break;
         case 2:
            m_pDisplayedObject->setBitmapType( PMBumpMap::BitmapIff );
            break;
         case 3:
            m_pDisplayedObject->setBitmapType( PMBumpMap::BitmapPpm );
            break;
         case 4:
            m_pDisplayedObject->setBitmapType( PMBumpMap::BitmapPgm );
            break;
         case 5:
            m_pDisplayedObject->setBitmapType( PMBumpMap::BitmapPng );
            break;
         case 6:
            m_pDisplayedObject->setBitmapType( PMBumpMap::BitmapJpeg );
            break;
         case 7:
            m_pDisplayedObject->setBitmapType( PMBumpMap::BitmapTiff );
            break;
         case 8:
            m_pDisplayedObject->setBitmapType( PMBumpMap::BitmapSys );
            break;
      }

      switch( m_pInterpolateTypeEdit->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setInterpolateType( PMBumpMap::InterpolateNone );
            break;
         case 1:
            m_pDisplayedObject->setInterpolateType( PMBumpMap::InterpolateBilinear );
            break;
         case 2:
            m_pDisplayedObject->setInterpolateType( PMBumpMap::InterpolateNormalized );
            break;
      }

      switch( m_pMapTypeEdit->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setMapType( PMBumpMap::MapPlanar );
            break;
         case 1:
            m_pDisplayedObject->setMapType( PMBumpMap::MapSpherical );
            break;
         case 2:
            m_pDisplayedObject->setMapType( PMBumpMap::MapCylindrical );
            break;
         case 3:
            m_pDisplayedObject->setMapType( PMBumpMap::MapToroidal );
            break;
      }

      m_pDisplayedObject->setBitmapFileName( m_pImageFileNameEdit->text( ) );
      m_pDisplayedObject->enableOnce( m_pOnceEdit->isChecked( ) );
      m_pDisplayedObject->enableUseIndex( m_pUseIndexEdit->isChecked( ) );
      m_pDisplayedObject->setBumpSize( m_pBumpSizeEdit->value( ) );
   }
}

bool PMBumpMapEdit::isDataValid( )
{
   if( !m_pBumpSizeEdit->isDataValid( ) ) return false;

   return Base::isDataValid( );
}

void PMBumpMapEdit::slotInterpolateTypeChanged( const int /*a*/ )
{
   emit dataChanged( );
}

void PMBumpMapEdit::slotImageFileTypeChanged( const int /*a*/ )
{
   emit dataChanged( );
}

void PMBumpMapEdit::slotMapTypeChanged( const int /*a*/ )
{
   emit dataChanged( );
}

void PMBumpMapEdit::slotImageFileNameChanged( const QString& /*a*/ )
{
   emit dataChanged( );
}

void PMBumpMapEdit::slotImageFileBrowseClicked( )
{
   QString str = KFileDialog::getOpenFileName( QString::null, QString::null );

   if( !str.isEmpty() )
   {
      m_pImageFileNameEdit->setText( str );
      emit dataChanged( );
   }
}

#include "pmbumpmapedit.moc"
