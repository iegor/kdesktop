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

    $Id: SVGEllipseElement.h 490589 2005-12-22 12:56:21Z scripty $
 */

#ifndef SVGEllipseElement_H
#define SVGEllipseElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGTransformable.h"

namespace KSVG
{

class SVGAnimatedLength;
class SVGEllipseElementImpl;

/**
 * The <code>ellipse</code> element defines an ellipse which is
 * axis-aligned with the current user coordinate system based on a
 * center point and two radii.
 *
 * For more info look here : <a href =
 * "http://www.w3.org/TR/SVG/shapes.html#EllipseElement">9.4 The
 * 'ellipse' element</a>.
 */
class SVGEllipseElement : public SVGElement,
						  public SVGTests,
						  public SVGLangSpace,
						  public SVGExternalResourcesRequired,
						  public SVGStylable,
						  public SVGTransformable
{
public:
	SVGEllipseElement();
	SVGEllipseElement(const SVGEllipseElement &);
	SVGEllipseElement &operator=(const SVGEllipseElement &other);
	SVGEllipseElement(SVGEllipseElementImpl *);
	~SVGEllipseElement();

	/**
	 * The x-axis coordinate of the center of the ellipse.
	 * If the attribute is not specified, the effect is as if a value
	 * of "0" were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The x-axis coordinate of the center of the ellipse.
	 */
	SVGAnimatedLength cx();

	/**
	 * The y-axis coordinate of the center of the ellipse.
	 * If the attribute is not specified, the effect is as if a value
	 * of "0" were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The y-axis coordinate of the center of the ellipse.
	 */
	SVGAnimatedLength cy();

	/**
	 * The x-axis radius of the ellipse.
	 * A negative value is an error (see <a href =
	 * "http://www.w3.org/TR/SVG/implnote.html#ErrorProcessing">
	 * Error processing</a>). A value of zero disables rendering of
	 * the element.
	 *
	 * This attribute is animatable.
	 *
	 * @return The x-radius of the ellipse.
	 */
	SVGAnimatedLength rx();

	/**
	 * The y-axis radius of the ellipse.
	 * A negative value is an error (see <a href =
	 * "http://www.w3.org/TR/SVG/implnote.html#ErrorProcessing">
	 * Error processing</a>). A value of zero disables rendering of
	 * the element.
	 *
	 * This attribute is animatable.
	 *
	 * @return The y-radius of the ellipse.
	 */
	SVGAnimatedLength ry();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGEllipseElementImpl *handle() const { return impl; }

private:
	SVGEllipseElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
