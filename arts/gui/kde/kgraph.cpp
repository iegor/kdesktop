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

#include "kgraph.h"
#include "kgraph.moc"
#include "kgraphline_impl.h"
#include "qpainter.h"

#include <cstdlib>
#include <math.h>

using namespace Arts;
using namespace std;

KGraph::KGraph( QWidget * parent, const char * name )
	: QWidget( parent, name )
{
	setBackgroundColor(white);

	selectedIndex = -1;
	minx = miny = 0.0;
	maxx = maxy = 1.0;
}

KGraph::~KGraph()
{
}

void KGraph::addLine(Arts::KGraphLine_impl *line)
{
	lines.push_back(line);
}

void KGraph::redrawLine(Arts::KGraphLine_impl * /*line*/)
{
	repaint();
}

void KGraph::removeLine(Arts::KGraphLine_impl *line)
{
	if(line == selectedLine)
	{
		selectedLine = 0;
		selectedIndex = -1;
	}
	lines.remove(line);
}

inline QPoint KGraph::g2qPoint(const GraphPoint &gp)
{
	return QPoint(int(((gp.x - minx)/(maxx-minx)) * (width()-1)),
	              int((1.0 - (gp.y - miny)/(maxy-miny)) * (height()-1)));
}

inline GraphPoint KGraph::q2gPoint(const QPoint &qp)
{
	return GraphPoint((float(qp.x())/float(width()-1)) * (maxx-minx) + minx,
					  (1.0 - (float(qp.y())/float(height()-1))) * (maxy-miny) + miny);
}

void KGraph::paintEvent( QPaintEvent *e )
{
	QPainter painter(this);
	painter.setClipRect(e->rect());

	std::list<KGraphLine_impl *>::iterator li;
	for(li = lines.begin(); li != lines.end(); li++)
	{
		KGraphLine_impl *gline = *li;

		vector<GraphPoint>::iterator pi;
		QPoint lastp;
		bool first = true;

		painter.setPen(gline->_color.c_str());

		for(pi = gline->_points.begin(); pi != gline->_points.end(); pi++)
		{
			QPoint p = g2qPoint(*pi);

			if(!first)
				painter.drawLine(lastp,p);

			if(gline->_editable)
				painter.drawEllipse(p.x()-3,p.y()-3,7,7);

			lastp = p;
			first = false;
		}
	}
}

void KGraph::mousePressEvent(QMouseEvent *e)
{
	if(e->button() == LeftButton || e->button() == RightButton)
	{
		std::list<KGraphLine_impl *>::iterator li;
		for(li = lines.begin(); li != lines.end(); li++)
		{
			KGraphLine_impl *gline = *li;

			vector<GraphPoint>::iterator pi;
			int index = 0;
			for(pi = gline->_points.begin(); pi != gline->_points.end(); pi++, index++)
			{
				QPoint p = g2qPoint(*pi);

				int dx = e->x() - p.x();
				int dy = e->y() - p.y();

				if(::sqrt(double(dx*dx + dy*dy)) < 5.0)
				{
					selectedIndex = index;
					selectedLine = gline;
					selectedPoint = *pi;
				}
			}
		}
	}

	if(selectedIndex >= 0)
	{
		// erase point
		if(e->button() == RightButton)
		{
			if(selectedIndex != 0 && selectedIndex != (( int )( selectedLine->_points.size() )-1))
			{
				vector<GraphPoint> points;
	
				for(int i=0;i<( int )selectedLine->_points.size();i++)
				{
					if(selectedIndex != i)
						points.push_back(selectedLine->_points[i]);
				}
	
				selectedLine->points(points);
			}
	
			selectedLine = 0;
			selectedIndex = -1;
		}
	}
	else if(e->button() == LeftButton)
	{
		// try to insert a point
		std::list<KGraphLine_impl *>::iterator li;
		for(li = lines.begin(); li != lines.end(); li++)
		{
			KGraphLine_impl *gline = *li;

			QPoint lastp;
			bool first = true;

			vector<GraphPoint>::iterator pi;
			int index = 0;
			for(pi = gline->_points.begin(); pi != gline->_points.end(); pi++, index++)
			{
				QPoint p = g2qPoint(*pi);

				if(!first && (e->x() > lastp.x()+2) && (e->x() < p.x()-2))
				{
					float pos = float(e->x()-lastp.x())/float(p.x()-lastp.x());
					int y = (int)((1.0-pos) * lastp.y() + pos * p.y());

					if(abs(y-e->y()) < 5)
					{
						GraphPoint gp = q2gPoint(QPoint(e->x(),y));
						vector<GraphPoint> newPoints;

						for(int i=0;i<( int )gline->_points.size();i++)
						{
							if(index == i)
								newPoints.push_back(gp);
							newPoints.push_back(gline->_points[i]);
						}
						gline->points(newPoints);

						selectedLine = gline;
						selectedIndex = index;
						selectedPoint = gp;

						return;
					}
				}
				lastp = p;
				first = false;
			}
		}
	}
}

void KGraph::mouseMoveEvent(QMouseEvent *e)
{
	QPoint pos = e->pos();

	if(pos.x() < 0) pos.setX(0);
	if(pos.y() < 0) pos.setY(0);
	if(pos.x() >= width()) pos.setX(width()-1);
	if(pos.y() >= height()) pos.setY(height()-1);

	if(selectedIndex >= 0)
	{
		vector<GraphPoint> points(selectedLine->_points);


		if((( int )points.size() <= selectedIndex)
		|| (fabs(selectedPoint.x-points[selectedIndex].x) > 0.000001)
		|| (fabs(selectedPoint.y-points[selectedIndex].y) > 0.000001))
		{
			selectedLine = 0;
			selectedIndex = -1;
			return; // line was modified from somewhere else, meanwhile
		}

		// I am not sure whether we always want to constrain it that way
		GraphPoint gp = q2gPoint(pos);
		selectedPoint.y = gp.y;

		if(selectedIndex != 0 && selectedIndex != (( int )( points.size() )-1))
		{
			float pixelsize = (maxx-minx)/float(width()-1);

			if(selectedIndex > 0 && points[selectedIndex-1].x > gp.x)
			{
				selectedPoint.x = points[selectedIndex-1].x+pixelsize;
			}
			else if(selectedIndex < (( int )( points.size() )-1) && points[selectedIndex+1].x < gp.x)
			{
				selectedPoint.x = points[selectedIndex+1].x-pixelsize;
			}
			else
			{
				selectedPoint.x = gp.x;
			}
		}
		points[selectedIndex] = selectedPoint;
		selectedLine->points(points);
	}
}

void KGraph::mouseReleaseEvent(QMouseEvent *)
{
	selectedIndex = -1;
	selectedLine = 0;
}

// vim: sw=4 ts=4
