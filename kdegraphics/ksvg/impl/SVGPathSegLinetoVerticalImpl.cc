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

#include "SVGPathSegLinetoVerticalImpl.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

#include "SVGPathSegLinetoVerticalImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGPathSegLinetoVerticalAbsImpl::SVGPathSegLinetoVerticalAbsImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegLinetoVerticalAbsImpl::~SVGPathSegLinetoVerticalAbsImpl()
{
}

void SVGPathSegLinetoVerticalAbsImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegLinetoVerticalAbsImpl::y() const
{
	return m_y;
}

void SVGPathSegLinetoVerticalAbsImpl::getDeltasAndSlopes(double curx, double cury, double *pdx, double *pdy, double *pstartSlope, double *pendSlope) const
{
	Q_UNUSED(curx);
	double dx = 0;
	double dy = y() - cury;
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
@begin SVGPathSegLinetoVerticalAbsImpl::s_hashTable 2
 y		SVGPathSegLinetoVerticalAbsImpl::Y		DontDelete
@end
*/

Value SVGPathSegLinetoVerticalAbsImpl::getValueProperty(ExecState *, int token) const
{
    switch(token)
    {
        case Y:
			return Number(y());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegLinetoVerticalAbsImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case Y:
			m_y = value.toNumber(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}



SVGPathSegLinetoVerticalRelImpl::SVGPathSegLinetoVerticalRelImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegLinetoVerticalRelImpl::~SVGPathSegLinetoVerticalRelImpl()
{
}

void SVGPathSegLinetoVerticalRelImpl::setY(const double &y)
{
	m_y = y;
}

double SVGPathSegLinetoVerticalRelImpl::y() const
{
	return m_y;
}

void SVGPathSegLinetoVerticalRelImpl::getDeltasAndSlopes(double curx, double cury, double *pdx, double *pdy, double *pstartSlope, double *pendSlope) const
{
	Q_UNUSED(curx);
	Q_UNUSED(cury);
	double dx = 0;
	double dy = y();
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
@begin SVGPathSegLinetoVerticalRelImpl::s_hashTable 2
 y		SVGPathSegLinetoVerticalRelImpl::Y		DontDelete
@end
*/

Value SVGPathSegLinetoVerticalRelImpl::getValueProperty(ExecState *, int token) const
{
    switch(token)
    {
        case Y:
			return Number(y());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegLinetoVerticalRelImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case Y:
			m_y = value.toNumber(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
