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

#include "SVGPathElement.h"
#include "SVGPathElementImpl.h"
#include "SVGPoint.h"
#include "SVGPathSegClosePath.h"
#include "SVGPathSegLineto.h"
#include "SVGPathSegLinetoHorizontal.h"
#include "SVGPathSegLinetoVertical.h"
#include "SVGPathSegMoveto.h"
#include "SVGPathSegArc.h"
#include "SVGPathSegCurvetoCubic.h"
#include "SVGPathSegCurvetoCubicSmooth.h"
#include "SVGPathSegCurvetoQuadratic.h"
#include "SVGPathSegCurvetoQuadraticSmooth.h"

using namespace KSVG;

SVGPathElement::SVGPathElement() : SVGElement(), SVGTests(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable(), SVGTransformable(), SVGAnimatedPathData()
{
	impl = 0;
}

SVGPathElement::SVGPathElement(const SVGPathElement &other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGTransformable(other), SVGAnimatedPathData(other), impl(0)
{
	(*this) = other;
}

SVGPathElement &SVGPathElement::operator=(const SVGPathElement &other)
{
	SVGElement::operator=(other);
	SVGTests::operator=(other);
	SVGLangSpace::operator=(other);
	SVGExternalResourcesRequired::operator=(other);
	SVGStylable::operator=(other);
	SVGTransformable::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGPathElement::SVGPathElement(SVGPathElementImpl *other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGTransformable(other), SVGAnimatedPathData(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGPathElement::~SVGPathElement()
{
	if(impl)
		impl->deref();
}

/*
SVGAnimatedNumber SVGPathElement::pathLength() const
{
	return m_pathLength;
}
*/

float SVGPathElement::getTotalLength()
{
	return impl->getTotalLength();
}

SVGPoint SVGPathElement::getPointAtLength(float distance)
{
	return SVGPoint(impl->getPointAtLength(distance));
}

unsigned long SVGPathElement::getPathSegAtLength(float distance)
{
	return impl->getPathSegAtLength(distance);
}

SVGPathSegClosePath SVGPathElement::createSVGPathSegClosePath()
{
	return impl->createSVGPathSegClosePath();
}

SVGPathSegMovetoAbs SVGPathElement::createSVGPathSegMovetoAbs(float x,float y)
{
	return impl->createSVGPathSegMovetoAbs(x, y);;
}

SVGPathSegMovetoRel SVGPathElement::createSVGPathSegMovetoRel(float x,float y)
{
	return impl->createSVGPathSegMovetoRel(x, y);
}

SVGPathSegLinetoAbs SVGPathElement::createSVGPathSegLinetoAbs(float x,float y)
{
	return impl->createSVGPathSegLinetoAbs(x, y);
}

SVGPathSegLinetoRel SVGPathElement::createSVGPathSegLinetoRel(float x,float y)
{
	return impl->createSVGPathSegLinetoRel(x, y);
}

SVGPathSegCurvetoCubicAbs SVGPathElement::createSVGPathSegCurvetoCubicAbs(float x,float y,float x1,float y1,float x2,float y2)
{
	return impl->createSVGPathSegCurvetoCubicAbs(x, y, x1, y1, x2, y2);
}

SVGPathSegCurvetoCubicRel SVGPathElement::createSVGPathSegCurvetoCubicRel(float x,float y,float x1,float y1,float x2,float y2)
{
	return impl->createSVGPathSegCurvetoCubicRel(x, y, x1, y1, x2, y2);
}

SVGPathSegCurvetoQuadraticAbs SVGPathElement::createSVGPathSegCurvetoQuadraticAbs(float x,float y,float x1,float y1)
{
	return impl->createSVGPathSegCurvetoQuadraticAbs(x, y, x1, y1);
}

SVGPathSegCurvetoQuadraticRel SVGPathElement::createSVGPathSegCurvetoQuadraticRel(float x,float y,float x1,float y1)
{
	return impl->createSVGPathSegCurvetoQuadraticRel(x, y, x1, y1);
}

SVGPathSegArcAbs SVGPathElement::createSVGPathSegArcAbs(float x,float y,float r1,float r2,float angle,bool largeArcFlag,bool sweepFlag)
{
	return impl->createSVGPathSegArcAbs(x, y, r1, r2, angle, largeArcFlag, sweepFlag);
}

SVGPathSegArcRel SVGPathElement::createSVGPathSegArcRel(float x,float y,float r1,float r2,float angle,bool largeArcFlag,bool sweepFlag)
{
	return impl->createSVGPathSegArcRel(x, y, r1, r2, angle, largeArcFlag, sweepFlag);
}

SVGPathSegLinetoHorizontalAbs SVGPathElement::createSVGPathSegLinetoHorizontalAbs(float x)
{
	return impl->createSVGPathSegLinetoHorizontalAbs(x);
}

SVGPathSegLinetoHorizontalRel SVGPathElement::createSVGPathSegLinetoHorizontalRel(float x)
{
	return impl->createSVGPathSegLinetoHorizontalRel(x);
}

SVGPathSegLinetoVerticalAbs SVGPathElement::createSVGPathSegLinetoVerticalAbs(float y)
{
	return impl->createSVGPathSegLinetoVerticalAbs(y);
}

SVGPathSegLinetoVerticalRel SVGPathElement::createSVGPathSegLinetoVerticalRel(float y)
{
	return impl->createSVGPathSegLinetoVerticalRel(y);
}

SVGPathSegCurvetoCubicSmoothAbs SVGPathElement::createSVGPathSegCurvetoCubicSmoothAbs(float x,float y,float x2,float y2)
{
	return impl->createSVGPathSegCurvetoCubicSmoothAbs(x, y, x2, y2);;
}

SVGPathSegCurvetoCubicSmoothRel SVGPathElement::createSVGPathSegCurvetoCubicSmoothRel(float x,float y,float x2,float y2)
{
	return impl->createSVGPathSegCurvetoCubicSmoothRel(x, y, x2, y2);
}

SVGPathSegCurvetoQuadraticSmoothAbs SVGPathElement::createSVGPathSegCurvetoQuadraticSmoothAbs(float x,float y)
{
	return impl->createSVGPathSegCurvetoQuadraticSmoothAbs(x, y);
}

SVGPathSegCurvetoQuadraticSmoothRel SVGPathElement::createSVGPathSegCurvetoQuadraticSmoothRel(float x,float y)
{
	return impl->createSVGPathSegCurvetoQuadraticSmoothRel(x, y);
}

// vim:ts=4:noet
