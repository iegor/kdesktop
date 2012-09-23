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

#include "SVGNumberImpl.h"

using namespace KSVG;

#include "SVGNumberImpl.lut.h"
#include "ksvg_bridge.h"

SVGNumberImpl::SVGNumberImpl()
{
	KSVG_EMPTY_FLAGS

	m_value = 0;
}

SVGNumberImpl::~SVGNumberImpl()
{
}

void SVGNumberImpl::setValue(float value)
{
	m_value = value;
}

float SVGNumberImpl::value()
{
	return m_value;
}

/*
@namespace KSVG
@begin SVGNumberImpl::s_hashTable 2
 value	SVGNumberImpl::Value	DontDelete
@end
*/

Value SVGNumberImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case Value:
			return Number(m_value);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
	
	return Undefined();
}

void SVGNumberImpl::putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case Value:
			m_value = value.toNumber(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
