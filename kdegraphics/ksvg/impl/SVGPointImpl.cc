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

#include "SVGPointImpl.h"
#include "SVGSVGElementImpl.h"

using namespace KSVG;

#include "SVGPointImpl.lut.h"
#include "ksvg_bridge.h"

SVGPointImpl::SVGPointImpl() : DOM::DomShared()
{
	KSVG_EMPTY_FLAGS

	m_x = 0;
	m_y = 0;
}

SVGPointImpl::~SVGPointImpl()
{
}

void SVGPointImpl::setX(float x)
{
	m_x = x;
}

float SVGPointImpl::x() const
{
	return m_x;
}

void SVGPointImpl::setY(float y)
{
	m_y = y;
}

float SVGPointImpl::y() const
{
	return m_y;
}

SVGPointImpl *SVGPointImpl::matrixTransform(const SVGMatrixImpl &)
{
	SVGPointImpl *ret = SVGSVGElementImpl::createSVGPoint();
	return ret;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPointImpl::s_hashTable 3
 x	SVGPointImpl::X	DontDelete
 y	SVGPointImpl::Y	DontDelete
@end
*/

Value SVGPointImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case X:
			return Number(x());
		case Y:
			return Number(y());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPointImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case X:
			m_x = value.toNumber(exec);
			break;
		case Y:
			m_y = value.toNumber(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
