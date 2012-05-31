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

#include "SVGFilterPrimitiveStandardAttributes.h"
#include "SVGFilterPrimitiveStandardAttributesImpl.h"
#include "SVGAnimatedString.h"
#include "SVGAnimatedLength.h"

using namespace KSVG;

SVGFilterPrimitiveStandardAttributes::SVGFilterPrimitiveStandardAttributes()
{
	impl = new SVGFilterPrimitiveStandardAttributesImpl();
}

SVGFilterPrimitiveStandardAttributes::SVGFilterPrimitiveStandardAttributes(const SVGFilterPrimitiveStandardAttributes &other)
{
	impl = other.impl;
}

SVGFilterPrimitiveStandardAttributes &SVGFilterPrimitiveStandardAttributes::operator=(const SVGFilterPrimitiveStandardAttributes &other)
{
	if(impl == other.impl)
		return *this;

	delete impl;
	impl = other.impl;

	return *this;
}

SVGFilterPrimitiveStandardAttributes::SVGFilterPrimitiveStandardAttributes(SVGFilterPrimitiveStandardAttributesImpl *other)
{
	impl = other;
}

SVGFilterPrimitiveStandardAttributes::~SVGFilterPrimitiveStandardAttributes()
{
}

SVGAnimatedLength SVGFilterPrimitiveStandardAttributes::x() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->x());
}

SVGAnimatedLength SVGFilterPrimitiveStandardAttributes::y() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->y());
}

SVGAnimatedLength SVGFilterPrimitiveStandardAttributes::width() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->width());
}

SVGAnimatedLength SVGFilterPrimitiveStandardAttributes::height() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->height());
}

SVGAnimatedString SVGFilterPrimitiveStandardAttributes::result() const
{
	if(!impl) return SVGAnimatedString(0);
	return SVGAnimatedString(impl->result());
}

// vim:ts=4:noet
