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

#ifndef SVGAnimatedPathData_H
#define SVGAnimatedPathData_H

namespace KSVG
{

class SVGPathSegList;
class SVGAnimatedPathDataImpl;
class SVGAnimatedPathData
{
public:
	SVGAnimatedPathData();
	SVGAnimatedPathData(const SVGAnimatedPathData &other);
	SVGAnimatedPathData &operator=(const SVGAnimatedPathData &other);
	SVGAnimatedPathData(SVGAnimatedPathDataImpl *other);
	virtual ~SVGAnimatedPathData();

	SVGPathSegList pathSegList() const;
	SVGPathSegList normalizedPathSegList() const;
	SVGPathSegList animatedPathSegList() const;
	SVGPathSegList animatedNormalizedPathSegList() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGAnimatedPathDataImpl *handle() const { return impl; }

private:
	SVGAnimatedPathDataImpl *impl;
};

}

#endif

// vim:ts=4:noet
