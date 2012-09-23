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

#include "SVGAnimatedNumber.h"
#include "SVGAnimatedNumberImpl.h"

using namespace KSVG;

SVGAnimatedNumber::SVGAnimatedNumber()
{
	impl = new SVGAnimatedNumberImpl();
	impl->ref();
}

SVGAnimatedNumber::SVGAnimatedNumber(const SVGAnimatedNumber &other) : impl(0)
{
	(*this) = other;
}

SVGAnimatedNumber &SVGAnimatedNumber::operator=(const SVGAnimatedNumber &other)
{
	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGAnimatedNumber::SVGAnimatedNumber(SVGAnimatedNumberImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAnimatedNumber::~SVGAnimatedNumber()
{
	if(impl)
		impl->deref();
}

void SVGAnimatedNumber::setBaseVal(float baseVal)
{
	if(impl)
		impl->setBaseVal(baseVal);
}

float SVGAnimatedNumber::baseVal() const
{
	if(!impl) return -1;
	return impl->baseVal();
}

float SVGAnimatedNumber::animVal() const
{
	if(!impl) return -1;
	return impl->animVal();
}

// vim:ts=4:noet
