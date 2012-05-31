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

#include "SVGPathSegCurvetoCubicSmooth.h"
#include "SVGPathSegCurvetoCubicSmoothImpl.h"

using namespace KSVG;

SVGPathSegCurvetoCubicSmoothAbs::SVGPathSegCurvetoCubicSmoothAbs() : SVGPathSeg()
{
	impl = new SVGPathSegCurvetoCubicSmoothAbsImpl();
}

SVGPathSegCurvetoCubicSmoothAbs::SVGPathSegCurvetoCubicSmoothAbs(const SVGPathSegCurvetoCubicSmoothAbs &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegCurvetoCubicSmoothAbs::SVGPathSegCurvetoCubicSmoothAbs(SVGPathSegCurvetoCubicSmoothAbsImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegCurvetoCubicSmoothAbs::~SVGPathSegCurvetoCubicSmoothAbs()
{
	delete impl;
}

void SVGPathSegCurvetoCubicSmoothAbs::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegCurvetoCubicSmoothAbs::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegCurvetoCubicSmoothAbs::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegCurvetoCubicSmoothAbs::y() const
{
	if(!impl) return -1;
	return impl->y();
}

void SVGPathSegCurvetoCubicSmoothAbs::setX2(const float &x2)
{
	if(impl)
		impl->setX2(x2);
}

float SVGPathSegCurvetoCubicSmoothAbs::x2() const
{
	if(!impl) return -1;
	return impl->x2();
}

void SVGPathSegCurvetoCubicSmoothAbs::setY2(const float &y2)
{
	if(impl)
		impl->setY2(y2);
}

float SVGPathSegCurvetoCubicSmoothAbs::y2() const
{
	if(!impl) return -1;
	return impl->y2();
}





SVGPathSegCurvetoCubicSmoothRel::SVGPathSegCurvetoCubicSmoothRel() : SVGPathSeg()
{
	impl = new SVGPathSegCurvetoCubicSmoothRelImpl();
}

SVGPathSegCurvetoCubicSmoothRel::SVGPathSegCurvetoCubicSmoothRel(const SVGPathSegCurvetoCubicSmoothRel &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegCurvetoCubicSmoothRel::SVGPathSegCurvetoCubicSmoothRel(SVGPathSegCurvetoCubicSmoothRelImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegCurvetoCubicSmoothRel::~SVGPathSegCurvetoCubicSmoothRel()
{
	delete impl;
}

void SVGPathSegCurvetoCubicSmoothRel::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegCurvetoCubicSmoothRel::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegCurvetoCubicSmoothRel::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegCurvetoCubicSmoothRel::y() const
{
	if(!impl) return -1;
	return impl->y();
}

void SVGPathSegCurvetoCubicSmoothRel::setX2(const float &x2)
{
	if(impl)
		impl->setX2(x2);
}

float SVGPathSegCurvetoCubicSmoothRel::x2() const
{
	if(!impl) return -1;
	return impl->x2();
}

void SVGPathSegCurvetoCubicSmoothRel::setY2(const float &y2)
{
	if(impl)
		impl->setY2(y2);
}

float SVGPathSegCurvetoCubicSmoothRel::y2() const
{
	if(!impl) return -1;
	return impl->y2();
}

// vim:ts=4:noet
