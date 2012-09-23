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

#ifndef SVGFEOffsetElement_H
#define SVGFEOffsetElement_H

#include "SVGElement.h"
#include "SVGFilterPrimitiveStandardAttributes.h"

namespace KSVG
{

class SVGAnimatedString;
class SVGAnimatedNumber;
class SVGFEOffsetElementImpl;
class SVGFEOffsetElement : public SVGElement,
						   public SVGFilterPrimitiveStandardAttributes
{
public:
	SVGFEOffsetElement();
	SVGFEOffsetElement(const SVGFEOffsetElement &other);
	SVGFEOffsetElement &operator=(const SVGFEOffsetElement &other);
	SVGFEOffsetElement(SVGFEOffsetElementImpl *other);
	virtual ~SVGFEOffsetElement();

	SVGAnimatedString in1() const;
	SVGAnimatedNumber dx() const;
	SVGAnimatedNumber dy() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFEOffsetElementImpl *handle() const { return impl; }

private:
	SVGFEOffsetElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
