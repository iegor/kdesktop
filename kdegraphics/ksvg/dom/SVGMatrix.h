/*
    Copyright (C) 2001-2003 KSVG Team
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
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef SVGMatrix_H
#define SVGMatrix_H

namespace KSVG
{

class SVGMatrixImpl;
class SVGMatrix
{ 
public:
	SVGMatrix();
	SVGMatrix(double, double, double, double, double, double);
	SVGMatrix(const SVGMatrix &);
	SVGMatrix &operator=(const SVGMatrix &);
	SVGMatrix(SVGMatrixImpl *);
	~SVGMatrix();

	void setA(const double &);
	double a() const;

	void setB(const double &);
	double b() const;

	void setC(const double &);
	double c() const;

	void setD(const double &);
	double d() const;

	void setE(const double &);
	double e() const;

	void setF(const double &);
	double f() const;

	SVGMatrix multiply(SVGMatrix &secondMatrix);
	SVGMatrix inverse();
	SVGMatrix translate(const double &x, const double &y);
	SVGMatrix scale(const double &scaleFactor);
	SVGMatrix scaleNonUniform(const double &scaleFactorX, const double &scaleFactorY);
	SVGMatrix rotate(const double &angle);
	SVGMatrix rotateFromVector(const double &x, const double &y);
	SVGMatrix flipX();
	SVGMatrix flipY();
	SVGMatrix skewX(const double &angle);
	SVGMatrix skewY(const double &angle);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGMatrixImpl *handle() const { return impl; }

private:
	SVGMatrixImpl *impl;
};

}

#endif

// vim:ts=4:noet
