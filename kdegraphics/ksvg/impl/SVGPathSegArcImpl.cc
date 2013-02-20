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

#include <math.h>

#include <kdebug.h>

#include "SVGPathSegArcImpl.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

#include "SVGPathSegArcImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

static void getArcSlopes(bool relative, double curx, double cury, double angle, double x, double y, double r1, double r2, bool largeArcFlag, bool sweepFlag, double *pStartSlope, double *pEndSlope)
{
	double sin_th, cos_th;
	double a00, a01, a10, a11;
	double x0, y0, x1, y1, xc, yc;
	double d, sfactor, sfactor_sq;
	double th0, th1, th_arc;
	int i, n_segs;

	sin_th = sin(angle * (M_PI / 180.0));
	cos_th = cos(angle * (M_PI / 180.0));

	double dx;

	if(!relative)
		dx = (curx - x) / 2.0;
	else
		dx = -x / 2.0;

	double dy;
		
	if(!relative)
		dy = (cury - y) / 2.0;
	else
		dy = -y / 2.0;
		
	double _x1 =  cos_th * dx + sin_th * dy;
	double _y1 = -sin_th * dx + cos_th * dy;
	double Pr1 = r1 * r1;
	double Pr2 = r2 * r2;
	double Px = _x1 * _x1;
	double Py = _y1 * _y1;

	// Spec : check if radii are large enough
	double check = Px / Pr1 + Py / Pr2;
	if(check > 1)
	{
		r1 = r1 * sqrt(check);
		r2 = r2 * sqrt(check);
	}

	a00 = cos_th / r1;
	a01 = sin_th / r1;
	a10 = -sin_th / r2;
	a11 = cos_th / r2;

	x0 = a00 * curx + a01 * cury;
	y0 = a10 * curx + a11 * cury;

	if(!relative)
		x1 = a00 * x + a01 * y;
	else
		x1 = a00 * (curx + x) + a01 * (cury + y);
		
	if(!relative)
		y1 = a10 * x + a11 * y;
	else
		y1 = a10 * (curx + x) + a11 * (cury + y);

	/* (x0, y0) is current point in transformed coordinate space.
	   (x1, y1) is new point in transformed coordinate space.

	   The arc fits a unit-radius circle in this space.
     */

	d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);

	sfactor_sq = 1.0 / d - 0.25;

	if(sfactor_sq < 0)
		sfactor_sq = 0;

	sfactor = sqrt(sfactor_sq);

	if(sweepFlag == largeArcFlag)
		sfactor = -sfactor;

	xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
	yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);

	/* (xc, yc) is center of the circle. */
	th0 = atan2(y0 - yc, x0 - xc);
	th1 = atan2(y1 - yc, x1 - xc);

	th_arc = th1 - th0;
	if(th_arc < 0 && sweepFlag)
		th_arc += 2 * M_PI;
	else if(th_arc > 0 && !sweepFlag)
		th_arc -= 2 * M_PI;

	n_segs = (int) (int) ceil(fabs(th_arc / (M_PI * 0.5 + 0.001)));

	for(int step = 0; step < 2; step++)
	{
		i = step == 0 ? 0 : n_segs - 1;

		double sin_th, cos_th;
		double a00, a01, a10, a11;
		double x1, y1, x2, y2, x3, y3;
		double t;
		double th_half;

		double _th0 = th0 + i * th_arc / n_segs;
		double _th1 = th0 + (i + 1) * th_arc / n_segs;

		sin_th = sin(angle * (M_PI / 180.0));
		cos_th = cos(angle * (M_PI / 180.0));

		/* inverse transform compared with rsvg_path_arc */
		a00 = cos_th * r1;
		a01 = -sin_th * r2;
		a10 = sin_th * r1;
		a11 = cos_th * r2;

		th_half = 0.5 * (_th1 - _th0);
		t = (8.0 / 3.0) * sin(th_half * 0.5) * sin(th_half * 0.5) / sin(th_half);
		x1 = xc + cos(_th0) - t * sin(_th0);
		y1 = yc + sin(_th0) + t * cos(_th0);
		x3 = xc + cos(_th1);
		y3 = yc + sin(_th1);
		x2 = x3 + t * sin(_th1);
		y2 = y3 - t * cos(_th1);

		double bezX1 = a00 * x1 + a01 * y1;
		double bezY1 = a10 * x1 + a11 * y1;
		double bezX2 = a00 * x2 + a01 * y2;
		double bezY2 = a10 * x2 + a11 * y2;
		double bezX = a00 * x3 + a01 * y3;
		double bezY = a10 * x3 + a11 * y3;

		if(step == 0)
			*pStartSlope = SVGAngleImpl::todeg(atan2(bezY1 - cury, bezX1 - curx));
		else
			*pEndSlope = SVGAngleImpl::todeg(atan2(bezY - bezY2, bezX - bezX2));
	}
}

