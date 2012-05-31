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

#include "SVGPreserveAspectRatio.h"
#include "SVGPreserveAspectRatioImpl.h"

using namespace KSVG;

SVGPreserveAspectRatio::SVGPreserveAspectRatio()
{
	impl = new SVGPreserveAspectRatioImpl();
}

SVGPreserveAspectRatio::SVGPreserveAspectRatio(const SVGPreserveAspectRatio &other)
{
	impl = other.impl;
}

SVGPreserveAspectRatio &SVGPreserveAspectRatio::operator=(const SVGPreserveAspectRatio &other)
{
	if(impl == other.impl)
		return *this;

	delete impl;
	impl = other.impl;

	return *this;
}

SVGPreserveAspectRatio::SVGPreserveAspectRatio(SVGPreserveAspectRatioImpl *other)
{
	impl = other;
}

SVGPreserveAspectRatio::~SVGPreserveAspectRatio()
{
	delete impl;
}

void SVGPreserveAspectRatio::setAlign(unsigned short align)
{
	if(impl)
		impl->setAlign(align);
}

unsigned short SVGPreserveAspectRatio::align() const
{
	if(!impl) return SVG_PRESERVEASPECTRATIO_UNKNOWN;
	return impl->align();
}

void SVGPreserveAspectRatio::setMeetOrSlice(unsigned short meetOrSlice)
{
	if(impl)
		impl->setMeetOrSlice(meetOrSlice);
}

unsigned short SVGPreserveAspectRatio::meetOrSlice() const
{
	if(!impl) return SVG_MEETORSLICE_UNKNOWN;
	return impl->meetOrSlice();
}

// vim:ts=4:noet
