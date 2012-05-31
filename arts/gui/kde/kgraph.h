    /*

    Copyright (C) 2001 Stefan Westerfeld
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

#ifndef _KGRAPH_H
#define _KGRAPH_H

#include <qwidget.h>
#include <qpoint.h>
#include "artsgui.h"
#include <list>

namespace Arts {

class KGraphLine_impl;
class KGraph : public QWidget
{
Q_OBJECT

protected:
	std::list<KGraphLine_impl *> lines;
	float minx, maxx, miny, maxy;

	KGraphLine_impl *selectedLine;
	GraphPoint selectedPoint;
	int selectedIndex;	// -1 if nothing is selected

	inline GraphPoint q2gPoint(const QPoint &qp);
	inline QPoint g2qPoint(const GraphPoint &gp);
public:
	KGraph( QWidget * parent = 0, const char * name = 0 );
	virtual ~KGraph();

	void addLine(Arts::KGraphLine_impl *line);
	void redrawLine(Arts::KGraphLine_impl *line);
	void removeLine(Arts::KGraphLine_impl *line);

	void mousePressEvent(QMouseEvent *me);
	void mouseMoveEvent(QMouseEvent *me);
	void mouseReleaseEvent(QMouseEvent *me);

	void paintEvent(QPaintEvent *e);
};
}

#endif /* KGRAPH_H */
