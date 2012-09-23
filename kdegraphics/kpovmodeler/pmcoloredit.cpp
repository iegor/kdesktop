/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2001 by Andreas Zehender
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

#include "pmcoloredit.h"
#include "pmlineedits.h"
#include <kcolorbutton.h>
#include <klocale.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qcolor.h>
#include <kdialog.h>

PMColorEdit::PMColorEdit( bool filterAndTransmit, QWidget* parent, const char* name )
      : QWidget( parent, name )
{
   m_bFilterAndTransmit = filterAndTransmit;

   m_edits[0] = new PMFloatEdit( this );
   m_edits[1] = new PMFloatEdit( this );
   m_edits[2] = new PMFloatEdit( this );
   if( filterAndTransmit )
   {
      m_edits[3] = new PMFloatEdit( this );
      m_edits[4] = new PMFloatEdit( this );
   }
   else
   {
      m_edits[3] = 0;
      m_edits[4] = 0;
   }
   m_pButton = new KColorButton( this );

   QVBoxLayout* topLayout = new QVBoxLayout( this, 0, KDialog::spacingHint( ) );
   QHBoxLayout* l = new QHBoxLayout( topLayout );
   l->addWidget( m_pButton );
   l = new QHBoxLayout( topLayout );
   l->addWidget( new QLabel( i18n( "red:" ), this ) );
   l->addWidget( m_edits[0] );
   l->addWidget( new QLabel( i18n( "green:" ), this ) );
   l->addWidget( m_edits[1] );
   l->addWidget( new QLabel( i18n( "blue:" ), this ) );
   l->addWidget( m_edits[2] );
   if( filterAndTransmit )
   {
      l = new QHBoxLayout( topLayout );
      l->addWidget( new QLabel( i18n( "filter" ), this ) );
      l->addWidget( m_edits[3] );
      l->addWidget( new QLabel( i18n( "transmit" ), this ) );
      l->addWidget( m_edits[4] );
   }

   connect( m_edits[0], SIGNAL( dataChanged( ) ), SLOT( slotEditChanged( ) ) );
   connect( m_edits[1], SIGNAL( dataChanged( ) ), SLOT( slotEditChanged( ) ) );
   connect( m_edits[2], SIGNAL( dataChanged( ) ), SLOT( slotEditChanged( ) ) );
   if( filterAndTransmit )
   {
      connect( m_edits[3], SIGNAL( dataChanged( ) ), SLOT( slotEditChanged( ) ) );
      connect( m_edits[4], SIGNAL( dataChanged( ) ), SLOT( slotEditChanged( ) ) );
   }
   connect( m_pButton, SIGNAL( changed( const QColor& ) ),
            SLOT( slotColorChanged( const QColor& ) ) );
}

void PMColorEdit::setColor( const PMColor& c )
{
   bool blocked[5];
   int i;
   int num = m_bFilterAndTransmit ? 5 : 3;
   
   for( i = 0; i < num; i++ )
   {
      blocked[i] = m_edits[i]->signalsBlocked( );
      m_edits[i]->blockSignals( true );
   }

   m_color = c;
   m_edits[0]->setValue( c.red( ) );
   m_edits[1]->setValue( c.green( ) );
   m_edits[2]->setValue( c.blue( ) );
   if( m_bFilterAndTransmit )
   {
      m_edits[3]->setValue( c.filter( ) );
      m_edits[4]->setValue( c.transmit( ) );
   }
   updateButton( );

   for( i = 0; i < num; i++ )
      m_edits[i]->blockSignals( blocked[i] );
}

void PMColorEdit::updateButton( )
{
   bool b = m_pButton->signalsBlocked( );
   m_pButton->blockSignals( true );
   m_pButton->setColor( m_color.toQColor( ) );
   m_pButton->blockSignals( b );
}

bool PMColorEdit::isDataValid( )
{
   if( !m_edits[0]->isDataValid( ) )
      return false;
   if( !m_edits[1]->isDataValid( ) )
      return false;
   if( !m_edits[2]->isDataValid( ) )
      return false;
   if( m_bFilterAndTransmit )
   {
      if( !m_edits[3]->isDataValid( ) )
         return false;
      if( !m_edits[4]->isDataValid( ) )
         return false;
   }
   return true;
}

void PMColorEdit::setReadOnly( bool yes )
{
   m_edits[0]->setReadOnly( yes );
   m_edits[1]->setReadOnly( yes );
   m_edits[2]->setReadOnly( yes );
   if( m_bFilterAndTransmit )
   {
      m_edits[3]->setReadOnly( yes );
      m_edits[4]->setReadOnly( yes );
   }
   m_pButton->setEnabled( !yes );
}

void PMColorEdit::slotColorChanged( const QColor& c )
{
   int i;
   bool blocked[3];
   
   for( i = 0; i < 3; i++ )
   {
      blocked[i] = m_edits[i]->signalsBlocked( );
      m_edits[i]->blockSignals( true );
   }
   
   m_color.setRed( c.red( ) / 255.0 );
   m_color.setGreen( c.green( ) / 255.0 );
   m_color.setBlue( c.blue( ) / 255.0 );

   m_edits[0]->setValue( m_color.red( ) );
   m_edits[1]->setValue( m_color.green( ) );
   m_edits[2]->setValue( m_color.blue( ) );
   
   for( i = 0; i < 3; i++ )
      m_edits[i]->blockSignals( blocked[i] );

   emit dataChanged( );
}

void PMColorEdit::slotEditChanged( )
{
   bool ok;

   m_edits[0]->text( ).toDouble( &ok );
   if( ok )
      m_edits[1]->text( ).toDouble( &ok );
   if( ok )
      m_edits[2]->text( ).toDouble( &ok );
   if( m_bFilterAndTransmit )
   {
      if( ok )
         m_edits[3]->text( ).toDouble( &ok );
      if( ok )
         m_edits[4]->text( ).toDouble( &ok );
   }

   if( ok )
   {
      m_color.setRed( m_edits[0]->value( ) );
      m_color.setGreen( m_edits[1]->value( ) );
      m_color.setBlue( m_edits[2]->value( ) );
      if( m_bFilterAndTransmit )
      {
         m_color.setFilter( m_edits[3]->value( ) );
         m_color.setTransmit( m_edits[4]->value( ) );
      }
      updateButton( );
   }
   emit dataChanged( );
}

#include "pmcoloredit.moc"
