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

#include "SVGPathSeg.h"
#include "SVGPathSegImpl.h"

using namespace KSVG;

SVGPathSeg::SVGPathSeg()
{
	impl = new SVGPathSegImpl();
}

SVGPathSeg::SVGPathSeg(const SVGPathSeg &other)
{
	impl = other.impl;
}

SVGPathSeg &SVGPathSeg::operator=(const SVGPathSeg &other)
{
	if(impl == other.impl)
		return *this;

	delete impl;
	impl = other.impl;

	return *this;
}

SVGPathSeg::SVGPathSeg(SVGPathSegImpl *other)
{
	impl = other;
}

SVGPathSeg::~SVGPathSeg()
{
	delete impl;
}

unsigned short SVGPathSeg::pathSegType() const
{
	if(!impl) return PATHSEG_UNKNOWN;
	return impl->pathSegType();
}

DOM::DOMString SVGPathSeg::pathSegTypeAsLetter() const
{
	if(!impl) return DOM::DOMString("");
	return impl->pathSegTypeAsLetter();
}

// vim:ts=4:noet
