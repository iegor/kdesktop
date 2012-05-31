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
#ifndef SVGPathSeg_H
#define SVGPathSeg_H

#include <dom/dom_string.h>

namespace KSVG
{

enum
{
	PATHSEG_UNKNOWN = 0,
	PATHSEG_CLOSEPATH = 1,
	PATHSEG_MOVETO_ABS = 2,
	PATHSEG_MOVETO_REL = 3,
	PATHSEG_LINETO_ABS = 4,
	PATHSEG_LINETO_REL = 5,
	PATHSEG_CURVETO_CUBIC_ABS = 6,
	PATHSEG_CURVETO_CUBIC_REL = 7,
	PATHSEG_CURVETO_QUADRATIC_ABS = 8,
	PATHSEG_CURVETO_QUADRATIC_REL = 9,
	PATHSEG_ARC_ABS = 10,
	PATHSEG_ARC_REL = 11,
	PATHSEG_LINETO_HORIZONTAL_ABS = 12,
	PATHSEG_LINETO_HORIZONTAL_REL = 13,
	PATHSEG_LINETO_VERTICAL_ABS = 14,
	PATHSEG_LINETO_VERTICAL_REL = 15,
	PATHSEG_CURVETO_CUBIC_SMOOTH_ABS = 16,
	PATHSEG_CURVETO_CUBIC_SMOOTH_REL = 17,
	PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS = 18,
	PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL = 19
};

class SVGPathSegImpl;
class SVGPathSeg 
{ 
public:
	SVGPathSeg();
	SVGPathSeg(const SVGPathSeg &);
	SVGPathSeg &operator=(const SVGPathSeg &other);
	SVGPathSeg(SVGPathSegImpl *);
	~SVGPathSeg();

	unsigned short pathSegType() const;
	DOM::DOMString pathSegTypeAsLetter() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegImpl *handle() const { return impl; }

private:
	SVGPathSegImpl *impl;
};

}

#endif

// vim:ts=4:noet
