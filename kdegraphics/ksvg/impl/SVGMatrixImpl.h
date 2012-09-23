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

#ifndef SVGMatrixImpl_H
#define SVGMatrixImpl_H

#include <qwmatrix.h>

#include <dom/dom_misc.h>

#include "ksvg_lookup.h"

namespace KSVG
{
class KSVGPolygon;

class SVGMatrixImpl : public DOM::DomShared
{ 
public:
	SVGMatrixImpl();
	SVGMatrixImpl(QWMatrix mat);
	SVGMatrixImpl(double, double, double, double, double, double);
	~SVGMatrixImpl();

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

	void copy(const SVGMatrixImpl *other);

	SVGMatrixImpl *inverse();

	// Pre-multiplied operations, as per the specs.
	SVGMatrixImpl *multiply(const SVGMatrixImpl *secondMatrix);
	SVGMatrixImpl *translate(const double &x, const double &y);
	SVGMatrixImpl *scale(const double &scaleFactor);
	SVGMatrixImpl *scaleNonUniform(const double &scaleFactorX, const double &scaleFactorY);
	SVGMatrixImpl *rotate(const double &angle);
	SVGMatrixImpl *rotateFromVector(const double &x, const double &y);
	SVGMatrixImpl *flipX();
	SVGMatrixImpl *flipY();
	SVGMatrixImpl *skewX(const double &angle);
	SVGMatrixImpl *skewY(const double &angle);

	// Post-multiplied operations
	SVGMatrixImpl *postMultiply(const SVGMatrixImpl *secondMatrix);
	SVGMatrixImpl *postTranslate(const double &x, const double &y);
	SVGMatrixImpl *postScale(const double &scaleFactor);
	SVGMatrixImpl *postScaleNonUniform(const double &scaleFactorX, const double &scaleFactorY);
	SVGMatrixImpl *postRotate(const double &angle);
	SVGMatrixImpl *postRotateFromVector(const double &x, const double &y);
	SVGMatrixImpl *postFlipX();
	SVGMatrixImpl *postFlipY();
	SVGMatrixImpl *postSkewX(const double &angle);
	SVGMatrixImpl *postSkewY(const double &angle);

	void reset();

	// KSVG helper method
	QWMatrix &qmatrix();
	const QWMatrix &qmatrix() const;

	// Determine the scaling component of the matrix and factor it out. After
	// this operation, the matrix has x and y scale of one.
	void removeScale(double *xScale, double *yScale);

	KSVGPolygon map(const KSVGPolygon& polygon) const;
	KSVGPolygon inverseMap(const KSVGPolygon& polygon) const;

private:
	void setMatrix(QWMatrix mat);
	QWMatrix m_mat;
	
public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		A, B, C, D, E, F,
		// Functions
		Inverse, Multiply, Translate, Scale, Rotate,
		RotateFromVector, ScaleNonUniform,
		FlipX, FlipY, SkewX, SkewY
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

KSVG_DEFINE_PROTOTYPE(SVGMatrixImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGMatrixImplProtoFunc, SVGMatrixImpl)

#endif

// vim:ts=4:noet
