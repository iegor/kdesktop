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

#include "SVGRadialGradientElement.h"
#include "SVGRadialGradientElementImpl.h"
#include "SVGAnimatedLength.h"

using namespace KSVG;

SVGRadialGradientElement::SVGRadialGradientElement() : SVGGradientElement()
{
	impl = 0;
}

SVGRadialGradientElement::SVGRadialGradientElement(const SVGRadialGradientElement &other) : SVGGradientElement(other), impl(0)
{
	(*this) = other;
}

SVGRadialGradientElement &SVGRadialGradientElement::operator =(const SVGRadialGradientElement &other)
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

SVGRadialGradientElement::SVGRadialGradientElement(SVGRadialGradientElementImpl *other) : SVGGradientElement(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGRadialGradientElement::~SVGRadialGradientElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedLength SVGRadialGradientElement::cx() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->cx());
}

SVGAnimatedLength SVGRadialGradientElement::cy() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->cy());
}

SVGAnimatedLength SVGRadialGradientElement::r() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->r());
}

SVGAnimatedLength SVGRadialGradientElement::fx() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->fx());
}

SVGAnimatedLength SVGRadialGradientElement::fy() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->fy());
}

// vim:ts=4:noet
