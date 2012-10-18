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

#ifndef ARTS_KVOLUMEFADER_TICKMARKS_H
#define ARTS_KVOLUMEFADER_TICKMARKS_H

#include <qframe.h>
#include <math.h>

#include "artsgui.h"

#include "kframe_impl.h"

#include <dbvolcalc.h>

class KVolumeFader_Widget;

class KVolumeFader_impl : virtual public Arts::VolumeFader_skel,
                          virtual public Arts::KFrame_impl,
                          public dB2VolCalc
{
public:
	KVolumeFader_impl( QFrame* =0 );
	~KVolumeFader_impl();

	float dbmin();
	void dbmin( float );
	float dbmax();
	void dbmax( float );

	Arts::Direction direction();
	void direction( Arts::Direction );

	float volume();
	void volume( float );
	float dbvolume();
	void dbvolume( float );

	void normalizedvolume( float );

	void constructor( float min, float max, Arts::Direction dir );
private:
	KVolumeFader_Widget* _vfwidget;
	Arts::Direction _dir;
	bool dbmin_inupdate, dbmax_inupdate, direction_inupdate;
	float _min, _max, _volume;
	int ignoreUpdates;
};

class KPopupMenu;
class KAction;

class KVolumeFader_Widget : public QFrame {
   Q_OBJECT
private:
	KVolumeFader_impl* _impl;
	bool _inupdate;
	float _value;
	Arts::Direction _dir;
	KPopupMenu *_menu;
	KAction *_aExactValue;
	QColor interpolate( QColor, QColor, float );
public:
	KVolumeFader_Widget( QWidget* =0, const char* =0 );
	~KVolumeFader_Widget();
	void setImpl( KVolumeFader_impl* );
	void setValue( float );
	void setDirection( Arts::Direction );
protected:
	void drawContents( QPainter* );
	void mousePressEvent( QMouseEvent* );
	void mouseReleaseEvent( QMouseEvent* );
	void mouseMoveEvent( QMouseEvent* );
	void wheelEvent( QWheelEvent* );
private slots:
	void exactValue();
};

#endif
// vim: sw=4 ts=4
