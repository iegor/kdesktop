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

#ifndef ARTS_KLEVELMETER_TICKMARKS_H
#define ARTS_KLEVELMETER_TICKMARKS_H

#include <qframe.h>
#include <math.h>

#include "artsgui.h"

#include "kframe_impl.h"

#include <dbvolcalc.h>

class KTickmarks_Widget;

class KTickmarks_impl : virtual public Arts::Tickmarks_skel,
                        virtual public Arts::KFrame_impl
{
public:
	KTickmarks_impl( QFrame* =0 );

	float min();
	void min( float );
	float max();
	void max( float );

	float minstep();
	void minstep( float );
	float substep();
	void substep( float );

	Arts::Direction direction();
	void direction( Arts::Direction );

	long position();
	void position( long );

	void constructor( float min, float max, Arts::Direction, long );
private:
	KTickmarks_Widget* _tmwidget;
};

class KTickmarks_Widget : public QFrame, public dB2VolCalc {
   Q_OBJECT
private:
	KTickmarks_impl* _impl;
public:
	KTickmarks_Widget( KTickmarks_impl*, QWidget* =0, const char* =0 );
	void drawContents( QPainter* );
	long _pos;
	Arts::Direction _dir;

	float minstep, substep;
};

#endif
// vim: sw=4 ts=4
