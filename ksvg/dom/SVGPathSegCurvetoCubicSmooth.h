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

#ifndef SVGPathSegCurvetoCubicSmooth_H
#define SVGPathSegCurvetoCubicSmooth_H

#include "SVGPathSeg.h"

namespace KSVG
{

class SVGPathSegCurvetoCubicSmoothAbsImpl;
class SVGPathSegCurvetoCubicSmoothAbs : public SVGPathSeg 
{ 
public:
	SVGPathSegCurvetoCubicSmoothAbs();
	SVGPathSegCurvetoCubicSmoothAbs(const SVGPathSegCurvetoCubicSmoothAbs &);
	SVGPathSegCurvetoCubicSmoothAbs &operator=(const SVGPathSegCurvetoCubicSmoothAbs &other);
	SVGPathSegCurvetoCubicSmoothAbs(SVGPathSegCurvetoCubicSmoothAbsImpl *);
	~SVGPathSegCurvetoCubicSmoothAbs();

	void setX(const float &);
	float x() const;

	void setY(const float &);
	float y() const;

	void setX2(const float &);
	float x2() const;

	void setY2(const float &);
	float y2() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegCurvetoCubicSmoothAbsImpl *handle() const { return impl; }

private:
	SVGPathSegCurvetoCubicSmoothAbsImpl *impl;
};

class SVGPathSegCurvetoCubicSmoothRelImpl;
class SVGPathSegCurvetoCubicSmoothRel : public SVGPathSeg 
{ 
public:
	SVGPathSegCurvetoCubicSmoothRel();
	SVGPathSegCurvetoCubicSmoothRel(const SVGPathSegCurvetoCubicSmoothRel &);
	SVGPathSegCurvetoCubicSmoothRel &operator=(const SVGPathSegCurvetoCubicSmoothRel &other);
	SVGPathSegCurvetoCubicSmoothRel(SVGPathSegCurvetoCubicSmoothRelImpl *);
	~SVGPathSegCurvetoCubicSmoothRel();

	void setX(const float &);
	float x() const;

	void setY(const float &);
	float y() const;

	void setX2(const float &);
	float x2() const;

	void setY2(const float &);
	float y2() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegCurvetoCubicSmoothRelImpl *handle() const { return impl; }

private:
	SVGPathSegCurvetoCubicSmoothRelImpl *impl;
};

}

#endif

// vim:ts=4:noet
