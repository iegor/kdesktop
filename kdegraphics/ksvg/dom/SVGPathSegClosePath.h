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

#ifndef SVGPathSegClosePath_H
#define SVGPathSegClosePath_H

#include "SVGPathSeg.h"

namespace KSVG
{

class SVGPathSegClosePathImpl;
class SVGPathSegClosePath : public SVGPathSeg 
{
public:
	SVGPathSegClosePath();
	SVGPathSegClosePath(const SVGPathSegClosePath &);
	SVGPathSegClosePath &operator=(const SVGPathSegClosePath &other);
	SVGPathSegClosePath(SVGPathSegClosePathImpl *);
	~SVGPathSegClosePath();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegClosePathImpl *handle() const { return impl; }

private:
	SVGPathSegClosePathImpl *impl;
};

}

#endif

// vim:ts=4:noet
