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

#ifndef SVGPathElement_H
#define SVGPathElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGTransformable.h"
#include "SVGAnimatedPathData.h"

namespace KSVG
{

class SVGPoint;
class SVGPathSegClosePath;
class SVGPathSegLinetoAbs;
class SVGPathSegLinetoRel;
class SVGPathSegLinetoHorizontalAbs;
class SVGPathSegLinetoHorizontalRel;
class SVGPathSegLinetoVerticalAbs;
class SVGPathSegLinetoVerticalRel;
class SVGPathSegMovetoAbs;
class SVGPathSegMovetoRel;
class SVGPathSegArcAbs;
class SVGPathSegArcRel;
class SVGPathSegCurvetoCubicAbs;
class SVGPathSegCurvetoCubicRel;
class SVGPathSegCurvetoCubicSmoothAbs;
class SVGPathSegCurvetoCubicSmoothRel;
class SVGPathSegCurvetoQuadraticAbs;
class SVGPathSegCurvetoQuadraticRel;
class SVGPathSegCurvetoQuadraticSmoothAbs;
class SVGPathSegCurvetoQuadraticSmoothRel;
class SVGPathElementImpl;
class SVGPathElement : public SVGElement,
					   public SVGTests,
					   public SVGLangSpace,
					   public SVGExternalResourcesRequired,
					   public SVGStylable,
					   public SVGTransformable,
					   public SVGAnimatedPathData
{ 
public:
	SVGPathElement();
	SVGPathElement(const SVGPathElement &);
	SVGPathElement &operator=(const SVGPathElement &other);  
	SVGPathElement(SVGPathElementImpl *);
	~SVGPathElement();

	//SVGAnimatedNumber pathLength() const;
	float getTotalLength();
	SVGPoint getPointAtLength(float distance);
	unsigned long getPathSegAtLength(float distance);

	SVGPathSegClosePath createSVGPathSegClosePath();
	SVGPathSegMovetoAbs createSVGPathSegMovetoAbs(float x,float y);
	SVGPathSegMovetoRel createSVGPathSegMovetoRel(float x,float y);
	SVGPathSegLinetoAbs createSVGPathSegLinetoAbs(float x,float y);
	SVGPathSegLinetoRel createSVGPathSegLinetoRel(float x,float y);
	SVGPathSegCurvetoCubicAbs createSVGPathSegCurvetoCubicAbs(float x,float y,float x1,float y1,float x2,float y2);
	SVGPathSegCurvetoCubicRel createSVGPathSegCurvetoCubicRel(float x,float y,float x1,float y1,float x2,float y2);
	SVGPathSegCurvetoQuadraticAbs createSVGPathSegCurvetoQuadraticAbs(float x,float y,float x1,float y1);
	SVGPathSegCurvetoQuadraticRel createSVGPathSegCurvetoQuadraticRel(float x,float y,float x1,float y1);
	SVGPathSegArcAbs createSVGPathSegArcAbs(float x,float y,float r1,float r2,float angle,bool largeArcFlag,bool sweepFlag);
	SVGPathSegArcRel createSVGPathSegArcRel(float x,float y,float r1,float r2,float angle,bool largeArcFlag,bool sweepFlag);
	SVGPathSegLinetoHorizontalAbs createSVGPathSegLinetoHorizontalAbs(float x);
	SVGPathSegLinetoHorizontalRel createSVGPathSegLinetoHorizontalRel(float x);
	SVGPathSegLinetoVerticalAbs createSVGPathSegLinetoVerticalAbs(float y);
	SVGPathSegLinetoVerticalRel createSVGPathSegLinetoVerticalRel(float y);
	SVGPathSegCurvetoCubicSmoothAbs createSVGPathSegCurvetoCubicSmoothAbs(float x,float y,float x2,float y2);
	SVGPathSegCurvetoCubicSmoothRel createSVGPathSegCurvetoCubicSmoothRel(float x,float y,float x2,float y2);
	SVGPathSegCurvetoQuadraticSmoothAbs createSVGPathSegCurvetoQuadraticSmoothAbs(float x,float y);
	SVGPathSegCurvetoQuadraticSmoothRel createSVGPathSegCurvetoQuadraticSmoothRel(float x,float y);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathElementImpl *handle() const { return impl; }

private:
	SVGPathElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
