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

#include "SVGPoint.h"
#include "SVGMatrix.h"
#include "SVGPointImpl.h"

using namespace KSVG;

SVGPoint::SVGPoint()
{
	impl = new SVGPointImpl();
	impl->ref();
}

SVGPoint::SVGPoint(const SVGPoint &other) : impl(0)
{
	(*this) = other;
}
 
SVGPoint &SVGPoint::operator=(const SVGPoint &other)
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

SVGPoint::SVGPoint(SVGPointImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGPoint::~SVGPoint()
{
	if(impl)
		impl->deref();
}

void SVGPoint::setX(float x)
{
	if(impl)
		impl->setX(x);
}

float SVGPoint::x()
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPoint::setY(float y)
{
	if(impl)
		impl->setY(y);
}

float SVGPoint::y()
{
	if(!impl) return -1;
	return impl->y();
}

SVGPoint SVGPoint::matrixTransform(SVGMatrix &matrix)
{
	if(!impl) return SVGPoint(0);
	return SVGPoint(impl->matrixTransform(*matrix.handle()));
}

// vim:ts=4:noet
