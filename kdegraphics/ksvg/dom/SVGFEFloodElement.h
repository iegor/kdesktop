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

#ifndef SVGFEFloodElement_H
#define SVGFEFloodElement_H

#include "SVGElement.h"
#include "SVGStylable.h"
#include "SVGFilterPrimitiveStandardAttributes.h"

namespace KSVG
{

class SVGAnimatedString;
class SVGFEFloodElementImpl;
class SVGFEFloodElement :	public SVGElement,
							public SVGStylable,
							public SVGFilterPrimitiveStandardAttributes
{
public:
	SVGFEFloodElement();
	SVGFEFloodElement(const SVGFEFloodElement &other);
	SVGFEFloodElement &operator=(const SVGFEFloodElement &other);
	SVGFEFloodElement(SVGFEFloodElementImpl *other);
	virtual ~SVGFEFloodElement();

	SVGAnimatedString in1() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFEFloodElementImpl *handle() const { return impl; }

private:
	SVGFEFloodElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
