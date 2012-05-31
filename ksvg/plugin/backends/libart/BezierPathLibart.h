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

#ifndef T2P_BEZIERPATH_LIBART_H
#define T2P_BEZIERPATH_LIBART_H

#include "BezierPath.h"
#include <qmemarray.h>

class _ArtBpath;

#define ensureSpace(vec, index) if((int)vec.size() == index) vec.resize(index + 1);

namespace T2P
{
	class BezierPathLibart : public BezierPath
	{
	public:
		BezierPathLibart();
		BezierPathLibart(_ArtBpath *other);
		~BezierPathLibart();

		virtual double length(double t = 1.0);
		virtual void pointTangentNormalAt(double t, Point *p = 0, Point *tn = 0, Point *n = 0);
		virtual void boundingBox(Point *topLeft, Point *bottomRight);

		// Don't make those private members, these are all internal anyway...
		QMemArray<_ArtBpath> m_array;
		double m_length;
	};
}

#endif

// vim:ts=4:noet
