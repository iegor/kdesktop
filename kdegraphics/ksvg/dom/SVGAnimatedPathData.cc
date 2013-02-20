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

#include "SVGAnimatedPathData.h"
#include "SVGAnimatedPathDataImpl.h"
#include "SVGPathSegList.h"

using namespace KSVG;

SVGAnimatedPathData::SVGAnimatedPathData()
{
	impl = new SVGAnimatedPathDataImpl();
	impl->ref();
}

SVGAnimatedPathData::SVGAnimatedPathData(const SVGAnimatedPathData &other) : impl(0)
{
	(*this) = other;
}

SVGAnimatedPathData &SVGAnimatedPathData::operator=(const SVGAnimatedPathData &other)
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

SVGAnimatedPathData::SVGAnimatedPathData(SVGAnimatedPathDataImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAnimatedPathData::~SVGAnimatedPathData()
{
	if(impl)
		impl->deref();
}

SVGPathSegList SVGAnimatedPathData::pathSegList() const
{
	if(!impl) return SVGPathSegList(0);
	return SVGPathSegList(impl->pathSegList());
}

SVGPathSegList SVGAnimatedPathData::normalizedPathSegList() const
{
	if(!impl) return SVGPathSegList(0);
	return SVGPathSegList(impl->normalizedPathSegList());
}

SVGPathSegList SVGAnimatedPathData::animatedPathSegList() const
{
	if(!impl) return SVGPathSegList(0);
	return SVGPathSegList(impl->animatedPathSegList());
}

SVGPathSegList SVGAnimatedPathData::animatedNormalizedPathSegList() const
{
	if(!impl) return SVGPathSegList(0);
	return SVGPathSegList(impl->animatedNormalizedPathSegList());
}

// vim:ts=4:noet
