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

#ifndef SVGPathSegClosePathImpl_H
#define SVGPathSegClosePathImpl_H

#include "ksvg_lookup.h"

#include "SVGPathSegImpl.h"

namespace KSVG
{

class SVGPathSegClosePathImpl : public SVGPathSegImpl
{
public:
	SVGPathSegClosePathImpl();
	virtual ~SVGPathSegClosePathImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_CLOSEPATH; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return "Z"; }
	virtual QString toString() const { return "Z"; }

	void setX(double x) { m_x = x; }
	void setY(double y) { m_y = y; }

	double x() const { return m_x; }
	double y() const { return m_y; }

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

private:
	double m_x;
	double m_y;

public:
	KSVG_FORWARDGET
};

}

#endif

// vim:ts=4:noet