SVGPathSegArcAbsImpl::SVGPathSegArcAbsImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegArcAbsImpl::~SVGPathSegArcAbsImpl()
{
}

void SVGPathSegArcAbsImpl::setX(double x)
{
	m_x = x;
}

double SVGPathSegArcAbsImpl::x() const
{
	return m_x;
}

void SVGPathSegArcAbsImpl::setY(double y)
{
	m_y = y;
}

double SVGPathSegArcAbsImpl::y() const
{
	return m_y;
}

void SVGPathSegArcAbsImpl::setR1(double r1)
{
	m_r1 = r1;
}

double SVGPathSegArcAbsImpl::r1() const
{
	return m_r1;
}

void SVGPathSegArcAbsImpl::setR2(double r2)
{
	m_r2 = r2;
}

double SVGPathSegArcAbsImpl::r2() const
{
	return m_r2;
}

void SVGPathSegArcAbsImpl::setAngle(double angle)
{
	m_angle = angle;
}

double SVGPathSegArcAbsImpl::angle() const
{
	return m_angle;
}

void SVGPathSegArcAbsImpl::setLargeArcFlag(bool largeArcFlag)
{
	m_largeArcFlag = largeArcFlag;
}

bool SVGPathSegArcAbsImpl::largeArcFlag() const
{
	return m_largeArcFlag;
}

void SVGPathSegArcAbsImpl::setSweepFlag(bool sweepFlag)
{
	m_sweepFlag = sweepFlag;
}

bool SVGPathSegArcAbsImpl::sweepFlag() const
{
	return m_sweepFlag;
}

