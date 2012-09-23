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

#ifndef SVGPathSegLinetoHorizontalImpl_H
#define SVGPathSegLinetoHorizontalImpl_H

#include "ksvg_lookup.h"

#include "SVGPathSegImpl.h"

namespace KSVG
{

class SVGPathSegLinetoHorizontalAbsImpl : public SVGPathSegImpl
{
public:
	SVGPathSegLinetoHorizontalAbsImpl();
	virtual ~SVGPathSegLinetoHorizontalAbsImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_LINETO_HORIZONTAL_ABS; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "H"; }
	virtual QString toString() const { return QString("H %1").arg(m_x); }

	void setX(const double &);
	double x() const;
						 
	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_x;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		X
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGPathSegLinetoHorizontalRelImpl : public SVGPathSegImpl
{
public:
	SVGPathSegLinetoHorizontalRelImpl();
	virtual ~SVGPathSegLinetoHorizontalRelImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_LINETO_HORIZONTAL_REL; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "h"; }
	virtual QString toString() const { return QString("h %1").arg(m_x); }

	void setX(const double &);
	double x() const;

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_x;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		X
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

#endif

// vim:ts=4:noet
