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

#ifndef SVGFEGaussianBlurElement_H
#define SVGFEGaussianBlurElement_H

#include "SVGElement.h"
#include "SVGFilterPrimitiveStandardAttributes.h"

namespace KSVG
{

class SVGAnimatedString;
class SVGAnimatedNumber;
class SVGFEGaussianBlurElementImpl;
class SVGFEGaussianBlurElement : public SVGElement,
								 public SVGFilterPrimitiveStandardAttributes
{
public:
	SVGFEGaussianBlurElement();
	SVGFEGaussianBlurElement(const SVGFEGaussianBlurElement &other);
	SVGFEGaussianBlurElement &operator=(const SVGFEGaussianBlurElement &other);
	SVGFEGaussianBlurElement(SVGFEGaussianBlurElementImpl *other);
	virtual ~SVGFEGaussianBlurElement();

	SVGAnimatedString in1() const;
	SVGAnimatedNumber stdDeviationX() const;
	SVGAnimatedNumber stdDeviationY() const;

	void setStdDeviation(float stdDeviationX, float stdDeviationY);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFEGaussianBlurElementImpl *handle() const { return impl; }

private:
	SVGFEGaussianBlurElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
