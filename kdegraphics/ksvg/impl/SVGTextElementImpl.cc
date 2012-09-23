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

#include <Glyph.h>

#include "SVGRectImpl.h"
#include "SVGEventImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGTextElementImpl.h"

#include "CanvasItem.h"
#include "KSVGCanvas.h"

using namespace KSVG;

SVGTextElementImpl::SVGTextElementImpl(DOM::ElementImpl *impl) : SVGTextPositioningElementImpl(impl), SVGTransformableImpl()
{
	m_bboxX = 0;
	m_bboxY = 0;

	m_bboxWidth = 0;
	m_bboxHeight = 0;	
}

SVGTextElementImpl::~SVGTextElementImpl()
{
}

long SVGTextElementImpl::getNumberOfChars()
{
	return text().length();
}

QString SVGTextElementImpl::text()
{
	// Otherwhise some js scripts which require a child, don't work (Niko)
	if(!hasChildNodes())
	{
		DOM::Text impl = static_cast<DOM::Document *>(ownerDoc())->createTextNode(DOM::DOMString(""));
		appendChild(impl);
	}

	return textDirectionAwareText();
}

SVGRectImpl *SVGTextElementImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();
	ret->setX(m_bboxX);
	ret->setY(m_bboxY);
	ret->setWidth(m_bboxWidth);
	ret->setHeight(m_bboxHeight);
	return ret;
}

void SVGTextElementImpl::createItem(KSVGCanvas *c )
{
	if(!c)
		c = ownerDoc()->canvas();

	if(!m_item)
	{
		m_item = c->createText(this);
		// Set up bbox before insert(), as that may render the item
		QRect rect = m_item->bbox();
		m_bboxX = rect.x();
		m_bboxY = rect.y();
		m_bboxWidth = rect.width();
		m_bboxHeight = rect.height();
		c->insert(m_item);
	}
}

bool SVGTextElementImpl::prepareMouseEvent(const QPoint &p, const QPoint &, SVGMouseEventImpl *mev)
{
	// TODO : pointer-events should be stored here, not in SVGStylableImpl.
	SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(this);
	if(!style || style->getPointerEvents() == PE_NONE)
		return false;
	bool test = false;
	switch(style->getPointerEvents())
	{
		case PE_VISIBLE:			test = style->getVisible(); break;
		case PE_VISIBLE_PAINTED:	test = style->getVisible() && (style->isStroked() || style->isFilled()) ; break;
		case PE_VISIBLE_FILL:		test = style->getVisible() && style->isFilled(); break;
		case PE_VISIBLE_STROKE:		test = style->getVisible() && style->isStroked(); break;
		case PE_PAINTED:			test = style->isStroked() || style->isFilled(); break;
		case PE_FILL:				test = style->isFilled(); break;
		case PE_STROKE:				test = style->isStroked(); break;
		case PE_ALL:
		default: test = true;
	};

	if(test)
	{
		if(m_item->bbox().contains(p))
		{
			mev->setTarget(dynamic_cast<SVGElementImpl *>(this));
			return true;
		}
	}

	return false;
}

// vim:ts=4:noet
