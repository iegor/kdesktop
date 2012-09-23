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

#ifndef SVGGlyphRefElementImpl_H
#define SVGGlyphRefElementImpl_H

#include "ksvg_lookup.h"

#include "SVGElementImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGStylableImpl.h"

namespace KSVG
{

class SVGGlyphRefElementImpl :	public SVGElementImpl,
								public SVGURIReferenceImpl,
								public SVGStylableImpl
{
public:
	SVGGlyphRefElementImpl(DOM::ElementImpl *);
	virtual ~SVGGlyphRefElementImpl();
	virtual void setAttributes();

	DOM::DOMString glyphRef();
	DOM::DOMString format();

	float x() const;
	float y() const;
	float dx() const;
	float dy() const;

private:
	DOM::DOMString m_glyphRef;
	DOM::DOMString m_format;
	float m_x;
	float m_y;
	float m_dx;
	float m_dy;

public:
	KSVG_BRIDGE
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		GlyphRef, Format, X, Y, Dx, Dy
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGGlyphRefElementImpl, "glyphRef")

}

#endif

// vim:ts=4:noet
