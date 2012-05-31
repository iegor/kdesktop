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

#include "SVGPaint.h"
#include "SVGPaintImpl.h"

using namespace KSVG;

SVGPaint::SVGPaint() : SVGColor()
{
	// FIXME: no icc color support...
	impl = new SVGPaintImpl(0);
	impl->ref();
}

SVGPaint::SVGPaint(const SVGPaint &other) : SVGColor(other), impl(0)
{
	(*this) = other;
}

SVGPaint &SVGPaint::operator =(const SVGPaint &other)
{
	SVGColor::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGPaint::SVGPaint(SVGPaintImpl *other) : SVGColor(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGPaint::~SVGPaint()
{
	if(impl)
		impl->deref();
}

unsigned short SVGPaint::paintType() const
{
	if(!impl) return SVG_PAINTTYPE_UNKNOWN;
	return impl->paintType();
}

DOM::DOMString SVGPaint::uri() const
{
	if(!impl) return DOM::DOMString();
	return impl->uri();
}

void SVGPaint::setUri(const DOM::DOMString &uri)
{
	if(impl)
		impl->setUri(uri);
}

void SVGPaint::setPaint(unsigned short paintType, const DOM::DOMString &uri, const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor)
{
	if(impl)
		impl->setPaint(paintType, uri, rgbColor, iccColor);
}

// vim:ts=4:noet
