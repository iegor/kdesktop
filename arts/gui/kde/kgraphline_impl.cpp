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

#include "kgraphline_impl.h"
#include "kwidgetrepo.h"
#include "kgraph.h"

using namespace Arts;
using namespace std;

KGraphLine_impl::KGraphLine_impl()
{
	_graphID = -1;
}

KGraphLine_impl::~KGraphLine_impl()
{
	KGraph *kgraph = getKGraph();
	if(kgraph)
		kgraph->removeLine(this);
}

Graph KGraphLine_impl::graph()
{
	return Arts::DynamicCast(KWidgetRepo::the()->lookupWidget(_graphID));
}

void KGraphLine_impl::graph(Graph newGraph)
{
	KGraph *kgraph = getKGraph();
	if(kgraph)
		kgraph->removeLine(this);

	_graphID = newGraph.widgetID();

	kgraph = getKGraph();
	if(kgraph)
		kgraph->addLine(this);
}

bool KGraphLine_impl::editable()
{
	return _editable;
}

void KGraphLine_impl::editable(bool newEditable)
{
	_editable = newEditable;

	KGraph *kgraph = getKGraph();
	if(kgraph)
		kgraph->redrawLine(this);
}

string KGraphLine_impl::color()
{
	return _color;
}

void KGraphLine_impl::color(const std::string& newColor)
{
	_color = newColor;

	KGraph *kgraph = getKGraph();
	if(kgraph)
		kgraph->redrawLine(this);
}

vector<GraphPoint> *KGraphLine_impl::points()
{
	return new vector<GraphPoint>(_points);
}

void KGraphLine_impl::points(const vector<GraphPoint>& newPoints)
{
	_points = newPoints;

	KGraph *kgraph = getKGraph();
	if(kgraph)
		kgraph->redrawLine(this);

	// emitting a change notification is a bit tricky because no real
	// Arts::AnyRef support is there for sequence<Arts::GraphPoint>
	Arts::Any any;
	Arts::Buffer buffer;

	any.type = "*Arts::GraphPoint";
	writeTypeSeq(buffer,_points);
	buffer.read(any.value,buffer.size());

	_emit_changed("points_changed",any);
}

KGraph *KGraphLine_impl::getKGraph()
{
	QWidget *widget = KWidgetRepo::the()->lookupQWidget(_graphID);
	if(!widget)
		return 0;

	return dynamic_cast<KGraph *>(widget);
}

namespace Arts {
	REGISTER_IMPLEMENTATION(KGraphLine_impl);
}
