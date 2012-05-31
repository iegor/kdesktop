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

#ifndef SVGColorImpl_H
#define SVGColorImpl_H

#include <dom/dom_misc.h>
#include <dom/css_value.h>
#include <dom/dom_string.h>

#include "ksvg_lookup.h"

namespace KSVG
{
class SVGElementImpl;
class SVGICCColorImpl;
class SVGColorImpl : public DOM::DomShared
{
public:
	SVGColorImpl(SVGElementImpl *object);
	SVGColorImpl(const SVGColorImpl &);
	virtual ~SVGColorImpl();

	SVGColorImpl &operator=(const SVGColorImpl &);

	unsigned short colorType() const;

	DOM::RGBColor rgbColor() const;
	SVGICCColorImpl *iccColor() const;

	virtual void setRGBColor(const DOM::DOMString &rgbColor);
	virtual void setRGBColor(int r, int g, int b);
	virtual void setRGBColor(QColor color);
	virtual void setRGBColorICCColor(const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor);
	virtual void setColor(unsigned short colorType, const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor);

private:
	unsigned short m_colorType;
	DOM::RGBColor m_rgbColor;
	SVGICCColorImpl *m_iccColor;
	SVGElementImpl *m_object;

public:
	KSVG_BASECLASS_GET

	enum
	{
		// Properties
		ColorType, RGBColor, ICCColor,
		// Functions
		SetRGBColor, SetRGBColorICCColor, SetColor
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGColorImplConstructor : public KJS::ObjectImp
{
public:
	SVGColorImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;
	
	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGColorImplConstructor(KJS::ExecState *exec);

}

KSVG_DEFINE_PROTOTYPE(SVGColorImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGColorImplProtoFunc, SVGColorImpl)

#endif

// vim:ts=4:noet
