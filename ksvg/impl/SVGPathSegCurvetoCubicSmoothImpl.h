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

#ifndef SVGPathSegCurvetoCubicSmoothImpl_H
#define SVGPathSegCurvetoCubicSmoothImpl_H

#include "ksvg_lookup.h"

#include "SVGPathSegImpl.h"

namespace KSVG
{

class SVGPathSegCurvetoCubicSmoothAbsImpl : public SVGPathSegImpl
{ 
public:
	SVGPathSegCurvetoCubicSmoothAbsImpl();
	virtual ~SVGPathSegCurvetoCubicSmoothAbsImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_CURVETO_CUBIC_SMOOTH_ABS; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "S"; }
	virtual QString toString() const { return QString("S %1 %2 %3 %4").arg(m_x2).arg(m_y2).arg(m_x).arg(m_y); }

	void setX(const double &);
	double x() const;

	void setY(const double &);
	double y() const;

	void setX2(const double &);
	double x2() const;

	void setY2(const double &);
	double y2() const;

	void setPreviousX2(double x2);
	void setPreviousY2(double y2);
	
	double x1(double curx) const;
	double y1(double cury) const;

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_x;
	double m_y;
	double m_x2;
	double m_y2;
	double m_previousX2;
	double m_previousY2;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		X, Y, X2, Y2
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGPathSegCurvetoCubicSmoothRelImpl : public SVGPathSegImpl
{ 
public:
	SVGPathSegCurvetoCubicSmoothRelImpl();
	virtual ~SVGPathSegCurvetoCubicSmoothRelImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_CURVETO_CUBIC_SMOOTH_REL; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "s"; }
	virtual QString toString() const { return QString("s %1 %2 %3 %4").arg(m_x2).arg(m_y2).arg(m_x).arg(m_y); }

	void setX(const double &);
	double x() const;

	void setY(const double &);
	double y() const;

	void setX2(const double &);
	double x2() const;

	void setY2(const double &);
	double y2() const;

	void setPreviousAbsX2(double x2);
	void setPreviousAbsY2(double y2);
	
	double absX1(double curx) const;
	double absY1(double cury) const;

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_x;
	double m_y;
	double m_x2;
	double m_y2;
	double m_previousAbsX2;
	double m_previousAbsY2;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		X, Y, X2, Y2
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

#endif

// vim:ts=4:noet
