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

#ifndef SVGPathSegArc_H
#define SVGPathSegArc_H

#include "SVGPathSeg.h"

namespace KSVG
{

class SVGPathSegArcAbsImpl;
class SVGPathSegArcAbs : public SVGPathSeg 
{ 
public:
	SVGPathSegArcAbs();
	SVGPathSegArcAbs(const SVGPathSegArcAbs &);
	SVGPathSegArcAbs &operator=(const SVGPathSegArcAbs &other);
	SVGPathSegArcAbs(SVGPathSegArcAbsImpl *);
	~SVGPathSegArcAbs();

	void setX(const float &);
	float x() const;

	void setY(const float &);
	float y() const;

	void setR1(const float &);
	float r1() const;

	void setR2(const float &);
	float r2() const;

	void setAngle(const float &);
	float angle() const;

	void setLargeArcFlag(bool);
	bool largeArcFlag() const;

	void setSweepFlag(bool);
	bool sweepFlag() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
        SVGPathSegArcAbsImpl *handle() const { return impl; }

private:
	SVGPathSegArcAbsImpl *impl;
};

class SVGPathSegArcRelImpl;
class SVGPathSegArcRel : public SVGPathSeg 
{ 
public:
	SVGPathSegArcRel();
	SVGPathSegArcRel(const SVGPathSegArcRel &);
	SVGPathSegArcRel &operator=(const SVGPathSegArcRel &other);
	SVGPathSegArcRel(SVGPathSegArcRelImpl *);
	~SVGPathSegArcRel();

	void setX(const float &);
	float x() const;

	void setY(const float &);
	float y() const;

	void setR1(const float &);
	float r1() const;

	void setR2(const float &);
	float r2() const;

	void setAngle(const float &);
	float angle() const;

	void setLargeArcFlag(bool);
	bool largeArcFlag() const;

	void setSweepFlag(bool);
	bool sweepFlag() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPathSegArcRelImpl *handle() const { return impl; }

private:
	SVGPathSegArcRelImpl *impl;
};

}

#endif

// vim:ts=4:noet
