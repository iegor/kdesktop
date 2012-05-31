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

#ifndef SVGPathSegImpl_H
#define SVGPathSegImpl_H

#include <dom/dom_misc.h>

#include "ksvg_lookup.h"

#include "SVGPathSeg.h"

namespace KSVG
{

class SVGPathElementImpl;
class SVGPathSegImpl : public DOM::DomShared
{
public:
	SVGPathSegImpl();
	virtual ~SVGPathSegImpl();

	virtual unsigned short pathSegType() const { return PATHSEG_UNKNOWN; }
	virtual DOM::DOMString pathSegTypeAsLetter() const { return ""; }
	virtual QString toString() const { return ""; }

	virtual void getDeltasAndSlopes(double curx, double cury, double *dx, double *dy, double *startSlope, double *endSlope) const;

public:
	KSVG_BASECLASS_GET

	enum
	{
		// Properties
		PathSegType, PathSegTypeAsLetter
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;

};

class SVGPathSegImplConstructor : public KJS::ObjectImp
{
public:
	SVGPathSegImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;
	
	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGPathSegImplConstructor(KJS::ExecState *exec);

}

#endif

// vim:ts=4:noet
