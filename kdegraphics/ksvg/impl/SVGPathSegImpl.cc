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

#include "SVGPathSegImpl.h"

using namespace KSVG;

#include "SVGPathSegImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_cacheimpl.h"

SVGPathSegImpl::SVGPathSegImpl()
{
}

SVGPathSegImpl::~SVGPathSegImpl()
{
}

void SVGPathSegImpl::getDeltasAndSlopes(double, double, double *dx, double *dy, double *startSlope, double *endSlope) const
{
	*dx = 0;
	*dy = 0;
	*startSlope = 0;
	*endSlope = 0;
}

// Exma stuff

/*
@namespace KSVG
@begin SVGPathSegImpl::s_hashTable 3
 pathSegType				SVGPathSegImpl::PathSegType				DontDelete|ReadOnly
 pathSegTypeAsLetter		SVGPathSegImpl::PathSegTypeAsLetter		DontDelete|ReadOnly
@end
*/

Value SVGPathSegImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case PathSegType:
			return Number(pathSegType());
		case PathSegTypeAsLetter:
			return String(pathSegTypeAsLetter().string());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

/*
@namespace KSVG
@begin SVGPathSegImplConstructor::s_hashTable 23
 PATHSEG_UNKNOWN						KSVG::PATHSEG_UNKNOWN						DontDelete|ReadOnly
 PATHSEG_CLOSEPATH						KSVG::PATHSEG_CLOSEPATH						DontDelete|ReadOnly
 PATHSEG_MOVETO_ABS						KSVG::PATHSEG_MOVETO_ABS					DontDelete|ReadOnly
 PATHSEG_MOVETO_REL						KSVG::PATHSEG_MOVETO_REL					DontDelete|ReadOnly
 PATHSEG_LINETO_ABS						KSVG::PATHSEG_LINETO_ABS					DontDelete|ReadOnly
 PATHSEG_LINETO_REL						KSVG::PATHSEG_LINETO_REL					DontDelete|ReadOnly
 PATHSEG_CURVETO_CUBIC_ABS				KSVG::PATHSEG_CURVETO_CUBIC_ABS				DontDelete|ReadOnly
 PATHSEG_CURVETO_CUBIC_REL				KSVG::PATHSEG_CURVETO_CUBIC_REL				DontDelete|ReadOnly
 PATHSEG_CURVETO_QUADRATIC_ABS			KSVG::PATHSEG_CURVETO_QUADRATIC_ABS			DontDelete|ReadOnly
 PATHSEG_CURVETO_QUADRATIC_REL			KSVG::PATHSEG_CURVETO_QUADRATIC_REL			DontDelete|ReadOnly
 PATHSEG_ARC_ABS						KSVG::PATHSEG_ARC_ABS						DontDelete|ReadOnly
 PATHSEG_ARC_REL						KSVG::PATHSEG_ARC_REL						DontDelete|ReadOnly
 PATHSEG_LINETO_HORIZONTAL_ABS			KSVG::PATHSEG_LINETO_HORIZONTAL_ABS			DontDelete|ReadOnly
 PATHSEG_LINETO_HORIZONTAL_REL			KSVG::PATHSEG_LINETO_HORIZONTAL_REL			DontDelete|ReadOnly
 PATHSEG_LINETO_VERTICAL_ABS			KSVG::PATHSEG_LINETO_VERTICAL_ABS			DontDelete|ReadOnly
 PATHSEG_LINETO_VERTICAL_REL			KSVG::PATHSEG_LINETO_VERTICAL_REL			DontDelete|ReadOnly
 PATHSEG_CURVETO_CUBIC_SMOOTH_ABS		KSVG::PATHSEG_CURVETO_CUBIC_SMOOTH_ABS		DontDelete|ReadOnly
 PATHSEG_CURVETO_CUBIC_SMOOTH_REL		KSVG::PATHSEG_CURVETO_CUBIC_SMOOTH_REL		DontDelete|ReadOnly
 PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS	KSVG::PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS	DontDelete|ReadOnly
 PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL	KSVG::PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL	DontDelete|ReadOnly
@end
*/

Value SVGPathSegImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token);
}

Value KSVG::getSVGPathSegImplConstructor(ExecState *exec)
{
	return cacheGlobalBridge<SVGPathSegImplConstructor>(exec, "[[svgpathseg.constructor]]");
}

// vim:ts=4:noet
