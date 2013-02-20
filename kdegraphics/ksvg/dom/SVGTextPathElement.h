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

#ifndef SVGTextPathElement_H
#define SVGTextPathElement_H

#include "SVGTextContentElement.h"
#include "SVGURIReference.h"

namespace KSVG
{

enum
{
	TEXTPATH_METHODTYPE_UNKNOWN = 0,
	TEXTPATH_METHODTYPE_ALIGN = 1,
	TEXTPATH_METHODTYPE_STRETCH = 2
};

enum
{
	TEXTPATH_SPACINGTYPE_UNKNOWN = 0,
	TEXTPATH_SPACINGTYPE_AUTO = 1,
	TEXTPATH_SPACINGTYPE_EXACT = 2		
};

class SVGTextPathElementImpl;
class SVGTextPathElement : public SVGTextContentElement,
						   public SVGURIReference
{
public:
	SVGTextPathElement();
	SVGTextPathElement(const SVGTextPathElement &other);
	SVGTextPathElement &operator=(const SVGTextPathElement &other);
	SVGTextPathElement(SVGTextPathElementImpl *other);
	virtual ~SVGTextPathElement();

	SVGAnimatedLength startOffset() const;
	SVGAnimatedEnumeration method() const;
	SVGAnimatedEnumeration spacing() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGTextPathElementImpl *handle() const { return impl; }

private:
	SVGTextPathElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
