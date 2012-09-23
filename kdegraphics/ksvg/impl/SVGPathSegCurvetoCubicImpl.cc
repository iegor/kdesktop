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

#include "SVGPathSegCurvetoCubicImpl.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

#include "SVGPathSegCurvetoCubicImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGPathSegCurvetoCubicAbsImpl::SVGPathSegCurvetoCubicAbsImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegCurvetoCubicAbsImpl::~SVGPathSegCurvetoCubicAbsImpl()
{
}

void SVGPathSegCurvetoCubicAbsImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegCurvetoCubicAbsImpl::x() const
{
	return m_x;
}

void SVGPathSegCurvetoCubicAbsImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegCurvetoCubicAbsImpl::y() const
{
	return m_y;
}

void SVGPathSegCurvetoCubicAbsImpl::setX1(const double &x1)
{
	m_x1 = x1;
}

double SVGPathSegCurvetoCubicAbsImpl::x1() const
{
	return m_x1;
}

void SVGPathSegCurvetoCubicAbsImpl::setY1(const double &y1)
{
	m_y1 = y1;
}

double SVGPathSegCurvetoCubicAbsImpl::y1() const
{
	return m_y1;
}

void SVGPathSegCurvetoCubicAbsImpl::setX2(const double &x2)
{
	m_x2 = x2;
}

double SVGPathSegCurvetoCubicAbsImpl::x2() const
{
	return m_x2;
}

void SVGPathSegCurvetoCubicAbsImpl::setY2(const double &y2)
{
	m_y2 = y2;
}

double SVGPathSegCurvetoCubicAbsImpl::y2() const
{
	return m_y2;
}

void SVGPathSegCurvetoCubicAbsImpl::getDeltasAndSlopes(double curx, double cury, double *pDx, double *pDy, double *pStartSlope, double *pEndSlope) const
{
	double dx = x() - curx;
	double dy = y() - cury;
	double startSlope = SVGAngleImpl::todeg(atan2(y1() - cury, x1() - curx));
	double endSlope = SVGAngleImpl::todeg(atan2(y() - y2(), x() - x2()));
	*pDx = dx;
	*pDy = dy;
	*pStartSlope = startSlope;
	*pEndSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegCurvetoCubicAbsImpl::s_hashTable 7
 x		SVGPathSegCurvetoCubicAbsImpl::X		DontDelete
 y		SVGPathSegCurvetoCubicAbsImpl::Y		DontDelete
 x1		SVGPathSegCurvetoCubicAbsImpl::X1		DontDelete
 y1		SVGPathSegCurvetoCubicAbsImpl::Y1		DontDelete
 x2		SVGPathSegCurvetoCubicAbsImpl::X2		DontDelete
 y2		SVGPathSegCurvetoCubicAbsImpl::Y2		DontDelete
@end
*/

Value SVGPathSegCurvetoCubicAbsImpl::getValueProperty(ExecState *, int token) const
{
    switch(token)
    {
        case X:
			return Number(x());
        case Y:
			return Number(y());
        case X1:
			return Number(x1());
        case Y1:
			return Number(y1());
        case X2:
			return Number(x2());
        case Y2:
			return Number(y2());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegCurvetoCubicAbsImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case X:
			m_x = value.toNumber(exec);
			break;
		case Y:
			m_y = value.toNumber(exec);
			break;
		case X1:
			m_x1 = value.toNumber(exec);
			break;
		case Y1:
			m_y1 = value.toNumber(exec);
			break;
		case X2:
			m_x2 = value.toNumber(exec);
			break;
		case Y2:
			m_y2 = value.toNumber(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}




SVGPathSegCurvetoCubicRelImpl::SVGPathSegCurvetoCubicRelImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegCurvetoCubicRelImpl::~SVGPathSegCurvetoCubicRelImpl()
{
}

void SVGPathSegCurvetoCubicRelImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegCurvetoCubicRelImpl::x() const
{
	return m_x;
}

void SVGPathSegCurvetoCubicRelImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegCurvetoCubicRelImpl::y() const
{
	return m_y;
}

void SVGPathSegCurvetoCubicRelImpl::setX1(const double &x1)
{
	m_x1 = x1;
}

double SVGPathSegCurvetoCubicRelImpl::x1() const
{
	return m_x1;
}

void SVGPathSegCurvetoCubicRelImpl::setY1(const double &y1)
{
	m_y1 = y1;
}

double SVGPathSegCurvetoCubicRelImpl::y1() const
{
	return m_y1;
}

void SVGPathSegCurvetoCubicRelImpl::setX2(const double &x2)
{
	m_x2 = x2;
}

double SVGPathSegCurvetoCubicRelImpl::x2() const
{
	return m_x2;
}

void SVGPathSegCurvetoCubicRelImpl::setY2(const double &y2)
{
	m_y2 = y2;
}

double SVGPathSegCurvetoCubicRelImpl::y2() const
{
	return m_y2;
}

void SVGPathSegCurvetoCubicRelImpl::getDeltasAndSlopes(double curx, double cury, double *pDx, double *pDy, double *pStartSlope, double *pEndSlope) const
{
	Q_UNUSED(curx);
	Q_UNUSED(cury);
	double dx = x();
	double dy = y();
	double startSlope = SVGAngleImpl::todeg(atan2(y1(), x1()));
	double endSlope = SVGAngleImpl::todeg(atan2(y() - y2(), x() - x2()));
	*pDx = dx;
	*pDy = dy;
	*pStartSlope = startSlope;
	*pEndSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegCurvetoCubicRelImpl::s_hashTable 7
 x		SVGPathSegCurvetoCubicRelImpl::X		DontDelete
 y		SVGPathSegCurvetoCubicRelImpl::Y		DontDelete
 x1		SVGPathSegCurvetoCubicRelImpl::X1		DontDelete
 y1		SVGPathSegCurvetoCubicRelImpl::Y1		DontDelete
 x2		SVGPathSegCurvetoCubicRelImpl::X2		DontDelete
 y2		SVGPathSegCurvetoCubicRelImpl::Y2		DontDelete
@end
*/

Value SVGPathSegCurvetoCubicRelImpl::getValueProperty(ExecState *, int token) const
{
    switch(token)
    {
        case X:
			return Number(x());
        case Y:
			return Number(y());
        case X1:
			return Number(x1());
        case Y1:
			return Number(y1());
        case X2:
			return Number(x2());
        case Y2:
			return Number(y2());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegCurvetoCubicRelImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case X:
			m_x = value.toNumber(exec);
			break;
		case Y:
			m_y = value.toNumber(exec);
			break;
		case X1:
			m_x1 = value.toNumber(exec);
			break;
		case Y1:
			m_y1 = value.toNumber(exec);
			break;
		case X2:
			m_x2 = value.toNumber(exec);
			break;
		case Y2:
			m_y2 = value.toNumber(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
