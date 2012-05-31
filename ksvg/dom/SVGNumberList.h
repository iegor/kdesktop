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

#ifndef SVGNumberList_H
#define SVGNumberList_H

namespace KSVG
{

class SVGNumber; 
class SVGNumberListImpl; 
class SVGNumberList 
{ 
public:
	SVGNumberList();
	SVGNumberList(const SVGNumberList &);
	SVGNumberList &operator=(const SVGNumberList &);
	SVGNumberList(SVGNumberListImpl *);
	~SVGNumberList();

	unsigned long numberOfItems() const;
	void clear();
	SVGNumber *initialize(SVGNumber *newItem);
	SVGNumber *getItem(unsigned long index);
	SVGNumber *insertItemBefore(SVGNumber *newItem, unsigned long index);
	SVGNumber *replaceItem(SVGNumber *newItem, unsigned long index);
	SVGNumber *removeItem(unsigned long index);
	SVGNumber *appendItem(SVGNumber *newItem);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGNumberListImpl *handle() const { return impl; }

private:
	SVGNumberListImpl *impl;
};

}

#endif

// vim:ts=4:noet
