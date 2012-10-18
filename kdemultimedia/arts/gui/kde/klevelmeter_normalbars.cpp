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

#include "klevelmeter_normalbars.h"

#include <kdebug.h>
#include <qpainter.h>
#include <qlayout.h>

KLevelMeter_NormalBars::KLevelMeter_NormalBars( Arts::KLevelMeter_impl* impl, QWidget* parent, long substyle, long count, Arts::Direction dir, float _dbmin, float _dbmax ) : KLevelMeter_Template( impl, parent, substyle, count, dir, _dbmin, _dbmax ) {
//kdDebug()<<"KLevelMeter_NormalBars::KLevelMeter_NormalBars( Arts::KLevelMeter_impl* "<<impl<<", QWidget* "<<parent<<", long "<<substyle<<", long "<<count<<", float "<<_dbmin<<", float "<<_dbmax<<" )"<<endl;
	_layout = new QBoxLayout( this, QBoxLayout::BottomToTop );
}

void KLevelMeter_NormalBars::invalue( float n, float p ) {
	//kdDebug()<<"KLevelMeter_NormalBars::invalue( float "<<n<<", float "<<p<<" )"<<endl;
	_peak = amptondb( p );
	_value = amptondb( n );
	if ( _dir != Arts::Direction( _layout->direction() ) ) _layout->setDirection( QBoxLayout::Direction( _dir ) );
	for ( uint i=0; i<bars.count(); i++ ) bars.at( i )->setValue( _value );
}

long KLevelMeter_NormalBars::count() { return _count; }
void KLevelMeter_NormalBars::count( long n ) {
//kdDebug()<<"KLevelMeter_NormalBars::count( long "<<n<<" )"<<endl;
	if ( n != _count && n>0 ) {
		_count = n;
		resizeEvent();
	}
}

void KLevelMeter_NormalBars::resizeEvent( QResizeEvent* ) {
	//kdDebug()<<"KLevelMeter_NormalBars::resizeEvent( QResizeEvent* )"<<endl;
uint barscount = _count;
	//kdDebug()<<"[1] barscount="<<barscount<<" bars.count()="<<bars.count()<<endl;
	if ( _dir==Arts::BottomToTop || _dir==Arts::TopToBottom ) {
		if ( unsigned( this->height() ) < barscount ) barscount = this->height();
	} else {
		if ( unsigned( this->width() ) < barscount ) barscount = this->width();
	}
	if ( barscount != bars.count() ) {
		//kdDebug()<<"[2] barscount="<<barscount<<" bars.count()="<<bars.count()<<endl;
		while ( bars.count() > 0 ) {
			bars.last()->hide();
			delete bars.last();
			bars.removeLast();
			//kdDebug()<<"[2.5] barscount="<<barscount<<" bars.count()="<<bars.count()<<endl;
		}
		//kdDebug()<<"[3] barscount="<<barscount<<" bars.count()="<<bars.count()<<endl;
		uint i=0;
		while ( bars.count()<barscount ) {
			Bar* tmp = new Bar( i*1.0/barscount, ( i+1 )*1.0/barscount, color( i*1.0/barscount ), this );
			tmp->show();
			_layout->addWidget( tmp );
			bars.append( tmp );
			i++;
			//kdDebug()<<"[3."<<i<<"] barscount="<<barscount<<" bars.count()="<<bars.count()<<endl;
		}
		//kdDebug()<<"[4] barscount="<<barscount<<" bars.count()="<<bars.count()<<endl;
	}
}

#include <klevelmeter_normalbars.moc>
// vim: sw=4 ts=4
