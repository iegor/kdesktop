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

#include "SVGLengthList.h"
#include "SVGLengthListImpl.h"
#include "SVGLength.h"

using namespace KSVG;

SVGLengthList::SVGLengthList()
{
	impl = new SVGLengthListImpl();
	impl->ref();
}

SVGLengthList::SVGLengthList(const SVGLengthList &other) : impl(0)
{
	(*this) = other;
}

SVGLengthList &SVGLengthList::operator=(const SVGLengthList &other)
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

SVGLengthList::SVGLengthList(SVGLengthListImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGLengthList::~SVGLengthList()
{
	if(impl)
		impl->deref();
}

unsigned long SVGLengthList::numberOfItems() const
{
	if(!impl) return 0;
	return impl->numberOfItems();
}

void SVGLengthList::clear()
{
	if(impl)
		impl->clear();
}

SVGLength *SVGLengthList::initialize(SVGLength *newItem)
{
	if(!impl) return new SVGLength(0);
	return new SVGLength(impl->initialize(newItem->handle()));
}

SVGLength *SVGLengthList::getItem(unsigned long index)
{
	if(!impl) return new SVGLength(0);
	return new SVGLength(impl->getItem(index));
}

SVGLength *SVGLengthList::insertItemBefore(SVGLength *newItem, unsigned long index)
{
	if(!impl) return new SVGLength(0);
	return new SVGLength(impl->insertItemBefore(newItem->handle(), index));
}

SVGLength *SVGLengthList::replaceItem(SVGLength *newItem, unsigned long index)
{
	if(!impl) return new SVGLength(0);
	return new SVGLength(impl->replaceItem(newItem->handle(), index));
}

SVGLength *SVGLengthList::removeItem(unsigned long index)
{
	if(!impl) return new SVGLength(0);
	return new SVGLength(impl->removeItem(index));
}

SVGLength *SVGLengthList::appendItem(SVGLength *newItem)
{
	if(!impl) return new SVGLength(0);
	return new SVGLength(impl->appendItem(newItem->handle()));
}

// vim:ts=4:noet
