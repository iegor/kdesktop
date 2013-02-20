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

#ifndef T2P_BEZIERPATH_H
#define T2P_BEZIERPATH_H

#include <list>

namespace T2P
{
	class Point;
	class BezierPath
	{
	public:
		BezierPath() { }
		virtual ~BezierPath() { }

		/**
		 * Calculates the arclength from p0 to the point parametrized by 0 <= t <= 1.
		 */
		virtual double length(double t = 1.0) = 0;

		/**
		 * Calculates the point, the tangent vector and the normal vector for
		 * 0 <= t <= 1. The tangent vector and the normal vector are
		 * normalized (length=1).
		 */
		virtual void pointTangentNormalAt(double t, Point *p = 0, Point *tn = 0, Point *n = 0) = 0;		
	
		/**
		 * Calculates the axis-aligned bounding box.
		 */
		virtual void boundingBox(Point *topLeft, Point *bottomRight) = 0;		
	};
}

#endif

// vim:ts=4:noet
