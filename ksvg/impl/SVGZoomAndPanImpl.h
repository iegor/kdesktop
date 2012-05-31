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

#ifndef SVGZoomAndPanImpl_H
#define SVGZoomAndPanImpl_H

#include <dom/dom_misc.h>
#include <dom/dom_string.h>

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGZoomAndPanImpl
{
public:
	SVGZoomAndPanImpl();
	virtual ~SVGZoomAndPanImpl();

	void setZoomAndPan(unsigned short zoomAndPan);
	unsigned short zoomAndPan() const;

	void parseZoomAndPan(const DOM::DOMString &attr);

private:
	unsigned short m_zoomAndPan;

public:
	KSVG_BASECLASS_GET
	KSVG_PUT

	enum
	{
		// Properties
		ZoomAndPan
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGZoomAndPanImplConstructor : public KJS::ObjectImp
{
public:
	SVGZoomAndPanImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;

	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGZoomAndPanImplConstructor(KJS::ExecState *exec);

}

#endif

// vim:ts=4:noet
