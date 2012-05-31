/*
    Copyright (C) 2001-2003 KSVG Team
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option); any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "SVGColor.h"
#include "SVGICCColor.h"
#include "SVGColorImpl.h"

using namespace KSVG;

SVGColor::SVGColor()
{
	// FIXME: no icc color support...
	impl = new SVGColorImpl(0);
	impl->ref();
}

SVGColor::SVGColor(const SVGColor &other) : impl(0)
{
	(*this) = other;
}

SVGColor &SVGColor::operator=(const SVGColor &other)
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

SVGColor::SVGColor(SVGColorImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGColor::~SVGColor()
{
	if(impl)
		impl->deref();
}

unsigned short SVGColor::colorType() const
{
	if(!impl) return SVG_COLORTYPE_UNKNOWN;
	return impl->colorType();
}

DOM::RGBColor SVGColor::rgbColor() const
{
	if(!impl) return DOM::RGBColor();
	return impl->rgbColor();
}

SVGICCColor SVGColor::iccColor() const
{
	if(!impl) return SVGICCColor();
	return impl->iccColor();
}

void SVGColor::setRGBColor(const DOM::DOMString &rgbColor)
{
	if(impl)
		impl->setRGBColor(rgbColor);
}

void SVGColor::setRGBColorICCColor(const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor)
{
	if(impl)
		impl->setRGBColorICCColor(rgbColor, iccColor);
}

void SVGColor::setColor(unsigned short colorType, const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor)
{
	if(impl)
		impl->setColor(colorType, rgbColor, iccColor);
}

// vim:ts=4:noet
