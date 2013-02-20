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

#ifndef SVGStopElement_H
#define SVGStopElement_H

#include "SVGElement.h"
#include "SVGStylable.h"

namespace KSVG
{

class SVGAnimatedNumber;
class SVGStopElementImpl;

/**
 * The ramp of colors to use on a gradient is defined by the 'stop' elements that
 * are child elements to either the 'linearGradient' element or the 'radialGradient' element.
 */
class SVGStopElement : public SVGElement,
					   public SVGStylable
{
public:
	SVGStopElement();
	SVGStopElement(const SVGStopElement &other);
	SVGStopElement &operator=(const SVGStopElement &other);
	SVGStopElement(SVGStopElementImpl *other);
	virtual ~SVGStopElement();

	/**
	 * The <code>offset</code> attribute is either a <number> (usually ranging from 0 to 1)
	 * or a <percentage> (usually ranging from 0% to 100%) which indicates where the gradient
	 * stop is placed. For linear gradients, the offset attribute represents a location along
	 * the gradient vector. For radial gradients, it represents a percentage distance from
	 * (fx,fy) to the edge of the outermost/largest circle.
	 *
	 * This attribute is animatable.
	 *
	 * @return The offset where this gradient stop is placed.
	 */
	SVGAnimatedNumber offset() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGStopElementImpl *handle() const { return impl; }

private:
	SVGStopElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
