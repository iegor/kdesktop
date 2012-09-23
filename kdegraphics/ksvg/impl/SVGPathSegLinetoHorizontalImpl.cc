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

#include "SVGPathSegLinetoHorizontalImpl.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

#include "SVGPathSegLinetoHorizontalImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGPathSegLinetoHorizontalAbsImpl::SVGPathSegLinetoHorizontalAbsImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegLinetoHorizontalAbsImpl::~SVGPathSegLinetoHorizontalAbsImpl()
{
}

void SVGPathSegLinetoHorizontalAbsImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegLinetoHorizontalAbsImpl::x() const
{
	return m_x;
}

void SVGPathSegLinetoHorizontalAbsImpl::getDeltasAndSlopes(double curx, double cury, double *pdx, double *pdy, double *pstartSlope, double *pendSlope) const
{
	Q_UNUSED(cury);
	double dx = x() - curx;
	double dy = 0;
	double startSlope = SVGAngleImpl::todeg(atan2(dy, dx));
	double endSlope = startSlope;
	*pdx = dx;
	*pdy = dy;
	*pstartSlope = startSlope;
	*pendSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegLinetoHorizontalAbsImpl::s_hashTable 2
 x		SVGPathSegLinetoHorizontalAbsImpl::X		DontDelete
@end
*/

Value SVGPathSegLinetoHorizontalAbsImpl::getValueProperty(ExecState *, int token) const
{
    switch(token)
    {
        case X:
			return Number(x());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegLinetoHorizontalAbsImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case X:
			m_x = value.toNumber(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}





SVGPathSegLinetoHorizontalRelImpl::SVGPathSegLinetoHorizontalRelImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegLinetoHorizontalRelImpl::~SVGPathSegLinetoHorizontalRelImpl()
{
}

void SVGPathSegLinetoHorizontalRelImpl::setX(const double &x)
{
	m_x = x;
}

double SVGPathSegLinetoHorizontalRelImpl::x() const
{
	return m_x;
}

void SVGPathSegLinetoHorizontalRelImpl::getDeltasAndSlopes(double curx, double cury, double *pdx, double *pdy, double *pstartSlope, double *pendSlope) const
{
	Q_UNUSED(curx);
	Q_UNUSED(cury);
	double dx = x();
	double dy = 0;
	double startSlope = SVGAngleImpl::todeg(atan2(dy, dx));
	double endSlope = startSlope;
	*pdx = dx;
	*pdy = dy;
	*pstartSlope = startSlope;
	*pendSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegLinetoHorizontalRelImpl::s_hashTable 2
 x		SVGPathSegLinetoHorizontalRelImpl::X		DontDelete
@end
*/

Value SVGPathSegLinetoHorizontalRelImpl::getValueProperty(ExecState *, int token) const
{
    switch(token)
    {
        case X:
			return Number(x());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegLinetoHorizontalRelImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case X:
			m_x = value.toNumber(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
