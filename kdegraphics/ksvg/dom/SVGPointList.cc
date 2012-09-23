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

#include "SVGPoint.h"
#include "SVGPointImpl.h"
#include "SVGPointList.h"
#include "SVGPointListImpl.h"

using namespace KSVG;

SVGPointList::SVGPointList()
{
	impl = new SVGPointListImpl();
	impl->ref();
}

SVGPointList::SVGPointList(const SVGPointList &other) : impl(0)
{
	(*this) = other;
}

SVGPointList &SVGPointList::operator=(const SVGPointList &other)
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

SVGPointList::SVGPointList(SVGPointListImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGPointList::~SVGPointList()
{
	if(impl)
		impl->deref();
}

unsigned long SVGPointList::numberOfItems() const
{
	if(!impl) return 0;
	return impl->numberOfItems();
}

void SVGPointList::clear()
{
	if(impl)
		impl->clear();
}

SVGPoint *SVGPointList::initialize(SVGPoint *newItem)
{
	if(!impl) return new SVGPoint(0);
	return new SVGPoint(impl->initialize(newItem->handle()));
}

SVGPoint *SVGPointList::getItem(unsigned long index)
{
	if(!impl) return new SVGPoint(0);
	return new SVGPoint(impl->getItem(index));
}

SVGPoint *SVGPointList::insertItemBefore(SVGPoint *newItem, unsigned long index)
{
	if(!impl) return new SVGPoint(0);
	return new SVGPoint(impl->insertItemBefore(newItem->handle(), index));
}

SVGPoint *SVGPointList::replaceItem(SVGPoint *newItem, unsigned long index)
{
	if(!impl) return new SVGPoint(0);
	return new SVGPoint(impl->replaceItem(newItem->handle(), index));
}

SVGPoint *SVGPointList::removeItem(unsigned long index)
{
	if(!impl) return new SVGPoint(0);
	return new SVGPoint(impl->removeItem(index));
}

SVGPoint *SVGPointList::appendItem(SVGPoint *newItem)
{
	if(!impl) return new SVGPoint(0);
	return new SVGPoint(impl->appendItem(newItem->handle()));
}
