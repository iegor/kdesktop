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

#include "SVGPathSegArc.h"
#include "SVGPathSegArcImpl.h"

using namespace KSVG;

SVGPathSegArcAbs::SVGPathSegArcAbs() : SVGPathSeg()
{
	impl = new SVGPathSegArcAbsImpl();
}

SVGPathSegArcAbs::SVGPathSegArcAbs(const SVGPathSegArcAbs &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegArcAbs::SVGPathSegArcAbs(SVGPathSegArcAbsImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegArcAbs::~SVGPathSegArcAbs()
{
	delete impl;
}

void SVGPathSegArcAbs::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegArcAbs::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegArcAbs::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegArcAbs::y() const
{
	if(!impl) return -1;
	return impl->y();
}

void SVGPathSegArcAbs::setR1(const float &r1)
{
	if(impl)
		impl->setR1(r1);
}

float SVGPathSegArcAbs::r1() const
{
	if(!impl) return -1;
	return impl->r1();
}

void SVGPathSegArcAbs::setR2(const float &r2)
{
	if(impl)
		impl->setR2(r2);
}

float SVGPathSegArcAbs::r2() const
{
	if(!impl) return -1;
	return impl->r2();
}

void SVGPathSegArcAbs::setAngle(const float &angle)
{
	if(impl)
		impl->setAngle(angle);
}

float SVGPathSegArcAbs::angle() const
{
	if(!impl) return -1;
	return impl->angle();
}

void SVGPathSegArcAbs::setLargeArcFlag(bool largeArcFlag)
{
	if(impl)
		impl->setLargeArcFlag(largeArcFlag);
}

bool SVGPathSegArcAbs::largeArcFlag() const
{
	if(!impl) return false;
	return impl->largeArcFlag();
}

void SVGPathSegArcAbs::setSweepFlag(bool sweepFlag)
{
	if(impl)
		impl->setSweepFlag(sweepFlag);
}

bool SVGPathSegArcAbs::sweepFlag() const
{
	if(!impl) return false;
	return impl->sweepFlag();
}





SVGPathSegArcRel::SVGPathSegArcRel() : SVGPathSeg()
{
	impl = new SVGPathSegArcRelImpl();
}

SVGPathSegArcRel::SVGPathSegArcRel(const SVGPathSegArcRel &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegArcRel::SVGPathSegArcRel(SVGPathSegArcRelImpl *other) : SVGPathSeg(other)
{
	impl = other;
}

SVGPathSegArcRel::~SVGPathSegArcRel()
{
	delete impl;
}

void SVGPathSegArcRel::setX(const float &x)
{
	if(impl)
		impl->setX(x);
}

float SVGPathSegArcRel::x() const
{
	if(!impl) return -1;
	return impl->x();
}

void SVGPathSegArcRel::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegArcRel::y() const
{
	if(!impl) return -1;
	return impl->y();
}

void SVGPathSegArcRel::setR1(const float &r1)
{
	if(impl)
		impl->setR1(r1);
}

float SVGPathSegArcRel::r1() const
{
	if(!impl) return -1;
	return impl->r1();
}

void SVGPathSegArcRel::setR2(const float &r2)
{
	if(impl)
		impl->setR2(r2);
}

float SVGPathSegArcRel::r2() const
{
	if(!impl) return -1;
	return impl->r2();
}

void SVGPathSegArcRel::setAngle(const float &angle)
{
	if(impl)
		impl->setAngle(angle);
}

float SVGPathSegArcRel::angle() const
{
	if(!impl) return -1;
	return impl->angle();
}

void SVGPathSegArcRel::setLargeArcFlag(bool largeArcFlag)
{
	if(impl)
		impl->setLargeArcFlag(largeArcFlag);
}

bool SVGPathSegArcRel::largeArcFlag() const
{
	if(!impl) return false;
	return impl->largeArcFlag();
}

void SVGPathSegArcRel::setSweepFlag(bool sweepFlag)
{
	if(impl)
		impl->setSweepFlag(sweepFlag);
}

bool SVGPathSegArcRel::sweepFlag() const
{
	if(!impl) return false;
	return impl->sweepFlag();
}

// vim:ts=4:noet
