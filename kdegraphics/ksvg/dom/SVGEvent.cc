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

#include "SVGEvent.h"
#include "SVGEventImpl.h"

using namespace KSVG;

SVGEvent::SVGEvent() : DOM::Event()
{
	impl = 0;
}

SVGEvent::SVGEvent(const SVGEvent &other) : DOM::Event(other), impl(0)
{
	(*this) = other;
}

SVGEvent &SVGEvent::operator =(const SVGEvent &other)
{
	DOM::Event::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGEvent::SVGEvent(SVGEventImpl *other) : DOM::Event()
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGEvent::~SVGEvent()
{
	if(impl)
		impl->deref();
}

SVGEvent::EventId SVGEvent::typeToId(DOM::DOMString type)
{
	if(type == "DOMFocusIn")
		return DOMFOCUSIN_EVENT;
	else if(type == "DOMFocusOut")
		return DOMFOCUSOUT_EVENT;
	else if(type == "DOMActivate")
		return DOMACTIVATE_EVENT;
	else if(type == "click")
		return CLICK_EVENT;
	else if(type == "mousedown")
		return MOUSEDOWN_EVENT;
	else if(type == "mouseup")
		return MOUSEUP_EVENT;
	else if(type == "mouseover")
		return MOUSEOVER_EVENT;
	else if(type == "mousemove")
		return MOUSEMOVE_EVENT;
	else if(type == "mouseout")
		return MOUSEOUT_EVENT;
	else if(type == "DOMSubtreeModified")
		return DOMSUBTREEMODIFIED_EVENT;
	else if(type == "DOMNodeInserted")
		return DOMNODEINSERTED_EVENT;
	else if(type == "DOMNodeRemoved")
		return DOMNODEREMOVED_EVENT;
	else if(type == "DOMNodeRemovedFromDocument")
		return DOMNODEREMOVEDFROMDOCUMENT_EVENT;
	else if(type == "DOMNodeInsertedIntoDocument")
		return DOMNODEINSERTEDINTODOCUMENT_EVENT;
	else if(type == "DOMAttrModified")
		return DOMATTRMODIFIED_EVENT;
	else if(type == "DOMCharacterDataModified")
		return DOMCHARACTERDATAMODIFIED_EVENT;
	else if(type == "load")
		return LOAD_EVENT;
	else if(type == "unload")
		return UNLOAD_EVENT;
	else if(type == "abort")
		return ABORT_EVENT;
	else if(type == "error")
		return ERROR_EVENT;
	else if(type == "resize")
		return RESIZE_EVENT;
	else if(type == "scroll")
		return SCROLL_EVENT;
	else if(type == "zoom")
		return ZOOM_EVENT;
 	else if(type == "keydown")
 		return KEYDOWN_EVENT;	
 	else if(type == "keyup")
 		return KEYUP_EVENT;	
 	else if(type == "keypress")
 		return KEYPRESS_EVENT;	

	return UNKNOWN_EVENT;
}

DOM::DOMString SVGEvent::idToType(EventId id)
{
	switch(id)
	{
		case DOMFOCUSIN_EVENT:
			return "DOMFocusIn";
		case DOMFOCUSOUT_EVENT:
			return "DOMFocusOut";
		case DOMACTIVATE_EVENT:
			return "DOMActivate";
		case CLICK_EVENT:
			return "click";
		case MOUSEDOWN_EVENT:
			return "mousedown";
		case MOUSEUP_EVENT:
			return "mouseup";
		case MOUSEOVER_EVENT:
			return "mouseover";
		case MOUSEMOVE_EVENT:
			return "mousemove";
		case MOUSEOUT_EVENT:
			return "mouseout";
		case DOMSUBTREEMODIFIED_EVENT:
			return "DOMSubtreeModified";
		case DOMNODEINSERTED_EVENT:
			return "DOMNodeInserted";
		case DOMNODEREMOVED_EVENT:
			return "DOMNodeRemoved";
		case DOMNODEREMOVEDFROMDOCUMENT_EVENT:
			return "DOMNodeRemovedFromDocument";
		case DOMNODEINSERTEDINTODOCUMENT_EVENT:
			return "DOMNodeInsertedIntoDocument";
		case DOMATTRMODIFIED_EVENT:
			return "DOMAttrModified";
		case DOMCHARACTERDATAMODIFIED_EVENT:
			return "DOMCharacterDataModified";
		case LOAD_EVENT:
			return "load";
		case UNLOAD_EVENT:
			return "unload";
		case ABORT_EVENT:
			return "abort";
		case ERROR_EVENT:
			return "error";
		case RESIZE_EVENT:
			return "resize";
		case SCROLL_EVENT:
			return "scroll";
		case ZOOM_EVENT:
			return "zoom";
		case KEYDOWN_EVENT:
			return "keydown";
		case KEYUP_EVENT:
			return "keyup";
		case KEYPRESS_EVENT:
			return "keypress";
		default:
			return DOM::DOMString();
			break;
	}
}

// vim:ts=4:noet
