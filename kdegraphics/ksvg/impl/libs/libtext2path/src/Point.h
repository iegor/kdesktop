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

#ifndef T2P_POINT_H
#define T2P_POINT_H

namespace T2P
{
	class Point
	{
	public:
		Point() { m_x = 0.0; m_y = 0.0; }
		Point(double x, double y) { m_x = x; m_y = y; }
		Point &operator=(const Point &other) { m_x = other.x(); m_y = other.y(); return *this; }

		friend inline Point operator+(const Point &, const Point &);
		friend inline Point operator-(const Point &, const Point &);
		friend inline Point operator*(const double &, const Point &);
		friend inline Point operator*(const Point &, const double &);

		double x() const { return m_x; }
		double y() const { return m_y; }

		void setX(double x) { m_x = x; }
		void setY(double y) { m_y = y; }

		const Point invert() const
		{
			Point temp;
			temp.setX(-x());
			temp.setY(-y());
			return temp;
		}

	private:
		double m_x;
		double m_y;
	};

	inline Point operator+(const Point &p1, const Point &p2) { return Point(p1.m_x + p2.m_x, p1.m_y + p2.m_y); }
	inline Point operator-(const Point &p1, const Point &p2) { return Point(p1.m_x - p2.m_x, p1.m_y - p2.m_y); }
	inline Point operator*(const double &c, const Point &p) { return Point(p.m_x * c, p.m_y * c); }
	inline Point operator*(const Point &p, const double &c) { return Point(p.m_x * c, p.m_y * c); }
}

#endif

// vim:ts=4:noet
