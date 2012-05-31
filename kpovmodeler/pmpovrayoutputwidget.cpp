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

#include "pmpovrayoutputwidget.h"

#include <qtextedit.h>
#include <qlayout.h>

#include <klocale.h>
#include <kglobalsettings.h>
#include <kconfig.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

#include "pmdebug.h"

QSize PMPovrayOutputWidget::s_size = QSize( 400, 400 );

PMPovrayOutputWidget::PMPovrayOutputWidget( QWidget* parent, const char* name )
      : KDialog( parent, name )
{
   QVBoxLayout* topLayout = new QVBoxLayout( this, KDialog::marginHint( ), KDialog::spacingHint( ) );

   m_pTextView = new QTextEdit( this );
   topLayout->addWidget( m_pTextView, 1 );
   m_pTextView->setFont( KGlobalSettings::fixedFont( ) );
   m_pTextView->setTextFormat( Qt::PlainText );
   m_pTextView->setReadOnly( true );

   QHBoxLayout* buttonLayout = new QHBoxLayout( topLayout );
   buttonLayout->addStretch( 1 );
   QPushButton* closeButton = new KPushButton( KStdGuiItem::close(), this );
   buttonLayout->addWidget( closeButton );
   closeButton->setDefault( true );
   connect( closeButton, SIGNAL( clicked( ) ), SLOT( hide( ) ) );

   setCaption( i18n( "Povray Output" ) );
   resize( s_size );

   m_startOfLastLine = 0;
}

PMPovrayOutputWidget::~PMPovrayOutputWidget( )
{
}

void PMPovrayOutputWidget::slotClear( )
{
   m_output = QString::null;
   m_startOfLastLine = 0;
   m_pTextView->clear( );
}

void PMPovrayOutputWidget::slotText( const QString& output )
{
   unsigned int i;

   for( i = 0; i < output.length( ); i++ )
   {
      QChar c = output[i];
      if( c == '\r' )
         m_output.truncate( m_startOfLastLine );
      else if( c == '\n' )
      {
         m_output += c;
         m_startOfLastLine = m_output.length( );
         //kdDebug( PMArea ) << m_startOfLastLine << endl;
      }
      else if( c.isPrint( ) )
         m_output += c;
   }

   m_pTextView->setText( m_output );
}

void PMPovrayOutputWidget::slotClose( )
{
   hide( );
}


void PMPovrayOutputWidget::saveConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );
   cfg->writeEntry( "PovrayOutputWidgetSize", s_size );
}

void PMPovrayOutputWidget::restoreConfig( KConfig* cfg )
{
   cfg->setGroup( "Appearance" );

   QSize defaultSize( 500, 400 );
   s_size = cfg->readSizeEntry( "PovrayOutputWidgetSize", &defaultSize );
}

void PMPovrayOutputWidget::resizeEvent( QResizeEvent* ev )
{
   s_size = ev->size( );
}

#include "pmpovrayoutputwidget.moc"
