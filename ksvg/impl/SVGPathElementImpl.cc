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

#include <math.h>
#include <cfloat>

#include <kdebug.h>
#include <klocale.h>

#include "SVGRectImpl.h"
#include "SVGPaintImpl.h"
#include "SVGPointImpl.h"
#include "SVGAngleImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGPathSegArcImpl.h"
#include "SVGPathSegListImpl.h"
#include "SVGPathElementImpl.h"
#include "SVGPathSegLinetoImpl.h"
#include "SVGPathSegMovetoImpl.h"
#include "SVGAnimatedNumberImpl.h"
#include "SVGPathSegClosePathImpl.h"
#include "SVGPathSegCurvetoCubicImpl.h"
#include "SVGPathSegLinetoVerticalImpl.h"
#include "SVGPathSegLinetoHorizontalImpl.h"
#include "SVGPathSegCurvetoQuadraticImpl.h"
#include "SVGPathSegCurvetoCubicSmoothImpl.h"
#include "SVGPathSegCurvetoQuadraticSmoothImpl.h"

#include "SVGPaint.h"

#include "CanvasItem.h"
#include "KSVGCanvas.h"
#include "BezierPath.h"
#include "Point.h"

using namespace KSVG;

#include "SVGPathElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGPathElementImpl::SVGPathElementImpl(DOM::ElementImpl *impl) : SVGShapeImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl(), SVGAnimatedPathDataImpl(), SVGPathParser()
{
	KSVG_EMPTY_FLAGS

	m_pathLength = new SVGAnimatedNumberImpl();
	m_pathLength->ref();

	m_pathLength->setBaseVal(0);
}

SVGPathElementImpl::~SVGPathElementImpl()
{
	pathSegList()->clear();

	if(m_pathLength)
		m_pathLength->deref();
}

SVGAnimatedNumberImpl *SVGPathElementImpl::pathLength() const
{
	return m_pathLength;
}

double SVGPathElementImpl::getTotalLength()
{
	T2P::BezierPath *path = ownerDoc()->canvas()->toBezierPath(m_item);
	if(path)
		return path->length();

	return 0;
}

SVGPointImpl *SVGPathElementImpl::getPointAtLength(double distance)
{
	SVGPointImpl *ret = SVGSVGElementImpl::createSVGPoint();
	double totalDistance = getTotalLength();
	T2P::BezierPath *path = ownerDoc()->canvas()->toBezierPath(m_item);
	if(path)
	{
		T2P::Point p;
		path->pointTangentNormalAt(distance / totalDistance, &p);
		ret->setX(p.x());
		ret->setY(p.y());
	}

	return ret;
}

unsigned long SVGPathElementImpl::getPathSegAtLength(double)
{
	return 0;
}

SVGPathSegClosePathImpl *SVGPathElementImpl::createSVGPathSegClosePath()
{
	SVGPathSegClosePathImpl *temp = new SVGPathSegClosePathImpl();
	temp->ref();

	return temp;
}

SVGPathSegMovetoAbsImpl *SVGPathElementImpl::createSVGPathSegMovetoAbs(double x, double y)
{
	SVGPathSegMovetoAbsImpl *temp = new SVGPathSegMovetoAbsImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	return temp;
}

SVGPathSegMovetoRelImpl *SVGPathElementImpl::createSVGPathSegMovetoRel(double x, double y)
{
	SVGPathSegMovetoRelImpl *temp = new SVGPathSegMovetoRelImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	return temp;
}

SVGPathSegLinetoAbsImpl *SVGPathElementImpl::createSVGPathSegLinetoAbs(double x, double y)
{
	SVGPathSegLinetoAbsImpl *temp = new SVGPathSegLinetoAbsImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	return temp;
}

SVGPathSegLinetoRelImpl *SVGPathElementImpl::createSVGPathSegLinetoRel(double x, double y)
{
	SVGPathSegLinetoRelImpl *temp = new SVGPathSegLinetoRelImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	return temp;
}

