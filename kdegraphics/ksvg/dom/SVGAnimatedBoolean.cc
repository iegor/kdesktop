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

#include "SVGAnimatedBoolean.h"
#include "SVGAnimatedBooleanImpl.h"

using namespace KSVG;

SVGAnimatedBoolean::SVGAnimatedBoolean()
{
	impl = new SVGAnimatedBooleanImpl();
	impl->ref();
}

SVGAnimatedBoolean::SVGAnimatedBoolean(const SVGAnimatedBoolean &other) : impl(0)
{
	(*this) = other;
}

SVGAnimatedBoolean &SVGAnimatedBoolean::operator=(const SVGAnimatedBoolean &other)
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

SVGAnimatedBoolean::SVGAnimatedBoolean(SVGAnimatedBooleanImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAnimatedBoolean::~SVGAnimatedBoolean()
{
	if(impl)
		impl->deref();
}

void SVGAnimatedBoolean::setBaseVal(bool baseVal)
{
	if(impl)
		impl->setBaseVal(baseVal);
}

bool SVGAnimatedBoolean::baseVal() const
{
	if(!impl) return false;
	return impl->baseVal();
}

bool SVGAnimatedBoolean::animVal() const
{
	if(!impl) return false;
	return impl->animVal();
}

// vim:ts=4:noet
