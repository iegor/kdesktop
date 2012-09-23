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

#ifndef SVGPathSegCurvetoQuadraticSmooth_H
#define SVGPathSegCurvetoQuadraticSmooth_H

#include "SVGPathSeg.h"

namespace KSVG
{

class SVGPathSegCurvetoQuadraticSmoothAbsImpl;
class SVGPathSegCurvetoQuadraticSmoothAbs : public SVGPathSeg 
{ 
public:
	SVGPathSegCurvetoQuadraticSmoothAbs();
	SVGPathSegCurvetoQuadraticSmoothAbs(const SVGPathSegCurvetoQuadraticSmoothAbs &);
	SVGPathSegCurvetoQuadraticSmoothAbs &operator=(const SVGPathSegCurvetoQuadraticSmoothAbs &other);
	SVGPathSegCurvetoQuadraticSmoothAbs(SVGPathSegCurvetoQuadraticSmoothAbsImpl *);
	~SVGPathSegCurvetoQuadraticSmoothAbs();

	void setX(const float &);
	float x() const;

	void setY(const float &);
	float y() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegCurvetoQuadraticSmoothAbsImpl *handle() const { return impl; }

private:
	SVGPathSegCurvetoQuadraticSmoothAbsImpl *impl;
};

class SVGPathSegCurvetoQuadraticSmoothRelImpl;
class SVGPathSegCurvetoQuadraticSmoothRel : public SVGPathSeg 
{ 
public:
	SVGPathSegCurvetoQuadraticSmoothRel();
	SVGPathSegCurvetoQuadraticSmoothRel(const SVGPathSegCurvetoQuadraticSmoothRel &);
	SVGPathSegCurvetoQuadraticSmoothRel &operator=(const SVGPathSegCurvetoQuadraticSmoothRel &other);
	SVGPathSegCurvetoQuadraticSmoothRel(SVGPathSegCurvetoQuadraticSmoothRelImpl *);
	~SVGPathSegCurvetoQuadraticSmoothRel();

	void setX(const float &);
	float x() const;

	void setY(const float &);
	float y() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegCurvetoQuadraticSmoothRelImpl *handle() const { return impl; }

private:
	SVGPathSegCurvetoQuadraticSmoothRelImpl *impl;
};

}

#endif

// vim:ts=4:noet