SVGPathSegCurvetoCubicAbsImpl *SVGPathElementImpl::createSVGPathSegCurvetoCubicAbs(double x, double y, double x1, double y1, double x2, double y2)
{
	SVGPathSegCurvetoCubicAbsImpl *temp = new SVGPathSegCurvetoCubicAbsImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	temp->setX1(x1);
	temp->setY1(y1);
	temp->setX2(x2);
	temp->setY2(y2);
	return temp;
}

SVGPathSegCurvetoCubicRelImpl *SVGPathElementImpl::createSVGPathSegCurvetoCubicRel(double x, double y, double x1, double y1, double x2, double y2)
{
	SVGPathSegCurvetoCubicRelImpl *temp = new SVGPathSegCurvetoCubicRelImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	temp->setX1(x1);
	temp->setY1(y1);
	temp->setX2(x2);
	temp->setY2(y2);
	return temp;
}

SVGPathSegCurvetoQuadraticAbsImpl *SVGPathElementImpl::createSVGPathSegCurvetoQuadraticAbs(double x, double y, double x1, double y1)
{
	SVGPathSegCurvetoQuadraticAbsImpl *temp = new SVGPathSegCurvetoQuadraticAbsImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	temp->setX1(x1);
	temp->setY1(y1);
	return temp;
}

SVGPathSegCurvetoQuadraticRelImpl *SVGPathElementImpl::createSVGPathSegCurvetoQuadraticRel(double x, double y, double x1, double y1)
{
	SVGPathSegCurvetoQuadraticRelImpl *temp = new SVGPathSegCurvetoQuadraticRelImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	temp->setX1(x1);
	temp->setY1(y1);
	return temp;
}

SVGPathSegArcAbsImpl *SVGPathElementImpl::createSVGPathSegArcAbs(double x, double y, double r1, double r2, double angle, bool largeArcFlag, bool sweepFlag)
{
	SVGPathSegArcAbsImpl *temp = new SVGPathSegArcAbsImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	temp->setR1(r1);
	temp->setR2(r2);
	temp->setAngle(angle);
	temp->setLargeArcFlag(largeArcFlag);
	temp->setSweepFlag(sweepFlag);
	return temp;
}

SVGPathSegArcRelImpl *SVGPathElementImpl::createSVGPathSegArcRel(double x, double y, double r1, double r2, double angle, bool largeArcFlag, bool sweepFlag)
{
	SVGPathSegArcRelImpl *temp = new SVGPathSegArcRelImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	temp->setR1(r1);
	temp->setR2(r2);
	temp->setAngle(angle);
	temp->setLargeArcFlag(largeArcFlag);
	temp->setSweepFlag(sweepFlag);
	return temp;
}

SVGPathSegLinetoHorizontalAbsImpl *SVGPathElementImpl::createSVGPathSegLinetoHorizontalAbs(double x)
{
	SVGPathSegLinetoHorizontalAbsImpl *temp = new SVGPathSegLinetoHorizontalAbsImpl();
	temp->ref();

	temp->setX(x);
	return temp;
}

SVGPathSegLinetoHorizontalRelImpl *SVGPathElementImpl::createSVGPathSegLinetoHorizontalRel(double x)
{
	SVGPathSegLinetoHorizontalRelImpl *temp = new SVGPathSegLinetoHorizontalRelImpl();
	temp->ref();

	temp->setX(x);
	return temp;
}

SVGPathSegLinetoVerticalAbsImpl *SVGPathElementImpl::createSVGPathSegLinetoVerticalAbs(double y)
{
	SVGPathSegLinetoVerticalAbsImpl *temp = new SVGPathSegLinetoVerticalAbsImpl();
	temp->ref();

	temp->setY(y);
	return temp;
}

SVGPathSegLinetoVerticalRelImpl *SVGPathElementImpl::createSVGPathSegLinetoVerticalRel(double y)
{
	SVGPathSegLinetoVerticalRelImpl *temp = new SVGPathSegLinetoVerticalRelImpl();
	temp->ref();

	temp->setY(y);
	return temp;
}

SVGPathSegCurvetoCubicSmoothAbsImpl *SVGPathElementImpl::createSVGPathSegCurvetoCubicSmoothAbs(double x, double y, double x2, double y2)
{
	SVGPathSegCurvetoCubicSmoothAbsImpl *temp = new SVGPathSegCurvetoCubicSmoothAbsImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	temp->setX2(x2);
	temp->setY2(y2);
	return temp;
}

