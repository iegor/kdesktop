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

#ifndef SVGPathSegCurvetoQuadraticSmoothImpl_H
#define SVGPathSegCurvetoQuadraticSmoothImpl_H

#include "ksvg_lookup.h"

#include "SVGPathSegImpl.h"

namespace KSVG
{

class SVGPathSegCurvetoQuadraticSmoothAbsImpl : public SVGPathSegImpl
{ 
public:
	SVGPathSegCurvetoQuadraticSmoothAbsImpl();
	virtual ~SVGPathSegCurvetoQuadraticSmoothAbsImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "T"; }
	virtual QString toString() const { return QString("T %1 %2").arg(m_x).arg(m_y); }

	void setX(const double &);
	double x() const;

	void setY(const double &);
	double y() const;

	void setPreviousX1(double x1);
	void setPreviousY1(double y1);
	
	double x1(double curx) const;
	double y1(double cury) const;

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_x;
	double m_y;
	double m_previousX1;
	double m_previousY1;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		X, Y
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGPathSegCurvetoQuadraticSmoothRelImpl : public SVGPathSegImpl 
{ 
public:
	SVGPathSegCurvetoQuadraticSmoothRelImpl();
	virtual ~SVGPathSegCurvetoQuadraticSmoothRelImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "t"; }
	virtual QString toString() const { return QString("t %1 %2").arg(m_x).arg(m_y); }

	void setX(const double &);
	double x() const;

	void setY(const double &);
	double y() const;

	void setPreviousAbsX1(double x1);
	void setPreviousAbsY1(double y1);
	
	double absX1(double curx) const;
	double absY1(double cury) const;

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_x;
	double m_y;
	double m_previousAbsX1;
	double m_previousAbsY1;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		X, Y
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

#endif

// vim:ts=4:noet
