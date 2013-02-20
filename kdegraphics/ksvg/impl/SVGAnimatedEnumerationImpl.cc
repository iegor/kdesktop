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

#include "SVGAnimatedEnumerationImpl.h"

using namespace KSVG;

#include "SVGAnimatedEnumerationImpl.lut.h"
#include "ksvg_bridge.h"

SVGAnimatedEnumerationImpl::SVGAnimatedEnumerationImpl() : DOM::DomShared()
{
	KSVG_EMPTY_FLAGS

	m_baseVal = 0;
	m_animVal = 0;
}

SVGAnimatedEnumerationImpl::~SVGAnimatedEnumerationImpl()
{
}

void SVGAnimatedEnumerationImpl::setBaseVal(unsigned short baseVal)
{
	m_baseVal = baseVal;
}

unsigned short SVGAnimatedEnumerationImpl::baseVal() const
{
	return m_baseVal;
}

unsigned short SVGAnimatedEnumerationImpl::animVal() const
{
	return m_animVal;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGAnimatedEnumerationImpl::s_hashTable 3
 baseVal	SVGAnimatedEnumerationImpl::BaseVal	DontDelete
 animVal	SVGAnimatedEnumerationImpl::AnimVal	DontDelete|ReadOnly
@end
*/

Value SVGAnimatedEnumerationImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case BaseVal:
			return Number(baseVal());
		case AnimVal:
			return Number(animVal());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGAnimatedEnumerationImpl::putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case BaseVal:
			setBaseVal(static_cast<unsigned short>(value.toNumber(exec)));
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
