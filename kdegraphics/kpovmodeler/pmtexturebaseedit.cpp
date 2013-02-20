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


#include "pmtexturebaseedit.h"
#include "pmpigment.h"
#include "pmlinkedit.h"

#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>


PMTextureBaseEdit::PMTextureBaseEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}


void PMTextureBaseEdit::createTopWidgets( )
{
   Base::createTopWidgets( );
   m_pLinkEdit = new PMLinkEdit( this );
   topLayout( )->addWidget( m_pLinkEdit );
   connect( m_pLinkEdit, SIGNAL( dataChanged( ) ), SIGNAL( dataChanged( ) ) );
}


void PMTextureBaseEdit::displayObject( PMObject* o )
{
   if( o->isA( "TextureBase" ) )
   {
      m_pDisplayedObject = ( PMTextureBase* ) o;
      m_pLinkEdit->setReadOnly( o->isReadOnly( ) );
      m_pLinkEdit->setDisplayedObject( o );
      m_pLinkEdit->setLinkPossibility( m_pDisplayedObject->type( ) );
      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMTextureBaseEdit: Can't display object\n";
}

void PMTextureBaseEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setLinkedObject( m_pLinkEdit->link( ) );
   }
}

void PMTextureBaseEdit::enableLinkEdit( bool enable )
{
   m_pLinkEdit->setEnabled( enable );
}

#include "pmtexturebaseedit.moc"
