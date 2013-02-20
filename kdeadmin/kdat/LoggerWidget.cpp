// KDat - a tar-based DAT archiver
// Copyright (C) 1998-2000  Sean Vyain, svyain@mail.tds.net
// Copyright (C) 2001-2002  Lawrence Widman, kdat@cardiothink.com
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

#include <qlabel.h>
#include <qlayout.h>
#include <qmultilineedit.h>

#include <kapplication.h>
#include <kfiledialog.h>

#include "LoggerWidget.h"
#include <klocale.h>
#include <kmessagebox.h>

#include "LoggerWidget.moc"

LoggerWidget::LoggerWidget( const QString & title, QWidget* parent, const char* name )
        : QWidget( parent, name )
{
    QLabel* lbl1 = new QLabel( title, this );
    lbl1->setFixedHeight( lbl1->sizeHint().height() );

    _mle = new QMultiLineEdit( this );
    _mle->setReadOnly( TRUE );

    QVBoxLayout* l1 = new QVBoxLayout( this, 0, 4 );
    l1->addWidget( lbl1 );
    l1->addWidget( _mle, 1 );
}

LoggerWidget::~LoggerWidget()
{
}

void LoggerWidget::append( const QString & text )
{
    _mle->append( text );
    _mle->setCursorPosition( _mle->numLines(), 0 );
}

void LoggerWidget::save()
{
    QString file = KFileDialog::getSaveFileName( QString::null, QString::null, this );
    if ( !file.isNull() ) {
        QFile f( file );
        if ( f.exists() ) {
            int result = KMessageBox::warningContinueCancel( this,
		   i18n( "Log file exists, overwrite?" ),
                   i18n( "KDat: Save Log" ), 
                   i18n("&Overwrite"));
            if (result != KMessageBox::Continue)
                return;
        }
        if ( f.open( IO_WriteOnly ) ) {
            QCString line;
            for ( int i = 0; i < _mle->numLines(); i++ ) {
                line = _mle->textLine( i ).utf8();
                f.writeBlock( line, line.length() );
                f.writeBlock( "\n", 1 );
            }
            f.close();
        }
    }
}
