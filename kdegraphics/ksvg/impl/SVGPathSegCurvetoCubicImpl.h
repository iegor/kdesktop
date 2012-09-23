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

#ifndef SVGPathSegCurvetoCubicImpl_H
#define SVGPathSegCurvetoCubicImpl_H

#include "ksvg_lookup.h"

#include "SVGPathSegImpl.h"

namespace KSVG
{

class SVGPathSegCurvetoCubicAbsImpl : public SVGPathSegImpl 
{ 
public:
	SVGPathSegCurvetoCubicAbsImpl();
	virtual ~SVGPathSegCurvetoCubicAbsImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_CURVETO_CUBIC_ABS; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "C"; }
	virtual QString toString() const { return QString("C %1 %2 %3 %4 %5 %6").arg(m_x1).arg(m_y1).arg(m_x2).arg(m_y2).arg(m_x).arg(m_y); }

	void setX(const double &);
	double x() const;

	void setY(const double &);
	double y() const;

	void setX1(const double &);
	double x1() const;

	void setY1(const double &);
	double y1() const;

	void setX2(const double &);
	double x2() const;

	void setY2(const double &);
	double y2() const;

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_x;
	double m_y;
	double m_x1;
	double m_y1;
	double m_x2;
	double m_y2;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		X, Y, X1, Y1, X2, Y2
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGPathSegCurvetoCubicRelImpl : public SVGPathSegImpl 
{ 
public:
	SVGPathSegCurvetoCubicRelImpl();
	virtual ~SVGPathSegCurvetoCubicRelImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_CURVETO_CUBIC_REL; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "c"; }
	virtual QString toString() const { return QString("c %1 %2 %3 %4 %5 %6").arg(m_x1).arg(m_y1).arg(m_x2).arg(m_y2).arg(m_x).arg(m_y); }

	void setX(const double &);
	double x() const;

	void setY(const double &);
	double y() const;

	void setX1(const double &);
	double x1() const;

	void setY1(const double &);
	double y1() const;

	void setX2(const double &);
	double x2() const;

	void setY2(const double &);
	double y2() const;

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_x;
	double m_y;
	double m_x1;
	double m_y1;
	double m_x2;
	double m_y2;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		X, Y, X1, Y1, X2, Y2
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

#endif

// vim:ts=4:noet
