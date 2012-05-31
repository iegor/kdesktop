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


#include "pmheightfieldedit.h"
#include "pmheightfield.h"
#include "pmlineedits.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>

PMHeightFieldEdit::PMHeightFieldEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMHeightFieldEdit::createTopWidgets( )
{
   Base::createTopWidgets( );

   QHBoxLayout* hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( new QLabel( i18n( "Type:" ), this ) );
   m_pHeightFieldType = new QComboBox( false, this );
   hl->addWidget( m_pHeightFieldType );
   hl->addStretch( 0 );
   m_pHeightFieldType->insertItem( "gif" );
   m_pHeightFieldType->insertItem( "tga" );
   m_pHeightFieldType->insertItem( "pot" );
   m_pHeightFieldType->insertItem( "png" );
   m_pHeightFieldType->insertItem( "pgm" );
   m_pHeightFieldType->insertItem( "ppm" );
   m_pHeightFieldType->insertItem( "sys" );

   hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( new QLabel( i18n( "File name:" ), this ) );
   m_pFileName = new QLineEdit( this );
   hl->addWidget( m_pFileName );
   m_pChooseFileName = new QPushButton( this );
   m_pChooseFileName->setPixmap( SmallIcon( "fileopen" ) );
   hl->addWidget( m_pChooseFileName );

   hl = new QHBoxLayout( topLayout( ) );
   hl->addWidget( new QLabel( i18n( "Water level:" ), this ) );
   m_pWaterLevel = new PMFloatEdit( this );
   m_pWaterLevel->setValidation( true, 0.0, true, 1.0 );
   hl->addWidget( m_pWaterLevel );
   hl->addStretch( 1 );

   m_pHierarchy = new QCheckBox( i18n( "Hierarchy" ), this );
   topLayout( )->addWidget( m_pHierarchy );

   m_pSmooth = new QCheckBox( i18n( "Smooth" ), this );
   topLayout( )->addWidget( m_pSmooth );

   connect( m_pHeightFieldType, SIGNAL( activated( int ) ),
            SLOT( slotTypeChanged( int ) ) );
   connect( m_pFileName, SIGNAL( textChanged( const QString& ) ),
            SLOT( slotFileNameChanged( const QString& ) ) );
   connect( m_pChooseFileName, SIGNAL( clicked( ) ),
            SLOT( slotFileNameClicked( ) ) );
   connect( m_pWaterLevel, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pHierarchy, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pSmooth, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
}

void PMHeightFieldEdit::displayObject( PMObject* o )
{
   if( o->isA( "HeightField" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_pDisplayedObject = ( PMHeightField* ) o;

      switch( m_pDisplayedObject->heightFieldType( ) )
      {
         case PMHeightField::HFgif:
            m_pHeightFieldType->setCurrentItem( 0 );
            break;
         case PMHeightField::HFtga:
            m_pHeightFieldType->setCurrentItem( 1 );
            break;
         case PMHeightField::HFpot:
            m_pHeightFieldType->setCurrentItem( 2 );
            break;
         case PMHeightField::HFpng:
            m_pHeightFieldType->setCurrentItem( 3 );
            break;
         case PMHeightField::HFpgm:
            m_pHeightFieldType->setCurrentItem( 4 );
            break;
         case PMHeightField::HFppm:
            m_pHeightFieldType->setCurrentItem( 5 );
            break;
         case PMHeightField::HFsys:
            m_pHeightFieldType->setCurrentItem( 6 );
            break;
      }
      m_pFileName->setText( m_pDisplayedObject->fileName( ) );
      m_pWaterLevel->setValue( m_pDisplayedObject->waterLevel( ) );
      m_pHierarchy->setChecked( m_pDisplayedObject->hierarchy( ) );
      m_pSmooth->setChecked( m_pDisplayedObject->smooth( ) );

      m_pHeightFieldType->setEnabled( !readOnly );
      m_pFileName->setReadOnly( readOnly );
      m_pChooseFileName->setEnabled( !readOnly );
      m_pHierarchy->setEnabled( !readOnly );
      m_pSmooth->setEnabled( !readOnly );

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMHeightFieldEdit: Can't display object\n";
}

void PMHeightFieldEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      switch( m_pHeightFieldType->currentItem( ) )
      {
         case 0:
            m_pDisplayedObject->setHeightFieldType( PMHeightField::HFgif );
            break;
         case 1:
            m_pDisplayedObject->setHeightFieldType( PMHeightField::HFtga );
            break;
         case 2:
            m_pDisplayedObject->setHeightFieldType( PMHeightField::HFpot );
            break;
         case 3:
            m_pDisplayedObject->setHeightFieldType( PMHeightField::HFpng );
            break;
         case 4:
            m_pDisplayedObject->setHeightFieldType( PMHeightField::HFpgm );
            break;
         case 5:
            m_pDisplayedObject->setHeightFieldType( PMHeightField::HFppm );
            break;
         case 6:
            m_pDisplayedObject->setHeightFieldType( PMHeightField::HFsys );
            break;
      }
      m_pDisplayedObject->setFileName( m_pFileName->text( ) );
      m_pDisplayedObject->setWaterLevel( m_pWaterLevel->value( ) );
      m_pDisplayedObject->setHierarchy( m_pHierarchy->isChecked( ) );
      m_pDisplayedObject->setSmooth( m_pSmooth->isChecked( ) );
   }
}

bool PMHeightFieldEdit::isDataValid( )
{
   if( m_pWaterLevel->isDataValid( ) )
      return Base::isDataValid( );
   return false;
}

void PMHeightFieldEdit::slotTypeChanged( int )
{
   emit dataChanged( );
}

void PMHeightFieldEdit::slotFileNameChanged( const QString& )
{
   emit dataChanged( );
}

void PMHeightFieldEdit::slotFileNameClicked( )
{
   QString str = KFileDialog::getOpenFileName( QString::null, QString::null );

   if( !str.isEmpty() )
   {
      m_pFileName->setText( str );
      emit dataChanged( );
   }
}

#include "pmheightfieldedit.moc"
