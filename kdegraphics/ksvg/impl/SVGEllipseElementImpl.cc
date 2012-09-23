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

#include "CanvasItem.h"
#include "KSVGCanvas.h"

#include "SVGRectImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGEllipseElementImpl.h"
#include "SVGAnimatedLengthImpl.h"

using namespace KSVG;

#include "SVGEllipseElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGEllipseElementImpl::SVGEllipseElementImpl(DOM::ElementImpl *impl) : SVGShapeImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl()
{
	KSVG_EMPTY_FLAGS

	m_cx = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_cx->ref();
	m_cx->baseVal()->setValueAsString("-1");

	m_cy = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_cy->ref();
	m_cy->baseVal()->setValueAsString("-1");

	m_rx = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_rx->ref();
	m_rx->baseVal()->setValueAsString("-1");

	m_ry = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_ry->ref();
	m_ry->baseVal()->setValueAsString("-1");
}

SVGEllipseElementImpl::~SVGEllipseElementImpl()
{
	if(m_cx)
		m_cx->deref();
	if(m_cy)
		m_cy->deref();
	if(m_rx)
		m_rx->deref();
	if(m_ry)
		m_ry->deref();
}

SVGAnimatedLengthImpl *SVGEllipseElementImpl::cx()
{
	return m_cx;
}

SVGAnimatedLengthImpl *SVGEllipseElementImpl::cy()
{
	return m_cy;
}

SVGAnimatedLengthImpl *SVGEllipseElementImpl::rx()
{
	return m_rx;
}

SVGAnimatedLengthImpl *SVGEllipseElementImpl::ry()
{
	return m_ry;
}

/*
@namespace KSVG
@begin SVGEllipseElementImpl::s_hashTable 5
 cx	SVGEllipseElementImpl::Cx	DontDelete|ReadOnly
 cy	SVGEllipseElementImpl::Cy	DontDelete|ReadOnly
 rx	SVGEllipseElementImpl::Rx	DontDelete|ReadOnly
 ry	SVGEllipseElementImpl::Ry	DontDelete|ReadOnly
@end
*/

Value SVGEllipseElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE
		
	switch(token)
	{
		case Cx:
			if(!attributeMode)
				return m_cx->cache(exec);
			else
				return Number(m_cx->baseVal()->value());
		case Cy:
			if(!attributeMode)
				return m_cy->cache(exec);
			else
				return Number(m_cy->baseVal()->value());
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

void SVGEllipseElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case Cx:
			cx()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Cy:
			cy()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Rx:
			rx()->baseVal()->setValueAsString(value.toString(exec).qstring());
			if(rx()->baseVal()->value() < 0) // A negative value is an error
				gotError("Negative value for attribute rx of element <ellipse> is illegal");
			break;
		case Ry:
			ry()->baseVal()->setValueAsString(value.toString(exec).qstring());
			if(ry()->baseVal()->value() < 0) // A negative value is an error
				gotError(i18n("Negative value for attribute ry of element <ellipse> is illegal"));
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

SVGRectImpl *SVGEllipseElementImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();
	ret->setX(m_cx->baseVal()->value() - m_rx->baseVal()->value());
	ret->setY(m_cy->baseVal()->value() - m_ry->baseVal()->value());
	ret->setWidth(m_rx->baseVal()->value() * 2.0);
	ret->setHeight(m_ry->baseVal()->value() * 2.0);
	return ret;
}

void SVGEllipseElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(Cx))
		KSVG_SET_ALT_ATTRIBUTE(Cx, "0")

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(Cy))
		KSVG_SET_ALT_ATTRIBUTE(Cy, "0")
}

void SVGEllipseElementImpl::createItem(KSVGCanvas *c)
{
    if(!c)
        c = ownerDoc()->canvas();

	if(!m_item)
	{
		m_item = c->createEllipse(this);
		c->insert(m_item);
	}
}

// vim:ts=4:noet
