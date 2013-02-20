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

#include "SVGPathSegList.h"
#include "SVGPathSegListImpl.h"

using namespace KSVG;

SVGPathSegList::SVGPathSegList()
{
	impl = new SVGPathSegListImpl();
	impl->ref();
}

SVGPathSegList::SVGPathSegList(const SVGPathSegList &other)
{
	(*this) = other;
}

SVGPathSegList &SVGPathSegList::operator=(const SVGPathSegList &other)
{
	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGPathSegList::SVGPathSegList(SVGPathSegListImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGPathSegList::~SVGPathSegList()
{
	if(impl)
		impl->deref();
}

unsigned long SVGPathSegList::numberOfItems() const
{
	if(!impl) return 0;
	return impl->numberOfItems();
}

void SVGPathSegList::clear()
{
	if(impl)
		impl->clear();
}

SVGPathSeg *SVGPathSegList::initialize(SVGPathSeg *newItem)
{
	if(!impl) return new SVGPathSeg(0);
	return new SVGPathSeg(impl->initialize(newItem->handle()));
}

SVGPathSeg *SVGPathSegList::getItem(unsigned long index)
{
	if(!impl) return new SVGPathSeg(0);
	return new SVGPathSeg(impl->getItem(index));
}

SVGPathSeg *SVGPathSegList::insertItemBefore(SVGPathSeg *newItem, unsigned long index)
{
	if(!impl) return new SVGPathSeg(0);
	return new SVGPathSeg(impl->insertItemBefore(newItem->handle(), index));
}

SVGPathSeg *SVGPathSegList::replaceItem(SVGPathSeg *newItem, unsigned long index)
{
	if(!impl) return new SVGPathSeg(0);
	return new SVGPathSeg(impl->replaceItem(newItem->handle(), index));
}

SVGPathSeg *SVGPathSegList::removeItem(unsigned long index)
{
	if(!impl) return new SVGPathSeg(0);
	return new SVGPathSeg(impl->removeItem(index));
}

SVGPathSeg *SVGPathSegList::appendItem(SVGPathSeg *newItem)
{
	if(!impl) return new SVGPathSeg(0);
	return new SVGPathSeg(impl->appendItem(newItem->handle()));
}

// vim:ts=4:noet
