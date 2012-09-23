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

#ifndef SVGComponentTransferFunctionElement_H
#define SVGComponentTransferFunctionElement_H

#include "SVGElement.h"

namespace KSVG
{

enum
{
	SVG_FECOMPONENTTRANSFER_TYPE_UNKNOWN = 0,
	SVG_FECOMPONENTTRANSFER_TYPE_IDENTITY = 1,
	SVG_FECOMPONENTTRANSFER_TYPE_TABLE = 2,
	SVG_FECOMPONENTTRANSFER_TYPE_DISCRETE = 3,
	SVG_FECOMPONENTTRANSFER_TYPE_LINEAR = 4,
	SVG_FECOMPONENTTRANSFER_TYPE_GAMMA = 5
};

class SVGAnimatedEnumeration;
class SVGAnimatedNumberList;
class SVGAnimatedNumber;
class SVGComponentTransferFunctionElementImpl;
class SVGComponentTransferFunctionElement : public SVGElement
{
public:
	SVGComponentTransferFunctionElement();
	SVGComponentTransferFunctionElement(const SVGComponentTransferFunctionElement &other);
	SVGComponentTransferFunctionElement &operator=(const SVGComponentTransferFunctionElement &other);
	SVGComponentTransferFunctionElement(SVGComponentTransferFunctionElementImpl *other);
	virtual ~SVGComponentTransferFunctionElement();

	SVGAnimatedEnumeration type() const;
	SVGAnimatedNumberList tableValues() const;
	SVGAnimatedNumber slope() const;
	SVGAnimatedNumber intercept() const;
	SVGAnimatedNumber amplitude() const;
	SVGAnimatedNumber exponent() const;
	SVGAnimatedNumber offset() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGComponentTransferFunctionElementImpl *handle() const { return impl; }

private:
	SVGComponentTransferFunctionElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
