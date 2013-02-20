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

#include "SVGPathSegCurvetoQuadratic.h"
#include "SVGPathSegCurvetoQuadraticImpl.h"

using namespace KSVG;

SVGPathSegCurvetoQuadraticAbs::SVGPathSegCurvetoQuadraticAbs() : SVGPathSeg()
{
	impl = new SVGPathSegCurvetoQuadraticAbsImpl();
}

SVGPathSegCurvetoQuadraticAbs::SVGPathSegCurvetoQuadraticAbs(const SVGPathSegCurvetoQuadraticAbs &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegCurvetoQuadraticAbs::SVGPathSegCurvetoQuadraticAbs(SVGPathSegCurvetoQuadraticAbsImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegCurvetoQuadraticAbs::~SVGPathSegCurvetoQuadraticAbs()
{
	delete impl;
}

void SVGPathSegCurvetoQuadraticAbs::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegCurvetoQuadraticAbs::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegCurvetoQuadraticAbs::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegCurvetoQuadraticAbs::y() const
{
	if(!impl) return -1;
	return impl->y();
}

void SVGPathSegCurvetoQuadraticAbs::setX1(const float &x1)
{
	if(impl)
		impl->setX1(x1);
}

float SVGPathSegCurvetoQuadraticAbs::x1() const
{
	if(!impl) return -1;
	return impl->x1();
}

void SVGPathSegCurvetoQuadraticAbs::setY1(const float &y1)
{
	if(impl)
		impl->setY1(y1);
}

float SVGPathSegCurvetoQuadraticAbs::y1() const
{
	if(!impl) return -1;
	return impl->y1();
}





SVGPathSegCurvetoQuadraticRel::SVGPathSegCurvetoQuadraticRel() : SVGPathSeg()
{
	impl = new SVGPathSegCurvetoQuadraticRelImpl();
}

SVGPathSegCurvetoQuadraticRel::SVGPathSegCurvetoQuadraticRel(const SVGPathSegCurvetoQuadraticRel &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegCurvetoQuadraticRel::SVGPathSegCurvetoQuadraticRel(SVGPathSegCurvetoQuadraticRelImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegCurvetoQuadraticRel::~SVGPathSegCurvetoQuadraticRel()
{
	delete impl;
}

void SVGPathSegCurvetoQuadraticRel::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegCurvetoQuadraticRel::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegCurvetoQuadraticRel::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegCurvetoQuadraticRel::y() const
{
	if(!impl) return -1;
	return impl->y();
}

void SVGPathSegCurvetoQuadraticRel::setX1(const float &x1)
{
	if(impl)
		impl->setX1(x1);
}

float SVGPathSegCurvetoQuadraticRel::x1() const
{
	if(!impl) return -1;
	return impl->x1();
}

void SVGPathSegCurvetoQuadraticRel::setY1(const float &y1)
{
	if(impl)
		impl->setY1(y1);
}

float SVGPathSegCurvetoQuadraticRel::y1() const
{
	if(!impl) return -1;
	return impl->y1();
}

// vim:ts=4:noet
