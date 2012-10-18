/*
    Copyright (  C ) 2003 Arnold Krille <arnold@arnoldarts.de>

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

#include "kvolumefader_impl.h"

#include <kdebug.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <kglobalsettings.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <kinputdialog.h>

KVolumeFader_impl::KVolumeFader_impl( QFrame* w )
  : Arts::KFrame_impl( w ? w : new KVolumeFader_Widget( 0 ) )
  , dB2VolCalc( -36, 6 )
  , _dir( Arts::BottomToTop )
  , dbmin_inupdate( false )
  , dbmax_inupdate( false )
  , direction_inupdate( false )
  , ignoreUpdates( 0 )
{
	//kdDebug()<<"KVolumeFader_impl::KVolumeFader_impl( QFrame* w="<<w<<" )"<<endl;
	_vfwidget = static_cast<KVolumeFader_Widget*>( _qwidget );
	_vfwidget->setImpl( this );
}

KVolumeFader_impl::~KVolumeFader_impl() {
}

float KVolumeFader_impl::dbmin() { return dB2VolCalc::dbmin; }
void KVolumeFader_impl::dbmin( float n ) {
	if ( n != dB2VolCalc::dbmin && !dbmin_inupdate ) {
		dbmin_inupdate = true;
		dB2VolCalc::dbmin = n;
		dbmin_changed( dbmin() );
		dbmin_inupdate = false;
	}
}
float KVolumeFader_impl::dbmax() { return dB2VolCalc::dbmax; }
void KVolumeFader_impl::dbmax( float n ) {
	if ( n != dB2VolCalc::dbmax && !dbmax_inupdate ) {
		dbmax_inupdate = true;
		dB2VolCalc::dbmax = n;
		dbmax_changed( dbmax() );
		dbmax_inupdate = false;
	}
}

Arts::Direction KVolumeFader_impl::direction() { return _dir; }
void KVolumeFader_impl::direction( Arts::Direction n ) {
	if ( n != _dir && !direction_inupdate ) {
		direction_inupdate = true;
		_dir = n;
		_vfwidget->setDirection( _dir );
//		direction_changed( direction() );
		direction_inupdate = false;
	}
}

float KVolumeFader_impl::volume() { return _volume; }
void KVolumeFader_impl::volume( float n ) {
	//kdDebug() << k_funcinfo << n << " ignore: " << ignoreUpdates << endl;
	if ( ignoreUpdates > 0 ) {
		--ignoreUpdates;
		return;
	}
	_volume = n;
	_vfwidget->setValue( amptondb( _volume ) );
}

float KVolumeFader_impl::dbvolume() { return amptodb( _volume ); }
void KVolumeFader_impl::dbvolume( float n ) {
	//kdDebug() << k_funcinfo << n << endl;
	normalizedvolume( dbtondb( n ) );
}

void KVolumeFader_impl::normalizedvolume( float n ) {
	if ( ( ndbtodb( n ) > dbmin() ) && ( ndbtodb( n ) < dbmax() ) ) {
		float amp = ndbtoamp( n );
		if ( amp != _volume ) {
			++ignoreUpdates;
			//kdDebug() << k_funcinfo << ignoreUpdates << endl;
			_volume = amp;
			_vfwidget->setValue( n );
			volume_changed( _volume );
		}
	}
}

void KVolumeFader_impl::constructor( float dbmin, float dbmax, Arts::Direction dir ) {
	this->dbmin( dbmin ); this->dbmax( dbmax ); direction( dir );
}



KVolumeFader_Widget::KVolumeFader_Widget( QWidget* p, const char* n )
  : QFrame( p,n )
  , _impl( 0 )
  , _inupdate( false )
  , _value( -1 )
  , _dir( Arts::BottomToTop )
  , _menu( new KPopupMenu( this ) )
  , _aExactValue( new KAction( i18n( "Set Exact Value..." ), KShortcut(), this, SLOT( exactValue() ), this ) )
{
	//kdDebug() << k_funcinfo << endl;
	setMinimumSize( 10,10 );
	_aExactValue->plug( _menu );
}

void KVolumeFader_Widget::setImpl( KVolumeFader_impl* n ) { _impl = n; update(); }

KVolumeFader_Widget::~KVolumeFader_Widget() {
	//kdDebug() << k_funcinfo << endl;
}

void KVolumeFader_Widget::setDirection( Arts::Direction n ) { _dir = n; update(); }

void KVolumeFader_Widget::setValue( float n ) {
	//kdDebug() << k_funcinfo << n << endl;
	if ( n != _value ) {
		_value = n;
		update();
	}
}

QColor KVolumeFader_Widget::interpolate(  QColor low, QColor high, float percent ) {
	if (  percent<=0 ) return low; else
	if (  percent>=1 ) return high; else
		return QColor(
			int( low.red() + ( high.red()-low.red() ) * percent ),
			int( low.green() + ( high.green()-low.green() ) * percent ),
			int( low.blue() + ( high.blue()-low.blue() ) * percent ) );
}

void KVolumeFader_Widget::drawContents( QPainter* p ){
	if ( _dir==Arts::BottomToTop || _dir==Arts::BottomToTop ) {
		float h;
		if ( _dir==Arts::BottomToTop ) h = contentsRect().height() * ( 1 - _value );
			else h = contentsRect().height() * _value;
		for ( int i=int( h ); i<contentsRect().height(); i++ ) {
			p->setPen( interpolate( colorGroup().light(), colorGroup().highlight(), float( i )/contentsRect().height() ) );
			p->drawLine( contentsRect().left(), this->frameWidth() + i, contentsRect().right(), this->frameWidth() + i );
		}
		p->setPen( colorGroup().dark() );
		p->drawLine( contentsRect().left(), this->frameWidth() + int( h ), contentsRect().right(), this->frameWidth() + int( h ) );
	} else {
		float w;
		p->translate( this->width(),0 );
		if ( _dir==Arts::LeftToRight ) w = - contentsRect().width() * ( 1 - _value );
			else w = - contentsRect().width() * _value;
		for ( int i=int( w ); i>=-contentsRect().width(); i-- ) {
			p->setPen( interpolate( colorGroup().light(), colorGroup().highlight(), float( -i )/contentsRect().width() ) );
			p->drawLine( this->frameWidth() + i, contentsRect().top(), this->frameWidth() + i, contentsRect().bottom() );
		}
		p->setPen( colorGroup().dark() );
		p->drawLine( this->frameWidth() + int( w ), contentsRect().top(), this->frameWidth() + int( w ), contentsRect().bottom() );
	}
}

void KVolumeFader_Widget::mousePressEvent( QMouseEvent* ){
	//kdDebug() << k_funcinfo << endl;
}

void KVolumeFader_Widget::mouseReleaseEvent( QMouseEvent* qme ){
	bool setValue = false;
	if ( KGlobalSettings::mouseSettings().handed == 0 && qme->button() == Qt::LeftButton ) setValue=true;
	if ( KGlobalSettings::mouseSettings().handed == 1 && qme->button() == Qt::RightButton ) setValue=true;
	if ( setValue )
	{
		switch ( _dir ) {
			default:
			case Arts::BottomToTop:
				if ( _impl ) _impl->normalizedvolume( 1 - float( qme->y() ) / contentsRect().height() );
				break;
			case Arts::TopToBottom:
				if ( _impl ) _impl->normalizedvolume( float( qme->y() ) / contentsRect().height() );
				break;
			case Arts::LeftToRight:
				if ( _impl ) _impl->normalizedvolume( float( qme->x() ) / contentsRect().width() );
				break;
			case Arts::RightToLeft:
				if ( _impl ) _impl->normalizedvolume( 1 - float( qme->x() ) / contentsRect().width() );
				break;
		}
	} else _menu->exec( qme->globalPos() );
}

void KVolumeFader_Widget::mouseMoveEvent( QMouseEvent* qme ){
	switch ( _dir ) {
		default:
		case Arts::BottomToTop:
			if ( _impl ) _impl->normalizedvolume( 1 - float( qme->y() ) / contentsRect().height() );
			break;
		case Arts::TopToBottom:
			if ( _impl ) _impl->normalizedvolume( float( qme->y() ) / contentsRect().height() );
			break;
		case Arts::LeftToRight:
			if ( _impl ) _impl->normalizedvolume( float( qme->x() ) / contentsRect().width() );
			break;
		case Arts::RightToLeft:
			if ( _impl ) _impl->normalizedvolume( 1 - float( qme->x() ) / contentsRect().width() );
			break;
	}
}

void KVolumeFader_Widget::wheelEvent( QWheelEvent* qwe ){
	//kdDebug() << k_funcinfo << endl;
	if ( qwe->delta() < 0 ) { if ( _impl ) _impl->normalizedvolume( _impl->dbtondb( _impl->dbvolume() - 1 ) ); }
	if ( qwe->delta() > 0 ) { if ( _impl ) _impl->normalizedvolume( _impl->dbtondb( _impl->dbvolume() + 1 ) ); }
}

void KVolumeFader_Widget::exactValue() {
	//kdDebug() << k_funcinfo << endl;
	bool ok=false;
	double n = KInputDialog::getDouble( i18n( "Set Exact Volume Value" ), i18n( "Exact volume (dB):" ), _impl->dbvolume(), _impl->dbmin(), _impl->dbmax(), 1, &ok, this );
	if ( ok ) _impl->dbvolume( n );
}

REGISTER_IMPLEMENTATION( KVolumeFader_impl );

#include "kvolumefader_impl.moc"
// vim: sw=4 ts=4
