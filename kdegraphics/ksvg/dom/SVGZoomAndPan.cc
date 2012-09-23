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

#include "SVGZoomAndPan.h"
#include "SVGZoomAndPanImpl.h"

using namespace KSVG;

// This class can't be constructed seperately.
SVGZoomAndPan::SVGZoomAndPan()
{
	impl = 0;
}

SVGZoomAndPan::SVGZoomAndPan(const SVGZoomAndPan &other) : impl(0)
{
	(*this) = other;
}

SVGZoomAndPan &SVGZoomAndPan::operator=(const SVGZoomAndPan &other)
{
	if(impl == other.impl)
		return *this;

	impl = other.impl;
	
	return *this;
}

SVGZoomAndPan::SVGZoomAndPan(SVGZoomAndPanImpl *other)
{
	impl = other;
}

SVGZoomAndPan::~SVGZoomAndPan()
{
	// We are not allowed to delete 'impl' as it's not refcounted.
	// delete impl;
}

void SVGZoomAndPan::setZoomAndPan(unsigned short zoomAndPan)
{
	if(impl)
		impl->setZoomAndPan(zoomAndPan);
}

unsigned short SVGZoomAndPan::zoomAndPan() const
{
	if(!impl) return SVG_ZOOMANDPAN_UNKNOWN;
	return impl->zoomAndPan();
}

// vim:ts=4:noet
