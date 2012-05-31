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

#ifndef SVGFEDisplacementMapElement_H
#define SVGFEDisplacementMapElement_H

#include "SVGElement.h"
#include "SVGFilterPrimitiveStandardAttributes.h"

namespace KSVG
{
enum
{
	SVG_CHANNEL_UNKNOWN = 0,
	SVG_CHANNEL_R       = 1,
	SVG_CHANNEL_G       = 2,
	SVG_CHANNEL_B       = 3,
	SVG_CHANNEL_A       = 4
};

class SVGAnimatedNumber;
class SVGAnimatedString;
class SVGAnimatedEnumeration;
class SVGFEDisplacementMapElementImpl;
class SVGFEDisplacementMapElement :	public SVGElement,
									public SVGFilterPrimitiveStandardAttributes
{
public:
	SVGFEDisplacementMapElement();
	SVGFEDisplacementMapElement(const SVGFEDisplacementMapElement &other);
	SVGFEDisplacementMapElement &operator=(const SVGFEDisplacementMapElement &other);
	SVGFEDisplacementMapElement(SVGFEDisplacementMapElementImpl *other);
	virtual ~SVGFEDisplacementMapElement();

	SVGAnimatedString in1() const;
	SVGAnimatedString in2() const;
	SVGAnimatedNumber scale() const;
	SVGAnimatedEnumeration xChannelSelector() const;
	SVGAnimatedEnumeration yChannelSelector() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFEDisplacementMapElementImpl *handle() const { return impl; }

private:
	SVGFEDisplacementMapElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
