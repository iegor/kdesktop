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

#include "SVGPathSegMoveto.h"
#include "SVGPathSegMovetoImpl.h"

using namespace KSVG;

SVGPathSegMovetoAbs::SVGPathSegMovetoAbs() : SVGPathSeg()
{
	impl = new SVGPathSegMovetoAbsImpl();
}

SVGPathSegMovetoAbs::SVGPathSegMovetoAbs(const SVGPathSegMovetoAbs &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegMovetoAbs::SVGPathSegMovetoAbs(SVGPathSegMovetoAbsImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegMovetoAbs::~SVGPathSegMovetoAbs()
{
	delete impl;
}

void SVGPathSegMovetoAbs::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegMovetoAbs::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegMovetoAbs::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegMovetoAbs::y() const
{
	if(!impl) return -1;
	return impl->y();
}





SVGPathSegMovetoRel::SVGPathSegMovetoRel() : SVGPathSeg()
{
	impl = new SVGPathSegMovetoRelImpl();
}

SVGPathSegMovetoRel::SVGPathSegMovetoRel(const SVGPathSegMovetoRel &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegMovetoRel::SVGPathSegMovetoRel(SVGPathSegMovetoRelImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegMovetoRel::~SVGPathSegMovetoRel()
{
	delete impl;
}

void SVGPathSegMovetoRel::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegMovetoRel::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegMovetoRel::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegMovetoRel::y() const
{
	if(!impl) return -1;
	return impl->y();
}

// vim:ts=4:noet
