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

#include "SVGFEDistantLightElement.h"
#include "SVGFEDistantLightElementImpl.h"
#include "SVGAnimatedNumber.h"

using namespace KSVG;

SVGFEDistantLightElement::SVGFEDistantLightElement() : SVGElement()
{
	impl = 0;
}

SVGFEDistantLightElement::SVGFEDistantLightElement(const SVGFEDistantLightElement &other) : SVGElement(other), impl(0)
{
	(*this) = other;
}

SVGFEDistantLightElement &SVGFEDistantLightElement::operator =(const SVGFEDistantLightElement &other)
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

SVGFEDistantLightElement::SVGFEDistantLightElement(SVGFEDistantLightElementImpl *other) : SVGElement(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGFEDistantLightElement::~SVGFEDistantLightElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedNumber SVGFEDistantLightElement::azimuth() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->azimuth());
}

SVGAnimatedNumber SVGFEDistantLightElement::elevation() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->elevation());
}

// vim:ts=4:noet
