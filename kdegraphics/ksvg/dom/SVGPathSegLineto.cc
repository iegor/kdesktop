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

#include "SVGPathSegLineto.h"
#include "SVGPathSegLinetoImpl.h"

using namespace KSVG;

SVGPathSegLinetoAbs::SVGPathSegLinetoAbs() : SVGPathSeg()
{
	impl = new SVGPathSegLinetoAbsImpl();
}

SVGPathSegLinetoAbs::SVGPathSegLinetoAbs(const SVGPathSegLinetoAbs &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegLinetoAbs::SVGPathSegLinetoAbs(SVGPathSegLinetoAbsImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegLinetoAbs::~SVGPathSegLinetoAbs()
{
	delete impl;
}

void SVGPathSegLinetoAbs::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegLinetoAbs::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegLinetoAbs::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegLinetoAbs::y() const
{
	if(!impl) return -1;
	return impl->y();
}





SVGPathSegLinetoRel::SVGPathSegLinetoRel() : SVGPathSeg()
{
	impl = new SVGPathSegLinetoRelImpl();
}

SVGPathSegLinetoRel::SVGPathSegLinetoRel(const SVGPathSegLinetoRel &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegLinetoRel::SVGPathSegLinetoRel(SVGPathSegLinetoRelImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegLinetoRel::~SVGPathSegLinetoRel()
{
	delete impl;
}

void SVGPathSegLinetoRel::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegLinetoRel::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegLinetoRel::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegLinetoRel::y() const
{
	if(!impl) return -1;
	return impl->y();
}

// vim:ts=4:noet
