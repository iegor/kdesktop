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

#ifndef SVGStyleElementImpl_H
#define SVGStyleElementImpl_H

#include <dom/dom_string.h>

#include "SVGElementImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGStyleElementImpl : public SVGElementImpl
{
public:
	SVGStyleElementImpl(DOM::ElementImpl *);
	virtual ~SVGStyleElementImpl();

	void setXmlspace(const DOM::DOMString &xmlspace);
	DOM::DOMString xmlspace() const;

	void setType(const DOM::DOMString &type);
	DOM::DOMString type() const;

	void setMedia(const DOM::DOMString &media);
	DOM::DOMString media() const;

	void setTitle(const DOM::DOMString &title);
	DOM::DOMString title() const;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		Xmlspace, Type, Media, Title
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGStyleElementImpl, "style")

}

#endif

// vim:ts=4:noet
