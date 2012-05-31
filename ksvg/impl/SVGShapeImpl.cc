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

#include <kdebug.h>

#include "SVGPaint.h"

#include "SVGRectImpl.h"
#include "SVGEventImpl.h"
#include "SVGShapeImpl.h"
#include "SVGPaintImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGClipPathElementImpl.h"
#include "SVGAnimatedLengthListImpl.h"

#include "CanvasItem.h"
#include "KSVGCanvas.h"

using namespace KSVG;

SVGShapeImpl::SVGShapeImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl)
{
	m_item = 0;
}

SVGShapeImpl::~SVGShapeImpl()
{
	if(hasChildNodes())
	{
		DOM::Node node = firstChild();
		for(; !node.isNull(); node = node.nextSibling())
		{
			SVGShapeImpl *rend = dynamic_cast<SVGShapeImpl *>(ownerDoc()->getElementFromHandle(node.handle()));
			if(rend)
				rend->deref();
		}
	}
}

bool SVGShapeImpl::directRender()
{
	SVGShapeImpl *parent = dynamic_cast<SVGShapeImpl *>(ownerDoc()->getElementFromHandle(parentNode().handle()));
	if(parent)
		return parent->directRender();
	else
		return true;
}

SVGRectImpl *SVGShapeImpl::getBBox()
{
	SVGRectImpl *rect = SVGSVGElementImpl::createSVGRect();
	return rect;
}

SVGRectImpl *SVGShapeImpl::getBBoxInternal()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();
	if(m_item)
	{
		QRect r = m_item->bbox();
		ret->setX(r.x());
		ret->setY(r.y());
		ret->setWidth(r.width());
		ret->setHeight(r.height());
	}
	return ret;
}

bool SVGShapeImpl::prepareMouseEvent(const QPoint &p, const QPoint &, SVGMouseEventImpl *mev)
{
	// TODO : pointer-events should be stored here, not in SVGStylableImpl.
	SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(this);
	if(!style || style->getPointerEvents() == PE_NONE)
		return false;
	bool testFill = false;
	bool testStroke = false;
	switch(style->getPointerEvents())
	{
		case PE_VISIBLE:			testFill = testStroke = style->getVisible(); break;
		case PE_VISIBLE_PAINTED:	testStroke = style->getVisible() && style->isStroked();
		case PE_VISIBLE_FILL:		testFill = style->getVisible() && style->isFilled(); break;
		case PE_VISIBLE_STROKE:		testStroke = style->getVisible() && style->isStroked(); break;
		case PE_PAINTED:			testStroke = style->isStroked();
		case PE_FILL:				testFill = style->isFilled(); break;
		case PE_STROKE:				testStroke = style->isStroked(); break;
		case PE_ALL:
		default: testFill = testStroke = true;
	};

	if(testFill || testStroke)
	{
		if((testFill && m_item->fillContains(p)) || (testStroke && m_item->strokeContains(p)))
		{
			mev->setTarget(this);
			return true;
		}
	}

	return false;
}

void SVGShapeImpl::update(CanvasItemUpdate reason, int param1, int param2)
{
	if(m_item)
		m_item->update(reason, param1, param2);
}

void SVGShapeImpl::invalidate(KSVGCanvas *c, bool recalc)
{
	if(m_item && c)
		c->invalidate(m_item, recalc);
}

void SVGShapeImpl::setReferenced(bool referenced)
{
	if(m_item)
		m_item->setReferenced(referenced);
}

void SVGShapeImpl::draw()
{
	if(m_item)
		m_item->draw();
}

void SVGShapeImpl::blit(KSVGCanvas *c)
{
	SVGRectImpl *rect = getBBoxInternal();
	c->blit(rect->qrect(), true);
	rect->deref();
}

void SVGShapeImpl::removeItem(KSVGCanvas *c)
{
	if(m_item && c)
	{
		c->removeItem(m_item);
		m_item = 0;
	}
}

// vim:ts=4:noet
