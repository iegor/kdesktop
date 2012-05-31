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

#include "SVGAnimatedPoints.h"
#include "SVGAnimatedPointsImpl.h"
#include "SVGPointList.h"

using namespace KSVG;

SVGAnimatedPoints::SVGAnimatedPoints()
{
	impl = new SVGAnimatedPointsImpl();
	impl->ref();
}

SVGAnimatedPoints::SVGAnimatedPoints(const SVGAnimatedPoints &other) : impl(0)
{
	(*this) = other;;
}

SVGAnimatedPoints &SVGAnimatedPoints::operator=(const SVGAnimatedPoints &other)
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

SVGAnimatedPoints::SVGAnimatedPoints(SVGAnimatedPointsImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAnimatedPoints::~SVGAnimatedPoints()
{
	if(impl)
		impl->deref();
}

SVGPointList SVGAnimatedPoints::points() const
{
	if(!impl) return SVGPointList(0);
	return SVGPointList(impl->points());
}

SVGPointList SVGAnimatedPoints::animatedPoints() const
{
	if(!impl) return SVGPointList(0);
	return SVGPointList(impl->animatedPoints());
}

// vim:ts=4:noet
