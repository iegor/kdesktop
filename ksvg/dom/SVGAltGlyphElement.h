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

#ifndef SVGAltGlyphElement_H
#define SVGAltGlyphElement_H

#include "SVGTextPositioningElement.h"
#include "SVGURIReference.h"

namespace KSVG
{

class SVGAltGlyphElementImpl;
class SVGAltGlyphElement :	public SVGTextPositioningElement,
							public SVGURIReference 
{
public:
	SVGAltGlyphElement();
	SVGAltGlyphElement(const SVGAltGlyphElement &other);
	SVGAltGlyphElement &operator=(const SVGAltGlyphElement &other);
	SVGAltGlyphElement(SVGAltGlyphElementImpl *other);
	virtual ~SVGAltGlyphElement();

	DOM::DOMString glyphRef();
	DOM::DOMString format();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGAltGlyphElementImpl *handle() const { return impl; }

private:
	SVGAltGlyphElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
