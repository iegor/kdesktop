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

#ifndef SVGTSpanElement_H
#define SVGTSpanElement_H

#include "SVGTextPositioningElement.h"

namespace KSVG
{

class SVGTSpanElementImpl;

/**
 * Within a <code>text</code> element, text and font properties and the current
 * text position can be adjusted with absolute or relative coordinate values by
 * including a <code>tspan</code> element.
 *
 * For more information :
 * <a href="http://www.w3.org/TR/SVG/text.html#TSpanElement">
 * 10.5 the 'tspan' element</a>
 */
class SVGTSpanElement : public SVGTextPositioningElement
{
public:
	SVGTSpanElement();
	SVGTSpanElement(const SVGTSpanElement &other);
	SVGTSpanElement &operator=(const SVGTSpanElement &other);
	SVGTSpanElement(SVGTSpanElementImpl *other);
	virtual ~SVGTSpanElement();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGTSpanElementImpl *handle() const { return impl; }

private:
	SVGTSpanElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
