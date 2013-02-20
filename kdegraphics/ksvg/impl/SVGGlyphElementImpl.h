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

#ifndef SVGGlyphElementImpl_H
#define SVGGlyphElementImpl_H

#include "ksvg_lookup.h"

#include "SVGElementImpl.h"
#include "SVGStylableImpl.h"

namespace KSVG
{

class SVGGlyphElementImpl : public SVGElementImpl,
							public SVGStylableImpl
{
public:
	SVGGlyphElementImpl(DOM::ElementImpl *);
	virtual ~SVGGlyphElementImpl();
	
	QString d() const;
	
private:
	QString m_d;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		Unicode, GlyphName, D, Orientation, ArabicForm,
		Lang, HorizAdvX, VertOriginX, VertOriginY, VertAdvY
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGGlyphElementImpl, "glyph")

}

#endif

// vim:ts=4:noet
