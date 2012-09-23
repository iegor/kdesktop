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

#include "SVGAnimatedTransformList.h"
#include "SVGAnimatedTransformListImpl.h"
#include "SVGTransformList.h"

using namespace KSVG;

SVGAnimatedTransformList::SVGAnimatedTransformList()
{
	impl = new SVGAnimatedTransformListImpl();
	impl->ref();
}

SVGAnimatedTransformList::SVGAnimatedTransformList(const SVGAnimatedTransformList &other) : impl(0)
{
	(*this) = other;
}

SVGAnimatedTransformList &SVGAnimatedTransformList::operator=(const SVGAnimatedTransformList &other)
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

SVGAnimatedTransformList::SVGAnimatedTransformList(SVGAnimatedTransformListImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAnimatedTransformList::~SVGAnimatedTransformList()
{
	if(impl)
		impl->deref();
}

SVGTransformList SVGAnimatedTransformList::baseVal() const
{
	if(!impl) return SVGTransformList(0);
	return SVGTransformList(impl->baseVal());
}

SVGTransformList SVGAnimatedTransformList::animVal() const
{
	if(!impl) return SVGTransformList(0);
	return SVGTransformList(impl->animVal());
}

// vim:ts=4:noet
