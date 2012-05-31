/** 
 * Copyright (C) 1997-2003 the KGhostView authors. See file AUTHORS.
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

#include "thumbnailservice.h"

#include "kgv_miniwidget.h"
#include "kpswidget.h"
#include "kgv_view.h"

#include <kdebug.h>
#include <qtimer.h>
#include <algorithm>
#include <cassert>
#include <cstring>

ThumbnailService::ThumbnailService( KGVMiniWidget* parent, const char* name ) :
	QObject( parent, name ),
	_mini( parent ),
	timer_( new QTimer( this ) ),
	_busy( false ),
	_enabled( false )
{
	_thumbnailDrawer = new KPSWidget( parent->_part->widget(), "thumbnail-drawer" );
	_thumbnailDrawer->readSettings();
	connect(  _thumbnailDrawer, SIGNAL(  newPageImage(  QPixmap ) ), SLOT( slotDone( QPixmap ) ) );
	connect( timer_, SIGNAL( timeout() ), SLOT( processOne() ) );
	_thumbnailDrawer->hide();
}

ThumbnailService::~ThumbnailService()
{
}

bool ThumbnailService::Request::operator < ( ThumbnailService::Request b ) const
{
	if ( urgent != b.urgent ) return urgent;
	if ( page != b.page ) return page < b.page;
	// below is just so that == can be in terms of "<"
	if ( receiver != b.receiver ) return std::less<QObject*>()( receiver, b.receiver );
	if ( slot != b.slot ) return std::strcmp( slot, b.slot ) < 0;
	return false;
}

void ThumbnailService::delayedGetThumbnail( const int page, QObject* rec, const char* slot, bool urgent )
{
	kdDebug( 4500 ) << "ThumbnailService::delayedGetThumbnail: request for page " << page << endl;
	pending.insert( Request( page, rec, slot, urgent ) );
	if ( !_busy ) {
		_busy = true;
		// The reason for the code below might not be obvious:
		// It is much nicer to have the the thumbnails appear from top to bottom.
		// However, when several are requested at once (or almost), then we cannot control the order
		// unless we delay a bit the first one
		if ( urgent ) processOne();
		else timer_->start( 150, true );
	}
}

void ThumbnailService::cancelRequests( const int page, QObject* rec, const char* slot )
{
	std::set<Request>::iterator first = pending.begin(), last = pending.end();
	while ( first != last ) {
		if ( ( page == -1 || page == first->page ) &&
				( !rec || rec == first->receiver ) &&
				( !slot || !strcmp( slot, first->slot ) ) ) {
			std::set<Request>::iterator next = first;
			++next;
			pending.erase( first );
			first = next;
		} else {
			++first;
		}
	}
}

void ThumbnailService::reset()
{
	kdDebug( 4500 ) << "ThumbnailService::reset()" << endl;
	timer_->stop();
	pending.clear();
	assert( _thumbnailDrawer );
	_thumbnailDrawer->stopInterpreter();
	_busy = false;
	_enabled = false;
}

void ThumbnailService::setEnabled( const bool e )
{
	kdDebug( 4500 ) << "ThumbnailService::setEnabled( " << ( e ? "true" : "false" ) << " )" << endl;
	_enabled = e;
	if ( _enabled && _busy ) processOne();
}

void ThumbnailService::slotDone( QPixmap pix )
{
	kdDebug( 4500 ) << "ThumbnailService::slotDone(): got page" << endl;
	pix.detach();
	emit relayPixmap( pix );
	processOne();
}


void ThumbnailService::processOne()
{
	kdDebug( 4500 ) << "ThumbnailService::processOne()" << endl;
	if ( !_enabled ) return;
	if ( !_mini || !_mini->dsc() || !_mini->dsc()->isStructured() ) {
		_busy = false;
		pending.clear();
		return;
	}
	assert( _thumbnailDrawer );
	if ( pending.empty() ) {
		_busy = false;
		return;
	}
	_busy = true;
	FILE* file = _mini->psFile();
	Request req = *pending.begin();
	kdDebug( 4500 ) << "ThumbnailService::processOne(): processing " << req.page << "(of " << pending.size() << " requests)" << endl;
	disconnect( SIGNAL( relayPixmap( QPixmap ) ) );
	while ( !pending.empty() && req.page == pending.begin()->page ) {
		req = *pending.begin();
		connect( this, SIGNAL( relayPixmap( QPixmap ) ), req.receiver, req.slot );
		pending.erase( pending.begin() );
	}
	_thumbnailDrawer->setOrientation( _mini->orientation( req.page ) );
	_thumbnailDrawer->setBoundingBox( _mini->boundingBox( req.page ) );
	_thumbnailDrawer->setMagnification(  0.2 );
	if ( !_thumbnailDrawer->isInterpreterRunning() ) {
		_thumbnailDrawer->setFileName( _mini->_document->fileName(), true );
		_thumbnailDrawer->startInterpreter();
		_thumbnailDrawer->sendPS(  file, _mini->dsc()->beginprolog(),
				_mini->dsc()->endprolog() );
		_thumbnailDrawer->sendPS(  file, _mini->dsc()->beginsetup(),
				_mini->dsc()->endsetup() );
	}
	else {
		_thumbnailDrawer->nextPage();
	}
	_thumbnailDrawer->sendPS(  file, _mini->dsc()->page()[ req.page ].begin,
			_mini->dsc()->page()[ req.page ].end );
}

#include "thumbnailservice.moc"

