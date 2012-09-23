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

#ifndef SVGPathSegLineto_H
#define SVGPathSegLineto_H

#include "SVGPathSeg.h"

namespace KSVG
{

class SVGPathSegLinetoAbsImpl;
class SVGPathSegLinetoAbs : public SVGPathSeg 
{ 
public:
	SVGPathSegLinetoAbs();
	SVGPathSegLinetoAbs(const SVGPathSegLinetoAbs &);
	SVGPathSegLinetoAbs &operator=(const SVGPathSegLinetoAbs &other);
	SVGPathSegLinetoAbs(SVGPathSegLinetoAbsImpl *);
	~SVGPathSegLinetoAbs();

	void setX(const float &);
	float x() const;

	void setY(const float &);
	float y() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegLinetoAbsImpl *handle() const { return impl; }

private:
	SVGPathSegLinetoAbsImpl *impl;
};

class SVGPathSegLinetoRelImpl;
class SVGPathSegLinetoRel : public SVGPathSeg 
{ 
public:
	SVGPathSegLinetoRel();
	SVGPathSegLinetoRel(const SVGPathSegLinetoRel &);
	SVGPathSegLinetoRel &operator=(const SVGPathSegLinetoRel &other);
	SVGPathSegLinetoRel(SVGPathSegLinetoRelImpl *);
	~SVGPathSegLinetoRel();

	void setX(const float &);
	float x() const;

	void setY(const float &);
	float y() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegLinetoRelImpl *handle() const { return impl; }

private:
	SVGPathSegLinetoRelImpl *impl;
};

}

#endif

// vim:ts=4:noet
