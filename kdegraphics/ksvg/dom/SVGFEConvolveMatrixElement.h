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

#ifndef SVGFEConvolveMatrixElement_H
#define SVGFEConvolveMatrixElement_H

#include "SVGElement.h"
#include "SVGFilterPrimitiveStandardAttributes.h"

namespace KSVG
{

enum
{
	SVG_EDGEMODE_UNKNOWN = 0,
	SVG_EDGEMODE_DUPLICATE = 1,
	SVG_EDGEMODE_WRAP = 2,
	SVG_EDGEMODE_NONE = 3
};

class SVGAnimatedInteger;
class SVGAnimatedNumberList;
class SVGAnimatedNumber;
class SVGAnimatedEnumeration;
class SVGAnimatedLength;
class SVGAnimatedBoolean;
class SVGFEConvolveMatrixElementImpl;
class SVGFEConvolveMatrixElement :	public SVGElement,
									public SVGFilterPrimitiveStandardAttributes
{
public:
	SVGFEConvolveMatrixElement();
	SVGFEConvolveMatrixElement(const SVGFEConvolveMatrixElement &other);
	SVGFEConvolveMatrixElement &operator=(const SVGFEConvolveMatrixElement &other);
	SVGFEConvolveMatrixElement(SVGFEConvolveMatrixElementImpl *other);
	virtual ~SVGFEConvolveMatrixElement();

	SVGAnimatedInteger orderX() const;
	SVGAnimatedInteger orderY() const;
	SVGAnimatedNumberList kernelMatrix() const;
	SVGAnimatedNumber divisor() const;
	SVGAnimatedNumber bias() const;
	SVGAnimatedInteger targetX() const;
	SVGAnimatedInteger targetY() const;
	SVGAnimatedEnumeration edgeMode() const;
	SVGAnimatedLength kernelUnitLengthX() const;
	SVGAnimatedLength kernelUnitLengthY() const;
	SVGAnimatedBoolean preserveAlpha() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFEConvolveMatrixElementImpl *handle() const { return impl; }

private:
	SVGFEConvolveMatrixElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
