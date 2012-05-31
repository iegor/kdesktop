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

#include "SVGRectImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGContainerImpl.h"
#include "SVGSVGElementImpl.h"
#include "kdebug.h"

using namespace KSVG;

SVGContainerImpl::SVGContainerImpl(DOM::ElementImpl *impl) : SVGShapeImpl(impl)
{
}

SVGContainerImpl::~SVGContainerImpl()
{
}

SVGRectImpl *SVGContainerImpl::getBBox()
{
	// just get the union of the children bboxes
	QRect rect;
	DOM::Node node = firstChild();
	for(; !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *elem = ownerDoc()->getElementFromHandle(node.handle());
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(elem);
		SVGTestsImpl *tests = dynamic_cast<SVGTestsImpl *>(elem);
		SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(elem);

		bool ok = tests ? tests->ok() : true;

		if(shape && style && ok && style->getVisible() && style->getDisplay())
		{
			SVGRectImpl *current = shape->getBBox();
			rect = rect.unite(current->qrect());
			current->deref();
		}
	}

	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();
	*ret = rect;
	return ret;
}

void SVGContainerImpl::createItem(KSVGCanvas *c)
{
	DOM::Node node = firstChild();
	for(; !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *elem = ownerDoc()->getElementFromHandle(node.handle());
		if(elem)
			elem->createItem(c);
	}
}

void SVGContainerImpl::removeItem(KSVGCanvas *c)
{
	SVGShapeImpl::removeItem(c);

	for(DOM::Node node = firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *elem = ownerDoc()->getElementFromHandle(node.handle());
		if(elem)
			elem->removeItem(c);
	}
}

void SVGContainerImpl::update(CanvasItemUpdate reason, int param1, int param2)
{
	SVGShapeImpl::update(reason, param1, param2);

	for(DOM::Node node = firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(ownerDoc()->getElementFromHandle(node.handle()));
		if(shape)
			shape->update(reason, param1, param2);
	}
}

void SVGContainerImpl::invalidate(KSVGCanvas *c, bool recalc)
{
	SVGShapeImpl::invalidate(c, recalc);

	for(DOM::Node node = firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(ownerDoc()->getElementFromHandle(node.handle()));
		if(shape)
			shape->invalidate(c, recalc);
	}
}

void SVGContainerImpl::setReferenced(bool referenced)
{
	SVGShapeImpl::setReferenced(referenced);

	for(DOM::Node node = firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(ownerDoc()->getElementFromHandle(node.handle()));
		if(shape)
			shape->setReferenced(referenced);
	}
}

void SVGContainerImpl::draw()
{
	SVGShapeImpl::draw();

	for(DOM::Node node = firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(ownerDoc()->getElementFromHandle(node.handle()));
		if(shape)
			shape->draw();
	}
}

// vim:ts=4:noet