SVGPathSegCurvetoCubicSmoothRelImpl *SVGPathElementImpl::createSVGPathSegCurvetoCubicSmoothRel(double x, double y, double x2, double y2)
{
	SVGPathSegCurvetoCubicSmoothRelImpl *temp = new SVGPathSegCurvetoCubicSmoothRelImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	temp->setX2(x2);
	temp->setY2(y2);
	return temp;
}

SVGPathSegCurvetoQuadraticSmoothAbsImpl *SVGPathElementImpl::createSVGPathSegCurvetoQuadraticSmoothAbs(double x, double y)
{
	SVGPathSegCurvetoQuadraticSmoothAbsImpl *temp = new SVGPathSegCurvetoQuadraticSmoothAbsImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	return temp;
}

SVGPathSegCurvetoQuadraticSmoothRelImpl *SVGPathElementImpl::createSVGPathSegCurvetoQuadraticSmoothRel(double x, double y)
{
	SVGPathSegCurvetoQuadraticSmoothRelImpl *temp = new SVGPathSegCurvetoQuadraticSmoothRelImpl();
	temp->ref();

	temp->setX(x);
	temp->setY(y);
	return temp;
}

void SVGPathElementImpl::svgMoveTo(double x1, double y1, bool, bool abs)
{
	if(abs)
		pathSegList()->appendItem(createSVGPathSegMovetoAbs(x1, y1));
	else
		pathSegList()->appendItem(createSVGPathSegMovetoRel(x1, y1));
}

void SVGPathElementImpl::svgLineTo(double x1, double y1, bool abs)
{
	if(abs)
		pathSegList()->appendItem(createSVGPathSegLinetoAbs(x1, y1));
	else
		pathSegList()->appendItem(createSVGPathSegLinetoRel(x1, y1));
}

void SVGPathElementImpl::svgLineToHorizontal(double x, bool abs)
{
	if(abs)
		pathSegList()->appendItem(createSVGPathSegLinetoHorizontalAbs(x));
	else
		pathSegList()->appendItem(createSVGPathSegLinetoHorizontalRel(x));
}

void SVGPathElementImpl::svgLineToVertical(double y, bool abs)
{
	if(abs)
		pathSegList()->appendItem(createSVGPathSegLinetoVerticalAbs(y));
	else
		pathSegList()->appendItem(createSVGPathSegLinetoVerticalRel(y));
}

void SVGPathElementImpl::svgCurveToCubic(double x1, double y1, double x2, double y2, double x, double y, bool abs)
{
	if(abs)
		pathSegList()->appendItem(createSVGPathSegCurvetoCubicAbs(x, y, x1, y1, x2, y2));
	else
		pathSegList()->appendItem(createSVGPathSegCurvetoCubicRel(x, y, x1, y1, x2, y2));
}

void SVGPathElementImpl::svgCurveToCubicSmooth(double x, double y, double x2, double y2, bool abs)
{
	if(abs)
		pathSegList()->appendItem(createSVGPathSegCurvetoCubicSmoothAbs(x2, y2, x, y));
	else
		pathSegList()->appendItem(createSVGPathSegCurvetoCubicSmoothRel(x2, y2, x, y));
}

void SVGPathElementImpl::svgCurveToQuadratic(double x, double y, double x1, double y1, bool abs)
{
	if(abs)
		pathSegList()->appendItem(createSVGPathSegCurvetoQuadraticAbs(x1, y1, x, y));
	else
		pathSegList()->appendItem(createSVGPathSegCurvetoQuadraticRel(x1, y1, x, y));
}

void SVGPathElementImpl::svgCurveToQuadraticSmooth(double x, double y, bool abs)
{
	if(abs)
		pathSegList()->appendItem(createSVGPathSegCurvetoQuadraticSmoothAbs(x, y));
	else
		pathSegList()->appendItem(createSVGPathSegCurvetoQuadraticSmoothRel(x, y));
}

