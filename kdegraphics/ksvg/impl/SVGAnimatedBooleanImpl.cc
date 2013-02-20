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

#include "SVGAnimatedBooleanImpl.h"

using namespace KSVG;

#include "SVGAnimatedBooleanImpl.lut.h"
#include "ksvg_bridge.h"

SVGAnimatedBooleanImpl::SVGAnimatedBooleanImpl() : DOM::DomShared()
{
	KSVG_EMPTY_FLAGS

	m_baseVal = false;
	m_animVal = false;
}

SVGAnimatedBooleanImpl::~SVGAnimatedBooleanImpl()
{
}

void SVGAnimatedBooleanImpl::setBaseVal(bool baseVal)
{
	m_baseVal = baseVal;
}

bool SVGAnimatedBooleanImpl::baseVal() const
{
	return m_baseVal;
}

bool SVGAnimatedBooleanImpl::animVal() const
{
	return m_animVal;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGAnimatedBooleanImpl::s_hashTable 3
 baseVal	SVGAnimatedBooleanImpl::BaseVal	DontDelete
 animVal	SVGAnimatedBooleanImpl::AnimVal	DontDelete|ReadOnly
@end
*/

Value SVGAnimatedBooleanImpl::getValueProperty(ExecState *, int token) const
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

void SVGAnimatedBooleanImpl::putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case BaseVal:
			setBaseVal(value.toBoolean(exec));
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
