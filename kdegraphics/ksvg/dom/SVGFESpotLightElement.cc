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

#include "SVGFESpotLightElement.h"
#include "SVGFESpotLightElementImpl.h"
#include "SVGAnimatedNumber.h"

using namespace KSVG;

SVGFESpotLightElement::SVGFESpotLightElement() : SVGElement()
{
	impl = 0;
}

SVGFESpotLightElement::SVGFESpotLightElement(const SVGFESpotLightElement &other) : SVGElement(other), impl(0)
{
	(*this) = other;
}

SVGFESpotLightElement &SVGFESpotLightElement::operator =(const SVGFESpotLightElement &other)
{
	SVGElement::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGFESpotLightElement::SVGFESpotLightElement(SVGFESpotLightElementImpl *other) : SVGElement(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGFESpotLightElement::~SVGFESpotLightElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedNumber SVGFESpotLightElement::x() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->x());
}

SVGAnimatedNumber SVGFESpotLightElement::y() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->y());
}

SVGAnimatedNumber SVGFESpotLightElement::z() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->z());
}

SVGAnimatedNumber SVGFESpotLightElement::pointsAtX() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->pointsAtX());
}

SVGAnimatedNumber SVGFESpotLightElement::pointsAtY() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->pointsAtY());
}

SVGAnimatedNumber SVGFESpotLightElement::pointsAtZ() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->pointsAtZ());
}

SVGAnimatedNumber SVGFESpotLightElement::specularExponent() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->specularExponent());
}

SVGAnimatedNumber SVGFESpotLightElement::limitingConeAngle() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->limitingConeAngle());
}

// vim:ts=4:noet
