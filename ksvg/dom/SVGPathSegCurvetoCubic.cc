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

#include "SVGPathSegCurvetoCubic.h"
#include "SVGPathSegCurvetoCubicImpl.h"

using namespace KSVG;

SVGPathSegCurvetoCubicAbs::SVGPathSegCurvetoCubicAbs() : SVGPathSeg()
{
	impl = new SVGPathSegCurvetoCubicAbsImpl();
}

SVGPathSegCurvetoCubicAbs::SVGPathSegCurvetoCubicAbs(const SVGPathSegCurvetoCubicAbs &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegCurvetoCubicAbs::SVGPathSegCurvetoCubicAbs(SVGPathSegCurvetoCubicAbsImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegCurvetoCubicAbs::~SVGPathSegCurvetoCubicAbs()
{
	delete impl;
}

void SVGPathSegCurvetoCubicAbs::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegCurvetoCubicAbs::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegCurvetoCubicAbs::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegCurvetoCubicAbs::y() const
{
	if(!impl) return -1;
	return impl->y();
}

void SVGPathSegCurvetoCubicAbs::setX1(const float &x1)
{
	if(impl)
		impl->setX1(x1);
}

float SVGPathSegCurvetoCubicAbs::x1() const
{
	if(!impl) return -1;
	return impl->x1();
}

void SVGPathSegCurvetoCubicAbs::setY1(const float &y1)
{
	if(impl)
		impl->setY1(y1);
}

float SVGPathSegCurvetoCubicAbs::y1() const
{
	if(!impl) return -1;
	return impl->y1();
}

void SVGPathSegCurvetoCubicAbs::setX2(const float &x2)
{
	if(impl)
		impl->setX2(x2);
}

float SVGPathSegCurvetoCubicAbs::x2() const
{
	if(!impl) return -1;
	return impl->x2();
}

void SVGPathSegCurvetoCubicAbs::setY2(const float &y2)
{
	if(impl)
		impl->setY2(y2);
}

float SVGPathSegCurvetoCubicAbs::y2() const
{
	if(!impl) return -1;
	return impl->y2();
}





SVGPathSegCurvetoCubicRel::SVGPathSegCurvetoCubicRel() : SVGPathSeg()
{
	impl = new SVGPathSegCurvetoCubicRelImpl();
}

SVGPathSegCurvetoCubicRel::SVGPathSegCurvetoCubicRel(const SVGPathSegCurvetoCubicRel &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegCurvetoCubicRel::SVGPathSegCurvetoCubicRel(SVGPathSegCurvetoCubicRelImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegCurvetoCubicRel::~SVGPathSegCurvetoCubicRel()
{
	delete impl;
}

void SVGPathSegCurvetoCubicRel::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegCurvetoCubicRel::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegCurvetoCubicRel::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegCurvetoCubicRel::y() const
{
	if(!impl) return -1;
	return impl->y();
}

void SVGPathSegCurvetoCubicRel::setX1(const float &x1)
{
	if(impl)
		impl->setX1(x1);
}

float SVGPathSegCurvetoCubicRel::x1() const
{
	if(!impl) return -1;
	return impl->x1();
}

void SVGPathSegCurvetoCubicRel::setY1(const float &y1)
{
	if(impl)
		impl->setY1(y1);
}

float SVGPathSegCurvetoCubicRel::y1() const
{
	if(!impl) return -1;
	return impl->y1();
}

void SVGPathSegCurvetoCubicRel::setX2(const float &x2)
{
	if(impl)
		impl->setX2(x2);
}

float SVGPathSegCurvetoCubicRel::x2() const
{
	if(!impl) return -1;
	return impl->x2();
}

void SVGPathSegCurvetoCubicRel::setY2(const float &y2)
{
	if(impl)
		impl->setY2(y2);
}

float SVGPathSegCurvetoCubicRel::y2() const
{
	if(!impl) return -1;
	return impl->y2();
}

// vim:ts=4:noet
