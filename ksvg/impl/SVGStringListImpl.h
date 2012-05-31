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

#ifndef SVGStringListImpl_H
#define SVGStringListImpl_H

#include <dom/dom_string.h>

#include "SVGList.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SharedString : public DOM::DomShared,
					 public DOM::DOMString
{
public:
	SharedString() : DOM::DomShared(), DOM::DOMString() { }
	SharedString(const QString &string) : DOM::DomShared(), DOM::DOMString(string) { }
	SharedString(const SharedString &string) : DOM::DomShared(), DOM::DOMString(string) { }
	SharedString(DOM::DOMString *string) : DOM::DomShared(), DOM::DOMString(string->implementation()) { }
	virtual ~SharedString() { }

public:
	KSVG_GET

	enum
	{
		// Properties
		Dummy
	};

	KJS::Value getValueProperty(KJS::ExecState *, int token) const
	{
		switch(token)
		{
			case Dummy:
				return KJS::Undefined();
			default:
				kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
				return KJS::Undefined();
		}
	}
};
	
class SVGStringListImpl : public SVGList<SharedString>
{
public:
	KSVG_GET

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;

	QString join(const QString &seperator) const;
};

}

KSVG_DEFINE_PROTOTYPE(SVGStringListImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGStringListImplProtoFunc, SVGStringListImpl)	

#endif

// vim:ts=4:noet
