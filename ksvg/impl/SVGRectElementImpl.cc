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
#include <klocale.h>

#include "SVGRectImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGRectElementImpl.h"
#include "SVGAnimatedLengthImpl.h"

#include "KSVGCanvas.h"
#include "CanvasItem.h"

using namespace KSVG;

#include "SVGRectElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGRectElementImpl::SVGRectElementImpl(DOM::ElementImpl *impl) : SVGShapeImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl()
{
	KSVG_EMPTY_FLAGS

	m_x = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_x->ref();
	m_x->baseVal()->setValueAsString("-1");

	m_y = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_y->ref();
	m_y->baseVal()->setValueAsString("-1");

	m_width = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_width->ref();
	m_width->baseVal()->setValueAsString("-1");

	m_height = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_height->ref();
	m_height->baseVal()->setValueAsString("-1");

	m_rx = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_rx->ref();
	m_rx->baseVal()->setValueAsString("-1");

	m_ry = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_ry->ref();
	m_ry->baseVal()->setValueAsString("-1");
}

SVGRectElementImpl::~SVGRectElementImpl()
{
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_width)
		m_width->deref();
	if(m_height)
		m_height->deref();
	if(m_rx)
		m_rx->deref();
	if(m_ry)
		m_ry->deref();
}

SVGAnimatedLengthImpl *SVGRectElementImpl::x()
{
	return m_x;
}

SVGAnimatedLengthImpl *SVGRectElementImpl::y()
{
	return m_y;
}

SVGAnimatedLengthImpl *SVGRectElementImpl::width()
{
	return m_width;
}

SVGAnimatedLengthImpl *SVGRectElementImpl::height()
{
	return m_height;
}

SVGAnimatedLengthImpl *SVGRectElementImpl::rx()
{
	return m_rx;
}

SVGAnimatedLengthImpl *SVGRectElementImpl::ry()
{
	return m_ry;
}

/*
@namespace KSVG
@begin SVGRectElementImpl::s_hashTable 7
 x			SVGRectElementImpl::X		DontDelete|ReadOnly
 y			SVGRectElementImpl::Y		DontDelete|ReadOnly
 width		SVGRectElementImpl::Width	DontDelete|ReadOnly
 height		SVGRectElementImpl::Height	DontDelete|ReadOnly
 rx			SVGRectElementImpl::Rx		DontDelete|ReadOnly
 ry			SVGRectElementImpl::Ry		DontDelete|ReadOnly
@end
*/

Value SVGRectElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE
	
	switch(token)
	{
		case X:
			if(!attributeMode)
				return m_x->cache(exec);
			else
				return Number(m_x->baseVal()->value());
		case Y:
			if(!attributeMode)
				return m_y->cache(exec);
			else
				return Number(m_y->baseVal()->value());
		case Width:
			if(!attributeMode)
				return m_width->cache(exec);
			else
				return Number(m_width->baseVal()->value());
		case Height:
			if(!attributeMode)
				return m_height->cache(exec);
			else
				return Number(m_height->baseVal()->value());
		case Rx:
			if(!attributeMode)
				return m_rx->cache(exec);
			else
				return Number(m_rx->baseVal()->value());
		case Ry:
			if(!attributeMode)
				return m_ry->cache(exec);
			else
				return Number(m_ry->baseVal()->value());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGRectElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case X:
			x()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Y:
			y()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Width:
			width()->baseVal()->setValueAsString(value.toString(exec).qstring());
			if(width()->baseVal()->value() < 0) // A negative value is an error
				gotError(i18n("Negative value for attribute width of element <rect> is illegal"));
			break;
		case Height:
			height()->baseVal()->setValueAsString(value.toString(exec).qstring());
			if(height()->baseVal()->value() < 0) // A negative value is an error
				gotError(i18n("Negative value for attribute height of element <rect> is illegal"));
			break;
		case Rx:
			rx()->baseVal()->setValueAsString(value.toString(exec).qstring());
			if(rx()->baseVal()->value() < 0) // A negative value is an error
				gotError(i18n("Negative value for attribute rx of element <rect> is illegal"));
			break;
		case Ry:
			ry()->baseVal()->setValueAsString(value.toString(exec).qstring());
			if(ry()->baseVal()->value() < 0) // A negative value is an error
				gotError(i18n("Negative value for attribute ry of element <rect> is illegal"));
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

SVGRectImpl *SVGRectElementImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();
	ret->setX(m_x->baseVal()->value());
	ret->setY(m_y->baseVal()->value());
	ret->setWidth(m_width->baseVal()->value());
	ret->setHeight(m_height->baseVal()->value());
	return ret;
}

void SVGRectElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(X))
		KSVG_SET_ALT_ATTRIBUTE(X, "0")

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(Y))
		KSVG_SET_ALT_ATTRIBUTE(Y, "0")
}

void SVGRectElementImpl::createItem(KSVGCanvas *c)
{
	if(!c)
		c = ownerDoc()->canvas();

	if(!m_item)
	{
		m_item = c->createRectangle(this);
		c->insert(m_item);
	}
}

// vim:ts=4:noet
