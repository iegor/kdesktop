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

#include "SVGLinearGradientElement.h"
#include "SVGLinearGradientElementImpl.h"
#include "SVGAnimatedLength.h"

using namespace KSVG;

SVGLinearGradientElement::SVGLinearGradientElement() : SVGGradientElement()
{
	impl = 0;
}

SVGLinearGradientElement::SVGLinearGradientElement(const SVGLinearGradientElement &other) : SVGGradientElement(other), impl(0)
{
	(*this) = other;
}

SVGLinearGradientElement &SVGLinearGradientElement::operator =(const SVGLinearGradientElement &other)
{
	SVGGradientElement::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGLinearGradientElement::SVGLinearGradientElement(SVGLinearGradientElementImpl *other) : SVGGradientElement(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGLinearGradientElement::~SVGLinearGradientElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedLength SVGLinearGradientElement::x1() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->x1());
}

SVGAnimatedLength SVGLinearGradientElement::y1() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->y1());
}

SVGAnimatedLength SVGLinearGradientElement::x2() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->x2());
}

SVGAnimatedLength SVGLinearGradientElement::y2() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->y2());
}

// vim:ts=4:noet
