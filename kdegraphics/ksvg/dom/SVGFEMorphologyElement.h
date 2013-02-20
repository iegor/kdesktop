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

#ifndef SVGFEMorphologyElement_H
#define SVGFEMorphologyElement_H

#include "SVGElement.h"
#include "SVGFilterPrimitiveStandardAttributes.h"

namespace KSVG
{

enum
{
	SVG_MORPHOLOGY_OPERATOR_UNKNOWN = 0,
	SVG_MORPHOLOGY_OPERATOR_ERODE   = 1,
	SVG_MORPHOLOGY_OPERATOR_DILATE  = 2
};

class SVGAnimatedString;
class SVGAnimatedEnumeration;
class SVGAnimatedLength;
class SVGFEMorphologyElementImpl;
class SVGFEMorphologyElement : public SVGElement,
							   public SVGFilterPrimitiveStandardAttributes
{
public:
	SVGFEMorphologyElement();
	SVGFEMorphologyElement(const SVGFEMorphologyElement &other);
	SVGFEMorphologyElement &operator=(const SVGFEMorphologyElement &other);
	SVGFEMorphologyElement(SVGFEMorphologyElementImpl *other);
	virtual ~SVGFEMorphologyElement();

	SVGAnimatedString in1() const;
	SVGAnimatedEnumeration Operator() const;
	SVGAnimatedLength radiusX() const;
	SVGAnimatedLength radiusY() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFEMorphologyElementImpl *handle() const { return impl; }

private:
	SVGFEMorphologyElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
