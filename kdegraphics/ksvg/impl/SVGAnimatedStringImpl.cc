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

#include "SVGAnimatedStringImpl.h"

using namespace KSVG;

#include "SVGAnimatedStringImpl.lut.h"
#include "ksvg_bridge.h"

SVGAnimatedStringImpl::SVGAnimatedStringImpl() : DOM::DomShared()
{
	KSVG_EMPTY_FLAGS
}

SVGAnimatedStringImpl::~SVGAnimatedStringImpl()
{
}

void SVGAnimatedStringImpl::setBaseVal(const DOM::DOMString &baseVal)
{
	m_baseVal = baseVal;
}

DOM::DOMString SVGAnimatedStringImpl::baseVal() const
{
	return m_baseVal;
}

DOM::DOMString SVGAnimatedStringImpl::animVal() const
{
	return m_animVal;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGAnimatedStringImpl::s_hashTable 3
 baseVal	SVGAnimatedStringImpl::BaseVal	DontDelete
 animVal	SVGAnimatedStringImpl::AnimVal	DontDelete|ReadOnly
@end
*/

Value SVGAnimatedStringImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case BaseVal:
			return String(baseVal().string());
		case AnimVal:
			return String(animVal().string());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGAnimatedStringImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case BaseVal:
			setBaseVal(value.toString(exec).string());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
