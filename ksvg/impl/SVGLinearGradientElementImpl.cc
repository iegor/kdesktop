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
#include "SVGLinearGradientElementImpl.h"

#include "SVGDocumentImpl.h"
#include "KSVGCanvas.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGLengthImpl.h"
#include "SVGUnitConverter.h"

using namespace KSVG;

#include "SVGLinearGradientElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGLinearGradientElementImpl::SVGLinearGradientElementImpl(DOM::ElementImpl *impl) : SVGGradientElementImpl(impl)
{
	KSVG_EMPTY_FLAGS

	m_x1 = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_x1->ref();

	m_y1 = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_y1->ref();

	m_x2 = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_x2->ref();

	m_y2 = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_y2->ref();

	converter()->add(m_x1);
	converter()->add(m_y1);
	converter()->add(m_x2);
	converter()->add(m_y2);
}

SVGLinearGradientElementImpl::~SVGLinearGradientElementImpl()
{
	if(m_x1)
		m_x1->deref();
	if(m_y1)
		m_y1->deref();
	if(m_x2)
		m_x2->deref();
	if(m_y2)
		m_y2->deref();
}

SVGAnimatedLengthImpl *SVGLinearGradientElementImpl::x1() const
{
	return m_x1;
}

SVGAnimatedLengthImpl *SVGLinearGradientElementImpl::y1() const
{
	return m_y1;
}

SVGAnimatedLengthImpl *SVGLinearGradientElementImpl::x2() const
{
	return m_x2;
}

SVGAnimatedLengthImpl *SVGLinearGradientElementImpl::y2() const
{
	return m_y2;
}

/*
@namespace KSVG
@begin SVGLinearGradientElementImpl::s_hashTable 5
 x1		SVGLinearGradientElementImpl::X1	DontDelete|ReadOnly
 y1		SVGLinearGradientElementImpl::Y1	DontDelete|ReadOnly
 x2		SVGLinearGradientElementImpl::X2	DontDelete|ReadOnly
 y2		SVGLinearGradientElementImpl::Y2	DontDelete|ReadOnly
@end
*/

Value SVGLinearGradientElementImpl::getValueProperty(ExecState *exec, int token) const
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

void SVGLinearGradientElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case X1:
			converter()->modify(x1(), value.toString(exec).qstring());
			break;
		case Y1:
			converter()->modify(y1(), value.toString(exec).qstring());
			break;
		case X2:
			converter()->modify(x2(), value.toString(exec).qstring());
			break;
		case Y2:
			converter()->modify(y2(), value.toString(exec).qstring());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

void SVGLinearGradientElementImpl::setAttributes()
{
	SVGGradientElementImpl::setAttributes();

	// Spec: no attribute, effect is af value 0% is specified
	if(KSVG_TOKEN_NOT_PARSED(X1))
		KSVG_SET_ALT_ATTRIBUTE(X1, "0")

	// Spec: no attribute, effect is af value 0% is specified
	if(KSVG_TOKEN_NOT_PARSED(Y1))
		KSVG_SET_ALT_ATTRIBUTE(Y1, "0")

	// Spec: no attribute, effect is af value 100% is specified
	if(KSVG_TOKEN_NOT_PARSED(X2))
		KSVG_SET_ALT_ATTRIBUTE(X2, "100%")

	// Spec: no attribute, effect is af value 0% is specified
	if(KSVG_TOKEN_NOT_PARSED(Y2))
		KSVG_SET_ALT_ATTRIBUTE(Y2, "0")
}

QMap<QString, DOM::DOMString> SVGLinearGradientElementImpl::gradientAttributes()
{
	setAttributes();

	QMap<QString, DOM::DOMString> gradAttributes;
	QDictIterator<DOM::DOMString> it(attributes());

	for(; it.current(); ++it)
	{
		DOM::DOMString name = it.currentKey();
		DOM::DOMString value = it.current()->string();

		if(name == "gradientUnits" || name == "gradientTransform" || name == "spreadMethod" || name == "x1" || name == "x2" || name == "y1" || name == "y2")
		{
			gradAttributes.insert(name.string(), value.copy());
		}
	}
	
	return gradAttributes;
}

// vim:ts=4:noet
