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

    $Id: SVGRectElement.h 490589 2005-12-22 12:56:21Z scripty $
 */

#ifndef SVGRectElement_H
#define SVGRectElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGTransformable.h"

namespace KSVG
{

class SVGAnimatedLength;
class SVGRectElementImpl;

/**
 * The <code> rect </code> element defines a rectangle which is
 * axis-aligned with the current <a href =
 * "http://www.w3.org/TR/SVG/intro.html#TermUserCoordinateSystem">
 * user coordinate</a> system.
 * Rounded rectangles can be achieved by setting appropriate values
 * for attributes <code> x </code> and <code> y</code>.
 *
 * For more info look here : <a href =
 * "http://www.w3.org/TR/SVG/shapes.html#RectElement"> 9.2 The
 * 'rect' element</a>.
 */
class SVGRectElement : public SVGElement,
					   public SVGTests,
					   public SVGLangSpace,
					   public SVGExternalResourcesRequired,
					   public SVGStylable,
					   public SVGTransformable
{
public:
	SVGRectElement();
	SVGRectElement(const SVGRectElement &);
	SVGRectElement &operator=(const SVGRectElement &other);
	SVGRectElement(SVGRectElementImpl *);
	~SVGRectElement();

	/**
	 * The x-axis coordinate of the side of the rectangle which has
	 * the smaller x-axis coordinate value in the current user
	 * coordinate system.
	 * If the attribute is not specified, the effect is as if a value
	 * of "0" were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The x-axis coordinate of the rectangle.
	 */
	SVGAnimatedLength x();

	/**
	 * The y-axis coordinate of the side of the rectangle which has
	 * the smaller y-axis coordinate value in the current user
	 * coordinate system. If the attribute is not specified, the
	 * effect is as if a value of "0" were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The y-axis coordinate of the rectangle.
	 */
	SVGAnimatedLength y();

	/**
	 * The width of the rectangle. A negative value is an error (see
	 * <a href =
	 * "http://www.w3.org/TR/SVG/implnote.html#ErrorProcessing">
	 * Error processing</a>).
	 * A value of zero disables rendering of the element.
	 *
	 * This attribute is animatable.
	 *
	 * @return The width of the rectangle.
	 */
	SVGAnimatedLength width();

	/**
	 * The heigth of the rectangle. A negative value is an error (see
	 * <a href =
	 * "http://www.w3.org/TR/SVG/implnote.html#ErrorProcessing">
	 * Error processing</a>). A value of zero disables rendering of
	 * the element.
	 *
	 * This attribute is animatable.
	 *
	 * @return The height of the rectangle
	 */
	SVGAnimatedLength height();

	/**
	 * For rounded rectangles, the x-axis radius of the ellipse used
	 * to round off the corners of the rectangle. A negative value is
	 * an error (see <a 	 href =
	 * "http://www.w3.org/TR/SVG/implnote.html#ErrorProcessing"> Error
	 * processing</a>).
	 *
	 * See <a href="http://www.w3.org/TR/SVG/shapes.html#RectElement">
	 * 9.2 The 'rect' element </a> for info about what happens if the
	 * attribute is not specified.
	 *
	 * This attribute is animatable
	 *
	 * @return The x-axis radius of the ellipse used to round off the
	 * corners of the rectangle.
	 */
	SVGAnimatedLength rx();

	/**
	 * For rounded rectangles, the y-axis radius of the ellipse used
	 * to round off the corners of the rectangle. A negative value is
	 * an error (see <a href =
	 * "http://www.w3.org/TR/SVG/implnote.html#ErrorProcessing">Error
	 * processing</a>).
	 *
	 * See <a href="http://www.w3.org/TR/SVG/shapes.html#RectElement">
	 * 9.2 The 'rect' element </a> for info about what happens if the
	 * attribute is not specified.
	 *
	 * This attribute is animatable
	 *
	 * @return The y-axis radius of the ellipse used to round off the
	 * corners of the rectangle.
	 */
	SVGAnimatedLength ry();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGRectElementImpl *handle() const { return impl; }

private:
	SVGRectElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
