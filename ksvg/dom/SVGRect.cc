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

#include "SVGRect.h"
#include "SVGRectImpl.h"

using namespace KSVG;

SVGRect::SVGRect()
{
	impl = new SVGRectImpl();
	impl->ref();
}

SVGRect::SVGRect(const SVGRect &other) : impl(0)
{
	(*this) = other;
}
 
SVGRect &SVGRect::operator=(const SVGRect &other)
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

SVGRect::SVGRect(SVGRectImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGRect::~SVGRect()
{
	if(impl)
		impl->deref();
}

void SVGRect::setX(float x)
{
	if(impl)
		impl->setX(x);
}

float SVGRect::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGRect::setY(float y)
{
	if(impl)
		impl->setY(y);
}

float SVGRect::y() const
{
	if(!impl) return -1;
	return impl->y();
}

void SVGRect::setWidth(float width)
{
	if(impl)
		impl->setWidth(width);
}

float SVGRect::width() const
{
	if(!impl) return -1;
	return impl->width();
}

void SVGRect::setHeight(float height)
{
	if(impl)
		impl->setHeight(height);
}

float SVGRect::height() const
{
	if(!impl) return -1;
	return impl->height();
}

// vim:ts=4:noet
