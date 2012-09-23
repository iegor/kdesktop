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

#ifndef SVGTransform_H
#define SVGTransform_H

namespace KSVG
{

enum
{
	SVG_TRANSFORM_UNKNOWN = 0,
	SVG_TRANSFORM_MATRIX = 1,
	SVG_TRANSFORM_TRANSLATE = 2,
	SVG_TRANSFORM_SCALE = 3,
	SVG_TRANSFORM_ROTATE = 4,
	SVG_TRANSFORM_SKEWX = 5,
	SVG_TRANSFORM_SKEWY = 6
};

class SVGMatrix;
class SVGTransformImpl;
class SVGTransform
{ 
public:
	SVGTransform();
	SVGTransform(const SVGTransform &);
	SVGTransform &operator=(const SVGTransform &);
	SVGTransform(SVGTransformImpl *);
	~SVGTransform();

	unsigned short type() const;
	SVGMatrix matrix() const;
	double angle() const;

	void setMatrix(SVGMatrix);
	void setTranslate(double, double);
	void setScale(double, double);
	void setRotate(double, double, double);
	void setSkewX(double);
	void setSkewY(double);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGTransformImpl *handle() const { return impl; }

private:
	SVGTransformImpl *impl;
};

}

#endif

// vim:ts=4:noet
