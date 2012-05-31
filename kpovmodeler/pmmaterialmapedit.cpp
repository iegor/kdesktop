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


#include "pmmaterialmapedit.h"
#include "pmmaterialmap.h"
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

PMMaterialMapEdit::PMMaterialMapEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMMaterialMapEdit::createTopWidgets( )
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

   connect( m_pImageFileTypeEdit, SIGNAL( activated( int ) ), SLOT( slotImageFileTypeChanged( int ) ) );
   connect( m_pMapTypeEdit, SIGNAL( activated( int ) ), SLOT( slotMapTypeChanged( int ) ) );
   connect( m_pInterpolateTypeEdit, SIGNAL( activated( int ) ), SLOT( slotInterpolateTypeChanged( int ) ) );
   connect( m_pImageFileNameBrowse, SIGNAL( clicked( ) ), SLOT( slotImageFileBrowseClicked( ) ) );
   connect( m_pImageFileNameEdit, SIGNAL( textChanged( const QString& ) ), SLOT( slotImageFileNameChanged( const QString& ) ) );
   connect( m_pOnceEdit, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
}

void PMMaterialMapEdit::displayObject( PMObject* o )
{
   bool readOnly;

   if( o->isA( "MaterialMap" ) )
   {
      m_pDisplayedObject = ( PMMaterialMap* ) o;
      readOnly = m_pDisplayedObject->isReadOnly( );

      switch( m_pDisplayedObject->bitmapType( ) )
      {
         case PMMaterialMap::BitmapGif:
            m_pImageFileTypeEdit->setCurrentItem( 0 );
            break;
         case PMMaterialMap::BitmapTga:
            m_pImageFileTypeEdit->setCurrentItem( 1 );
            break;
         case PMMaterialMap::BitmapIff:
            m_pImageFileTypeEdit->setCurrentItem( 2 );
            break;
         case PMMaterialMap::BitmapPpm:
            m_pImageFileTypeEdit->setCurrentItem( 3 );
            break;
         case PMMaterialMap::BitmapPgm:
            m_pImageFileTypeEdit->setCurrentItem( 4 );
            break;
         case PMMaterialMap::BitmapPng:
            m_pImageFileTypeEdit->setCurrentItem( 5 );
            break;
         case PMMaterialMap::BitmapJpeg:
            m_pImageFileTypeEdit->setCurrentItem( 6 );
            break;
         case PMMaterialMap::BitmapTiff:
            m_pImageFileTypeEdit->setCurrentItem( 7 );
            break;
         case PMMaterialMap::BitmapSys:
            m_pImageFileTypeEdit->setCurrentItem( 8 );
            break;
      }
      m_pImageFileTypeEdit->setEnabled( !readOnly );

      switch( m_pDisplayedObject->interpolateType( ) )
      {
         case PMMaterialMap::InterpolateNone:
            m_pInterpolateTypeEdit->setCurrentItem( 0 );
            break;
         case PMMaterialMap::InterpolateBilinear:
            m_pInterpolateTypeEdit->setCurrentItem( 1);
            break;
         case PMMaterialMap::InterpolateNormalized:
            m_pInterpolateTypeEdit->setCurrentItem( 2 );
            break;
      }
      m_pInterpolateTypeEdit->setEnabled( !readOnly );

      switch( m_pDisplayedObject->mapType( ) )
      {
         case PMMaterialMap::MapPlanar:
            m_pMapTypeEdit->setCurrentItem( 0 );
            break;
         case PMMaterialMap::MapSpherical:
            m_pMapTypeEdit->setCurrentItem( 1 );
            break;
         case PMMaterialMap::MapCylindrical:
            m_pMapTypeEdit->setCurrentItem( 2 );
            break;
         case PMMaterialMap::MapToroidal:
            m_pMapTypeEdit->setCurrentItem( 3 );
            break;
      }
      m_pMapTypeEdit->setEnabled( !readOnly );

      m_pImageFileNameEdit->setText( m_pDisplayedObject->bitmapFile( ) );
      m_pImageFileNameEdit->setEnabled( !readOnly );
      m_pOnceEdit->setChecked( m_pDisplayedObject->isOnceEnabled( ) );
      m_pOnceEdit->setEnabled( !readOnly );

      Base::displayObject( o );
   }
    
}

void PMMaterialMapEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      switch( m_pImageFileTypeEdit->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setBitmapType( PMMaterialMap::BitmapGif );
            break;
         case 1:
            m_pDisplayedObject->setBitmapType( PMMaterialMap::BitmapTga );
            break;
         case 2:
            m_pDisplayedObject->setBitmapType( PMMaterialMap::BitmapIff );
            break;
         case 3:
            m_pDisplayedObject->setBitmapType( PMMaterialMap::BitmapPpm );
            break;
         case 4:
            m_pDisplayedObject->setBitmapType( PMMaterialMap::BitmapPgm );
            break;
         case 5:
            m_pDisplayedObject->setBitmapType( PMMaterialMap::BitmapPng );
            break;
         case 6:
            m_pDisplayedObject->setBitmapType( PMMaterialMap::BitmapJpeg );
            break;
         case 7:
            m_pDisplayedObject->setBitmapType( PMMaterialMap::BitmapTiff );
            break;
         case 8:
            m_pDisplayedObject->setBitmapType( PMMaterialMap::BitmapSys );
            break;
      }

      switch( m_pInterpolateTypeEdit->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setInterpolateType( PMMaterialMap::InterpolateNone );
            break;
         case 1:
            m_pDisplayedObject->setInterpolateType( PMMaterialMap::InterpolateBilinear );
            break;
         case 2:
            m_pDisplayedObject->setInterpolateType( PMMaterialMap::InterpolateNormalized );
            break;
      }

      switch( m_pMapTypeEdit->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setMapType( PMMaterialMap::MapPlanar );
            break;
         case 1:
            m_pDisplayedObject->setMapType( PMMaterialMap::MapSpherical );
            break;
         case 2:
            m_pDisplayedObject->setMapType( PMMaterialMap::MapCylindrical );
            break;
         case 3:
            m_pDisplayedObject->setMapType( PMMaterialMap::MapToroidal );
            break;
      }

      m_pDisplayedObject->setBitmapFileName( m_pImageFileNameEdit->text( ) );
      m_pDisplayedObject->enableOnce( m_pOnceEdit->isChecked( ) );
   }
}

bool PMMaterialMapEdit::isDataValid( )
{
   return Base::isDataValid( );
}

void PMMaterialMapEdit::slotInterpolateTypeChanged( const int /*a*/ )
{
   emit dataChanged( );
}

void PMMaterialMapEdit::slotImageFileTypeChanged( const int /*a*/ )
{
   emit dataChanged( );
}

void PMMaterialMapEdit::slotMapTypeChanged( const int /*a*/ )
{
   emit dataChanged( );
}

void PMMaterialMapEdit::slotImageFileNameChanged( const QString& /*a*/ )
{
   emit dataChanged( );
}

void PMMaterialMapEdit::slotImageFileBrowseClicked( )
{
   QString str = KFileDialog::getOpenFileName( QString::null, QString::null );

   if( !str.isEmpty() )
   {
      m_pImageFileNameEdit->setText( str );
      emit dataChanged( );
   }
}

#include "pmmaterialmapedit.moc"
