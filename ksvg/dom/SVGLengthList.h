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

#ifndef SVGLengthList_H
#define SVGLengthList_H

namespace KSVG
{

class SVGLength;
class SVGLengthListImpl; 
class SVGLengthList 
{ 
public:
	SVGLengthList();
	SVGLengthList(const SVGLengthList &);
	SVGLengthList &operator=(const SVGLengthList &);
	SVGLengthList(SVGLengthListImpl *);
	~SVGLengthList();

	unsigned long numberOfItems() const;
	void clear();

	SVGLength *initialize(SVGLength *newItem);
	SVGLength *getItem(unsigned long index);
	SVGLength *insertItemBefore(SVGLength *newItem, unsigned long index);
	SVGLength *replaceItem(SVGLength *newItem, unsigned long index);
	SVGLength *removeItem(unsigned long index);
	SVGLength *appendItem(SVGLength *newItem);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
        SVGLengthListImpl *handle() const { return impl; }

private:
	SVGLengthListImpl *impl;
};

}

#endif

// vim:ts=4:noet
