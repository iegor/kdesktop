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

#include <kdebug.h>
#include "SVGTextPositioningElement.h"
#include "SVGTextPositioningElementImpl.h"
#include "SVGAnimatedNumber.h"
#include "SVGAnimatedNumberList.h"
#include "SVGAnimatedLength.h"
#include "SVGAnimatedLengthList.h"

using namespace KSVG;

SVGTextPositioningElement::SVGTextPositioningElement() : SVGTextContentElement()
{
	impl = new SVGTextPositioningElementImpl(0);
}

SVGTextPositioningElement::SVGTextPositioningElement(const SVGTextPositioningElement &other) : SVGTextContentElement(other)
{
	impl = other.impl;
}

SVGTextPositioningElement &SVGTextPositioningElement::operator=(const SVGTextPositioningElement &other)
{
	SVGTextContentElement::operator=(other);

	if(impl == other.impl)
		return *this;

	delete impl;
	impl = other.impl;

	return *this;
}
SVGTextPositioningElement::SVGTextPositioningElement(SVGTextPositioningElementImpl *other) : SVGTextContentElement(other)
{
	impl = other;
}

SVGTextPositioningElement::~SVGTextPositioningElement()
{
}

SVGAnimatedLengthList SVGTextPositioningElement::x()
{
	return SVGAnimatedLengthList(impl->x());
}

SVGAnimatedLengthList SVGTextPositioningElement::y()
{
	return SVGAnimatedLengthList(impl->y());
}

SVGAnimatedLengthList SVGTextPositioningElement::dx()
{
	return SVGAnimatedLengthList(impl->dx());
}

SVGAnimatedLengthList SVGTextPositioningElement::dy()
{
	return SVGAnimatedLengthList(impl->dy());
}

SVGAnimatedNumberList SVGTextPositioningElement::rotate()
{
	return SVGAnimatedNumberList(impl->rotate());
}
