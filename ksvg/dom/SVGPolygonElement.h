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

    You should have received a copy of the GNU Library General Public
    License along with this library; see the file COPYING.LIB.  If
    not, write to the Free Software Foundation, Inc., 51 Franklin Street,
    Fifth Floor, Boston, MA 02110-1301, USA.


    This file includes excerpts from the Scalable Vector Graphics
    (SVG) 1.0 Specification (Proposed Recommendation)
    http://www.w3.org/TR/SVG

    Copyright © 2001 World Wide Web Consortium, (Massachusetts
    Institute of Technology, Institut National de Recherche en
    Informatique et en Automatique, Keio University).
    All Rights Reserved.

    $Id: SVGPolygonElement.h 490589 2005-12-22 12:56:21Z scripty $
 */

#ifndef SVGPolygonElement_H
#define SVGPolygonElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGTransformable.h"
#include "SVGAnimatedPoints.h"

namespace KSVG
{

class SVGPointList;
class SVGPolygonElementImpl;

/**
 * The <code>polygon</code> element defines a closed shape consisting
 * of connected straight line segments.
 *
 * For more info look here : <a href =
 * "http://www.w3.org/TR/SVG/shapes.html#PolylineElement">9.7 The
 * 'polygon' element</a>.
 */
class SVGPolygonElement : public SVGElement,
						  public SVGTests,
						  public SVGLangSpace,
						  public SVGExternalResourcesRequired,
						  public SVGStylable,
						  public SVGTransformable,
						  public SVGAnimatedPoints
{
public:
	SVGPolygonElement();
	SVGPolygonElement(const SVGPolygonElement &);
	SVGPolygonElement &operator=(const SVGPolygonElement &);
	SVGPolygonElement(SVGPolygonElementImpl *);
	~SVGPolygonElement();

	/**
	 * Provides access to the base (i.e., static) contents of the
	 * points attribute.
	 *
	 * @return A static list of the polygons points
	 */
	SVGPointList points();

	/**
	 * Provides access to the current animated contents of the points
	 * attribute.
	 * If the given attribute or property is being animated, contains
	 * the current animated value of the attribute or property.
	 * If the given attribute or property is not currently being
	 * animated, contains the same value as <code>points</code>'.
	 *
	 * This attribute is animatable.
	 *
	 * @return A list of the polygons points
	 */
	SVGPointList animatedPoints();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPolygonElementImpl *handle() const { return impl; }

private:
	SVGPolygonElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
