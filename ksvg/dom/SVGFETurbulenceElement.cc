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

#include "SVGFETurbulenceElement.h"
#include "SVGFETurbulenceElementImpl.h"
#include "SVGAnimatedEnumeration.h"
#include "SVGAnimatedNumber.h"
#include "SVGAnimatedInteger.h"

using namespace KSVG;

SVGFETurbulenceElement::SVGFETurbulenceElement() : SVGElement(), SVGFilterPrimitiveStandardAttributes()
{
	impl = 0;
}

SVGFETurbulenceElement::SVGFETurbulenceElement(const SVGFETurbulenceElement &other) : SVGElement(other), SVGFilterPrimitiveStandardAttributes(other), impl(0)
{
	(*this) = other;
}

SVGFETurbulenceElement &SVGFETurbulenceElement::operator =(const SVGFETurbulenceElement &other)
{
	SVGElement::operator=(other);
        SVGFilterPrimitiveStandardAttributes::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGFETurbulenceElement::SVGFETurbulenceElement(SVGFETurbulenceElementImpl *other) : SVGElement(other), SVGFilterPrimitiveStandardAttributes(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGFETurbulenceElement::~SVGFETurbulenceElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedNumber SVGFETurbulenceElement::baseFrequencyX() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->baseFrequencyX());
}

SVGAnimatedNumber SVGFETurbulenceElement::baseFrequencyY() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->baseFrequencyY());
}

SVGAnimatedInteger SVGFETurbulenceElement::numOctaves() const
{
	if(!impl) return SVGAnimatedInteger(0);
	return SVGAnimatedInteger(impl->numOctaves());
}

SVGAnimatedNumber SVGFETurbulenceElement::seed() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->seed());
}

SVGAnimatedEnumeration SVGFETurbulenceElement::stitchTiles() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->stitchTiles());
}

SVGAnimatedEnumeration SVGFETurbulenceElement::type() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->type());
}

// vim:ts=4:noet
