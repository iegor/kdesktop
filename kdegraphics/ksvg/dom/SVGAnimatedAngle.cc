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

#include "SVGAnimatedAngle.h"
#include "SVGAnimatedAngleImpl.h"
#include "SVGAngle.h"

using namespace KSVG;

SVGAnimatedAngle::SVGAnimatedAngle()
{
	impl = new SVGAnimatedAngleImpl();
	impl->ref();
}

SVGAnimatedAngle::SVGAnimatedAngle(const SVGAnimatedAngle &other) : impl(0)
{
	(*this) = other;
}

SVGAnimatedAngle &SVGAnimatedAngle::operator=(const SVGAnimatedAngle &other)
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

SVGAnimatedAngle::SVGAnimatedAngle(SVGAnimatedAngleImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAnimatedAngle::~SVGAnimatedAngle()
{
	if(impl)
		impl->deref();
}

SVGAngle SVGAnimatedAngle::baseVal() const
{
	if(!impl) return SVGAngle(0);
	return SVGAngle(impl->baseVal());
}

SVGAngle SVGAnimatedAngle::animVal() const
{
	if(!impl) return SVGAngle(0);
	return SVGAngle(impl->animVal());
}

// vim:ts=4:noet
