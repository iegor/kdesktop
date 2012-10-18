    /*

    Copyright (C) 2000,2001 Stefan Westerfeld
                            stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#ifndef ARTS_GUI_KGRAPH_IMPL_H
#define ARTS_GUI_KGRAPH_IMPL_H
#include "kwidget_impl.h"
#include "kgraph.h"

#include <qobject.h>
#include <qstring.h>


namespace Arts {

class KGraph_impl;

class KGraph_impl : virtual public Arts::Graph_skel,
                    public Arts::KWidget_impl
{
protected:
	QString _caption;
	float _minx, _miny, _maxx, _maxy;

	KGraph * _kgraph;

public:
	KGraph_impl( KGraph * w = 0 );
	void constructor( Widget p ) { parent( p ); }

	std::string caption();
	void caption(const std::string& newCaption);

	float minx();
	void minx(float newMin);
	float maxx();
	void maxx(float newMax);
	float miny();
	void miny(float newMin);
	float maxy();
	void maxy(float newMax);
};

}
#endif /* ARTS_GUI_KGRAPH_IMPL_H */

// vim: sw=4 ts=4
