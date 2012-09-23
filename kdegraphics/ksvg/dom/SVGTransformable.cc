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

#include "SVGTransformable.h"
#include "SVGTransformableImpl.h"
#include "SVGAnimatedTransformList.h"

using namespace KSVG;

// This class can't be constructed seperately.
SVGTransformable::SVGTransformable() : SVGLocatable()
{
	impl = 0;
}

SVGTransformable::SVGTransformable(const SVGTransformable &other) : SVGLocatable(other)
{
	(*this) = other;
}

SVGTransformable &SVGTransformable::operator=(const SVGTransformable &other)
{
	SVGLocatable::operator=(other);

	if(impl == other.impl)
		return *this;

	impl = other.impl;

	return *this;
}

SVGTransformable::SVGTransformable(SVGTransformableImpl *other) : SVGLocatable(other)
{
	impl = other;
}

SVGTransformable::~SVGTransformable()
{
	// We are not allowed to delete 'impl' as it's not refcounted.
	// delete impl;
}

SVGAnimatedTransformList SVGTransformable::transform()
{
	if(!impl) return SVGAnimatedTransformList(0);
	return SVGAnimatedTransformList(impl->transform());
}

// vim:ts=4:noet
