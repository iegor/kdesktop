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

#include "klevelmeter_small.h"

#include <kdebug.h>
#include <qpainter.h>

KLevelMeter_Small::KLevelMeter_Small( Arts::KLevelMeter_impl* impl, QWidget* parent, long substyle, long count, Arts::Direction dir, float _dbmin, float _dbmax ) : KLevelMeter_Template( impl, parent, substyle, count, dir, _dbmin, _dbmax ) {
//kdDebug()<<"KLevelMeter_Small::KLevelMeter_Small( Arts::KLevelMeter_impl* "<<impl<<", QWidget* "<<parent<<", long "<<substyle<<", long "<<count<<", float "<<_dbmin<<", float "<<_dbmax<<" )"<<endl;
}

void KLevelMeter_Small::invalue( float n, float p ) {
//kdDebug()<<"KLevelMeter_Small::invalue( float "<<n<<", float "<<p<<" )"<<endl;
	_peak = amptondb( p );
	if ( n ) this->setBackgroundColor( color( amptondb( n ) ) );
		else this->setBackgroundColor( QColor( 0,255,0 ).dark() );
}

void KLevelMeter_Small::paintEvent( QPaintEvent* /*qpe*/ ) {
//kdDebug()<<"KLevelMeter_Small::paintEvent( QPaintEvent* "<<qpe<<" )"<<endl;
	if ( _peak && _peak <= 1.0 ) {
		QPainter p( this );
		//p.setPen( QColor( 0,0,0 ) );
		QColor bgcolor = this->paletteBackgroundColor();
		p.setPen( QColor( 255-bgcolor.red(), 255-bgcolor.green(), 255-bgcolor.blue() ) );
		if ( _dir==Arts::BottomToTop || _dir==Arts::TopToBottom ) {
			if ( _dir==Arts::BottomToTop ) p.translate( 0, rect().bottom() );
			int h = int( this->height() * _peak );
			if ( _dir==Arts::BottomToTop ) h *= -1;
			p.drawLine( 0, h, this->width(), h );
		} else {
			if ( _dir==Arts::RightToLeft ) p.translate( 0, rect().right() );
			int w = int( this->width() * _peak );
			if ( _dir==Arts::RightToLeft ) w *= -1;
			p.drawLine( w, 0, w, this->height() );
		}
	}
}

#include <klevelmeter_small.moc>
// vim: sw=4 ts=4