void SVGPathElementImpl::svgArcTo(double x, double y, double r1, double r2, double angle, bool largeArcFlag, bool sweepFlag, bool abs)
{
	if(abs)
		pathSegList()->appendItem(createSVGPathSegArcAbs(x, y, r1, r2, angle, largeArcFlag, sweepFlag));
	else
		pathSegList()->appendItem(createSVGPathSegArcRel(x, y, r1, r2, angle, largeArcFlag, sweepFlag));
}

void SVGPathElementImpl::svgClosePath()
{
	pathSegList()->appendItem(createSVGPathSegClosePath());
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathElementImpl::s_hashTable 3
 d											SVGPathElementImpl::D										DontDelete|ReadOnly
 pathLength									SVGPathElementImpl::PathLength								DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGPathElementImplProto::s_hashTable 23
 getTotalLength								SVGPathElementImpl::GetTotalLength							DontDelete|Function 0
 getPointAtLength							SVGPathElementImpl::GetPointAtLength						DontDelete|Function 1
 getPathSegAtLength							SVGPathElementImpl::GetPathSegAtLength						DontDelete|Function 1
 createSVGPathSegClosePath					SVGPathElementImpl::CreateSVGPathSegClosePath				DontDelete|Function 0
 createSVGPathSegMovetoAbs					SVGPathElementImpl::CreateSVGPathSegMovetoAbs				DontDelete|Function 2
 createSVGPathSegMovetoRel					SVGPathElementImpl::CreateSVGPathSegMovetoRel				DontDelete|Function 2
 createSVGPathSegLinetoAbs					SVGPathElementImpl::CreateSVGPathSegLinetoAbs				DontDelete|Function 2
 createSVGPathSegLinetoRel					SVGPathElementImpl::CreateSVGPathSegLinetoRel				DontDelete|Function 2
 createSVGPathSegArcAbs						SVGPathElementImpl::CreateSVGPathSegArcAbs					DontDelete|Function 7
 createSVGPathSegArcRel						SVGPathElementImpl::CreateSVGPathSegArcRel					DontDelete|Function 7
 createSVGPathSegCurvetoCubicAbs			SVGPathElementImpl::CreateSVGPathSegCurvetoCubicAbs			DontDelete|Function 6
 createSVGPathSegCurvetoCubicRel			SVGPathElementImpl::CreateSVGPathSegCurvetoCubicRel			DontDelete|Function 6
 createSVGPathSegCurvetoQuadraticAbs		SVGPathElementImpl::CreateSVGPathSegCurvetoQuadraticAbs		DontDelete|Function 4
 createSVGPathSegCurvetoQuadraticRel		SVGPathElementImpl::CreateSVGPathSegCurvetoQuadraticRel		DontDelete|Function 4
 createSVGPathSegLinetoHorizontalAbs		SVGPathElementImpl::CreateSVGPathSegLinetoHorizontalAbs		DontDelete|Function 1
 createSVGPathSegLinetoHorizontalRel		SVGPathElementImpl::CreateSVGPathSegLinetoHorizontalRel		DontDelete|Function 1
 createSVGPathSegLinetoVerticalAbs			SVGPathElementImpl::CreateSVGPathSegLinetoVerticalAbs		DontDelete|Function 1
 createSVGPathSegLinetoVerticalRel			SVGPathElementImpl::CreateSVGPathSegLinetoVerticalRel		DontDelete|Function 1
 createSVGPathSegCurvetoCubicAbs			SVGPathElementImpl::CreateSVGPathSegCurvetoCubicAbs			DontDelete|Function 4
 createSVGPathSegCurvetoCubicRel			SVGPathElementImpl::CreateSVGPathSegCurvetoCubicRel			DontDelete|Function 4
 createSVGPathSegCurvetoQuadraticAbs		SVGPathElementImpl::CreateSVGPathSegCurvetoQuadraticAbs		DontDelete|Function 2
 createSVGPathSegCurvetoQuadraticRel		SVGPathElementImpl::CreateSVGPathSegCurvetoQuadraticRel		DontDelete|Function 2
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGPathElementImpl", SVGPathElementImplProto, SVGPathElementImplProtoFunc)

Value SVGPathElementImpl::getValueProperty(ExecState *exec, int token) const
{
	//KSVG_CHECK_ATTRIBUTE
		
	switch(token)
	{
		case PathLength:
			return m_pathLength->cache(exec);
		case D:
//	if(!attributeMode)
		{
				QString d;
				unsigned int nrSegs = pathSegList()->numberOfItems();
				SVGPathSegImpl *curseg = 0;
				for(unsigned int i = 0; i < nrSegs; i++)
				{
					curseg = pathSegList()->getItem(i);
					if(curseg)
						d += curseg->toString() + " ";
				}
				return String(d);
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case D:
		{
			pathSegList()->clear();
			QString d = value.toString(exec).qstring();
			parseSVG(d, false);
			if(hasMarkers())
				m_markerData = MarkerData(pathSegList());
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

Value SVGPathElementImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGPathElementImpl)

	switch(id)
	{
		case SVGPathElementImpl::GetTotalLength:
			return Number(obj->getTotalLength());
		case SVGPathElementImpl::GetPointAtLength:
			return obj->getPointAtLength(args[0].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::GetPathSegAtLength:
			return Number(obj->getPathSegAtLength(args[0].toNumber(exec)));
		case SVGPathElementImpl::CreateSVGPathSegClosePath:
			return obj->createSVGPathSegClosePath()->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegMovetoAbs:
			return obj->createSVGPathSegMovetoAbs(args[0].toNumber(exec), args[1].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegMovetoRel:
			return obj->createSVGPathSegMovetoRel(args[0].toNumber(exec), args[1].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegLinetoAbs:
			return obj->createSVGPathSegLinetoAbs(args[0].toNumber(exec), args[1].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegLinetoRel:
			return obj->createSVGPathSegLinetoRel(args[0].toNumber(exec), args[1].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegCurvetoCubicAbs:
			return obj->createSVGPathSegCurvetoCubicAbs(args[0].toNumber(exec), args[1].toNumber(exec), args[2].toNumber(exec), args[3].toNumber(exec), args[4].toNumber(exec), args[5].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegCurvetoCubicRel:
			return obj->createSVGPathSegCurvetoCubicRel(args[0].toNumber(exec), args[1].toNumber(exec), args[2].toNumber(exec), args[3].toNumber(exec), args[4].toNumber(exec), args[5].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegCurvetoQuadraticAbs:
			return obj->createSVGPathSegCurvetoQuadraticAbs(args[0].toNumber(exec), args[1].toNumber(exec), args[2].toNumber(exec), args[3].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegCurvetoQuadraticRel:
			return obj->createSVGPathSegCurvetoQuadraticRel(args[0].toNumber(exec), args[1].toNumber(exec), args[2].toNumber(exec), args[3].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegArcAbs:
			return obj->createSVGPathSegArcAbs(args[0].toNumber(exec), args[1].toNumber(exec), args[2].toNumber(exec), args[3].toNumber(exec), args[4].toNumber(exec), args[5].toBoolean(exec), args[6].toBoolean(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegArcRel:
			return obj->createSVGPathSegArcRel(args[0].toNumber(exec), args[1].toNumber(exec), args[2].toNumber(exec), args[3].toNumber(exec), args[4].toNumber(exec), args[5].toBoolean(exec), args[6].toBoolean(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegLinetoHorizontalAbs:
			return obj->createSVGPathSegLinetoHorizontalAbs(args[0].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegLinetoHorizontalRel:
			return obj->createSVGPathSegLinetoHorizontalRel(args[0].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegLinetoVerticalAbs:
			return obj->createSVGPathSegLinetoVerticalAbs(args[0].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegLinetoVerticalRel:
			return obj->createSVGPathSegLinetoVerticalRel(args[0].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegCurvetoCubicSmoothAbs:
			return obj->createSVGPathSegCurvetoCubicSmoothAbs(args[0].toNumber(exec), args[1].toNumber(exec), args[2].toNumber(exec), args[3].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegCurvetoCubicSmoothRel:
			return obj->createSVGPathSegCurvetoCubicSmoothRel(args[0].toNumber(exec), args[1].toNumber(exec), args[2].toNumber(exec), args[3].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegCurvetoQuadraticSmoothAbs:
			return obj->createSVGPathSegCurvetoQuadraticSmoothAbs(args[0].toNumber(exec), args[1].toNumber(exec))->cache(exec);
		case SVGPathElementImpl::CreateSVGPathSegCurvetoQuadraticSmoothRel:
			return obj->createSVGPathSegCurvetoQuadraticSmoothRel(args[0].toNumber(exec), args[1].toNumber(exec))->cache(exec);
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

SVGRectImpl *SVGPathElementImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();

	if(m_item)
	{
		T2P::BezierPath *path = ownerDoc()->canvas()->toBezierPath(m_item);
		if(path)
		{
			T2P::Point topLeft;
			T2P::Point bottomRight;

			path->boundingBox(&topLeft, &bottomRight);

			ret->setX(topLeft.x());
			ret->setY(topLeft.y());
			ret->setWidth(bottomRight.x() - topLeft.x());
			ret->setHeight(bottomRight.y() - topLeft.y());
		}
	}
	return ret;
}

void SVGPathElementImpl::createItem(KSVGCanvas *c)
{
	if(!c)
		c = ownerDoc()->canvas();

	if(!m_item)
	{
		// TODO : this is a quick fix for this problem:
		// d attribute encountered before marker attributes.
		// Try to process the attributes in the right order, ie.
		// d attr processing should be last.
		if(hasMarkers() && m_markerData.numMarkers() == 0)
			m_markerData = MarkerData(pathSegList());
		m_item = c->createPath(this);
		c->insert(m_item);
	}
}

SVGPathElementImpl::MarkerData::MarkerData(SVGPathSegListImpl *path)
{
	unsigned int numSegments = path->numberOfItems();
	double curx = 0;
	double cury = 0;
	int currentSubpathStartIndex = -1;
	double previousQuadraticX1 = 0;
	double previousQuadraticY1 = 0;
	double previousCubicX2 = 0;
	double previousCubicY2 = 0;

	QValueVector<SegmentData> pathSegmentData(numSegments);

	for(unsigned int i = 0; i < numSegments; i++)
	{
		SVGPathSegImpl *segment = path->getItem(i);
		struct SegmentData data;

		data.type = segment->pathSegType();

		if(segment->pathSegType() == PATHSEG_MOVETO_ABS || segment->pathSegType() == PATHSEG_MOVETO_REL)
		{
			if(currentSubpathStartIndex >= 0)
			{
				// Finish the previous subpath.
				for(unsigned int j = currentSubpathStartIndex; j < i; j++)
				{
					pathSegmentData[j].subpathStartIndex = currentSubpathStartIndex;
					pathSegmentData[j].subpathEndIndex = i - 1;
					pathSegmentData[j].subpathIsClosed = false;
				}
			}

			currentSubpathStartIndex = i;
		}
		else if(segment->pathSegType() == PATHSEG_CLOSEPATH)
		{
			if(currentSubpathStartIndex >= 0)
			{
				SVGPathSegClosePathImpl *s = static_cast<SVGPathSegClosePathImpl *>(segment);

				s->setX(pathSegmentData[currentSubpathStartIndex].startx + pathSegmentData[currentSubpathStartIndex].dx);
				s->setY(pathSegmentData[currentSubpathStartIndex].starty + pathSegmentData[currentSubpathStartIndex].dy);

				for(unsigned int j = currentSubpathStartIndex; j < i; j++)
				{
					pathSegmentData[j].subpathStartIndex = currentSubpathStartIndex;
					pathSegmentData[j].subpathEndIndex = i;
					pathSegmentData[j].subpathIsClosed = true;
				}

				data.subpathStartIndex = currentSubpathStartIndex;
				data.subpathEndIndex = i;
				data.subpathIsClosed = true;
			}

			currentSubpathStartIndex = i + 1;
		}

		switch(segment->pathSegType())
		{
			case PATHSEG_CURVETO_CUBIC_ABS:
			{
				SVGPathSegCurvetoCubicAbsImpl *s = static_cast<SVGPathSegCurvetoCubicAbsImpl *>(segment);
				previousCubicX2 = s->x2();
				previousCubicY2 = s->y2();
				break;
			}
			case PATHSEG_CURVETO_CUBIC_REL:
			{
				SVGPathSegCurvetoCubicRelImpl *s = static_cast<SVGPathSegCurvetoCubicRelImpl *>(segment);
				previousCubicX2 = curx + s->x2();
				previousCubicY2 = cury + s->y2();
				break;
			}
			case PATHSEG_CURVETO_CUBIC_SMOOTH_ABS:
			{
				SVGPathSegCurvetoCubicSmoothAbsImpl *s = static_cast<SVGPathSegCurvetoCubicSmoothAbsImpl *>(segment);
				s->setPreviousX2(previousCubicX2);
				s->setPreviousY2(previousCubicY2);
				previousCubicX2 = s->x2();
				previousCubicY2 = s->y2();
				break;
			}
			case PATHSEG_CURVETO_CUBIC_SMOOTH_REL:
			{
				SVGPathSegCurvetoCubicSmoothRelImpl *s = static_cast<SVGPathSegCurvetoCubicSmoothRelImpl *>(segment);
				s->setPreviousAbsX2(previousCubicX2);
				s->setPreviousAbsY2(previousCubicY2);
				previousCubicX2 = curx + s->x2();
				previousCubicY2 = cury + s->y2();
				break;
			}
			case PATHSEG_CURVETO_QUADRATIC_ABS:
			{
				SVGPathSegCurvetoQuadraticAbsImpl *s = static_cast<SVGPathSegCurvetoQuadraticAbsImpl *>(segment);
				previousQuadraticX1 = s->x1();
				previousQuadraticY1 = s->y1();
				break;
			}
			case PATHSEG_CURVETO_QUADRATIC_REL:
			{
				SVGPathSegCurvetoQuadraticRelImpl *s = static_cast<SVGPathSegCurvetoQuadraticRelImpl *>(segment);
				previousQuadraticX1 = curx + s->x1();
				previousQuadraticY1 = cury + s->y1();
				break;
			}
			case PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS:
			{
				SVGPathSegCurvetoQuadraticSmoothAbsImpl *s = static_cast<SVGPathSegCurvetoQuadraticSmoothAbsImpl *>(segment);
				s->setPreviousX1(previousQuadraticX1);
				s->setPreviousY1(previousQuadraticY1);
				previousQuadraticX1 = s->x1(curx);
				previousQuadraticY1 = s->y1(cury);
				break;
			}
			case PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL:
			{
				SVGPathSegCurvetoQuadraticSmoothRelImpl *s = static_cast<SVGPathSegCurvetoQuadraticSmoothRelImpl *>(segment);
				s->setPreviousAbsX1(previousQuadraticX1);
				s->setPreviousAbsY1(previousQuadraticY1);
				previousQuadraticX1 = s->absX1(curx);
				previousQuadraticY1 = s->absY1(cury);
				break;
			}
			default:
				previousCubicX2 = curx;
				previousCubicY2 = cury;
				previousQuadraticX1 = curx;
				previousQuadraticY1 = cury;
				break;
		}

		data.startx = curx;
		data.starty = cury;

		segment->getDeltasAndSlopes(curx, cury, &data.dx, &data.dy, &data.startSlope, &data.endSlope);

		pathSegmentData[i] = data;

		curx += data.dx;
		cury += data.dy;
	}

	if(currentSubpathStartIndex >= 0)
	{
		// Finish the previous subpath.
		for(unsigned int j = currentSubpathStartIndex; j < numSegments; j++)
		{
			pathSegmentData[j].subpathStartIndex = currentSubpathStartIndex;
			pathSegmentData[j].subpathEndIndex = numSegments - 1;
			pathSegmentData[j].subpathIsClosed = false;
		}
	}

	m_markers.resize(numSegments);

	for(unsigned int i = 0; i < numSegments; i++)
	{
		struct Marker marker;

		marker.x = pathSegmentData[i].startx + pathSegmentData[i].dx;
		marker.y = pathSegmentData[i].starty + pathSegmentData[i].dy;

		double inSlope;
		double outSlope;
		bool haveInSlope = false;
		bool haveOutSlope = false;

		if(pathSegmentData[i].subpathStartIndex == i && pathSegmentData[i].subpathIsClosed)
		{
			// Spec: For closed subpaths, the marker for the initial vertex uses the end direction
			// of the corresponding closepath for its incoming slope and the first segment's
			// start slope for its outgoing slope.
			haveInSlope = getEndSlope(pathSegmentData, pathSegmentData[i].subpathEndIndex, &inSlope);
			haveOutSlope = getStartSlope(pathSegmentData, i + 1, &outSlope);
		}
		else if(pathSegmentData[i].type == PATHSEG_CLOSEPATH)
		{
			haveInSlope = getEndSlope(pathSegmentData, i, &inSlope);

			// Spec: If the segment following a closepath is other than a moveto, the marker
			// for the closepath uses the following segment's start direction as its
			// outgoing direction.
			if(i + 1 < numSegments && (pathSegmentData[i + 1].type != PATHSEG_MOVETO_ABS && pathSegmentData[i + 1].type != PATHSEG_MOVETO_REL))
				haveOutSlope = getStartSlope(pathSegmentData, i + 1, &outSlope);
			else
				haveOutSlope = getStartSlope(pathSegmentData, pathSegmentData[i].subpathStartIndex, &outSlope);
		}
		else
		{
			haveOutSlope = getStartSlope(pathSegmentData, i + 1, &outSlope);
			haveInSlope = getEndSlope(pathSegmentData, i, &inSlope);
		}

		if(!haveInSlope && !haveOutSlope)
		{
			outSlope = 0;
			inSlope = 0;
		}
		else if(haveInSlope && !haveOutSlope)
			outSlope = inSlope;
		else if(!haveInSlope && haveOutSlope)
			inSlope = outSlope;

		double bisector = SVGAngleImpl::shortestArcBisector(inSlope, outSlope);
		marker.angle = bisector;

		m_markers[i] = marker;
	}
}

bool SVGPathElementImpl::MarkerData::getStartSlope(QValueVector<SegmentData> segments, unsigned int i, double *pStartSlope)
{
	if(i > segments.count() - 1 || segments[i].type == PATHSEG_MOVETO_ABS || segments[i].type == PATHSEG_MOVETO_REL)
		return false;
	else
	{
		const double epsilon = DBL_EPSILON;

		if(fabs(segments[i].dx) > epsilon || fabs(segments[i].dy) > epsilon)
		{
			*pStartSlope = segments[i].startSlope;
			return true;
		}
		else
		{
			int subpathStartIndex = segments[i].subpathStartIndex;

			for(int j = i - 1; j >= subpathStartIndex; j--)
			{
				if(segments[j].type == PATHSEG_MOVETO_ABS || segments[j].type == PATHSEG_MOVETO_REL)
					return false;

				if(fabs(segments[j].dx) > epsilon || fabs(segments[j].dy) > epsilon)
				{
					*pStartSlope = segments[j].endSlope;
					return true;
				}
			}

			return false;
		}
	}
}

bool SVGPathElementImpl::MarkerData::getEndSlope(QValueVector<SegmentData> segments, unsigned int i, double *pEndSlope)
{
	if(i > segments.count() - 1 || segments[i].type == PATHSEG_MOVETO_ABS || segments[i].type == PATHSEG_MOVETO_REL)
		return false;
	else
	{
		const double epsilon = DBL_EPSILON;

		if(fabs(segments[i].dx) > epsilon || fabs(segments[i].dy) > epsilon)
		{
			*pEndSlope = segments[i].endSlope;
			return true;
		}
		else
		{
			int subpathEndIndex = segments[i].subpathEndIndex;

			for(int j = i + 1; j <= subpathEndIndex; j++)
			{
				if(segments[j].type == PATHSEG_MOVETO_ABS || segments[j].type == PATHSEG_MOVETO_REL)
					return false;

				if(fabs(segments[j].dx) > epsilon || fabs(segments[j].dy) > epsilon)
				{
					*pEndSlope = segments[j].startSlope;
					return true;
				}
			}

			return false;
		}
	}
}

// vim:ts=4:noet
