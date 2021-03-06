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

#ifndef ARTS_KLEVELMETER_FIREBARS_H
#define ARTS_KLEVELMETER_FIREBARS_H

#include "klevelmeter_template.h"

class QPixmap;
class KLevelMeter_FireBars;

class KLevelMeter_FireBars_private : public QWidget {
   Q_OBJECT
public:
	KLevelMeter_FireBars_private( KLevelMeter_FireBars*, const char* );
	void paintEvent( QPaintEvent* );

	Arts::Direction dir;
private:
	KLevelMeter_FireBars* _parent;
	QPixmap *_pixmap;
};

class KLevelMeter_FireBars : public KLevelMeter_Template {
   Q_OBJECT
public:
	KLevelMeter_FireBars( Arts::KLevelMeter_impl*, QWidget* =0, long substyle=0, long count=0, Arts::Direction =Arts::BottomToTop, float _dbmin=-24, float _dbmax=6 );

	void invalue( float, float =0 );

	void paintEvent( QPaintEvent* );

	void mouseMoveEvent( QMouseEvent* );
private:
	float _value, _peak;
	KLevelMeter_FireBars_private *_bar;
	QWidget* _peakwidget;
};

#endif
// vim: sw=4 ts=4
