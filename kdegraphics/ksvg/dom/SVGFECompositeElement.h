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

#ifndef SVGFECompositeElement_H
#define SVGFECompositeElement_H

#include "SVGElement.h"
#include "SVGFilterPrimitiveStandardAttributes.h"

namespace KSVG
{

enum
{
	SVG_FECOMPOSITE_OPERATOR_UNKNOWN = 0,
	SVG_FECOMPOSITE_OPERATOR_OVER = 1,
	SVG_FECOMPOSITE_OPERATOR_IN = 2,
	SVG_FECOMPOSITE_OPERATOR_OUT = 3,
	SVG_FECOMPOSITE_OPERATOR_ATOP = 4,
	SVG_FECOMPOSITE_OPERATOR_XOR = 5,
	SVG_FECOMPOSITE_OPERATOR_ARITHMETIC = 6
};

class SVGAnimatedString;
class SVGAnimatedEnumeration;
class SVGAnimatedNumber;
class SVGFECompositeElementImpl;
class SVGFECompositeElement : public SVGElement,
							  public SVGFilterPrimitiveStandardAttributes
{
public:
	SVGFECompositeElement();
	SVGFECompositeElement(const SVGFECompositeElement &other);
	SVGFECompositeElement &operator=(const SVGFECompositeElement &other);
	SVGFECompositeElement(SVGFECompositeElementImpl *other);
	virtual ~SVGFECompositeElement();

	SVGAnimatedString in1() const;
	SVGAnimatedString in2() const;
	SVGAnimatedEnumeration Operator() const; // TODO : impossible otherwise ?
	SVGAnimatedNumber k1() const;
	SVGAnimatedNumber k2() const;
	SVGAnimatedNumber k3() const;
	SVGAnimatedNumber k4() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFECompositeElementImpl *handle() const { return impl; }

private:
	SVGFECompositeElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
