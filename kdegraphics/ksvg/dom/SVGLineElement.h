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

    $Id: SVGLineElement.h 490589 2005-12-22 12:56:21Z scripty $
 */

#ifndef SVGLineElement_H
#define SVGLineElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGTransformable.h"

namespace KSVG
{

class SVGAnimatedLength;
class SVGLineElementImpl;

/**
 * The <code>line</code> element defines a line segment that starts at
 * one point and ends at another.
 *
 * For more info look here : <a href =
 * "http://www.w3.org/TR/SVG/shapes.html#CircleElement">9.5 The
 * 'line' element</a>.
 */
class SVGLineElement : public SVGElement,
					   public SVGTests,
				   	   public SVGLangSpace,
					   public SVGExternalResourcesRequired,
					   public SVGStylable,
					   public SVGTransformable
{
public:
	SVGLineElement();
	SVGLineElement(const SVGLineElement &);
	SVGLineElement &operator=(const SVGLineElement &other);
	SVGLineElement(SVGLineElementImpl *);
	~SVGLineElement();

	/**
	 * The x-axis coordinate of the start of the line.
	 * If the attribute is not specified, the effect is as if a value
	 * of "0" were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The x-axis coordinate of the start of the line.
	 */
	SVGAnimatedLength x1();

	/**
	 * The y-axis coordinate of the start of the line.
	 * If the attribute is not specified, the effect is as if a value
	 * of "0" were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The y-axis coordinate of the start of the line.
	 */
	SVGAnimatedLength y1();

	/**
	 * The x-axis coordinate of the end of the line.
	 * If the attribute is not specified, the effect is as if a value
	 * of "0" were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The x-axis coordinate of the end of the line.
	 */
	SVGAnimatedLength x2();

	/**
	 * The y-axis coordinate of the end of the line.
	 * If the attribute is not specified, the effect is as if a value
	 * of "0" were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The y-axis coordinate of the end of the line.
	 */
	SVGAnimatedLength y2();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGLineElementImpl *handle() const { return impl; }

private:
	SVGLineElementImpl *impl;
};

}

#endif

//vim:ts=4:noet
