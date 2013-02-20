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
#include "SVGLengthImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGStopElementImpl.h"
#include "SVGAnimatedNumberImpl.h"
#include "SVGColorImpl.h"

using namespace KSVG;

#include "SVGStopElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGStopElementImpl::SVGStopElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGStylableImpl(this)
{
	KSVG_EMPTY_FLAGS

	m_offset = new SVGAnimatedNumberImpl();
	m_offset->ref();

	m_stopOpacity = 1;
}

SVGStopElementImpl::~SVGStopElementImpl()
{
	if(m_offset)
		m_offset->deref();
}

SVGAnimatedNumberImpl *SVGStopElementImpl::offset() const
{
	return m_offset;
}

float SVGStopElementImpl::stopOpacity() const
{
	return m_stopOpacity;
}


/*
@namespace KSVG
@begin SVGStopElementImpl::s_hashTable 3
 offset			SVGStopElementImpl::Offset		DontDelete|ReadOnly
 stop-opacity	SVGStopElementImpl::StopOpacity	DontDelete|ReadOnly
@end
*/

Value SVGStopElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE

	switch(token)
	{
		case Offset:
			if(!attributeMode)
				return m_offset->cache(exec);
			else
				return Number(m_offset->baseVal());
		case StopOpacity:
			if(!attributeMode)
				return Undefined();
			else
				return Number(m_stopOpacity);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGStopElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case Offset:
			float temp;
			SVGLengthImpl::convertPercentageToFloat(value.toString(exec).qstring(), temp);
			offset()->setBaseVal(temp);
			break;
		case StopOpacity:
		{
			SVGLengthImpl::convertPercentageToFloat(value.toString(exec).qstring(), m_stopOpacity);
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

void SVGStopElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: if not set, specifiy 0
	if(KSVG_TOKEN_NOT_PARSED(Offset))
		KSVG_SET_ALT_ATTRIBUTE(Offset, "0")
}

// vim:ts=4:noet
