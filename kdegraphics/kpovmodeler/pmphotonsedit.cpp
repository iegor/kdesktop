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


#include "pmphotonsedit.h"
#include "pmphotons.h"
#include "pmlineedits.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <klocale.h>
#include <kdialog.h>
#include <kmessagebox.h>


PMPhotonsEdit::PMPhotonsEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMPhotonsEdit::createTopWidgets( )
{
   QGridLayout* gl;
	QHBoxLayout* hl;

   Base::createTopWidgets( );

   m_pLayoutWidget = new QWidget( this );
   m_pTarget = new QCheckBox( i18n( "Target" ), m_pLayoutWidget );
   m_pSpacingMultiLabel = new QLabel( i18n( "Spacing multiplier:" ), m_pLayoutWidget );
   m_pSpacingMulti = new PMFloatEdit( m_pLayoutWidget );
   m_pSpacingMulti->setValidation( true, 0, false, 0 );

   m_pRefraction = new QCheckBox( i18n( "Refraction" ), this );
   m_pReflection = new QCheckBox( i18n( "Reflection" ), this );
   m_pCollect = new QCheckBox( i18n( "Collect" ), this );
   m_pPassThrough = new QCheckBox( i18n( "Pass through" ), this );
   m_pAreaLight = new QCheckBox( i18n( "Area light" ), this );

	hl = new QHBoxLayout( m_pLayoutWidget, 0, KDialog::spacingHint( ) );
   gl = new QGridLayout( hl, 2, 2 );
   gl->addMultiCellWidget( m_pTarget, 0, 0, 0, 1 );
   gl->addWidget( m_pSpacingMultiLabel, 1, 0 );
   gl->addWidget( m_pSpacingMulti, 1, 1 );
	hl->addStretch( 1 );
   topLayout( )->addWidget( m_pLayoutWidget );

   gl = new QGridLayout( topLayout( ), 2, 2 );
   gl->addWidget( m_pRefraction, 0, 0 );
   gl->addWidget( m_pReflection, 0, 1 );
   gl->addWidget( m_pCollect, 1, 0 );
   gl->addWidget( m_pPassThrough, 1, 1 );
   gl->addWidget( m_pAreaLight, 1, 0 );

   connect( m_pTarget, SIGNAL( clicked( ) ), SLOT( slotTargetClicked( ) ) );
   connect( m_pSpacingMulti, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pRefraction, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pReflection, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pCollect, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pPassThrough, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
   connect( m_pAreaLight, SIGNAL( clicked( ) ), SIGNAL( dataChanged( ) ) );
}

void PMPhotonsEdit::displayObject( PMObject* o )
{
   if( o->isA( "Photons" ) )
   {
      bool readOnly =  o->isReadOnly( );
      m_pDisplayedObject = ( PMPhotons* ) o;

      if ( o->parent( ) && ( o->parent( )->isA( "Light" ) ) )
      {
         m_pLayoutWidget->hide( );
         m_pCollect->hide( );
         m_pPassThrough->hide( );
         m_pAreaLight->show( );
      }
      else
      {
         m_pLayoutWidget->show( );
         m_pCollect->show( );
         m_pPassThrough->show( );
         m_pAreaLight->hide( );
      }

      m_pTarget->setChecked( m_pDisplayedObject->target( ) );
      m_pTarget->setEnabled( !readOnly );
      m_pSpacingMulti->setValue( m_pDisplayedObject->spacingMulti( ) );
      m_pSpacingMulti->setReadOnly( readOnly );
      m_pRefraction->setChecked( m_pDisplayedObject->refraction( ) );
      m_pRefraction->setEnabled( !readOnly );
      m_pReflection->setChecked( m_pDisplayedObject->reflection( ) );
      m_pReflection->setEnabled( !readOnly );
      m_pCollect->setChecked( m_pDisplayedObject->collect( ) );
      m_pCollect->setEnabled( !readOnly );
      m_pPassThrough->setChecked( m_pDisplayedObject->passThrough( ) );
      m_pPassThrough->setEnabled( !readOnly );
      m_pAreaLight->setChecked( m_pDisplayedObject->areaLight( ) );
      m_pAreaLight->setEnabled( !readOnly );

      slotTargetClicked( );

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMPhotonsEdit: Can't display object\n";
}

void PMPhotonsEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setTarget( m_pTarget->isChecked( ) );
      m_pDisplayedObject->setSpacingMulti( m_pSpacingMulti->value( ) );
      m_pDisplayedObject->setRefraction( m_pRefraction->isChecked( ) );
      m_pDisplayedObject->setReflection( m_pReflection->isChecked( ) );
      m_pDisplayedObject->setCollect( m_pCollect->isChecked( ) );
      m_pDisplayedObject->setPassThrough( m_pPassThrough->isChecked( ) );
      m_pDisplayedObject->setAreaLight( m_pAreaLight->isChecked( ) );
   }
}

bool PMPhotonsEdit::isDataValid( )
{
   if( !m_pSpacingMulti->isDataValid( ) ) return false;

   return Base::isDataValid( );
}

void PMPhotonsEdit::slotTargetClicked( )
{
   if ( m_pTarget->isChecked( ) && m_pTarget->isEnabled( ) )
   {
      m_pSpacingMulti->setEnabled( true );
   }
   else
   {
      m_pSpacingMulti->setEnabled( false );
   }
	emit dataChanged( );
}

#include "pmphotonsedit.moc"
