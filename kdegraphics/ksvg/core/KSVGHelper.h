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
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KSVGHelper_H
#define KSVGHelper_H

#ifdef __cplusplus

#include <qcolor.h>
#include <qvaluevector.h>
#include "Affine.h"
#include "Point.h"
#include "SVGMatrixImpl.h"

namespace KSVG
{

class KSVGHelper
{
public:
	KSVGHelper();

	static void matrixToAffine(SVGMatrixImpl *matrix, double affine[6])
	{
		affine[0] = matrix->a();
		affine[1] = matrix->b();
		affine[2] = matrix->c();
		affine[3] = matrix->d();
		affine[4] = matrix->e();
		affine[5] = matrix->f();
	}

	static void matrixToAffine(const SVGMatrixImpl *matrix, double affine[6])
	{
		KSVGHelper::matrixToAffine(const_cast<SVGMatrixImpl *>(matrix), affine);
	}

	static void matrixToAffine(const SVGMatrixImpl *matrix, T2P::Affine &affine)
	{
		KSVGHelper::matrixToAffine(const_cast<SVGMatrixImpl *>(matrix), affine.data());
	}

	static void matrixToAffine(SVGMatrixImpl *matrix, T2P::Affine &affine)
	{
		KSVGHelper::matrixToAffine(matrix, affine.data());
	}

	static QString toColorString(QColor color)
	{
		int r = color.red();
		int g = color.green();
		int b = color.blue();

		return "rgb(" + QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + ")";
	}

	static unsigned int toArtColor(const QColor &color)
	{
		return (qRed(color.rgb()) << 24) | (qGreen(color.rgb()) << 16) | ( qBlue(color.rgb()) << 8) | (qAlpha(color.rgb()));
	}

	static unsigned int toArtColor(const QColor &color, short opacity)
	{
		return (qRed(color.rgb()) << 24) | (qGreen(color.rgb()) << 16) | ( qBlue(color.rgb()) << 8) | (opacity);
	}

	static int linearRGBFromsRGB(int sRGB8bit) { return m_linearRGBFromsRGB[sRGB8bit]; }
	static int sRGBFromLinearRGB(int linearRGB8bit) { return m_sRGBFromLinearRGB[linearRGB8bit]; }

private:
	static void initialise();
	static int calcLinearRGBFromsRGB(int sRGB8bit);
	static int calcSRGBFromLinearRGB(int linearRGB8bit);

	static bool m_initialised;

	static int m_linearRGBFromsRGB[256];
	static int m_sRGBFromLinearRGB[256];
};

typedef T2P::Point KSVGPoint;

class KSVGPolygon
{
public:
	KSVGPolygon() {}

	void addPoint(const KSVGPoint& point) { m_points.append(point); }
	void addPoint(double x, double y) { m_points.append(KSVGPoint(x, y)); }

	KSVGPoint point(unsigned int index) const { return index < m_points.count() ? m_points[index] : KSVGPoint(); }

	unsigned int numPoints() const { return m_points.count(); }
	bool isEmpty() const { return m_points.isEmpty(); }

	void clear() { m_points.clear(); }

private:
	QValueVector<KSVGPoint> m_points;
};

class KSVGRectangle : public KSVGPolygon
{
public:
	KSVGRectangle() { addPoint(0, 0); addPoint(0, 0); addPoint(0, 0); addPoint(0, 0); }

	// Convenience constructor for an axis-aligned rectangle
	KSVGRectangle(double x, double y, double width, double height)
	{ addPoint(KSVGPoint(x, y)); addPoint(KSVGPoint(x, y + height)); addPoint(KSVGPoint(x + width, y + height)); addPoint(KSVGPoint(x + width, y)); }

};
}

extern "C"
{
#endif // __cplusplus

int linearRGBFromsRGB(int sRGB8bit);
int sRGBFromLinearRGB(int linearRGB8bit);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif

// vim:ts=4:noet
