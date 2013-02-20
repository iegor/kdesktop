/*
**************************************************************************
                                 description
                             --------------------
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


#include "pmtexturemapedit.h"
#include "pmtexturemap.h"
#include "pmlineedits.h"

#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>
#include <kmessagebox.h>


PMTextureMapEdit::PMTextureMapEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
   m_numValues = 0;
}

void PMTextureMapEdit::createTopWidgets( )
{
   Base::createTopWidgets( );
   topLayout( )->addWidget( new QLabel( i18n( "Map values:" ), this ) );
   m_pNoChildLabel = new QLabel( i18n( "(No Child Objects)" ), this );
   m_pPureLinkLabel = new QLabel( i18n( "(Pure Link)" ), this );
   topLayout( )->addWidget( m_pNoChildLabel );
   topLayout( )->addWidget( m_pPureLinkLabel );
   QHBoxLayout* hl = new QHBoxLayout( topLayout( ) );
   m_pEditLayout = new QVBoxLayout( hl );
   hl->addStretch( 1 );
}

void PMTextureMapEdit::displayObject( PMObject* o )
{
   QString str;

   if( o->isA( "TextureMapBase" ) )
   {
      bool readOnly = o->isReadOnly( );
      m_pDisplayedObject = ( PMTextureMapBase* ) o;
      QValueList<double> mv = m_pDisplayedObject->mapValues( );
      QValueList<double>::Iterator vit = mv.begin( );
      QPtrListIterator<PMFloatEdit> eit( m_edits );
      PMFloatEdit* edit;

      m_numValues = 0;

      for( ; vit != mv.end( ); ++vit )
      {
         if( eit.current( ) )
         {
            eit.current( )->setValue( *vit );
            eit.current( )->show( );
            eit.current( )->setReadOnly( readOnly );
            ++eit;
         }
         else
         {
            edit = new PMFloatEdit( this );
            m_pEditLayout->addWidget( edit );
            m_edits.append( edit );
            edit->setValue( *vit );
            edit->setValidation( true, 0.0, true, 1.0 );
            edit->setReadOnly( readOnly );
            connect( edit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
         }
         m_numValues++;
      }
      for( ; eit.current( ); ++eit )
         eit.current( )->hide( );
      if( m_numValues == 0 )
      {
         if( o->linkedObject( ) )
         {
            m_pPureLinkLabel->show( );
            m_pNoChildLabel->hide( );
         }
         else
         {
            m_pPureLinkLabel->hide( );
            m_pNoChildLabel->show( );
         }
      }
      else
      {
         m_pNoChildLabel->hide( );
         m_pPureLinkLabel->hide( );
      }
   }
   else
      kdError( PMArea ) << "PMTextureMapEdit: Can't display object\n";
   Base::displayObject( o );
   enableLinkEdit( m_numValues == 0 );
}

void PMTextureMapEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      if( m_numValues > 0 )
      {
         QPtrListIterator<PMFloatEdit> it( m_edits );
         int i = 0;
         QValueList<double> values;
         
         for( ; ( i < m_numValues ) && it.current( ); ++i, ++it )
            values.append( it.current( )->value( ) );
         m_pDisplayedObject->setMapValues( values );
      }
      Base::saveContents( );
   }
}

bool PMTextureMapEdit::isDataValid( )
{
   QPtrListIterator<PMFloatEdit> it( m_edits );
   int i = 0;
   double last = 0.0;

   for( ; ( i < m_numValues ) && it.current( ); ++i, ++it )
   {
      if( !it.current( )->isDataValid( ) )
         return false;
      if( it.current( )->value( ) < last )
      {
         KMessageBox::error( this, i18n( "The map values have to be increasing." ),
                             i18n( "Error" ) );
         it.current( )->setFocus( );
         return false;
      }
      last = it.current( )->value( );
   }
   return Base::isDataValid( );
}

#include "pmtexturemapedit.moc"
