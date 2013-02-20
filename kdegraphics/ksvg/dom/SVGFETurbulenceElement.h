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

#ifndef SVGFETurbulenceElement_H
#define SVGFETurbulenceElement_H

#include "SVGElement.h"
#include "SVGFilterPrimitiveStandardAttributes.h"

namespace KSVG
{
enum
{
	SVG_TURBULENCE_TYPE_UNKNOWN      = 0,
	SVG_TURBULENCE_TYPE_FRACTALNOISE = 1,
	SVG_TURBULENCE_TYPE_TURBULENCE   = 2,
	SVG_STITCHTYPE_UNKNOWN  = 0,
	SVG_STITCHTYPE_STITCH   = 1,
	SVG_STITCHTYPE_NOSTITCH = 2
};

class SVGAnimatedEnumeration;
class SVGAnimatedNumber;
class SVGAnimatedInteger;
class SVGFETurbulenceElementImpl;
class SVGFETurbulenceElement : public SVGElement,
							   public SVGFilterPrimitiveStandardAttributes
{
public:
	SVGFETurbulenceElement();
	SVGFETurbulenceElement(const SVGFETurbulenceElement &other);
	SVGFETurbulenceElement &operator=(const SVGFETurbulenceElement &other);
	SVGFETurbulenceElement(SVGFETurbulenceElementImpl *other);
	virtual ~SVGFETurbulenceElement();

	SVGAnimatedNumber baseFrequencyX() const;
	SVGAnimatedNumber baseFrequencyY() const;
	SVGAnimatedInteger numOctaves() const;
	SVGAnimatedNumber seed() const;
	SVGAnimatedEnumeration stitchTiles() const;
	SVGAnimatedEnumeration type() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFETurbulenceElementImpl *handle() const { return impl; }

private:
	SVGFETurbulenceElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
