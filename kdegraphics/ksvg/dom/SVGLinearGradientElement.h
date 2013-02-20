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


    This file includes excerpts from the Scalable Vector Graphics
    (SVG) 1.0 Specification (Proposed Recommendation)
    http://www.w3.org/TR/SVG

    Copyright © 2001 World Wide Web Consortium, (Massachusetts
    Institute of Technology, Institut National de Recherche en
    Informatique et en Automatique, Keio University).
    All Rights Reserved.
*/

#ifndef SVGLinearGradientElement_H
#define SVGLinearGradientElement_H

#include "SVGGradientElement.h"

namespace KSVG
{

class SVGAnimatedLength;
class SVGLinearGradientElementImpl;

/**
 * <code>linearGradient</code> elements are never rendered directly; their
 * only usage is as something that can be referenced using the
 * <code>fill</code> and <code>stroke</code> properties.
 * The <code>display</code> property does not apply to the
 * <code>linearGradient</code> element; thus,
 * <code>linearGradient</code> elements are not directly rendered even if the
 * <code>display</code> property is set to a value other than none, and
 * <code>linearGradient</code> elements are available for referencing even when
 * the <code>display</code> property on the <code>linearGradient</code> element
 * or any of its ancestors is set to none.
 *
 * For more information :
 * <a href="http://www.w3.org/TR/SVG/pservers.html#LinearGradients">
 * 13.2.2 Linear gradients</a>
 *
 */

class SVGLinearGradientElement : public SVGGradientElement
{
public:
	SVGLinearGradientElement();
	SVGLinearGradientElement(const SVGLinearGradientElement &other);
	SVGLinearGradientElement &operator=(const SVGLinearGradientElement &other);
	SVGLinearGradientElement(SVGLinearGradientElementImpl *other);
	virtual ~SVGLinearGradientElement();

	/**
	 * x1, y1, x2, y2 define a gradient vector for the linear gradient.
	 * This gradient vector provides starting and ending points onto which the
	 * @ref SVGGradientElement (gradient stops) are mapped. The values of x1,
	 * y1, x2, y2 can be either numbers or percentages.
	 *
	 * If the attribute is not specified, the effect is as if a value of "0%"
	 * were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The x1 value
	 */
	SVGAnimatedLength x1() const;

	/**
	 * x1, y1, x2, y2 define a gradient vector for the linear gradient.
	 * This gradient vector provides starting and ending points onto which the
	 * @ref SVGGradientElement (gradient stops) are mapped. The values of x1,
	 * y1, x2, y2 can be either numbers or percentages.
	 *
	 * If the attribute is not specified, the effect is as if a value of "0%"
	 * were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The y1 value
	 */
	SVGAnimatedLength y1() const;

	/**
	 * x1, y1, x2, y2 define a gradient vector for the linear gradient.
	 * This gradient vector provides starting and ending points onto which the
	 * @ref SVGGradientElement (gradient stops) are mapped. The values of x1,
	 * y1, x2, y2 can be either numbers or percentages.
	 *
	 * If the attribute is not specified, the effect is as if a value of "100%"
	 * were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The x2 value
	 */
	SVGAnimatedLength x2() const;

	/**
	 * x1, y1, x2, y2 define a gradient vector for the linear gradient.
	 * This gradient vector provides starting and ending points onto which the
	 * @ref SVGGradientElement (gradient stops) are mapped. The values of x1,
	 * y1, x2, y2 can be either numbers or percentages.
	 *
	 * If the attribute is not specified, the effect is as if a value of "0%"
	 * were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The y2 value
	 */
	SVGAnimatedLength y2() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGLinearGradientElementImpl *handle() const { return impl; }

private:
	SVGLinearGradientElementImpl *impl;
};

}

#endif

// vim:ts=4:noet

