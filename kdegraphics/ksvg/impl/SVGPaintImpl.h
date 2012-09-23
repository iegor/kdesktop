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

#ifndef SVGPaintImpl_H
#define SVGPaintImpl_H

#include <dom/dom_string.h>

#include "ksvg_lookup.h"

#include "SVGColorImpl.h"

namespace KSVG
{

class SVGPaintImpl : public SVGColorImpl 
{
public:
	SVGPaintImpl(SVGElementImpl *object);
	SVGPaintImpl(const SVGPaintImpl &);
	virtual ~SVGPaintImpl();

	SVGPaintImpl &operator=(const SVGPaintImpl &);

	unsigned short paintType() const;
	DOM::DOMString uri() const;
	void setUri(const DOM::DOMString &uri);
	void setPaint(unsigned short paintType, const DOM::DOMString &uri = "", const DOM::DOMString &rgbColor = "", const DOM::DOMString &iccColor = "");

	virtual void setRGBColor(const DOM::DOMString &rgbColor);
	virtual void setRGBColor(int r, int g, int b);
	virtual void setRGBColor(QColor color);
	virtual void setRGBColorICCColor(const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor);
	virtual void setColor(unsigned short colorType, const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor);

private:
	unsigned short m_paintType;
	DOM::DOMString m_uri;
	
public:
	KSVG_GET

	enum
	{
		// Properties
		PaintType, URI
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
};

class SVGPaintImplConstructor : public KJS::ObjectImp
{
public:
	SVGPaintImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;

	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGPaintImplConstructor(KJS::ExecState *exec);

}

#endif

// vim:ts=4:noet
