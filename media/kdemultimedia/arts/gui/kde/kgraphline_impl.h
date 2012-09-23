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

#ifndef _KGRAPHLINE_IMPL_H
#define _KGRAPHLINE_IMPL_H

#include "artsgui.h"

namespace Arts {

class KGraph;
class KGraphLine_impl : virtual public GraphLine_skel {
protected:
	friend class KGraph; // for efficiency

	long _graphID;
	bool _editable;
	std::string _color;
	std::vector<GraphPoint> _points;

	KGraph *getKGraph();

public:
	KGraphLine_impl();
	~KGraphLine_impl();

	Graph graph();
	void graph(Graph newGraph);

	bool editable();
	void editable(bool newEditable);

	std::string color();
	void color(const std::string& newColor);

	std::vector<GraphPoint> *points();
	void points(const std::vector<GraphPoint>& newPoints);
};

}

#endif /* _KGRAPHLINE_IMPL_H */
