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

#include "SVGGradientElement.h"
#include "SVGRadialGradientElementImpl.h"

#include "SVGDocumentImpl.h"
#include "KSVGCanvas.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGUnitConverter.h"

using namespace KSVG;

#include "SVGRadialGradientElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGRadialGradientElementImpl::SVGRadialGradientElementImpl(DOM::ElementImpl *impl) : SVGGradientElementImpl(impl)
{
	KSVG_EMPTY_FLAGS

	m_cx = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_cx->ref();

	m_cy = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_cy->ref();

	m_r = new SVGAnimatedLengthImpl(LENGTHMODE_OTHER, this);
	m_r->ref();

	m_fx = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_fx->ref();

	m_fy = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_fy->ref();

	converter()->add(m_cx);
	converter()->add(m_cy);
	converter()->add(m_r);
	converter()->add(m_fx);
	converter()->add(m_fy);
}

SVGRadialGradientElementImpl::~SVGRadialGradientElementImpl()
{
	if(m_cx)
		m_cx->deref();
	if(m_cy)
		m_cy->deref();
	if(m_r)
		m_r->deref();
	if(m_fx)
		m_fx->deref();
	if(m_fy)
		m_fy->deref();
}

SVGAnimatedLengthImpl *SVGRadialGradientElementImpl::cx() const
{
	return m_cx;
}

SVGAnimatedLengthImpl *SVGRadialGradientElementImpl::cy() const
{
	return m_cy;
}

SVGAnimatedLengthImpl *SVGRadialGradientElementImpl::r() const
{
	return m_r;
}

SVGAnimatedLengthImpl *SVGRadialGradientElementImpl::fx() const
{
	return m_fx;
}

SVGAnimatedLengthImpl *SVGRadialGradientElementImpl::fy() const
{
	return m_fy;
}

/*
@namespace KSVG
@begin SVGRadialGradientElementImpl::s_hashTable 7
 cx		SVGRadialGradientElementImpl::Cx	DontDelete|ReadOnly
 cy		SVGRadialGradientElementImpl::Cy	DontDelete|ReadOnly
 r		SVGRadialGradientElementImpl::R		DontDelete|ReadOnly
 fx		SVGRadialGradientElementImpl::Fx	DontDelete|ReadOnly
 fy		SVGRadialGradientElementImpl::Fy	DontDelete|ReadOnly
@end
*/

Value SVGRadialGradientElementImpl::getValueProperty(ExecState *exec, int token) const
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
		case Fx:
			if(!attributeMode)
				return m_fx->cache(exec);
			else
				return Number(m_fx->baseVal()->value());
		case Fy:
			if(!attributeMode)
				return m_fy->cache(exec);
			else
				return Number(m_fy->baseVal()->value());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGRadialGradientElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case Cx:
			converter()->modify(cx(), value.toString(exec).qstring());
			break;
		case Cy:
			converter()->modify(cy(), value.toString(exec).qstring());
			break;
		case R:
			converter()->modify(r(), value.toString(exec).qstring());
			break;
		case Fx:
			converter()->modify(fx(), value.toString(exec).qstring());
			break;
		case Fy:
			converter()->modify(fy(), value.toString(exec).qstring());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

void SVGRadialGradientElementImpl::setAttributes()
{
	SVGGradientElementImpl::setAttributes();

	// Spec: no attribute, effect is af value 50% is specified
	if(KSVG_TOKEN_NOT_PARSED(Cx))
		KSVG_SET_ALT_ATTRIBUTE(Cx, "50%")

	// Spec: no attribute, effect is af value 50% is specified
	if(KSVG_TOKEN_NOT_PARSED(Cy))
		KSVG_SET_ALT_ATTRIBUTE(Cy, "50%")

	// Spec: no attribute, effect is af value 50% is specified
	if(KSVG_TOKEN_NOT_PARSED(R))
		KSVG_SET_ALT_ATTRIBUTE(R, "50%")
}

QMap<QString, DOM::DOMString> SVGRadialGradientElementImpl::gradientAttributes()
{
	setAttributes();

	QMap<QString, DOM::DOMString> gradAttributes;
	QDictIterator<DOM::DOMString> it(attributes());

	for(; it.current(); ++it)
	{
		DOM::DOMString name = it.currentKey();
		DOM::DOMString value = it.current()->string();

		if(name == "gradientUnits" || name == "gradientTransform" || name == "spreadMethod" || name == "cx" || name == "cy" || name == "r" || name == "fx" || name == "fy")
		{
			gradAttributes.insert(name.string(), value.copy());
		}
	}
	
	return gradAttributes;
}

// vim:ts=4:noet
