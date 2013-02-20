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
#include "Affine.h"

using namespace T2P;

Affine::Affine()
{
	m_affine[0] = 1.0;
	m_affine[1] = 0.0;
	m_affine[2] = 0.0;
	m_affine[3] = 1.0;
	m_affine[4] = 0.0;
	m_affine[5] = 0.0;
}

Affine::Affine(double affine[6])
{
	for(int i = 0; i < 6; i++)
		m_affine[i] = affine[i];
}

Affine::Affine(const Affine &other)
{
	(*this) = other;
}

Affine::Affine(double m11, double m12, double m21, double m22, double dx, double dy)
{
	m_affine[0] = m11; m_affine[1] = m12; m_affine[2] = m21;
	m_affine[3] = m22; m_affine[4] = dx; m_affine[5] = dy;
}
		
Affine::~Affine()
{
}

Affine &Affine::operator*=(Affine &other)
{
	double d0 = m11() * other.m11() + m12() * other.m21();
	double d1 = m11() * other.m12() + m12() * other.m22();
	double d2 = m21() * other.m11() + m22() * other.m21();
	double d3 = m21() * other.m12() + m22() * other.m22();
	double d4 = dx() * other.m11() + dy() * other.m21() + other.dx();
	double d5 = dx() * other.m12() + dy() * other.m22() + other.dy();

	m_affine[0] = d0; m_affine[1] = d1; m_affine[2] = d2;
	m_affine[3] = d3; m_affine[4] = d4; m_affine[5] = d5;

	return *this;
}

Affine &Affine::operator=(const Affine &other)
{
	for(int i = 0; i < 6; i++)
		m_affine[i] = other.m_affine[i];

	return *this;
}

double &Affine::m11()
{
	return m_affine[0];
}

double &Affine::m12()
{
	return m_affine[1];
}

double &Affine::m21()
{
	return m_affine[2];
}

double &Affine::m22()
{
	return m_affine[3];
}

double &Affine::dx()
{
	return m_affine[4];
}

double &Affine::dy()
{
	return m_affine[5];
}

void Affine::scale(double s)
{
	scale(s, s);
}

void Affine::scale(double sx, double sy)
{
	m_affine[0] = sx;
	m_affine[3] = sy;
}

void Affine::translate(const Point &point)
{
	m_affine[4] += point.x();
	m_affine[5] += point.y();
}

void Affine::translate(double tx, double ty)
{
	m_affine[4] += tx * m_affine[0] + ty * m_affine[2];
	m_affine[5] += tx * m_affine[1] + ty * m_affine[3];
}

void Affine::rotate(double angle)
{
	double sina = sin(angle);
	double cosa = cos(angle);
		
	double tm11 = cosa * m_affine[0] + sina * m_affine[2];
	double tm12 = cosa * m_affine[1] + sina * m_affine[3];
	double tm21 = -sina * m_affine[0] + cosa * m_affine[2];
	double tm22 = -sina * m_affine[1] + cosa * m_affine[3];
	
	m_affine[0] = tm11; m_affine[1] = tm12;
	m_affine[2] = tm21; m_affine[3] = tm22;
}

void Affine::rotateAround(double angle, const Point &point)
{
	translate(point);
	rotate(angle);
	translate(point.invert());
}

void Affine::rotateAround(double angle, double cx, double cy)
{
	translate(-cx, -cy);
	rotate(angle);
	translate(cx, cy);
}

Point Affine::mapPoint(const Point &p) const
{
	return Point(p.x() * m_affine[0] + p.y() * m_affine[2] + m_affine[4],
				 p.x() * m_affine[1] + p.y() * m_affine[3] + m_affine[5]);
}

double *Affine::data()
{
	return m_affine;
}

// vim:ts=4:noet
