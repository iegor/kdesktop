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

#include "SVGStyleElementImpl.h"

using namespace KSVG;

#include "SVGStyleElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGStyleElementImpl::SVGStyleElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl)
{
	KSVG_EMPTY_FLAGS
}

SVGStyleElementImpl::~SVGStyleElementImpl()
{
}

void SVGStyleElementImpl::setXmlspace(const DOM::DOMString &xmlspace)
{
	setAttribute("xml:space", xmlspace);
}

DOM::DOMString SVGStyleElementImpl::xmlspace() const
{
	return getAttribute("xml:space");
}

void SVGStyleElementImpl::setType(const DOM::DOMString &type)
{
	setAttribute("type", type);
}

DOM::DOMString SVGStyleElementImpl::type() const
{
	return getAttribute("type");
}

void SVGStyleElementImpl::setMedia(const DOM::DOMString &media)
{
	setAttribute("media", media);
}

DOM::DOMString SVGStyleElementImpl::media() const
{
	return getAttribute("media");
}

void SVGStyleElementImpl::setTitle(const DOM::DOMString &title)
{
	setAttribute("title", title);
}

DOM::DOMString SVGStyleElementImpl::title() const
{
	return getAttribute("title");
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGStyleElementImpl::s_hashTable 5
 xmlspace		SVGStyleElementImpl::Xmlspace		DontDelete
 type   		SVGStyleElementImpl::Type			DontDelete
 media			SVGStyleElementImpl::Media			DontDelete
 title			SVGStyleElementImpl::Title			DontDelete
@end
*/

Value SVGStyleElementImpl::getValueProperty(ExecState *, int token) const
{
	//KSVG_CHECK_ATTRIBUTE

	switch(token)
	{
		case Xmlspace:
			return String(xmlspace());
		case Type:
			return String(type());
		case Media:
			return String(media());
		case Title:
			return String(title());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGStyleElementImpl::putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case Xmlspace:
			setXmlspace(value.toString(exec).string());
			break;
		case Type:
			setType(value.toString(exec).string());
			break;
		case Media:
			setMedia(value.toString(exec).string());
			break;
		case Title:
			setTitle(value.toString(exec).string());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			break;
	}
}

// vim:ts=4:noet
