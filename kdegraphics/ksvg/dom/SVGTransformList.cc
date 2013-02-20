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

#include "SVGTransformList.h"
#include "SVGTransformListImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGMatrix.h"
#include "SVGTransform.h"

using namespace KSVG;

SVGTransformList::SVGTransformList()
{
	impl = new SVGTransformListImpl();
	impl->ref();
}

SVGTransformList::SVGTransformList(const SVGTransformList &other)
{
	(*this) = other;
}

SVGTransformList &SVGTransformList::operator=(const SVGTransformList &other)
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

SVGTransformList::SVGTransformList(SVGTransformListImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGTransformList::~SVGTransformList()
{
	if(impl)
		impl->deref();
}

unsigned long SVGTransformList::numberOfItems() const
{
	if(!impl) return 0;
	return impl->numberOfItems();
}

void SVGTransformList::clear()
{
	if(impl)
		impl->clear();
}

SVGTransform *SVGTransformList::initialize(SVGTransform *newItem)
{
	if(!impl) return new SVGTransform(0);
	return new SVGTransform(impl->initialize(newItem->handle()));
}

SVGTransform *SVGTransformList::getItem(unsigned long index)
{
	if(!impl) return new SVGTransform(0);
	return new SVGTransform(impl->getItem(index));
}

SVGTransform *SVGTransformList::insertItemBefore(SVGTransform *newItem, unsigned long index)
{
	if(!impl) return new SVGTransform(0);
	return new SVGTransform(impl->insertItemBefore(newItem->handle(), index));
}

SVGTransform *SVGTransformList::replaceItem(SVGTransform *newItem, unsigned long index)
{
	if(!impl) return new SVGTransform(0);
	return new SVGTransform(impl->replaceItem(newItem->handle(), index));
}

SVGTransform *SVGTransformList::removeItem(unsigned long index)
{
	if(!impl) return new SVGTransform(0);
	return new SVGTransform(impl->removeItem(index));
}

SVGTransform *SVGTransformList::appendItem(SVGTransform *newItem)
{
	if(!impl) return new SVGTransform(0);
	return new SVGTransform(impl->appendItem(newItem->handle()));
}

SVGTransform *SVGTransformList::createSVGTransformFromMatrix(SVGMatrix &matrix)
{
	if(!impl) return new SVGTransform(0);
	return new SVGTransform(SVGSVGElementImpl::createSVGTransformFromMatrix(matrix.handle()));
}

SVGTransform *SVGTransformList::consolidate()
{
	if(!impl || impl->numberOfItems()==0) return 0;
	return new SVGTransform(impl->consolidate());
}

// vim:ts=4:noet
