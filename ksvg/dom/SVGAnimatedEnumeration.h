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

#ifndef SVGAnimatedEnumeration_H
#define SVGAnimatedEnumeration_H

namespace KSVG
{

class SVGAnimatedEnumerationImpl;
class SVGAnimatedEnumeration 
{ 
public:
	SVGAnimatedEnumeration();
	SVGAnimatedEnumeration(const SVGAnimatedEnumeration &other);
	SVGAnimatedEnumeration &operator=(const SVGAnimatedEnumeration &other);
	SVGAnimatedEnumeration(SVGAnimatedEnumerationImpl *other);
	~SVGAnimatedEnumeration();

	void setBaseVal(unsigned short baseVal);
	unsigned short baseVal() const;

	unsigned short animVal() const;

    // Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGAnimatedEnumerationImpl *handle() const { return impl; }
	
private:
	SVGAnimatedEnumerationImpl *impl;
};

}

#endif

// vim:ts=4:noet
