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

#include "SVGPathSegMovetoImpl.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

#include "SVGPathSegMovetoImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGPathSegMovetoAbsImpl::SVGPathSegMovetoAbsImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegMovetoAbsImpl::~SVGPathSegMovetoAbsImpl()
{
}

void SVGPathSegMovetoAbsImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegMovetoAbsImpl::x() const
{
	return m_x;
}

void SVGPathSegMovetoAbsImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegMovetoAbsImpl::y() const
{
	return m_y;
}

void SVGPathSegMovetoAbsImpl::getDeltasAndSlopes(double curx, double cury, double *pdx, double *pdy, double *pstartSlope, double *pendSlope) const
{
	double dx = x() - curx;
	double dy = y() - cury;
	double startSlope = 0;
	double endSlope = 0;
	*pdx = dx;
	*pdy = dy;
	*pstartSlope = startSlope;
	*pendSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegMovetoAbsImpl::s_hashTable 3
 x		SVGPathSegMovetoAbsImpl::X		DontDelete
 y		SVGPathSegMovetoAbsImpl::Y		DontDelete
@end
*/

Value SVGPathSegMovetoAbsImpl::getValueProperty(ExecState *, int token) const
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

void SVGPathSegMovetoAbsImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
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



SVGPathSegMovetoRelImpl::SVGPathSegMovetoRelImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegMovetoRelImpl::~SVGPathSegMovetoRelImpl()
{
}

void SVGPathSegMovetoRelImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegMovetoRelImpl::x() const
{
	return m_x;
}

void SVGPathSegMovetoRelImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegMovetoRelImpl::y() const
{
	return m_y;
}

void SVGPathSegMovetoRelImpl::getDeltasAndSlopes(double curx, double cury, double *pdx, double *pdy, double *pstartSlope, double *pendSlope) const
{
	Q_UNUSED(curx);
	Q_UNUSED(cury);
	double dx = x();
	double dy = y();
	double startSlope = 0;
	double endSlope = 0;
	*pdx = dx;
	*pdy = dy;
	*pstartSlope = startSlope;
	*pendSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegMovetoRelImpl::s_hashTable 3
 x		SVGPathSegMovetoRelImpl::X		DontDelete
 y		SVGPathSegMovetoRelImpl::Y		DontDelete
@end
*/

Value SVGPathSegMovetoRelImpl::getValueProperty(ExecState *, int token) const
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

void SVGPathSegMovetoRelImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
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
