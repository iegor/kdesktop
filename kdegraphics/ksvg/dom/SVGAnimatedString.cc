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

#include "SVGAnimatedString.h"
#include "SVGAnimatedStringImpl.h"

using namespace KSVG;

SVGAnimatedString::SVGAnimatedString()
{
	impl = new SVGAnimatedStringImpl();
	impl->ref();
}

SVGAnimatedString::SVGAnimatedString(const SVGAnimatedString &other) : impl(0)
{
	(*this) = other;
}

SVGAnimatedString &SVGAnimatedString::operator=(const SVGAnimatedString &other)
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

SVGAnimatedString::SVGAnimatedString(SVGAnimatedStringImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAnimatedString::~SVGAnimatedString()
{
	if(impl)
		impl->deref();
}

void SVGAnimatedString::setBaseVal(const DOM::DOMString &baseVal)
{
	if(impl)
		impl->setBaseVal(baseVal);
}

DOM::DOMString SVGAnimatedString::baseVal() const
{
	if(!impl) return DOM::DOMString();
	return impl->baseVal();
}

DOM::DOMString SVGAnimatedString::animVal() const
{
	if(!impl) return DOM::DOMString();
	return impl->animVal();
}

// vim:ts=4:noet
