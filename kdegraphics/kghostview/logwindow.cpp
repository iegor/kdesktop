/** 
 * Copyright (C) 2003 the KGhostView authors. See file AUTHORS.
 * 	
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <qtextedit.h>
#include <qlabel.h>
#include <qvbox.h>

#include <kglobalsettings.h>
#include <kurllabel.h>
#include <klocale.h>

#include "logwindow.h"

LogWindow::LogWindow( const QString& caption, 
                      QWidget* parent, const char* name) :
    KDialogBase( parent, name, false, caption, User1|Close, Close, false, 
                 KStdGuiItem::clear() )
{
    QVBox * display = makeVBoxMainWidget();
    
    _errorIndication = new QLabel( "", display, "logview-label" );
    _errorIndication->hide();
    
    _configureGS = new KURLLabel( i18n( "Configure Ghostscript" ), QString::null, display );
    _configureGS->hide();

    _logView = new QTextEdit( display, "logview" );
    _logView->setTextFormat( Qt::PlainText );
    _logView->setReadOnly( true );
    _logView->setWordWrap( QTextEdit::NoWrap );
    _logView->setFont( KGlobalSettings::fixedFont() );
    _logView->setMinimumWidth( 80 * fontMetrics().width( " " ) );

    connect( this, SIGNAL( user1Clicked() ), SLOT( clear() ) );
    connect( _configureGS, SIGNAL( leftClickedURL() ), SLOT( emitConfigureGS() ) );
}

void LogWindow::emitConfigureGS() {
	emit configureGS();
}

void LogWindow::append( const QString& message )
{
    _logView->append( message );
}

void LogWindow::clear()
{
    _logView->clear();
    _errorIndication->clear();
}

void LogWindow::setLabel( const QString& text, bool showConfigureGS )
{
	_errorIndication->setText( text );
	_errorIndication->show();
	if ( showConfigureGS ) _configureGS->show();
	else _configureGS->hide();
}

#include "logwindow.moc"
