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

#include "Tools.h"
#include "Rectangle.h"

using namespace T2P;

Rectangle::Rectangle()
{
}

Rectangle::Rectangle(const Rectangle &other)
{
	(*this) = other;
}

Rectangle::Rectangle(const Point &a, const Point &b) : m_a(a), m_b(b)
{
}

Rectangle::Rectangle(double x, double y, double width, double height)
{
	m_a = Point(x, y);
	m_b = Point(x + width, y + height);
}
	
Rectangle::~Rectangle()
{
}

Rectangle &Rectangle::operator=(const Rectangle &other)
{
	m_a = other.m_a;
	m_b = other.m_b;

	return *this;
}

Point Rectangle::a() const
{
	return m_a;
}

void Rectangle::setA(const Point &a)
{
	m_a = a;
}

Point Rectangle::b() const
{
	return m_b;
}

void Rectangle::setB(const Point &b)
{
	m_b = b;
}

void Rectangle::bboxUnion(const Rectangle &src1, const Rectangle &src2)
{
	double src1x0 = src1.a().x(), src1x1 = src1.b().x();
	double src1y0 = src1.a().y(), src1y1 = src1.b().y();

	double src2x0 = src2.a().x(), src2x1 = src2.b().x();
	double src2y0 = src2.a().y(), src2y1 = src2.b().y();

	if(src1x1 <= src1x0 || src1y1 <= src1y0) // Is src1 empty?
	{ // Copy src2 to dst
		setA(Point(src2x0, src2y0));
		setB(Point(src2x1, src2y1));
	}
	else if(src2x1 <= src2x0 || src2y1 <= src2y0) // Is src2 empty?
	{ // Copy src1 to dest
		setA(Point(src1x0, src1y0));
		setB(Point(src1x1, src1y1));
	}
	else
	{
		setA(Point(T2PMIN(src1x0, src2x0), T2PMIN(src1y0, src2y0)));
		setB(Point(T2PMAX(src1x1, src2x1), T2PMAX(src1y1, src2y1)));
	}
}

// vim:ts=4:noet
