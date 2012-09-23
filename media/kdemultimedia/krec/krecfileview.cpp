/***************************************************************************
    copyright            : (C) 2003 by Arnold Krille
    email                : arnold@arnoldarts.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 *                                                                         *
 ***************************************************************************/

#include "krecfileview.h"
#include "krecfileview.moc"

#include "krecfilewidgets.h"
#include "krecfileviewhelpers.h"
#include "krecfile.h"
#include "krecglobal.h"

#include <kdebug.h>
#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qlineedit.h>

KRecFileView::KRecFileView( QWidget* p, const char* n )
  : QWidget( p,n )
{
kdDebug( 60005 ) << k_funcinfo << endl;
	_layout_td = new QBoxLayout( this, QBoxLayout::TopToBottom, 5, 5 );
	_filename = new QLabel( i18n( "<no file>" ), this );
	_layout_td->addWidget( _filename, 1 );
	_fileview = new KRecFileWidget( 0, this );
	_layout_td->addWidget( _fileview, 100 );
	_timebar = new KRecTimeBar( this );
	_layout_td->addWidget( _timebar, 50 );
	_layout_lr = new QBoxLayout( this, QBoxLayout::LeftToRight, 5, 5 );
	_layout_td->addLayout( _layout_lr, 1 );
	_layout_lr->addStretch( 20 );
	_timedisplay = new KRecTimeDisplay( this );
	_layout_td->addWidget( _timedisplay, 1 );
	_file = 0;
}
KRecFileView::~KRecFileView() {
kdDebug( 60005 ) << k_funcinfo << endl;
}

void KRecFileView::setFile( KRecFile* file ) {
kdDebug( 60005 ) << k_funcinfo << file << endl;
	if ( _file != file ) {
		_file = file;
		_fileview->setFile( _file );
		if ( _file ) {
			if ( !_file->filename().isNull() ) setFilename( _file->filename() );
				else _filename->setText( i18n( "file with no name" ) );
			connect( _file, SIGNAL( posChanged( int ) ), this, SLOT( setPos( int ) ) );
			connect( _file, SIGNAL( posChanged( int ) ), _timebar, SLOT( newPos( int ) ) );
			connect( _file, SIGNAL( posChanged( int ) ), _timedisplay, SLOT( newPos( int ) ) );
			connect( _file, SIGNAL( sizeChanged( int ) ), this, SLOT( setSize( int ) ) );
			connect( _file, SIGNAL( sizeChanged( int ) ), _timebar, SLOT( newSize( int ) ) );
			connect( _file, SIGNAL( sizeChanged( int ) ), _timedisplay, SLOT( newSize( int ) ) );
			connect( _file, SIGNAL( filenameChanged( const QString &) ), this, SLOT( setFilename( const QString &) ) );
			connect( _file, SIGNAL( filenameChanged( const QString &) ), _timedisplay, SLOT( newFilename( const QString &) ) );
			connect( _timebar, SIGNAL( sNewPos( int ) ), _file, SLOT( newPos( int ) ) );
			_timebar->newPos( _file->position() );
			_timebar->newSize( _file->size() );
			_timedisplay->newSamplingRate( _file->samplerate() );
			_timedisplay->newChannels( _file->channels() );
			_timedisplay->newBits( _file->bits() );
			_timedisplay->newFilename( _file->filename() );
			_timedisplay->newPos( _file->position() );
			_timedisplay->newSize( _file->size() );
		} else {
			disconnect( this, SLOT( setPos( QIODevice::Offset ) ) );
			_filename->setText( i18n( "<no file>" ) );
			_timedisplay->newFilename( QString::null );
		}
	}
}

void KRecFileView::updateGUI() { _fileview->resizeEvent(); }
void KRecFileView::setPos( int ) {}
void KRecFileView::setSize( int ) {}
void KRecFileView::setFilename( const QString &n ) { _filename->setText( n );}

