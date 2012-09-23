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

#ifndef SVGFEColorMatrixElement_H
#define SVGFEColorMatrixElement_H

#include "SVGElement.h"
#include "SVGFilterPrimitiveStandardAttributes.h"

namespace KSVG
{

enum
{
	SVG_FECOLORMATRIX_TYPE_UNKNOWN = 0,
	SVG_FECOLORMATRIX_TYPE_MATRIX = 1,
	SVG_FECOLORMATRIX_TYPE_SATURATE = 2,
	SVG_FECOLORMATRIX_TYPE_HUEROTATE = 3,
	SVG_FECOLORMATRIX_TYPE_LUMINANCETOALPHA = 4
};

class SVGAnimatedString;
class SVGAnimatedEnumeration;
class SVGAnimatedNumberList;
class SVGFEColorMatrixElementImpl;
class SVGFEColorMatrixElement :	public SVGElement,
								public SVGFilterPrimitiveStandardAttributes 
{ 
public:
	SVGFEColorMatrixElement();
	SVGFEColorMatrixElement(const SVGFEColorMatrixElement &other);
	SVGFEColorMatrixElement &operator=(const SVGFEColorMatrixElement &other);
	SVGFEColorMatrixElement(SVGFEColorMatrixElementImpl *other);
	virtual ~SVGFEColorMatrixElement();

	SVGAnimatedString in1() const;
	SVGAnimatedEnumeration type() const;
	SVGAnimatedNumberList values() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFEColorMatrixElementImpl *handle() const { return impl; }

private:
	SVGFEColorMatrixElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
