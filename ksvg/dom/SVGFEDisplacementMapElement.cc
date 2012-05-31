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

#include "SVGFEDisplacementMapElement.h"
#include "SVGFEDisplacementMapElementImpl.h"
#include "SVGAnimatedString.h"
#include "SVGAnimatedNumber.h"
#include "SVGAnimatedEnumeration.h"

using namespace KSVG;

SVGFEDisplacementMapElement::SVGFEDisplacementMapElement() : SVGElement(), SVGFilterPrimitiveStandardAttributes()
{
	impl = 0;
}

SVGFEDisplacementMapElement::SVGFEDisplacementMapElement(const SVGFEDisplacementMapElement &other) : SVGElement(other), SVGFilterPrimitiveStandardAttributes(other), impl(0)
{
	(*this) = other;
}

SVGFEDisplacementMapElement &SVGFEDisplacementMapElement::operator =(const SVGFEDisplacementMapElement &other)
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

SVGFEDisplacementMapElement::SVGFEDisplacementMapElement(SVGFEDisplacementMapElementImpl *other) : SVGElement(other), SVGFilterPrimitiveStandardAttributes(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGFEDisplacementMapElement::~SVGFEDisplacementMapElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedString SVGFEDisplacementMapElement::in1() const
{
	if(!impl) return SVGAnimatedString(0);
	return SVGAnimatedString(impl->in1());
}

SVGAnimatedString SVGFEDisplacementMapElement::in2() const
{
	if(!impl) return SVGAnimatedString(0);
	return SVGAnimatedString(impl->in2());
}

SVGAnimatedNumber SVGFEDisplacementMapElement::scale() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->scale());
}

SVGAnimatedEnumeration SVGFEDisplacementMapElement::xChannelSelector() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->xChannelSelector());
}

SVGAnimatedEnumeration SVGFEDisplacementMapElement::yChannelSelector() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->yChannelSelector());
}

// vim:ts=4:noet
