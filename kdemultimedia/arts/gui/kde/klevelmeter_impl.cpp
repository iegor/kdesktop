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


#include "klevelmeter_impl.h"

#include <math.h>

#include <qframe.h>
#include <kdebug.h>
#include <qlayout.h>
#include <kartswidget.h>

#include "klevelmeter_private.h"
#include "klevelmeter_private.moc"
#include "klevelmeter_template.h"
#include "klevelmeter_template.moc"

#include "klevelmeter_small.h"
#include "klevelmeter_linebars.h"
#include "klevelmeter_firebars.h"
#include "klevelmeter_normalbars.h"

using namespace Arts;
using namespace std;

KLevelMeter_Private::KLevelMeter_Private( KLevelMeter_impl* impl, QFrame* frame, LevelMeterStyle defstyle, QObject* parent, const char* name )
   : QObject( parent, name )
   , _impl( impl )
   , _frame( frame )
   , _levelmeter( 0 )
   , _style( defstyle )
   , _substyle( 0 )
   , _count( 20 )
   , _direction( Arts::BottomToTop )
   , _peak( 20 ), _peakvalue( 0.0 )
   , _dbmin( -36 ), _dbmax( 0 )
{
	_layout = new QBoxLayout( _frame, QBoxLayout::LeftToRight );
}

void KLevelMeter_Private::createWidget() {
	if ( _levelmeter != 0 ) { _levelmeter->hide(); delete _levelmeter; _levelmeter=0; }
	switch ( _style ) {
		case lmNormalBars:
			_levelmeter = new KLevelMeter_NormalBars( _impl, _frame, _substyle, _count, _direction, _dbmin, _dbmax );
			break;
		case lmFireBars:
			_levelmeter = new KLevelMeter_FireBars( _impl, _frame, _substyle, _count, _direction, _dbmin, _dbmax );
			break;
		default:
		case lmLineBars:
			_levelmeter = new KLevelMeter_LineBars( _impl, _frame, _substyle, _count, _direction, _dbmin, _dbmax );
			break;
		case lmSmall:
			_levelmeter = new KLevelMeter_Small( _impl, _frame, _substyle, _count, _direction, _dbmin, _dbmax );
			break;
	}
	_layout->addWidget( _levelmeter );
	_levelmeter->show();
	_levelmeter->setMinimumSize( 10,10 );
}

KLevelMeter_impl::KLevelMeter_impl( QFrame* w ) : Arts::KFrame_impl( w ? w : new QFrame( 0 ) ) {
//kdDebug()<<"KLevelMeter_impl::KLevelMeter_impl( QFrame* "<<w<<" )"<<endl;
	p = new KLevelMeter_Private( this, _qframe, lmLineBars );
	p->createWidget();
}

LevelMeterStyle KLevelMeter_impl::style() { return p->_style; }

void KLevelMeter_impl::style( LevelMeterStyle style ) {
	if ( p->_style!=style )
	{
		p->_style = style;
		p->createWidget();
	}
}

long KLevelMeter_impl::substyle()         { return p->_substyle; }
void KLevelMeter_impl::substyle( long n ) { p->_substyle = n; p->_levelmeter->substyle( n ); }

long KLevelMeter_impl::count()         { return p->_levelmeter->count(); }
void KLevelMeter_impl::count( long n ) { p->_levelmeter->count( n ); p->_count = n; }

long KLevelMeter_impl::peakfalloff()         { return p->_peak; }
void KLevelMeter_impl::peakfalloff( long n ) { p->_peak = n; }

float KLevelMeter_impl::mindB()         { return p->_levelmeter->dbmin; }
void KLevelMeter_impl::mindB( float n ) { p->_levelmeter->dbmin = n; p->_dbmin = n; }
float KLevelMeter_impl::maxdB()         { return p->_levelmeter->dbmax; }
void KLevelMeter_impl::maxdB( float n ) { p->_levelmeter->dbmax = n; p->_dbmax = n; }

float KLevelMeter_impl::invalue() { return 0.0; }
void KLevelMeter_impl::invalue( float n ) {
//kdDebug()<<"KLevelMeter_impl::invalue( float n="<<n<<" )"<<endl;
	if ( p->_peak ) {
		p->_peakvalue = ( p->_peak*p->_peakvalue + n ) / ( p->_peak + 1 );
		if ( p->_peakvalue < n ) p->_peakvalue = n;
	}
	else p->_peakvalue = 0.0;
	p->_levelmeter->invalue( n, p->_peakvalue );
}

Arts::Direction KLevelMeter_impl::direction() { return p->_direction; }
void KLevelMeter_impl::direction( Arts::Direction n ) {
	p->_direction = n;
	p->_levelmeter->direction( n );
}

REGISTER_IMPLEMENTATION( KLevelMeter_impl );

// vim: sw=4 ts=4

