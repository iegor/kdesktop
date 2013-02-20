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

#ifndef SVGTransformList_H
#define SVGTransformList_H

namespace KSVG
{

class SVGMatrix;
class SVGTransform;
class SVGTransformListImpl;
class SVGTransformList
{ 
public:
	SVGTransformList();
	SVGTransformList(const SVGTransformList &);
	SVGTransformList &operator=(const SVGTransformList &);
	SVGTransformList(SVGTransformListImpl *);
	~SVGTransformList();

	unsigned long numberOfItems() const;
	void clear();

	SVGTransform *initialize(SVGTransform *newItem);
	SVGTransform *getItem(unsigned long index);
	SVGTransform *insertItemBefore(SVGTransform *newItem, unsigned long index);
	SVGTransform *replaceItem(SVGTransform *newItem, unsigned long index);
	SVGTransform *removeItem(unsigned long index);
	SVGTransform *appendItem(SVGTransform *newItem);

	SVGTransform *createSVGTransformFromMatrix(SVGMatrix &matrix);
	SVGTransform *consolidate();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGTransformListImpl *handle() const { return impl; }

private:
	SVGTransformListImpl *impl;
};

}

#endif
// vim:ts=4:noet

