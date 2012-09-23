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

#include "SVGPathSegCurvetoQuadraticSmoothImpl.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

#include "SVGPathSegCurvetoQuadraticSmoothImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGPathSegCurvetoQuadraticSmoothAbsImpl::SVGPathSegCurvetoQuadraticSmoothAbsImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegCurvetoQuadraticSmoothAbsImpl::~SVGPathSegCurvetoQuadraticSmoothAbsImpl()
{
}

void SVGPathSegCurvetoQuadraticSmoothAbsImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegCurvetoQuadraticSmoothAbsImpl::x() const
{
	return m_x;
}

void SVGPathSegCurvetoQuadraticSmoothAbsImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegCurvetoQuadraticSmoothAbsImpl::y() const
{
	return m_y;
}

void SVGPathSegCurvetoQuadraticSmoothAbsImpl::setPreviousX1(double x1)
{
	m_previousX1 = x1;
}

void SVGPathSegCurvetoQuadraticSmoothAbsImpl::setPreviousY1(double y1)
{
	m_previousY1 = y1;
}
	
double SVGPathSegCurvetoQuadraticSmoothAbsImpl::x1(double curx) const
{
	return curx - (m_previousX1 - curx);
}

double SVGPathSegCurvetoQuadraticSmoothAbsImpl::y1(double cury) const
{
	return cury - (m_previousY1 - cury);
}

void SVGPathSegCurvetoQuadraticSmoothAbsImpl::getDeltasAndSlopes(double curx, double cury, double *pDx, double *pDy, double *pStartSlope, double *pEndSlope) const
{
	double dx = x() - curx;
	double dy = y() - cury;
	double startSlope = SVGAngleImpl::todeg(atan2(y1(cury) - cury, x1(curx) - curx));
	double endSlope = SVGAngleImpl::todeg(atan2(y() - y1(cury), x() - x1(curx)));
	*pDx = dx;
	*pDy = dy;
	*pStartSlope = startSlope;
	*pEndSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegCurvetoQuadraticSmoothAbsImpl::s_hashTable 3
 x		SVGPathSegCurvetoQuadraticSmoothAbsImpl::X		DontDelete
 y		SVGPathSegCurvetoQuadraticSmoothAbsImpl::Y		DontDelete
@end
*/

Value SVGPathSegCurvetoQuadraticSmoothAbsImpl::getValueProperty(ExecState *, int token) const
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

void SVGPathSegCurvetoQuadraticSmoothAbsImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
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




SVGPathSegCurvetoQuadraticSmoothRelImpl::SVGPathSegCurvetoQuadraticSmoothRelImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegCurvetoQuadraticSmoothRelImpl::~SVGPathSegCurvetoQuadraticSmoothRelImpl()
{
}

void SVGPathSegCurvetoQuadraticSmoothRelImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegCurvetoQuadraticSmoothRelImpl::x() const
{
	return m_x;
}

void SVGPathSegCurvetoQuadraticSmoothRelImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegCurvetoQuadraticSmoothRelImpl::y() const
{
	return m_y;
}

void SVGPathSegCurvetoQuadraticSmoothRelImpl::setPreviousAbsX1(double x1)
{
	m_previousAbsX1 = x1;
}

void SVGPathSegCurvetoQuadraticSmoothRelImpl::setPreviousAbsY1(double y1)
{
	m_previousAbsY1 = y1;
}
	
double SVGPathSegCurvetoQuadraticSmoothRelImpl::absX1(double curx) const
{
	return curx - (m_previousAbsX1 - curx);
}

double SVGPathSegCurvetoQuadraticSmoothRelImpl::absY1(double cury) const
{
	return cury - (m_previousAbsY1 - cury);
}

void SVGPathSegCurvetoQuadraticSmoothRelImpl::getDeltasAndSlopes(double curx, double cury, double *pDx, double *pDy, double *pStartSlope, double *pEndSlope) const
{
	double dx = x();
	double dy = y();
	double startSlope = SVGAngleImpl::todeg(atan2(absY1(cury) - cury, absX1(curx) - curx));
	double endSlope = SVGAngleImpl::todeg(atan2(cury + y() - absY1(cury), curx + x() - absX1(curx)));
	*pDx = dx;
	*pDy = dy;
	*pStartSlope = startSlope;
	*pEndSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegCurvetoQuadraticSmoothRelImpl::s_hashTable 3
 x		SVGPathSegCurvetoQuadraticSmoothRelImpl::X		DontDelete
 y		SVGPathSegCurvetoQuadraticSmoothRelImpl::Y		DontDelete
@end
*/

Value SVGPathSegCurvetoQuadraticSmoothRelImpl::getValueProperty(ExecState *, int token) const
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

void SVGPathSegCurvetoQuadraticSmoothRelImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
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
