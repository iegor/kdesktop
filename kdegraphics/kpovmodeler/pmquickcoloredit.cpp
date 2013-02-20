/*
**************************************************************************
                                 description
                             --------------------
    copyright            : (C) 2000-2001 by Leonardo Skorianez
    email                : lsk@if.ufrj.br
**************************************************************************

**************************************************************************
*                                                                        *
*  This program is free software; you can redistribute it and/or modify  *
*  it under the terms of the GNU General Public License as published by  *
*  the Free Software Foundation; either version 2 of the License, or     *
*  (at your option) any later version.                                   *
*                                                                        *
**************************************************************************/


#include "pmquickcoloredit.h"
#include "pmquickcolor.h"
#include "pmcoloredit.h"
#include "pmdebug.h"

#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>


PMQuickColorEdit::PMQuickColorEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMQuickColorEdit::createTopWidgets( )
{
   Base::createTopWidgets( );

   QHBoxLayout* layout = new QHBoxLayout( topLayout( ) );
   m_pColorEdit = new PMColorEdit( false, this );
   layout->addWidget( new QLabel( i18n( "Color:" ), this ), 0, AlignTop );
   layout->addWidget( m_pColorEdit );

   connect( m_pColorEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
}

void PMQuickColorEdit::displayObject( PMObject* o )
{
   if( o->isA( "QuickColor" ) )
   {
      m_pDisplayedObject = ( PMQuickColor* ) o;
      m_pColorEdit->setColor( m_pDisplayedObject->color( ) );
      m_pColorEdit->setReadOnly( m_pDisplayedObject->isReadOnly( ) );

      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMQuickColorEdit: Can't display object\n";
}

void PMQuickColorEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setColor( m_pColorEdit->color( ) );
   }
}

bool PMQuickColorEdit::isDataValid( )
{
   if( !m_pColorEdit->isDataValid( ) )
      return false;
   return Base::isDataValid( );
}

#include "pmquickcoloredit.moc"
