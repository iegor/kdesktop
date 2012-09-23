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

#include "SVGPathSegCurvetoQuadraticImpl.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

#include "SVGPathSegCurvetoQuadraticImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGPathSegCurvetoQuadraticAbsImpl::SVGPathSegCurvetoQuadraticAbsImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegCurvetoQuadraticAbsImpl::~SVGPathSegCurvetoQuadraticAbsImpl()
{
}

void SVGPathSegCurvetoQuadraticAbsImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegCurvetoQuadraticAbsImpl::x() const
{
	return m_x;
}

void SVGPathSegCurvetoQuadraticAbsImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegCurvetoQuadraticAbsImpl::y() const
{
	return m_y;
}

void SVGPathSegCurvetoQuadraticAbsImpl::setX1(const double &x1)
{
	m_x1 = x1;
}

double SVGPathSegCurvetoQuadraticAbsImpl::x1() const
{
	return m_x1;
}

void SVGPathSegCurvetoQuadraticAbsImpl::setY1(const double &y1)
{
	m_y1 = y1;
}

double SVGPathSegCurvetoQuadraticAbsImpl::y1() const
{
	return m_y1;
}

void SVGPathSegCurvetoQuadraticAbsImpl::getDeltasAndSlopes(double curx, double cury, double *pDx, double *pDy, double *pStartSlope, double *pEndSlope) const
{
	double dx = x() - curx;
	double dy = y() - cury;
	double startSlope = SVGAngleImpl::todeg(atan2(y1() - cury, x1() - curx));
	double endSlope = SVGAngleImpl::todeg(atan2(y() - y1(), x() - x1()));
	*pDx = dx;
	*pDy = dy;
	*pStartSlope = startSlope;
	*pEndSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegCurvetoQuadraticAbsImpl::s_hashTable 5
 x		SVGPathSegCurvetoQuadraticAbsImpl::X		DontDelete
 y		SVGPathSegCurvetoQuadraticAbsImpl::Y		DontDelete
 x1		SVGPathSegCurvetoQuadraticAbsImpl::X1		DontDelete
 y1		SVGPathSegCurvetoQuadraticAbsImpl::Y1		DontDelete
@end
*/

Value SVGPathSegCurvetoQuadraticAbsImpl::getValueProperty(ExecState *, int token) const
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
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegCurvetoQuadraticAbsImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
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
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}



SVGPathSegCurvetoQuadraticRelImpl::SVGPathSegCurvetoQuadraticRelImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegCurvetoQuadraticRelImpl::~SVGPathSegCurvetoQuadraticRelImpl()
{
}

void SVGPathSegCurvetoQuadraticRelImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegCurvetoQuadraticRelImpl::x() const
{
	return m_x;
}

void SVGPathSegCurvetoQuadraticRelImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegCurvetoQuadraticRelImpl::y() const
{
	return m_y;
}

void SVGPathSegCurvetoQuadraticRelImpl::setX1(const double &x1)
{
	m_x1 = x1;
}

double SVGPathSegCurvetoQuadraticRelImpl::x1() const
{
	return m_x1;
}

void SVGPathSegCurvetoQuadraticRelImpl::setY1(const double &y1)
{
	m_y1 = y1;
}

double SVGPathSegCurvetoQuadraticRelImpl::y1() const
{
	return m_y1;
}

void SVGPathSegCurvetoQuadraticRelImpl::getDeltasAndSlopes(double curx, double cury, double *pDx, double *pDy, double *pStartSlope, double *pEndSlope) const
{
	Q_UNUSED(curx);
	Q_UNUSED(cury);
	double dx = x();
	double dy = y();
	double startSlope = SVGAngleImpl::todeg(atan2(y1(), x1()));
	double endSlope = SVGAngleImpl::todeg(atan2(y() - y1(), x() - x1()));
	*pDx = dx;
	*pDy = dy;
	*pStartSlope = startSlope;
	*pEndSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegCurvetoQuadraticRelImpl::s_hashTable 5
 x		SVGPathSegCurvetoQuadraticRelImpl::X		DontDelete
 y		SVGPathSegCurvetoQuadraticRelImpl::Y		DontDelete
 x1		SVGPathSegCurvetoQuadraticRelImpl::X1		DontDelete
 y1		SVGPathSegCurvetoQuadraticRelImpl::Y1		DontDelete
@end
*/

Value SVGPathSegCurvetoQuadraticRelImpl::getValueProperty(ExecState *, int token) const
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
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegCurvetoQuadraticRelImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
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
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
