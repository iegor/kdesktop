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


#include "pmcommentedit.h"
#include "pmcomment.h"

#include <qlayout.h>
#include <qmultilineedit.h>
#include <kglobalsettings.h>

PMCommentEdit::PMCommentEdit( QWidget* parent, const char* name )
      : Base( parent, name )
{
   m_pDisplayedObject = 0;
}

void PMCommentEdit::createTopWidgets( )
{
   Base::createTopWidgets( );
   
   m_pEdit = new QMultiLineEdit( this );
#if ( QT_VERSION >= 300 )
   m_pEdit->setTextFormat( Qt::PlainText );
   m_pEdit->setWordWrap( QTextEdit::NoWrap );
#endif
   m_pEdit->setFont( KGlobalSettings::fixedFont( ) );
   topLayout( )->addWidget( m_pEdit, 2 );
   
   connect( m_pEdit, SIGNAL( textChanged( ) ), SIGNAL( dataChanged( ) ) );
}

void PMCommentEdit::displayObject( PMObject* o )
{
   if( o->isA( "Comment" ) )
   {
      m_pDisplayedObject = ( PMComment* ) o;
      m_pEdit->setText( m_pDisplayedObject->text( ) );

      m_pEdit->setReadOnly( o->isReadOnly( ) );
      
      Base::displayObject( o );
   }
   else
      kdError( PMArea ) << "PMCommentEdit: Can't display object\n";
}

void PMCommentEdit::saveContents( )
{
   if( m_pDisplayedObject )
   {
      Base::saveContents( );
      m_pDisplayedObject->setText( m_pEdit->text( ) );
   }
}

bool PMCommentEdit::isDataValid( )
{
   return Base::isDataValid( );
}
#include "pmcommentedit.moc"
