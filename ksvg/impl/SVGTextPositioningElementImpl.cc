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

#include "SVGHelperImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGAnimatedLengthListImpl.h"
#include "SVGAnimatedNumberListImpl.h"
#include "SVGTextPositioningElementImpl.h"

using namespace KSVG;

#include "SVGTextPositioningElementImpl.lut.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGTextPositioningElementImpl::SVGTextPositioningElementImpl(DOM::ElementImpl *impl) : SVGTextContentElementImpl(impl)
{
	KSVG_EMPTY_FLAGS

	m_x = new SVGAnimatedLengthListImpl();
	m_x->ref();

	m_y = new SVGAnimatedLengthListImpl();
	m_y->ref();

	m_dx = new SVGAnimatedLengthListImpl();
	m_dx->ref();

	m_dy = new SVGAnimatedLengthListImpl();
	m_dy->ref();

	m_rotate = new SVGAnimatedNumberListImpl();
	m_rotate->ref();
}

SVGTextPositioningElementImpl::~SVGTextPositioningElementImpl()
{
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_dx)
		m_dx->deref();
	if(m_dy)
		m_dy->deref();
	if(m_rotate)
		m_rotate->deref();
}

SVGAnimatedLengthListImpl *SVGTextPositioningElementImpl::x()
{
	return m_x;
}

SVGAnimatedLengthListImpl *SVGTextPositioningElementImpl::y()
{
	return m_y;
}

SVGAnimatedLengthListImpl *SVGTextPositioningElementImpl::dx()
{
	return m_dx;
}

SVGAnimatedLengthListImpl *SVGTextPositioningElementImpl::dy()
{
	return m_dy;
}

SVGAnimatedNumberListImpl *SVGTextPositioningElementImpl::rotate()
{
	return m_rotate;
}

/*
@namespace KSVG
@begin SVGTextPositioningElementImpl::s_hashTable 7
 x		SVGTextPositioningElementImpl::X		DontDelete|ReadOnly
 y		SVGTextPositioningElementImpl::Y		DontDelete|ReadOnly
 dx		SVGTextPositioningElementImpl::Dx		DontDelete|ReadOnly
 dy		SVGTextPositioningElementImpl::Dy		DontDelete|ReadOnly
 rotate	SVGTextPositioningElementImpl::Rotate	DontDelete|ReadOnly
@end
*/

Value SVGTextPositioningElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE

	switch(token)
	{
		case X:
			if(!attributeMode)
				return m_x->cache(exec);
			else
				return Number(m_x->baseVal()->getItem(0)->value());
		case Y:
			if(!attributeMode)
				return m_y->cache(exec);
			else
				return Number(m_y->baseVal()->getItem(0)->value());
		case Dx:
			if(!attributeMode)
				return m_dx->cache(exec);
			else
				return Number(m_dx->baseVal()->getItem(0)->value());
		case Dy:
			if(!attributeMode)
				return m_dy->cache(exec);
			else
				return Number(m_dy->baseVal()->getItem(0)->value());
		case Rotate:
			if(!attributeMode)
				return m_rotate->cache(exec);
			else
				return Number(m_rotate->baseVal()->getItem(0)->value());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGTextPositioningElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;
	
	switch(token)
	{
		case X:
			x()->baseVal()->clear();
			SVGHelperImpl::parseLengthList(x(), value.toString(exec).qstring(), LENGTHMODE_WIDTH, this);
			break;
		case Y:
			y()->baseVal()->clear();
			SVGHelperImpl::parseLengthList(y(), value.toString(exec).qstring(), LENGTHMODE_HEIGHT, this);
			break;
		case Dx:
			dx()->baseVal()->clear();
			SVGHelperImpl::parseLengthList(dx(), value.toString(exec).qstring(), LENGTHMODE_WIDTH, this);
			break;
		case Dy:
			dy()->baseVal()->clear();
			SVGHelperImpl::parseLengthList(dy(), value.toString(exec).qstring(), LENGTHMODE_HEIGHT, this);
			break;
		case Rotate:
		{
			rotate()->baseVal()->clear();

			SVGNumberImpl *number = SVGSVGElementImpl::createSVGNumber();
			number->setValue(value.toNumber(exec));
			rotate()->baseVal()->appendItem(number);
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

void SVGTextPositioningElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	if(tagName() != "text")
		return;
	
	// Spec: If the attribute is not specified, the effect is as if a value of "0" were specified.
	if(KSVG_TOKEN_NOT_PARSED(X))
		KSVG_SET_ALT_ATTRIBUTE(X, "0")

	// Spec: If the attribute is not specified, the effect is as if a value of "0" were specified.
	if(KSVG_TOKEN_NOT_PARSED(Y))
		KSVG_SET_ALT_ATTRIBUTE(Y, "0")
}

// vim:ts=4:noet
