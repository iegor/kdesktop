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

#ifndef SVGPathSegLinetoVerticalImpl_H
#define SVGPathSegLinetoVerticalImpl_H

#include "ksvg_lookup.h"

#include "SVGPathSegImpl.h"

namespace KSVG
{

class SVGPathSegLinetoVerticalAbsImpl : public SVGPathSegImpl
{
public:
	SVGPathSegLinetoVerticalAbsImpl();
	virtual~SVGPathSegLinetoVerticalAbsImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_LINETO_VERTICAL_ABS; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "V"; }
	virtual QString toString() const { return QString("V %1").arg(m_y); }

	void setY(const double &);
	double y() const;

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_y;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		Y
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGPathSegLinetoVerticalRelImpl : public SVGPathSegImpl
{
public:
	SVGPathSegLinetoVerticalRelImpl();
	virtual ~SVGPathSegLinetoVerticalRelImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_LINETO_VERTICAL_REL; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "v"; }
	virtual QString toString() const { return QString("v %1").arg(m_y); }

	void setY(const double &);
	double y() const;

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_y;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		Y
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

#endif

// vim:ts=4:noet
