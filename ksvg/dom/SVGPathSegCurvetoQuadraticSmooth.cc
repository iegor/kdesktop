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

#include "SVGPathSegCurvetoQuadraticSmooth.h"
#include "SVGPathSegCurvetoQuadraticSmoothImpl.h"

using namespace KSVG;

SVGPathSegCurvetoQuadraticSmoothAbs::SVGPathSegCurvetoQuadraticSmoothAbs() : SVGPathSeg()
{
	impl = new SVGPathSegCurvetoQuadraticSmoothAbsImpl();
}

SVGPathSegCurvetoQuadraticSmoothAbs::SVGPathSegCurvetoQuadraticSmoothAbs(const SVGPathSegCurvetoQuadraticSmoothAbs &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegCurvetoQuadraticSmoothAbs::SVGPathSegCurvetoQuadraticSmoothAbs(SVGPathSegCurvetoQuadraticSmoothAbsImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegCurvetoQuadraticSmoothAbs::~SVGPathSegCurvetoQuadraticSmoothAbs()
{
	delete impl;
}

void SVGPathSegCurvetoQuadraticSmoothAbs::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegCurvetoQuadraticSmoothAbs::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegCurvetoQuadraticSmoothAbs::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegCurvetoQuadraticSmoothAbs::y() const
{
	if(!impl) return -1;
	return impl->y();
}





SVGPathSegCurvetoQuadraticSmoothRel::SVGPathSegCurvetoQuadraticSmoothRel() : SVGPathSeg()
{
	impl = new SVGPathSegCurvetoQuadraticSmoothRelImpl();
}

SVGPathSegCurvetoQuadraticSmoothRel::SVGPathSegCurvetoQuadraticSmoothRel(const SVGPathSegCurvetoQuadraticSmoothRel &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegCurvetoQuadraticSmoothRel::SVGPathSegCurvetoQuadraticSmoothRel(SVGPathSegCurvetoQuadraticSmoothRelImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegCurvetoQuadraticSmoothRel::~SVGPathSegCurvetoQuadraticSmoothRel()
{
	delete impl;
}

void SVGPathSegCurvetoQuadraticSmoothRel::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegCurvetoQuadraticSmoothRel::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegCurvetoQuadraticSmoothRel::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegCurvetoQuadraticSmoothRel::y() const
{
	if(!impl) return -1;
	return impl->y();
}

// vim:ts=4:noet