void SVGPathSegArcAbsImpl::getDeltasAndSlopes(double curx, double cury, double *pDx, double *pDy, double *pStartSlope, double *pEndSlope) const
{
	double dx = x() - curx;
	double dy = y() - cury;
	double startSlope;
	double endSlope;
	getArcSlopes(false, curx, cury, angle(), x(), y(), r1(), r2(), largeArcFlag(), sweepFlag(), &startSlope, &endSlope);
	*pDx = dx;
	*pDy = dy;
	*pStartSlope = startSlope;
	*pEndSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegArcAbsImpl::s_hashTable 11
 x					SVGPathSegArcAbsImpl::X						DontDelete
 y					SVGPathSegArcAbsImpl::Y						DontDelete
 r1					SVGPathSegArcAbsImpl::R1					DontDelete
 r2					SVGPathSegArcAbsImpl::R2					DontDelete
 angle				SVGPathSegArcAbsImpl::Angle					DontDelete
 largeArcFlag		SVGPathSegArcAbsImpl::LargeArcFlag			DontDelete
 sweepFlag			SVGPathSegArcAbsImpl::SweepFlag				DontDelete
@end
*/

Value SVGPathSegArcAbsImpl::getValueProperty(ExecState *, int token) const
{
    switch(token)
    {
        case X:
			return Number(x());
        case Y:
			return Number(y());
        case R1:
			return Number(r1());
        case R2:
			return Number(r2());
        case Angle:
			return Number(angle());
		case LargeArcFlag:
			return Boolean(largeArcFlag());
		case SweepFlag:
			return Boolean(sweepFlag());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegArcAbsImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case X:
			m_x = value.toNumber(exec);
			break;
		case Y:
			m_y = value.toNumber(exec);
			break;
		case R1:
			m_r1 = value.toNumber(exec);
			break;
		case R2:
			m_r2 = value.toNumber(exec);
			break;
		case Angle:
			m_angle = value.toNumber(exec);
			break;
		case LargeArcFlag:
			m_largeArcFlag = value.toBoolean(exec);
			break;
		case SweepFlag:
			m_sweepFlag = value.toBoolean(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}



SVGPathSegArcRelImpl::SVGPathSegArcRelImpl() : SVGPathSegImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGPathSegArcRelImpl::~SVGPathSegArcRelImpl()
{
}

void SVGPathSegArcRelImpl::setX(double x)
{
	m_x = x;
}

double SVGPathSegArcRelImpl::x() const
{
	return m_x;
}

void SVGPathSegArcRelImpl::setY(double y)
{
	m_y = y;
}

double SVGPathSegArcRelImpl::y() const
{
	return m_y;
}

void SVGPathSegArcRelImpl::setR1(double r1)
{
	m_r1 = r1;
}

double SVGPathSegArcRelImpl::r1() const
{
	return m_r1;
}

void SVGPathSegArcRelImpl::setR2(double r2)
{
	m_r2 = r2;
}

double SVGPathSegArcRelImpl::r2() const
{
	return m_r2;
}

void SVGPathSegArcRelImpl::setAngle(double angle)
{
	m_angle = angle;
}

double SVGPathSegArcRelImpl::angle() const
{
	return m_angle;
}

void SVGPathSegArcRelImpl::setLargeArcFlag(bool largeArcFlag)
{
	m_largeArcFlag = largeArcFlag;
}

bool SVGPathSegArcRelImpl::largeArcFlag() const
{
	return m_largeArcFlag;
}

void SVGPathSegArcRelImpl::setSweepFlag(bool sweepFlag)
{
	m_sweepFlag = sweepFlag;
}

bool SVGPathSegArcRelImpl::sweepFlag() const
{
	return m_sweepFlag;
}

void SVGPathSegArcRelImpl::getDeltasAndSlopes(double curx, double cury, double *pDx, double *pDy, double *pStartSlope, double *pEndSlope) const
{
	double dx = x();
	double dy = y();
	double startSlope;
	double endSlope;
	getArcSlopes(true, curx, cury, angle(), x(), y(), r1(), r2(), largeArcFlag(), sweepFlag(), &startSlope, &endSlope);
	*pDx = dx;
	*pDy = dy;
	*pStartSlope = startSlope;
	*pEndSlope = endSlope;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegArcRelImpl::s_hashTable 11
 x				SVGPathSegArcRelImpl::X						DontDelete
 y				SVGPathSegArcRelImpl::Y						DontDelete
 r1				SVGPathSegArcRelImpl::R1					DontDelete
 r2				SVGPathSegArcRelImpl::R2					DontDelete
 angle			SVGPathSegArcRelImpl::Angle					DontDelete
 largeArcFlag	SVGPathSegArcRelImpl::LargeArcFlag			DontDelete
 sweepFlag		SVGPathSegArcRelImpl::SweepFlag				DontDelete
@end
*/

Value SVGPathSegArcRelImpl::getValueProperty(ExecState *, int token) const
{
    switch(token)
    {
        case X:
			return Number(x());
        case Y:
			return Number(y());
        case R1:
			return Number(r1());
        case R2:
			return Number(r2());
        case Angle:
			return Number(angle());
		case LargeArcFlag:
			return Boolean(largeArcFlag());
		case SweepFlag:
			return Boolean(sweepFlag());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGPathSegArcRelImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
	switch(token)
	{
		case X:
			m_x = value.toNumber(exec);
			break;
		case Y:
			m_y = value.toNumber(exec);
			break;
		case R1:
			m_r1 = value.toNumber(exec);
			break;
		case R2:
			m_r2 = value.toNumber(exec);
			break;
		case Angle:
			m_angle = value.toNumber(exec);
			break;
		case LargeArcFlag:
			m_largeArcFlag = value.toBoolean(exec);
			break;
		case SweepFlag:
			m_sweepFlag = value.toBoolean(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
