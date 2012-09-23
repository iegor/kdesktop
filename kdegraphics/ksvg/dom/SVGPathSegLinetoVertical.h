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

#ifndef SVGPathSegLinetoVertical_H
#define SVGPathSegLinetoVertical_H

#include "SVGPathSeg.h"

namespace KSVG
{

class SVGPathSegLinetoVerticalAbsImpl;
class SVGPathSegLinetoVerticalAbs : public SVGPathSeg
{
public:
	SVGPathSegLinetoVerticalAbs();
	SVGPathSegLinetoVerticalAbs(const SVGPathSegLinetoVerticalAbs &);
	SVGPathSegLinetoVerticalAbs &operator=(const SVGPathSegLinetoVerticalAbs &other);
	SVGPathSegLinetoVerticalAbs(SVGPathSegLinetoVerticalAbsImpl *);
	~SVGPathSegLinetoVerticalAbs();

	void setY(const float &);
	float y() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegLinetoVerticalAbsImpl *handle() const { return impl; }

private:
	SVGPathSegLinetoVerticalAbsImpl *impl;
};

class SVGPathSegLinetoVerticalRelImpl;
class SVGPathSegLinetoVerticalRel : public SVGPathSeg
{
public:
	SVGPathSegLinetoVerticalRel();
	SVGPathSegLinetoVerticalRel(const SVGPathSegLinetoVerticalRel &);
	SVGPathSegLinetoVerticalRel &operator=(const SVGPathSegLinetoVerticalRel &other);
	SVGPathSegLinetoVerticalRel(SVGPathSegLinetoVerticalRelImpl *);
	~SVGPathSegLinetoVerticalRel();

	void setY(const float &);
	float y() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegLinetoVerticalRelImpl *handle() const { return impl; }

private:
	SVGPathSegLinetoVerticalRelImpl *impl;
};

}

#endif

// vim:ts=4:noet
