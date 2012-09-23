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

#ifndef ARTS_KLEVELMETER_TEMPLATE_H
#define ARTS_KLEVELMETER_TEMPLATE_H

#include <qwidget.h>

#include <math.h>

#include "dbvolcalc.h"

#include "klevelmeter_impl.h"

class KLevelMeter_Template : public QWidget, public dB2VolCalc {
   Q_OBJECT
public:
	Arts::KLevelMeter_impl* _impl;

	KLevelMeter_Template( Arts::KLevelMeter_impl* impl, QWidget* p, long /*substyle*/, long count, Arts::Direction dir, float _dbmin, float _dbmax )
	  : QWidget( p )
	  , dB2VolCalc( _dbmin, _dbmax )
	  , _impl( impl )
	  , _count( count )
	  , nilline( 3/4.0 )
	  , _dir( dir )
	{}

	virtual void invalue( float, float =0 ) =0;

	virtual void substyle( long ) {}
	virtual long substyle() { return 0; }

	virtual void count( long ) {}
	virtual long count() { return 0; }
	long _count;

	void direction( Arts::Direction dir ) { _dir = dir; }
	Arts::Direction direction() { return _dir; }

	float nilline;
	/// Gives the colors between green and red
	QColor color( float n ) {
		return QColor( int( ( n<=nilline )?255*( 1/nilline )*n:255 ),
		               int( ( n<=1 && n>nilline )?255-255*( 1/nilline )*( n-nilline ):( ( n>1 )?0:255 ) ),
		               0 );
	}
protected:
	Arts::Direction _dir;
};

#endif
// vim: sw=4 ts=4
