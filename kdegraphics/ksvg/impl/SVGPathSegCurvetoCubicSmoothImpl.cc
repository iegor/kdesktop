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

#include "SVGPathSegCurvetoCubicSmoothImpl.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

#include "SVGPathSegCurvetoCubicSmoothImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGPathSegCurvetoCubicSmoothAbsImpl::SVGPathSegCurvetoCubicSmoothAbsImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegCurvetoCubicSmoothAbsImpl::~SVGPathSegCurvetoCubicSmoothAbsImpl()
{
}

void SVGPathSegCurvetoCubicSmoothAbsImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegCurvetoCubicSmoothAbsImpl::x() const
{
	return m_x;
}

void SVGPathSegCurvetoCubicSmoothAbsImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegCurvetoCubicSmoothAbsImpl::y() const
{
	return m_y;
}

void SVGPathSegCurvetoCubicSmoothAbsImpl::setX2(const double &x2)
{
	m_x2 = x2;
}

double SVGPathSegCurvetoCubicSmoothAbsImpl::x2() const
{
	return m_x2;
}

void SVGPathSegCurvetoCubicSmoothAbsImpl::setY2(const double &y2)
{
	m_y2 = y2;
}

double SVGPathSegCurvetoCubicSmoothAbsImpl::y2() const
{
	return m_y2;
}

void SVGPathSegCurvetoCubicSmoothAbsImpl::setPreviousX2(double x2)
{
	m_previousX2 = x2;
}

void SVGPathSegCurvetoCubicSmoothAbsImpl::setPreviousY2(double y2)
{
	m_previousY2 = y2;
}
	
double SVGPathSegCurvetoCubicSmoothAbsImpl::x1(double curx) const
{
	return curx - (m_previousX2 - curx);
}

double SVGPathSegCurvetoCubicSmoothAbsImpl::y1(double cury) const
{
	return cury - (m_previousY2 - cury);
}

void SVGPathSegCurvetoCubicSmoothAbsImpl::getDeltasAndSlopes(double curx, double cury, double *pDx, double *pDy, double *pStartSlope, double *pEndSlope) const
{
	double dx = x() - curx;
	double dy = y() - cury;
	double startSlope = SVGAngleImpl::todeg(atan2(y1(cury) - cury, x1(curx) - curx));
	double endSlope = SVGAngleImpl::todeg(atan2(y() - y2(), x() - x2()));
	*pDx = dx;
	*pDy = dy;
	*pStartSlope = startSlope;
	*pEndSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegCurvetoCubicSmoothAbsImpl::s_hashTable 5
 x		SVGPathSegCurvetoCubicSmoothAbsImpl::X		DontDelete
 y		SVGPathSegCurvetoCubicSmoothAbsImpl::Y		DontDelete
 x2		SVGPathSegCurvetoCubicSmoothAbsImpl::X2		DontDelete
 y2		SVGPathSegCurvetoCubicSmoothAbsImpl::Y2		DontDelete
@end
*/

Value SVGPathSegCurvetoCubicSmoothAbsImpl::getValueProperty(ExecState *, int token) const
{
    switch(token)
    {
        case X:
			return Number(x());
        case Y:
			return Number(y());
        case X2:
			return Number(x2());
        case Y2:
			return Number(y2());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegCurvetoCubicSmoothAbsImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case X:
			m_x = value.toNumber(exec);
			break;
		case Y:
			m_y = value.toNumber(exec);
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



SVGPathSegCurvetoCubicSmoothRelImpl::SVGPathSegCurvetoCubicSmoothRelImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegCurvetoCubicSmoothRelImpl::~SVGPathSegCurvetoCubicSmoothRelImpl()
{
}

void SVGPathSegCurvetoCubicSmoothRelImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegCurvetoCubicSmoothRelImpl::x() const
{
	return m_x;
}

void SVGPathSegCurvetoCubicSmoothRelImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegCurvetoCubicSmoothRelImpl::y() const
{
	return m_y;
}

void SVGPathSegCurvetoCubicSmoothRelImpl::setX2(const double &x2)
{
	m_x2 = x2;
}

double SVGPathSegCurvetoCubicSmoothRelImpl::x2() const
{
	return m_x2;
}

void SVGPathSegCurvetoCubicSmoothRelImpl::setY2(const double &y2)
{
	m_y2 = y2;
}

double SVGPathSegCurvetoCubicSmoothRelImpl::y2() const
{
	return m_y2;
}

void SVGPathSegCurvetoCubicSmoothRelImpl::setPreviousAbsX2(double x2)
{
	m_previousAbsX2 = x2;
}

void SVGPathSegCurvetoCubicSmoothRelImpl::setPreviousAbsY2(double y2)
{
	m_previousAbsY2 = y2;
}
	
double SVGPathSegCurvetoCubicSmoothRelImpl::absX1(double curx) const
{
	return curx - (m_previousAbsX2 - curx);
}

double SVGPathSegCurvetoCubicSmoothRelImpl::absY1(double cury) const
{
	return cury - (m_previousAbsY2 - cury);
}

void SVGPathSegCurvetoCubicSmoothRelImpl::getDeltasAndSlopes(double curx, double cury, double *pDx, double *pDy, double *pStartSlope, double *pEndSlope) const
{
	double dx = x();
	double dy = y();
	double startSlope = SVGAngleImpl::todeg(atan2(absY1(cury) - cury, absX1(curx) - curx));
	double endSlope = SVGAngleImpl::todeg(atan2(y() - y2(), x() - x2()));
	*pDx = dx;
	*pDy = dy;
	*pStartSlope = startSlope;
	*pEndSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegCurvetoCubicSmoothRelImpl::s_hashTable 5
 x		SVGPathSegCurvetoCubicSmoothRelImpl::X		DontDelete
 y		SVGPathSegCurvetoCubicSmoothRelImpl::Y		DontDelete
 x2		SVGPathSegCurvetoCubicSmoothRelImpl::X2		DontDelete
 y2		SVGPathSegCurvetoCubicSmoothRelImpl::Y2		DontDelete
@end
*/

Value SVGPathSegCurvetoCubicSmoothRelImpl::getValueProperty(ExecState *, int token) const
{
    switch(token)
    {
        case X:
			return Number(x());
        case Y:
			return Number(y());
        case X2:
			return Number(x2());
        case Y2:
			return Number(y2());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegCurvetoCubicSmoothRelImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case X:
			m_x = value.toNumber(exec);
			break;
		case Y:
			m_y = value.toNumber(exec);
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
