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
#include "BezierPathLibart.h"

#include <kdebug.h>

#include <libart_lgpl/art_bpath.h>
#include <libart_lgpl/art_vpath.h>
#include <libart_lgpl/art_vpath_bpath.h>

using namespace T2P;

BezierPathLibart::BezierPathLibart() : BezierPath()
{
	m_array.resize(0);
	m_length = -1;
}

BezierPathLibart::BezierPathLibart(ArtBpath *other) : BezierPath()
{
	int i = 0;
	for(;other[i].code != ART_END; i++)
	{
		ensureSpace(m_array, i)
		m_array[i] = other[i];
	}
	ensureSpace(m_array, i)
	m_array[i].code = ART_END;
}

BezierPathLibart::~BezierPathLibart()
{
}

double BezierPathLibart::length(double t)
{
	if(m_length < 0.0)
	{
		double total = 0.0;
		// We cheat a bit...
		ArtVpath *vpath = art_bez_path_to_vec(m_array.data(), 0.25);
		double x = 0.0, y = 0.0;
		for(int i = 0; vpath[i].code != ART_END; i++)
		{
			if(vpath[i].code == ART_MOVETO)
			{
				x = vpath[i].x;
				y = vpath[i].y;
			}
			else if(vpath[i].code == ART_LINETO)
			{
				double dx = x, dy = y;
				x = vpath[i].x;
				y = vpath[i].y;
				dx = x - dx;
				dy = y - dy;
				total += sqrt(pow(dx, 2) + pow(dy, 2));
			}
		}
		art_free(vpath);
		return total * t;
	}
	else
		return m_length * t;
}

void BezierPathLibart::pointTangentNormalAt(double t, Point *p, Point *tn, Point *n)
{
	double totallen = length(t);
	// We cheat a bit...
	ArtVpath *vpath = art_bez_path_to_vec(m_array.data(), 0.25);
	double total = 0.0;
	double x = 0.0, y = 0.0;
	for(int i = 0; vpath[i].code != ART_END; i++)
	{
		if(vpath[i].code == ART_MOVETO)
		{
			x = vpath[i].x;
			y = vpath[i].y;
		}
		else if(vpath[i].code == ART_LINETO)
		{
			double dx = x, dy = y;
			x = vpath[i].x;
			y = vpath[i].y;
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
				// Calculate tangent
				if(tn)
				{
					tn->setX(dx);
					tn->setY(dy);
				}
				// Calculate normal vector.
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
	art_free(vpath);
}	   

void BezierPathLibart::boundingBox(Point *topLeft, Point *bottomRight)
{
	if(m_array.count() > 0)
	{
		// We cheat a bit...
		ArtVpath *vpath = art_bez_path_to_vec(m_array.data(), 0.25);

		ArtDRect rect;
		art_vpath_bbox_drect(vpath, &rect);
		art_free(vpath);

		*topLeft = Point(rect.x0, rect.y0);
		*bottomRight = Point(rect.x1, rect.y1);
	}
	else
	{
		*topLeft = Point(0, 0);
		*bottomRight = Point(0, 0);
	}
}

// vim:ts=4:noet
