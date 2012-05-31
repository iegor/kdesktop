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

    $Id: SVGTextElement.h 490589 2005-12-22 12:56:21Z scripty $
 */


#ifndef SVGTextElement_H
#define SVGTextElement_H

#include "SVGTextPositioningElement.h"
#include "SVGTransformable.h"

namespace KSVG
{

class SVGAnimatedLength;
class SVGTextElementImpl;

/**
 * @short The <code>text</code> element defines a graphics element
 * consisting of text.
 *
 * The XML character data within the <code>text</code> element, along
 * with relevant attributes and properties and character-to-glyph
 * mapping tables within the font itself, define the glyphs to be
 * rendered. (See <a
 * href="http://www.w3.org/TR/SVG/text.html#CharactersAndGlyphs"> Characters
 * and their corresponding glyphs </a>.) The attributes and properties on the
 * 'text' element indicate such things as the writing direction, font
 * specification and painting attributes which describe how exactly to render
 * the characters. Since <code>text</code> elements are rendered using the
 * same rendering methods as other graphics elements, all of the same
 * coordinate system transformations, painting, clipping and masking
 * features that apply to shapes such as paths and rectangles also
 * apply to <code>text</code> elements.
 *
 * It is possible to apply a gradient, pattern, clipping path, mask or
 * filter to text.When one of these facilities is applied to text and
 * keyword objectBoundingBox is used to specify a graphical effect
 * relative to the "object bounding box", then the object bounding box
 * units are computed relative to the entire 'text' element in all
 * cases, even when different effects are applied to different 'tspan'
 * elements within the same 'text' element.
 *

 * The <code>text</code> element renders its first glyph (after <a
 * href = "http://www.w3.org/TR/SVG/text.html#RelationshipWithBiDirectionality"
 * >bidirectionality</a> reordering) at the initial current text
 * position, which is established by the <code>x</code> and
 * <code>y</code> attributes on the <code>text</code> element (with
 * possible adjustments due to the value of the @ref text-anchor
 * property, the presence of a @ref textPath element containing the
 * first character, and/or an <code>x</code>, <code>y</code>,
 * <code>dx</code> or <code>dy</code> attributes on a @ref tspan, @ref
 * tref or @ref altGlyph element which contains the first character).
 * After the glyph(s) corresponding to the given character is(are)
 * rendered, the current text position is updated for the next
 * character. In the simplest case, the new current text position is
 * the previous current text position plus the glyphs' advance value
 * (horizontal or vertical).
 *
 * @see SVGShape
 * @see SVGTextPositioningElement
 *
 * For more info look here : <a href =
 * "http://www.w3.org/TR/SVG/text.html#TextElement"> 10.4 The
 * 'text' element</a>.
 */
class SVGTextElement : public SVGTextPositioningElement,
					   public SVGTransformable
{
public:
	SVGTextElement();
	SVGTextElement(const SVGTextElement &);
	SVGTextElement &operator=(const SVGTextElement &other);
	SVGTextElement(SVGTextElementImpl *);
	~SVGTextElement();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGTextElementImpl *handle() const { return impl; }

private:
	SVGTextElementImpl *impl;
};

}

#endif
