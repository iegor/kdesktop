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

#ifndef SVGPointList_H
#define SVGPointList_H

namespace KSVG
{

class SVGPoint;
class SVGPointListImpl; 
class SVGPointList 
{ 
public:
	SVGPointList();
	SVGPointList(const SVGPointList &);
	SVGPointList &operator=(const SVGPointList &);
	SVGPointList(SVGPointListImpl *);
	~SVGPointList();

	unsigned long numberOfItems() const;
	void clear();

	SVGPoint *initialize(SVGPoint *newItem);
	SVGPoint *getItem(unsigned long index);
	SVGPoint *insertItemBefore(SVGPoint *newItem, unsigned long index);
	SVGPoint *replaceItem(SVGPoint *newItem, unsigned long index);
	SVGPoint *removeItem(unsigned long index);
	SVGPoint *appendItem(SVGPoint *newItem);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
        SVGPointListImpl *handle() const { return impl; }

private:
	SVGPointListImpl *impl;
};

}

#endif
