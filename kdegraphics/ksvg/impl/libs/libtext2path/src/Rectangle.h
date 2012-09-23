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

#ifndef T2P_RECTANGLE_H
#define T2P_RECTANGLE_H

#include "Point.h"

namespace T2P
{
	class Rectangle
	{
	public:
		Rectangle();
		Rectangle(const Rectangle &other);
		Rectangle(const Point &a, const Point &b);
		Rectangle(double x, double y, double width, double height);
		~Rectangle();

		Rectangle &operator=(const Rectangle &other);

		Point a() const;
		void setA(const Point &a);
		
		Point b() const;
		void setB(const Point &b);

		// Finds the smallest rectangle that includes src1 and src2.
		void bboxUnion(const Rectangle &src1, const Rectangle &src2);
	
	private:
		Point m_a, m_b;
	};
}

#endif

// vim:ts=4:noet
