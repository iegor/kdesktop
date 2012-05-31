/*
**************************************************************************
                               description
                           --------------------
  copyright            : (C) 2004 by Leon Pennington
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
#include "pmdetailobjectedit.h"
#include "pmdetailobject.h"

#include <qlayout.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <klocale.h>

PMDetailObjectEdit::PMDetailObjectEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMDetailObjectEdit::createTopWidgets( )
{
   Base::createTopWidgets( );

   QHBoxLayout* layout = new QHBoxLayout( topLayout( ) );
   m_pGlobalDetail = new QCheckBox( i18n( "Global detail" ), this );
   m_pLocalDetailLevelLabel = new QLabel( i18n( "Detail level:" ), this );
   m_pLocalDetailLevel = new QComboBox( this );
   m_pLocalDetailLevel->insertItem( i18n( "Very Low" ) );
   m_pLocalDetailLevel->insertItem( i18n( "Low" ) );
   m_pLocalDetailLevel->insertItem( i18n( "Medium" ) );
   m_pLocalDetailLevel->insertItem( i18n( "High" ) );
   m_pLocalDetailLevel->insertItem( i18n( "Very High" ) );

   layout->addWidget( m_pGlobalDetail );
   layout->addWidget( m_pLocalDetailLevelLabel );
   layout->addWidget( m_pLocalDetailLevel );
   layout->addStretch( );

   connect( m_pGlobalDetail, SIGNAL( clicked( ) ), SLOT( slotGlobalDetailClicked( ) ) );
   connect( m_pLocalDetailLevel, SIGNAL( activated( int ) ), SIGNAL( dataChanged( ) ) );
}

void PMDetailObjectEdit::displayObject( PMObject* o )
{
   if( o->isA( "DetailObject" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_pDisplayedObject = ( PMDetailObject* ) o;

      if( !m_pDisplayedObject->hasDisplayDetail( ) )
      {
         m_pGlobalDetail->hide( );
         m_pLocalDetailLevelLabel->hide( );
         m_pLocalDetailLevel->hide( );
      }

      if( m_pDisplayedObject->globalDetail( ) )
      {
         m_pGlobalDetail->setChecked( true );
         m_pLocalDetailLevelLabel->setEnabled( false );
         m_pLocalDetailLevel->setEnabled( false );
      }
      else
      {
         m_pGlobalDetail->setChecked( false );
         m_pLocalDetailLevelLabel->setEnabled( !readOnly );
         m_pLocalDetailLevel->setEnabled( !readOnly );
      }
      m_pGlobalDetail->setEnabled( !readOnly );

      m_pLocalDetailLevel->setCurrentItem( m_pDisplayedObject->localDetailLevel( ) - 1 );

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMNamedObjectEdit: Can't display object\n";
}

void PMDetailObjectEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setGlobalDetail( m_pGlobalDetail->isChecked( ) );
      m_pDisplayedObject->setLocalDetailLevel( m_pLocalDetailLevel->currentItem( ) + 1 );
   }
}

bool PMDetailObjectEdit::isDataValid( )
{
   return Base::isDataValid( );
}

void PMDetailObjectEdit::slotGlobalDetailClicked( )
{
   if( m_pGlobalDetail->isChecked( ) )
   {
      m_pLocalDetailLevelLabel->setEnabled( false );
      m_pLocalDetailLevel->setEnabled( false );
   }
   else
   {
      bool readOnly = m_pDisplayedObject->isReadOnly( );
      m_pLocalDetailLevelLabel->setEnabled( !readOnly );
      m_pLocalDetailLevel->setEnabled( !readOnly );
   }
   emit dataChanged( );
}

#include "pmdetailobjectedit.moc"
