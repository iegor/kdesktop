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

#ifndef SVGFEBlendElement_H
#define SVGFEBlendElement_H

#include "SVGElement.h"
#include "SVGFilterPrimitiveStandardAttributes.h"

namespace KSVG
{

enum
{
	SVG_FEBLEND_MODE_UNKNOWN = 0,
	SVG_FEBLEND_MODE_NORMAL = 1,
	SVG_FEBLEND_MODE_MULTIPLY = 2,
	SVG_FEBLEND_MODE_SCREEN = 3,
	SVG_FEBLEND_MODE_DARKEN = 4,
	SVG_FEBLEND_MODE_LIGHTEN = 5
};

class SVGAnimatedString;
class SVGAnimatedEnumeration;
class SVGFEBlendElementImpl;
class SVGFEBlendElement : public SVGElement,
						  public SVGFilterPrimitiveStandardAttributes 
{ 
public:
	SVGFEBlendElement();
	SVGFEBlendElement(const SVGFEBlendElement &other);
	SVGFEBlendElement &operator=(const SVGFEBlendElement &other);
	SVGFEBlendElement(SVGFEBlendElementImpl *other);
	virtual ~SVGFEBlendElement();

	SVGAnimatedString in1() const;
	SVGAnimatedString in2() const;
	SVGAnimatedEnumeration mode() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFEBlendElementImpl *handle() const { return impl; }

private:
	SVGFEBlendElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
