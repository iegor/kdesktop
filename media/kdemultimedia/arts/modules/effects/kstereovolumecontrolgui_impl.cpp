/*

    Copyright ( C ) 2003 Arnold Krille <arnold@arnoldarts.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation;
    version 2 of the License.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include "kstereovolumecontrolgui_impl.h"

#include <qframe.h>
#include <kdebug.h>
#include <iostream>

using namespace Arts;

KStereoVolumeControlGui_impl::KStereoVolumeControlGui_impl( QFrame* w ) : KLayoutBox_impl( w ? w : new QFrame( 0 ) )
{
	//kdDebug()<<"KStereoVolumeControlGui_impl::KStereoVolumeControlGui_impl( QFrame* "<<w<<" )"<<endl;
	_mapper = new KStereoVolumeControlGui_EventMapper( this, _qframe );
	this->addWidget( _label, -100 );
	_label.bottom( Arts::East );
	_label.text( "Volume" );
	this->addLine( 1, 0, -100 );
	this->addWidget( _left, 20 );
	this->addWidget( _tickmarks, -100 );
	this->addWidget( _right, 20 );
	this->addLine( 1, 0, -100 );
	this->addWidget( _volumefader, 20 );
	this->addWidget( _fadertickmarks, -100 );
	_fadertickmarks.position( posLeft );
	_tickmarks.position( posLeft|posRight );
	this->dbmin( -36 );
	this->dbmax( 6 );
	_left.framestyle( Arts::Raised|Arts::Panel ); _left.linewidth( 4 );
	_right.framestyle( Arts::Raised|Arts::Panel ); _right.linewidth( 4 );
	this->layoutmargin( 1 ); this->linewidth( 1 ); this->framestyle( Arts::Panel|Arts::Raised );
}

void KStereoVolumeControlGui_impl::constructor( Arts::StereoVolumeControl svc ) {
	//kdDebug() << k_funcinfo << endl;
	_svc = svc;
	connect( svc, "currentVolumeLeft_changed", _left, "invalue" );
	connect( svc, "currentVolumeRight_changed", _right, "invalue" );

	connect( svc, "scaleFactor_changed", _volumefader, "volume" );
	connect( _volumefader, "volume_changed", svc, "scaleFactor" );

	_volumefader.volume( svc.scaleFactor() );

	_mapper->_timer->start( 100 );
}

Arts::Direction KStereoVolumeControlGui_impl::direction() { return _dir; }
void KStereoVolumeControlGui_impl::direction( Arts::Direction n ) {
kdDebug() << k_funcinfo << n << endl;
	_dir = n;
	Arts::KLayoutBox_impl::direction( _dir );
	switch ( _dir ) {
		case Arts::LeftToRight:
		case Arts::RightToLeft:
			allWidgets( BottomToTop );
			_label.bottom( Arts::East );
			break;
		case Arts::TopToBottom:
			allWidgets( LeftToRight );
			_label.bottom( Arts::South );
			break;
		case Arts::BottomToTop:
			allWidgets( RightToLeft );
			_label.bottom( Arts::South );
			break;
		default: break;
	}
}

void KStereoVolumeControlGui_impl::allWidgets( Arts::Direction n ) {
	_left.direction( n );
	_right.direction( n );
	_volumefader.direction( n );
	_tickmarks.direction( n );
	_fadertickmarks.direction( n );
}

void KStereoVolumeControlGui_impl::title( const std::string& n ) { _label.text( n ); }
std::string KStereoVolumeControlGui_impl::title() { return _label.text(); }

float KStereoVolumeControlGui_impl::dbmin() { return _dbmin; }
void KStereoVolumeControlGui_impl::dbmin( float n ) {
	//kdDebug() << k_funcinfo << n << endl;
	_dbmin = n;
	_left.mindB( _dbmin );
	_right.mindB( _dbmin );
	_tickmarks.min( _dbmin );
	_volumefader.dbmin( _dbmin );
	_fadertickmarks.min( _dbmin );
}

float KStereoVolumeControlGui_impl::dbmax() { return _dbmax; }
void KStereoVolumeControlGui_impl::dbmax( float n ) {
	//kdDebug() << k_funcinfo << n << endl;
	_dbmax = n;
	_left.maxdB( 0 );
	_right.maxdB( 0 );
	_tickmarks.max( 0 );
	_volumefader.dbmax( _dbmax );
	_fadertickmarks.max( _dbmax );
}

void KStereoVolumeControlGui_impl::updateValues() {
	_left.invalue( _svc.currentVolumeLeft() );
	_right.invalue( _svc.currentVolumeRight() );
}

REGISTER_IMPLEMENTATION( KStereoVolumeControlGui_impl );

// vim: sw=4 ts=4
#include "kstereovolumecontrolgui_impl.moc"

