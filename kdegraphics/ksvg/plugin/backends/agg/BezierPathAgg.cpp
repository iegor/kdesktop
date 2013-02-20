/*
    Copyright (C) 2003 Nikolas Zimmermann <wildfox@kde.org>
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <math.h>
#include "Point.h"
#include "BezierPathAgg.h"

#include <kdebug.h>

#include <agg_basics.h>
#include <agg_bounding_rect.h>

using namespace T2P;

double BezierPathAgg::length(double t)
{
	if(m_length < 0.0)
	{
		double total = 0.0;
		double x = 0.0, y = 0.0;
		double x2, y2;
		unsigned cmd;
		unsigned int id = 0;
		m_curved_trans.rewind(id);
		while(!agg::is_stop(cmd = m_curved_trans.vertex(&x2, &y2)))
		{
			if(agg::is_move_to(cmd))
			{
				x = x2;
				y = y2;	
			}
			else if(agg::is_line_to(cmd))
			{
				double dx = x, dy = y;
				dx = x2 - dx;
				dy = y2 - dy;
				total += sqrt(pow(dx, 2) + pow(dy, 2));
				x = x2;
				y = y2;
			}
		}
		return total * t;
	}
	else
		return m_length * t;
}

void BezierPathAgg::pointTangentNormalAt(double t, Point *p, Point *tn, Point *n)
{
	double totallen = length(t);
	double total = 0.0;
        double x = 0.0, y = 0.0;
	double x2, y2;
	unsigned cmd;
	unsigned int id = 0;
	m_curved_trans.rewind(id);
	while(!agg::is_stop(cmd = m_curved_trans.vertex(&x2, &y2)))
	{
		if(agg::is_move_to(cmd))
		{
			x = x2;
			y = y2;	
		}
		else if(agg::is_line_to(cmd))
		{
			double dx = x, dy = y;
			x = x2;
			y = y2;
			dx = x - dx;
			dy = y - dy;
			double seg_len = sqrt(pow(dx, 2) + pow(dy, 2));
			total += seg_len;
			if(total >= totallen)
			{
				double fract = 1 - (totallen - (total - seg_len)) / seg_len;
				if(p)
				{
					p->setX(x - dx * fract);
					p->setY(y - dy * fract);
				}
				if(tn)
				{
					//kdDebug() << k_funcinfo << "dx : " << dx << endl;
					//kdDebug() << k_funcinfo << "dy : " << dy << endl;
					tn->setX(dx);
					tn->setY(dy);
				}
				if(n)
				{
					// Calculate vector product of "binormal" x tangent
					// (0,0,1) x (dx,dy,0), which is simply (dy,-dx,0).
					n->setX(dy);
					n->setY(-dx);
				}
				return;
			}
		}
	}
}

void BezierPathAgg::boundingBox(Point *topLeft, Point *bottomRight)
{
	double x1, y1, x2, y2;
	agg::bounding_rect(m_curved, *this, 0, 1, &x1, &y1, &x2, &y2);
	*topLeft = Point(x1, y1);
	*bottomRight = Point(x2, y2);
}

// vim:ts=4:noet
