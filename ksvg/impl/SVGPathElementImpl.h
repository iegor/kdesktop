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

#ifndef SVGPathElementImpl_H
#define SVGPathElementImpl_H

#include <qvaluevector.h>

#include "svgpathparser.h"

#include "ksvg_lookup.h"

#include "SVGShapeImpl.h"
#include "SVGTestsImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGTransformableImpl.h"
#include "SVGAnimatedPathDataImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

namespace KSVG
{

class SVGPointImpl;
class SVGPathSegImpl;
class SVGPathSegArcAbsImpl;
class SVGPathSegArcRelImpl;
class SVGAnimatedNumberImpl;
class SVGPathSegClosePathImpl;
class SVGPathSegLinetoAbsImpl;
class SVGPathSegLinetoRelImpl;
class SVGPathSegMovetoAbsImpl;
class SVGPathSegMovetoRelImpl;
class SVGPathSegCurvetoCubicAbsImpl;
class SVGPathSegCurvetoCubicRelImpl;
class SVGPathSegLinetoVerticalAbsImpl;
class SVGPathSegLinetoVerticalRelImpl;
class SVGPathSegLinetoHorizontalAbsImpl;
class SVGPathSegLinetoHorizontalRelImpl;
class SVGPathSegCurvetoQuadraticAbsImpl;
class SVGPathSegCurvetoQuadraticRelImpl;
class SVGPathSegCurvetoCubicSmoothAbsImpl;
class SVGPathSegCurvetoCubicSmoothRelImpl;
class SVGPathSegCurvetoQuadraticSmoothAbsImpl;
class SVGPathSegCurvetoQuadraticSmoothRelImpl;
class SVGPathElementImpl : public SVGShapeImpl,
						   public SVGTestsImpl,
						   public SVGLangSpaceImpl,
						   public SVGExternalResourcesRequiredImpl,
						   public SVGStylableImpl,
						   public SVGTransformableImpl,
						   public SVGAnimatedPathDataImpl,
						   public ::SVGPathParser
{
public:
	SVGPathElementImpl(DOM::ElementImpl *impl);
	virtual ~SVGPathElementImpl();

	SVGAnimatedNumberImpl *pathLength() const;
	double getTotalLength();
	SVGPointImpl *getPointAtLength(double distance);
	unsigned long getPathSegAtLength(double distance);

	SVGPathSegClosePathImpl *createSVGPathSegClosePath();
	SVGPathSegMovetoAbsImpl *createSVGPathSegMovetoAbs(double x, double y);
	SVGPathSegMovetoRelImpl *createSVGPathSegMovetoRel(double x, double y);
	SVGPathSegLinetoAbsImpl *createSVGPathSegLinetoAbs(double x, double y);
	SVGPathSegLinetoRelImpl *createSVGPathSegLinetoRel(double x, double y);
	SVGPathSegCurvetoCubicAbsImpl *createSVGPathSegCurvetoCubicAbs(double x, double y, double x1, double y1, double x2, double y2);
	SVGPathSegCurvetoCubicRelImpl *createSVGPathSegCurvetoCubicRel(double x, double y, double x1, double y1, double x2, double y2);
	SVGPathSegCurvetoQuadraticAbsImpl *createSVGPathSegCurvetoQuadraticAbs(double x, double y, double x1, double y1);
	SVGPathSegCurvetoQuadraticRelImpl *createSVGPathSegCurvetoQuadraticRel(double x, double y, double x1, double y1);
	SVGPathSegArcAbsImpl *createSVGPathSegArcAbs(double x, double y, double r1, double r2, double angle, bool largeArcFlag, bool sweepFlag);
	SVGPathSegArcRelImpl *createSVGPathSegArcRel(double x, double y, double r1, double r2, double angle, bool largeArcFlag, bool sweepFlag);
	SVGPathSegLinetoHorizontalAbsImpl *createSVGPathSegLinetoHorizontalAbs(double x);
	SVGPathSegLinetoHorizontalRelImpl *createSVGPathSegLinetoHorizontalRel(double x);
	SVGPathSegLinetoVerticalAbsImpl *createSVGPathSegLinetoVerticalAbs(double y);
	SVGPathSegLinetoVerticalRelImpl *createSVGPathSegLinetoVerticalRel(double y);
	SVGPathSegCurvetoCubicSmoothAbsImpl *createSVGPathSegCurvetoCubicSmoothAbs(double x, double y, double x2, double y2);
	SVGPathSegCurvetoCubicSmoothRelImpl *createSVGPathSegCurvetoCubicSmoothRel(double x, double y, double x2, double y2);
	SVGPathSegCurvetoQuadraticSmoothAbsImpl *createSVGPathSegCurvetoQuadraticSmoothAbs(double x, double y);
	SVGPathSegCurvetoQuadraticSmoothRelImpl *createSVGPathSegCurvetoQuadraticSmoothRel(double x, double y);

	virtual void createItem(KSVGCanvas *c = 0);

	virtual SVGRectImpl *getBBox();

	class MarkerData
	{
	public:
		struct Marker
		{
			double x;
			double y;
			double angle;
		};

		MarkerData() {}
		MarkerData(SVGPathSegListImpl *path);

		const Marker& marker(unsigned int i) const { return m_markers[i]; }
		unsigned int numMarkers() const { return m_markers.count(); }

	private:
		struct SegmentData
		{
			double startx;
			double starty;
			double dx;
			double dy;
			double startSlope;
			double endSlope;
			unsigned int subpathStartIndex;
			unsigned int subpathEndIndex;
			bool subpathIsClosed;
			int type;
		};

		static bool getStartSlope(QValueVector<SegmentData> segments, unsigned int i, double *pStartSlope);
		static bool getEndSlope(QValueVector<SegmentData> segments, unsigned int i, double *pEndSlope);

		QValueVector<Marker> m_markers;
	};

	MarkerData markerData() const { return m_markerData; }

private:
	SVGAnimatedNumberImpl *m_pathLength;
	MarkerData m_markerData;

	virtual void svgMoveTo(double x1, double y1, bool closed, bool abs = true);
	virtual void svgLineTo(double x1, double y1, bool abs = true);
	virtual void svgLineToHorizontal(double x, bool abs = true);
	virtual void svgLineToVertical(double y, bool abs = true);
	virtual void svgCurveToCubic(double x1, double y1, double x2, double y2, double x, double y, bool abs = true);
	virtual void svgCurveToCubicSmooth(double x, double y, double x2, double y2, bool abs = true);
	virtual void svgCurveToQuadratic(double x, double y, double x1, double y1, bool abs = true);
	virtual void svgCurveToQuadraticSmooth(double x, double y, bool abs = true);
	virtual void svgArcTo(double x, double y, double r1, double r2, double angle, bool largeArcFlag, bool sweepFlag, bool abs = true);
	virtual void svgClosePath();

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		D, PathLength,
		// Functions
		GetTotalLength, GetPointAtLength, GetPathSegAtLength,
		CreateSVGPathSegClosePath, CreateSVGPathSegMovetoAbs, CreateSVGPathSegMovetoRel,
		CreateSVGPathSegLinetoAbs, CreateSVGPathSegLinetoRel, CreateSVGPathSegCurvetoCubicAbs,
		CreateSVGPathSegCurvetoCubicRel, CreateSVGPathSegCurvetoQuadraticAbs,
		CreateSVGPathSegCurvetoQuadraticRel, CreateSVGPathSegArcAbs,
		CreateSVGPathSegArcRel, CreateSVGPathSegLinetoHorizontalAbs,
		CreateSVGPathSegLinetoHorizontalRel, CreateSVGPathSegLinetoVerticalAbs,
		CreateSVGPathSegLinetoVerticalRel, CreateSVGPathSegCurvetoCubicSmoothAbs,
		CreateSVGPathSegCurvetoCubicSmoothRel, CreateSVGPathSegCurvetoQuadraticSmoothAbs,
		CreateSVGPathSegCurvetoQuadraticSmoothRel
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGPathElementImpl, "path")

}

KSVG_DEFINE_PROTOTYPE(SVGPathElementImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGPathElementImplProtoFunc, SVGPathElementImpl)

#endif

// vim:ts=4:noet
