/*
	Copyright (C) 2001 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/* $Id: wavecapture.cpp 465369 2005-09-29 14:33:08Z mueller $ */
#include "wavecapture.h"

#include <noatun/player.h>
#include <noatun/app.h>
#include <noatun/engine.h>
#include <noatun/noatunarts.h>

#include <arts/soundserver.h>
#include <arts/mcoputils.h>

#include <klocale.h>
#include <kpopupmenu.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <krecentdocument.h>
#include <kio/job.h>
#include <kio/jobclasses.h>

#include <qfile.h>
#include <qstring.h>
#include <qtimer.h>

#include <string>

extern "C"
{
	Plugin *create_plugin()
	{
		KGlobal::locale()->insertCatalogue("wavecapture");
		return new WaveCapture();
	}
}

WaveCapture::WaveCapture() : QObject(0,0), Plugin(),
	_enabled( false ),
	_status( STOPPED ),
	_capture( Arts::DynamicCast( napp->player()->engine()->server()->createObject( "Arts::Effect_WAVECAPTURE" ) ) ),
	_count( 0 ),
	_id( 0 ),
	_filename( "" ),
	_timer( new QTimer( this ) ),
	m_job( 0 )
{
	NOATUNPLUGINC(WaveCapture);

	if( napp->player()->isPlaying() )
		_status = PLAYING;
	else if( napp->player()->isPaused() )
		_status = PAUSED;

	newSong();

	connect( _timer, SIGNAL( timeout() ), SLOT( saveAs() ) );

	connect(napp->player(), SIGNAL(changed()), SLOT(newSong()));
	connect(napp->player(), SIGNAL(stopped()), SLOT(stopped()));
	connect(napp->player(), SIGNAL(playing()), SLOT(playing()));
	connect(napp->player(), SIGNAL( paused()), SLOT( paused()));
}

WaveCapture::~WaveCapture()
{
	napp->pluginMenuRemove(pluginMenuItem);
	if( _enabled )
	{
		if( PLAYING == _status )
			stop();
		QString filename = QFile::decodeName( (Arts::MCOPUtils::createFilePath( _filename ) + ".wav").c_str() );
		QFile::remove( filename );
	}
	delete m_job;
}

void WaveCapture::init()
{
	pluginMenuItem = napp->pluginMenuAdd( i18n( "Wave Capture" ), this, SLOT( toggle() ) );
	napp->pluginMenu()->setCheckable( true );
}

void WaveCapture::toggle()
{
	_enabled = ! _enabled;
	if( PLAYING == _status )
	{
		if( _enabled )
			start();
		else
		{
			stop();
			QString filename = QFile::decodeName( (Arts::MCOPUtils::createFilePath( _filename ) + ".wav").c_str() );
			QFile::remove( filename );
		}
	}
	napp->pluginMenu()->setItemChecked( pluginMenuItem, _enabled );
}

void WaveCapture::newSong()
{
	if( napp->player()->current() )
	{
		QString title = napp->player()->current().title();
		_capture.filename( std::string( QFile::encodeName( title ) ) );
	}
	_timer->start( 0, true );
}

void WaveCapture::saveAs()
{
	// this could be a candidate for a race condition - but unlikely and harmless
	std::string filename = _filename;
	_filename = _capture.filename().c_str();
	if( _enabled && ( !filename.empty() ) )
	{
		KFileDialog dlg( ":savedir", "*.wav", 0, "filedialog", true );
		dlg.setCaption( i18n( "Save Last Wave File As" ) );
		dlg.setOperationMode( KFileDialog::Saving );
		dlg.setSelection( QFile::decodeName( filename.c_str() ) + ".wav" );
			 
		QString filename2 = QFile::decodeName( (Arts::MCOPUtils::createFilePath( filename ) + ".wav").c_str() );
		if( dlg.exec() )
		{
			KURL url = dlg.selectedURL();
			if (url.isValid())
				KRecentDocument::add( url );

			m_job = KIO::file_move( KURL( filename2 ), url, -1, true );
			connect( m_job, SIGNAL( result( KIO::Job* ) ), SLOT( copyFinished( KIO::Job* ) ) );
		}
		else
		{
			QFile::remove( filename2 );
		}
	}
}

void WaveCapture::stopped()
{
	if( _enabled && PLAYING == _status )
		stop();
	_status = STOPPED;
	_capture.filename( std::string( "" ) );
}

void WaveCapture::playing()
{
	if( _enabled && PLAYING != _status )
		start();
	_status = PLAYING;
}

void WaveCapture::paused()
{
	if( _enabled && PLAYING == _status )
		stop();
	_status = PAUSED;
}

void WaveCapture::start()
{
	_capture.start();
	_id = napp->player()->engine()->globalEffectStack()->insertBottom( _capture, "capture too wave" );
}

void WaveCapture::stop()
{
	napp->player()->engine()->globalEffectStack()->remove( _id );
	_capture.stop();
	_timer->start( 0, true );
}

void WaveCapture::copyFinished( KIO::Job* job )
{
	if( job == m_job )
		m_job = 0;
}

#include "wavecapture.moc"

// vim:ts=4:sw=4
