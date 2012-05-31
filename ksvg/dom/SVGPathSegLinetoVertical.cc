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

#include "SVGPathSegLinetoVertical.h"
#include "SVGPathSegLinetoVerticalImpl.h"

using namespace KSVG;

SVGPathSegLinetoVerticalAbs::SVGPathSegLinetoVerticalAbs() : SVGPathSeg()
{
	impl = new SVGPathSegLinetoVerticalAbsImpl();
}
 
SVGPathSegLinetoVerticalAbs::SVGPathSegLinetoVerticalAbs(const SVGPathSegLinetoVerticalAbs &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegLinetoVerticalAbs::SVGPathSegLinetoVerticalAbs(SVGPathSegLinetoVerticalAbsImpl *other) : SVGPathSeg(other)
{
	impl = other;
} 

SVGPathSegLinetoVerticalAbs::~SVGPathSegLinetoVerticalAbs()
{
	delete impl;
}
   
void SVGPathSegLinetoVerticalAbs::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegLinetoVerticalAbs::y() const
{
	if(!impl) return -1;
	return impl->y();
}





SVGPathSegLinetoVerticalRel::SVGPathSegLinetoVerticalRel() : SVGPathSeg()
{
	impl = new SVGPathSegLinetoVerticalRelImpl();
}
 
SVGPathSegLinetoVerticalRel::SVGPathSegLinetoVerticalRel(const SVGPathSegLinetoVerticalRel &other) : SVGPathSeg(other)
{
	impl = other.impl;
}

SVGPathSegLinetoVerticalRel::SVGPathSegLinetoVerticalRel(SVGPathSegLinetoVerticalRelImpl *other) : SVGPathSeg(other)
{
	impl = other;
} 

SVGPathSegLinetoVerticalRel::~SVGPathSegLinetoVerticalRel()
{
	delete impl;
}
   
void SVGPathSegLinetoVerticalRel::setY(const float &y)
{
	if(impl)
		impl->setY(y);
}

float SVGPathSegLinetoVerticalRel::y() const
{
	if(!impl) return -1;
	return impl->y();
}

// vim:ts=4:noet
