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

#ifndef SVGAngle_H
#define SVGAngle_H

#include <dom/dom_string.h>

namespace KSVG
{

enum
{
	SVG_ANGLETYPE_UNKNOWN = 0,
	SVG_ANGLETYPE_UNSPECIFIED = 1,
	SVG_ANGLETYPE_DEG = 2,
	SVG_ANGLETYPE_RAD = 3,
	SVG_ANGLETYPE_GRAD = 4
};
	
class SVGAngleImpl;
class SVGAngle 
{ 
public:

	SVGAngle();
	SVGAngle(const SVGAngle &other);
	SVGAngle &operator=(const SVGAngle &other);
	SVGAngle(SVGAngleImpl *other);
	~SVGAngle();

	unsigned short unitType() const;

	void setValue(float value);
	float value() const;

	void setValueInSpecifiedUnits(float valueInSpecifiedUnits);
	float valueInSpecifiedUnits() const;

	void setValueAsString(const DOM::DOMString &valueAsString);
	DOM::DOMString valueAsString() const;

	void newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits);
	void convertToSpecifiedUnits(unsigned short unitType);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGAngleImpl *handle() const { return impl; }

private:
	SVGAngleImpl *impl;
};

}

#endif

// vim:ts=4:noet
