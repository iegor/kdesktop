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

#ifndef SVGStringList_H
#define SVGStringList_H

#include <dom/dom_string.h>

namespace KSVG
{

class SVGStringListImpl; 
class SVGStringList 
{ 
public:
	SVGStringList();
	SVGStringList(const SVGStringList &);
	SVGStringList &operator=(const SVGStringList &);
	SVGStringList(SVGStringListImpl *);
	~SVGStringList();

	unsigned long numberOfItems() const;
	void clear();

	DOM::DOMString *initialize(DOM::DOMString *newItem);
	DOM::DOMString *getItem(unsigned long index);
	DOM::DOMString *insertItemBefore(DOM::DOMString *newItem, unsigned long index);
	DOM::DOMString *replaceItem(DOM::DOMString *newItem, unsigned long index);
	DOM::DOMString *removeItem(unsigned long index);
	DOM::DOMString *appendItem(DOM::DOMString *newItem);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
        SVGStringListImpl *handle() const { return impl; }

private:
	SVGStringListImpl *impl;
};

}

#endif

// vim:ts=4:noet
