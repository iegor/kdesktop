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

#include <dom/dom_string.h>
#include "SVGStringList.h"
#include "SVGStringListImpl.h"

using namespace KSVG;

SVGStringList::SVGStringList()
{
	impl = new SVGStringListImpl();
	impl->ref();
}

SVGStringList::SVGStringList(const SVGStringList &other) : impl(0)
{
	(*this) = other;
}

SVGStringList &SVGStringList::operator=(const SVGStringList &other)
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

SVGStringList::SVGStringList(SVGStringListImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGStringList::~SVGStringList()
{
	if(impl)
		impl->deref();
}

unsigned long SVGStringList::numberOfItems() const
{
	if(!impl) return 0;
	return impl->numberOfItems();
}

void SVGStringList::clear()
{
	if(impl)
		impl->clear();
}

DOM::DOMString *SVGStringList::initialize(DOM::DOMString *newItem)
{
	if(!impl) return new DOM::DOMString();
	return impl->initialize(new SharedString(newItem));
}

DOM::DOMString *SVGStringList::getItem(unsigned long index)
{
	if(!impl) return new DOM::DOMString();
	return impl->getItem(index);
}

DOM::DOMString *SVGStringList::insertItemBefore(DOM::DOMString *newItem, unsigned long index)
{
	if(!impl) return new DOM::DOMString();
	return impl->insertItemBefore(new SharedString(newItem), index);
}

DOM::DOMString *SVGStringList::replaceItem(DOM::DOMString *newItem, unsigned long index)
{
	if(!impl) return new DOM::DOMString();
	return impl->replaceItem(new SharedString(newItem), index);
}

DOM::DOMString *SVGStringList::removeItem(unsigned long index)
{
	if(!impl) return new DOM::DOMString();
	return impl->removeItem(index);
}

DOM::DOMString *SVGStringList::appendItem(DOM::DOMString *newItem)
{
	if(!impl) return new DOM::DOMString();
	return impl->appendItem(new SharedString(newItem));
}

// vim:ts=4:noet
