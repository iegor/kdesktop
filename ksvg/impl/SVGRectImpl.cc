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

#include <qrect.h>

#include "SVGRectImpl.h"

using namespace KSVG;

#include "SVGRectImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGRectImpl::SVGRectImpl()
{
	KSVG_EMPTY_FLAGS

	m_x = 0;
	m_y = 0;
	m_width = 0;
	m_height = 0;
}

SVGRectImpl::SVGRectImpl(const QRect &other)
{
	(*this) = other;
}

SVGRectImpl::~SVGRectImpl()
{
}

void SVGRectImpl::setX(float x)
{
	m_x = x;
}

float SVGRectImpl::x() const
{
	return m_x;
}

void SVGRectImpl::setY(float y)
{
	m_y = y;
}

float SVGRectImpl::y() const
{
	return m_y;
}

void SVGRectImpl::setWidth(float width)
{
	m_width = width;
}

float SVGRectImpl::width() const
{
	return m_width;
}

void SVGRectImpl::setHeight(float height)
{
	m_height = height;
}

float SVGRectImpl::height() const
{
	return m_height;
}

QRect SVGRectImpl::qrect() const
{
	// ceil() so the integer rectangle contains the whole real one.
	return QRect(int(m_x), int(m_y), int(ceil(m_width)), int(ceil(m_height)));
}

SVGRectImpl &SVGRectImpl::operator=(const QRect &other)
{
	m_x = other.x();
	m_y = other.y();
	m_width = other.width();
	m_height = other.height();

	return *this;
}

/*
@namespace KSVG
@begin SVGRectImpl::s_hashTable 5
 x			SVGRectImpl::X			DontDelete
 y			SVGRectImpl::Y			DontDelete
 width		SVGRectImpl::Width		DontDelete
 height		SVGRectImpl::Height		DontDelete
@end
*/

Value SVGRectImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case X:
			return Number(m_x);
		case Y:
			return Number(m_y);
		case Width:
			return Number(m_width);
		case Height:
			return Number(m_height);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGRectImpl::putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case X:
			m_x = value.toNumber(exec);
			break;
		case Y:
			m_y = value.toNumber(exec);
			break;
		case Width:
			m_width = value.toNumber(exec);
			break;
		case Height:
			m_height = value.toNumber(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
