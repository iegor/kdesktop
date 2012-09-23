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

#include <math.h>
#include <kglobal.h>

#include <kdebug.h>

#include "CanvasItem.h"
#include "KSVGCanvas.h"

#include "SVGRectImpl.h"
#include "SVGAngleImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGLineElementImpl.h"
#include "SVGAnimatedLengthImpl.h"

using namespace KSVG;

#include "SVGLineElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGLineElementImpl::SVGLineElementImpl(DOM::ElementImpl *impl) : SVGShapeImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl()
{
	KSVG_EMPTY_FLAGS

	m_x1 = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_x1->ref();
	m_x1->baseVal()->setValueAsString("-1");

	m_y1 = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_y1->ref();
	m_y1->baseVal()->setValueAsString("-1");

	m_x2 = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_x2->ref();
	m_x2->baseVal()->setValueAsString("-1");

	m_y2 = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_y2->ref();
	m_y2->baseVal()->setValueAsString("-1");
}

SVGLineElementImpl::~SVGLineElementImpl()
{
	if(m_x1)
		m_x1->deref();
	if(m_x2)
		m_x2->deref();
	if(m_y1)
		m_y1->deref();
	if(m_y2)
		m_y2->deref();
}

SVGAnimatedLengthImpl *SVGLineElementImpl::x1()
{
	return m_x1;
}

SVGAnimatedLengthImpl *SVGLineElementImpl::y1()
{
	return m_y1;
}

SVGAnimatedLengthImpl *SVGLineElementImpl::x2()
{
	return m_x2;
}

SVGAnimatedLengthImpl *SVGLineElementImpl::y2()
{
	return m_y2;
}

/*
@namespace KSVG
@begin SVGLineElementImpl::s_hashTable 5
 x1  SVGLineElementImpl::X1   DontDelete|ReadOnly
 y1  SVGLineElementImpl::Y1   DontDelete|ReadOnly
 x2  SVGLineElementImpl::X2   DontDelete|ReadOnly
 y2  SVGLineElementImpl::Y2   DontDelete|ReadOnly
@end
*/

Value SVGLineElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE
		
	switch(token)
	{
		case X1:
			if(!attributeMode)
				return m_x1->cache(exec);
			else
				return Number(m_x1->baseVal()->value());
		case Y1:
			if(!attributeMode)
				return m_y1->cache(exec);
			else
				return Number(m_y1->baseVal()->value());
		case X2:
			if(!attributeMode)
				return m_x2->cache(exec);
			else
				return Number(m_x2->baseVal()->value());
		case Y2:
			if(!attributeMode)
				return m_y2->cache(exec);
			else
				return Number(m_y2->baseVal()->value());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGLineElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case X1:
			x1()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Y1:
			y1()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case X2:
			x2()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Y2:
			y2()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

SVGRectImpl *SVGLineElementImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();

	float minx = kMin(m_x1->baseVal()->value(), m_x2->baseVal()->value());
	float miny = kMin(m_y1->baseVal()->value(), m_y2->baseVal()->value());
	float maxx = kMax(m_x1->baseVal()->value(), m_x2->baseVal()->value());
	float maxy = kMax(m_y1->baseVal()->value(), m_y2->baseVal()->value());
	ret->setX(minx);
	ret->setY(miny);
	ret->setWidth(maxx - minx);
	ret->setHeight(maxy - miny);

	return ret;
}

void SVGLineElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();
	
	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(X1))
		KSVG_SET_ALT_ATTRIBUTE(X1, "0")

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(Y1))
		KSVG_SET_ALT_ATTRIBUTE(Y1, "0")

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(X2))
		KSVG_SET_ALT_ATTRIBUTE(X2, "0")

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(Y2))
		KSVG_SET_ALT_ATTRIBUTE(Y2, "0")
}

void SVGLineElementImpl::createItem(KSVGCanvas *c)
{
    if(!c)
        c = ownerDoc()->canvas();

	if(!m_item)
	{
		m_item = c->createLine(this);
		c->insert(m_item);
	}
}

// vim:ts=4:noet
