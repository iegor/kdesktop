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

#ifndef SVGPathSegCurvetoQuadratic_H
#define SVGPathSegCurvetoQuadratic_H

#include "SVGPathSeg.h"

namespace KSVG
{

class SVGPathSegCurvetoQuadraticAbsImpl;
class SVGPathSegCurvetoQuadraticAbs : public SVGPathSeg 
{ 
public:
	SVGPathSegCurvetoQuadraticAbs();
	SVGPathSegCurvetoQuadraticAbs(const SVGPathSegCurvetoQuadraticAbs &);
	SVGPathSegCurvetoQuadraticAbs &operator=(const SVGPathSegCurvetoQuadraticAbs &other);
	SVGPathSegCurvetoQuadraticAbs(SVGPathSegCurvetoQuadraticAbsImpl *);
	~SVGPathSegCurvetoQuadraticAbs();

	void setX(const float &);
	float x() const;

	void setY(const float &);
	float y() const;

	void setX1(const float &);
	float x1() const;

	void setY1(const float &);
	float y1() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegCurvetoQuadraticAbsImpl *handle() const { return impl; }

private:
	SVGPathSegCurvetoQuadraticAbsImpl *impl;
};

class SVGPathSegCurvetoQuadraticRelImpl;
class SVGPathSegCurvetoQuadraticRel : public SVGPathSeg 
{ 
public:
	SVGPathSegCurvetoQuadraticRel();
	SVGPathSegCurvetoQuadraticRel(const SVGPathSegCurvetoQuadraticRel &);
	SVGPathSegCurvetoQuadraticRel &operator=(const SVGPathSegCurvetoQuadraticRel &other);
	SVGPathSegCurvetoQuadraticRel(SVGPathSegCurvetoQuadraticRelImpl *);
	~SVGPathSegCurvetoQuadraticRel();

	void setX(const float &);
	float x() const;

	void setY(const float &);
	float y() const;

	void setX1(const float &);
	float x1() const;

	void setY1(const float &);
	float y1() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegCurvetoQuadraticRelImpl *handle() const { return impl; }

private:
	SVGPathSegCurvetoQuadraticRelImpl *impl;
};

}

#endif

// vim:ts=4:noet
