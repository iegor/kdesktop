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

#include "SVGNumberList.h"
#include "SVGNumberListImpl.h"
#include "SVGNumber.h"

using namespace KSVG;

SVGNumberList::SVGNumberList()
{
	impl = new SVGNumberListImpl();
	impl->ref();
}

SVGNumberList::SVGNumberList(const SVGNumberList &other) : impl(0)
{
	(*this) = other;
}

SVGNumberList &SVGNumberList::operator=(const SVGNumberList &other)
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

SVGNumberList::SVGNumberList(SVGNumberListImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGNumberList::~SVGNumberList()
{
	if(impl)
		impl->deref();
}

unsigned long SVGNumberList::numberOfItems() const
{
	if(!impl) return 0;
	return impl->numberOfItems();
}

void SVGNumberList::clear()
{
	if(impl)
		impl->clear();
}

SVGNumber *SVGNumberList::initialize(SVGNumber *newItem)
{
	if(!impl) return new SVGNumber(0);
	return new SVGNumber(impl->initialize(newItem->handle()));
}

SVGNumber *SVGNumberList::getItem(unsigned long index)
{
	if(!impl) return new SVGNumber(0);
	return new SVGNumber(impl->getItem(index));
}

SVGNumber *SVGNumberList::insertItemBefore(SVGNumber *newItem, unsigned long index)
{
	if(!impl) return new SVGNumber(0);
	return new SVGNumber(impl->insertItemBefore(newItem->handle(), index));
}

SVGNumber *SVGNumberList::replaceItem(SVGNumber *newItem, unsigned long index)
{
	if(!impl) return new SVGNumber(0);
	return new SVGNumber(impl->replaceItem(newItem->handle(), index));
}

SVGNumber *SVGNumberList::removeItem(unsigned long index)
{
	if(!impl) return new SVGNumber(0);
	return new SVGNumber(impl->removeItem(index));
}

SVGNumber *SVGNumberList::appendItem(SVGNumber *newItem)
{
	if(!impl) return new SVGNumber(0);
	return new SVGNumber(impl->appendItem(newItem->handle()));
}

// vim:ts=4:noet
