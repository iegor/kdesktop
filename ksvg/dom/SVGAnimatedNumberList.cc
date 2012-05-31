/*
    Copyright (C) 2002 KSVG Team
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

#include "SVGAnimatedNumberList.h"
#include "SVGAnimatedNumberListImpl.h"

using namespace KSVG;

SVGAnimatedNumberList::SVGAnimatedNumberList()
{
	impl = new SVGAnimatedNumberListImpl();
	impl->ref();
}

SVGAnimatedNumberList::SVGAnimatedNumberList(const SVGAnimatedNumberList &other) : impl(0)
{
	(*this) = other;
}

SVGAnimatedNumberList &SVGAnimatedNumberList::operator=(const SVGAnimatedNumberList &other)
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

SVGAnimatedNumberList::SVGAnimatedNumberList(SVGAnimatedNumberListImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAnimatedNumberList::~SVGAnimatedNumberList()
{
	if(impl)
		impl->deref();
}

SVGNumberList SVGAnimatedNumberList::baseVal() const
{
	if(!impl) return 0;
	    return impl->animVal();
}

SVGNumberList SVGAnimatedNumberList::animVal() const
{
	if(!impl) return 0;
	    return impl->baseVal();
}

// vim:ts=4:noet
