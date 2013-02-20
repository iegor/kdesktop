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

#ifndef SVGElementInstanceList_H
#define SVGElementInstanceList_H

namespace KSVG
{

class SVGElementInstance;
class SVGElementInstanceListImpl;
class SVGElementInstanceList 
{
public:
	SVGElementInstanceList();
	SVGElementInstanceList(const SVGElementInstanceList &other);
	SVGElementInstanceList &operator=(const SVGElementInstanceList &other);
	SVGElementInstanceList(SVGElementInstanceListImpl *other);
	~SVGElementInstanceList();

	unsigned long length() const;
	SVGElementInstance item(unsigned long index);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGElementInstanceListImpl *handle() const { return impl; }

private:
	SVGElementInstanceListImpl *impl;
};

}

#endif

// vim:ts=4:noet
