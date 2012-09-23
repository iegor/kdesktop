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

#include "klevelmeter_firebars.h"

#include <kdebug.h>
#include <qpainter.h>
#include <qpixmap.h>

KLevelMeter_FireBars_private::KLevelMeter_FireBars_private( KLevelMeter_FireBars* p, const char* n ) : QWidget( p,n ) {
	_parent = p;
	_pixmap = new QPixmap( 0,0 );
//	setPaletteBackgroundColor( QColor( 0,0,255 ) );
}
void KLevelMeter_FireBars_private::paintEvent( QPaintEvent* ) {
	QPainter p;
	if ( _pixmap->size() != this->size() ) {
		_pixmap->resize( this->size() );
		p.begin( _pixmap );
		_pixmap->fill( paletteBackgroundColor() );
		if ( dir==Arts::BottomToTop || dir==Arts::TopToBottom ) {
kdDebug() << k_funcinfo << dir << endl;
			//if ( dir==Arts::BottomToTop ) p.translate( 0, rect().bottom() );
			for ( int i=this->height(); i>0; i-- ) {
				p.setPen( _parent->color( 1-float( i )/this->height() ) );
				//if ( dir==Arts::BottomToTop ) i *= -1;
				p.drawLine( 0, i, this->width(), i );
			}
		} else {
			if ( dir==Arts::RightToLeft ) p.translate( 0, rect().right() );
			for ( int i=this->width(); i>0; i-- ) {
				p.setPen( _parent->color( float( i )/this->width() ) );
				if ( dir==Arts::RightToLeft ) i *= -1;
				p.drawLine( i, 0, i, this->height() );
			}
		}
		p.end();
	}
	p.begin( this );
	p.translate( 0,0 );
	p.drawPixmap( QPoint( 0,0 ), *_pixmap );
	p.end();
}

KLevelMeter_FireBars::KLevelMeter_FireBars( Arts::KLevelMeter_impl* impl, QWidget* parent, long substyle, long count, Arts::Direction dir, float _dbmin, float _dbmax ) : KLevelMeter_Template( impl, parent, substyle, count, dir, _dbmin, _dbmax ) {
//kdDebug()<<"KLevelMeter_FireBars::KLevelMeter_FireBars( Arts::KLevelMeter_impl* "<<impl<<", QWidget* "<<parent<<", long "<<substyle<<", long "<<count<<", float "<<_dbmin<<", float "<<_dbmax<<" )"<<endl;
	this->setMinimumSize( 5, 5 );
	_bar = new KLevelMeter_FireBars_private( this, 0 );
	_peakwidget = new QWidget( this );
	_peakwidget->resize( size() );
	_peakwidget->setPaletteBackgroundColor( color( 1 ) );
	_peakwidget->hide();
}

void KLevelMeter_FireBars::invalue( float n, float p ) {
//kdDebug()<<"KLevelMeter_FireBars::invalue( float n )"<<endl;
	if ( _peakwidget->size() != this->size() ) _peakwidget->setGeometry( 0,0, size().width(), size().height() );
	_value = amptondb( n );
	_peak  = amptondb( p );
	if ( _peak > 1 )_peakwidget->show(); else _peakwidget->hide();
	_bar->dir = _dir;
	switch ( _dir ) {
		default:
		case Arts::BottomToTop:
			_bar->setGeometry( 0, int( this->height()-_value*this->height() ), this->width(), this->height() );
			break;
		case Arts::TopToBottom:
			_bar->setGeometry( 0, -int( this->height()-_value*this->height() ), this->width(), this->height() );
			break;
		case Arts::LeftToRight:
			_bar->setGeometry( -int( this->width()-_value*this->width() ), 0, this->width(), this->height() );
			break;
		case Arts::RightToLeft:
			_bar->setGeometry( int( this->width()-_value*this->width() ), 0, this->width(), this->height() );
			break;
	}
	repaint();
}

void KLevelMeter_FireBars::paintEvent( QPaintEvent* ) {
	QPainter p( this );
	//p.setPen( NoPen );
	p.setPen( QColor( 0,0,255 ) );
	// PeakBar
	if ( _peak > 1.0/1000 ) {
		if ( _dir == Arts::BottomToTop || _dir == Arts::TopToBottom ) {
			if ( _dir == Arts::BottomToTop ) p.translate( 0, rect().bottom() );
			int h = int( this->height()*_peak );
			if ( _dir==Arts::BottomToTop ) h *= -1;
			p.drawLine( 0, h, this->width(), h );
		} else {
			if ( _dir==Arts::RightToLeft ) p.translate( 0, rect().right() );
			int w = int( this->width()* _peak );
			if ( _dir==Arts::RightToLeft ) w *= -1;
			p.drawLine( w, 0, w, this->height() );
		}
	}
}

/**
	Planned feature: a little Tooltip showing the actual value of the volume in deziBel and perhaps as linear scaleFactor
*/
void KLevelMeter_FireBars::mouseMoveEvent( QMouseEvent* /*qme*/ ) {
//kdDebug()<<"KLevelMeter_FireBars::mouseMoveEvent(QMouseEvent* "<<qme<<" )"<<endl;
}

#include <klevelmeter_firebars.moc>
// vim: sw=4 ts=4
