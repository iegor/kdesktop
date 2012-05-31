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

#ifndef SVGEvent_H
#define SVGEvent_H

#include <dom/dom2_events.h>

namespace KSVG
{

class SVGEventImpl;
class SVGEvent : public DOM::Event
{
public:
	enum EventId
	{
		UNKNOWN_EVENT = 0,

		// UI events
		DOMFOCUSIN_EVENT,
		DOMFOCUSOUT_EVENT,
		DOMACTIVATE_EVENT,

		// Mouse events
		CLICK_EVENT,
		MOUSEDOWN_EVENT,
		MOUSEUP_EVENT,
		MOUSEOVER_EVENT,
		MOUSEMOVE_EVENT,
		MOUSEOUT_EVENT,

		// Mutation events
		DOMSUBTREEMODIFIED_EVENT,
		DOMNODEINSERTED_EVENT,
		DOMNODEREMOVED_EVENT,
		DOMNODEREMOVEDFROMDOCUMENT_EVENT,
		DOMNODEINSERTEDINTODOCUMENT_EVENT,
		DOMATTRMODIFIED_EVENT,
		DOMCHARACTERDATAMODIFIED_EVENT,

		// SVG events
		LOAD_EVENT,
		UNLOAD_EVENT,
		ABORT_EVENT,
		ERROR_EVENT,
		RESIZE_EVENT,
		SCROLL_EVENT,
		ZOOM_EVENT,

		// Key Events
		KEYDOWN_EVENT,
		KEYPRESS_EVENT,
		KEYUP_EVENT
	};

	SVGEvent();
	SVGEvent(const SVGEvent &other);
	SVGEvent &operator=(const SVGEvent &other);
	SVGEvent(SVGEventImpl *other);
	virtual ~SVGEvent();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGEventImpl *handle() const { return impl; }

	// Helper functions
	static EventId typeToId(DOM::DOMString type);
	static DOM::DOMString idToType(EventId id);

private:
	SVGEventImpl *impl;
};

}

#endif

// vim:ts=4:noet
