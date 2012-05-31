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
#include "SVGCircleElementImpl.h"
#include "SVGAnimatedLengthImpl.h"

using namespace KSVG;

#include "SVGCircleElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGCircleElementImpl::SVGCircleElementImpl(DOM::ElementImpl *impl) : SVGShapeImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl()
{
	KSVG_EMPTY_FLAGS

	m_cx = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_cx->ref();
	m_cx->baseVal()->setValueAsString("-1");

	m_cy = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_cy->ref();
	m_cy->baseVal()->setValueAsString("-1");

	m_r = new SVGAnimatedLengthImpl(LENGTHMODE_OTHER, this);
	m_r->ref();
	m_r->baseVal()->setValueAsString("-1");
}

SVGCircleElementImpl::~SVGCircleElementImpl()
{
	if(m_cx)
		m_cx->deref();
	if(m_cy)
		m_cy->deref();
	if(m_r)
		m_r->deref();
}

SVGAnimatedLengthImpl *SVGCircleElementImpl::cx()
{
	return m_cx;
}

SVGAnimatedLengthImpl *SVGCircleElementImpl::cy()
{
	return m_cy;
}

SVGAnimatedLengthImpl *SVGCircleElementImpl::r()
{
	return m_r;
}

/*
@namespace KSVG
@begin SVGCircleElementImpl::s_hashTable 5
 cx	SVGCircleElementImpl::Cx	DontDelete|ReadOnly
 cy	SVGCircleElementImpl::Cy	DontDelete|ReadOnly
 r	SVGCircleElementImpl::R		DontDelete|ReadOnly
@end
*/

Value SVGCircleElementImpl::getValueProperty(ExecState *exec, int token) const
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
		case R:
			if(!attributeMode)
				return m_r->cache(exec);
			else
				return Number(m_r->baseVal()->value());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGCircleElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
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
		case R:
			r()->baseVal()->setValueAsString(value.toString(exec).qstring());
			if(r()->baseVal()->value() < 0) // A negative value is an error
				gotError(i18n("Negative value for attribute r of element <circle> is illegal"));
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

SVGRectImpl *SVGCircleElementImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();
	ret->setX(m_cx->baseVal()->value() - m_r->baseVal()->value());
	ret->setY(m_cy->baseVal()->value() - m_r->baseVal()->value());
	ret->setWidth(m_r->baseVal()->value() * 2.0);
	ret->setHeight(m_r->baseVal()->value() * 2.0);
	return ret;
}

void SVGCircleElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(Cx))
		KSVG_SET_ALT_ATTRIBUTE(Cx, "0")

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(Cy))
		KSVG_SET_ALT_ATTRIBUTE(Cy, "0")
}

void SVGCircleElementImpl::createItem(KSVGCanvas *c)
{
    if(!c)
        c = ownerDoc()->canvas();

	if(!m_item)
	{
		m_item = c->createCircle(this);
		c->insert(m_item);
	}
}

// vim:ts=4:noet
