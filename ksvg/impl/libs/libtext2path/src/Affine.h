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

#ifndef T2P_AFFINE_H
#define T2P_AFFINE_H

namespace T2P
{
	class Point;
	class Affine
	{
	public:
		Affine();
		Affine(double affine[6]);
		Affine(const Affine &other);
		Affine(double m11, double m12, double m21, double m22, double dx, double dy);
		~Affine();

		Affine &operator*=(Affine &other);
		Affine &operator=(const Affine &other);

		double &m11();
		double &m12();
		double &m21();
		double &m22();
		double &dx();
		double &dy();
		
		void scale(double s);
		void scale(double sx, double sy);
		void translate(const Point &point); // Assumes point is already transformed
		void translate(double tx, double ty);
		
		void rotate(double angle);
		void rotateAround(double angle, const Point &point); // Assumes point is already transformed
		void rotateAround(double angle, double cx, double cy);

		Point mapPoint(const Point &p) const;

		// Internal
		double *data();
	
	private:
		double m_affine[6];
	};
}

#endif

// vim:ts=4:noet
