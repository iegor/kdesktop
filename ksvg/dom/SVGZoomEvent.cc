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

#include "SVGZoomEvent.h"
#include "SVGZoomEventImpl.h"
#include "SVGRect.h"
#include "SVGPoint.h"

using namespace KSVG;

SVGZoomEvent::SVGZoomEvent() : DOM::UIEvent()
{
	impl = 0;
}

SVGZoomEvent::SVGZoomEvent(const SVGZoomEvent &other) : DOM::UIEvent(other), impl(0)
{
	(*this) = other;
}

SVGZoomEvent &SVGZoomEvent::operator =(const SVGZoomEvent &other)
{
	DOM::UIEvent::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGZoomEvent::SVGZoomEvent(SVGZoomEventImpl *other) : DOM::UIEvent()
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGZoomEvent::~SVGZoomEvent()
{
	if(impl)
		impl->deref();
}

SVGRect SVGZoomEvent::zoomRectScreen() const
{
	if(!impl) return SVGRect(0);
	return SVGRect(impl->zoomRectScreen());
}

float SVGZoomEvent::previousScale() const
{
	if(!impl) return -1;
	return impl->previousScale();
}

SVGPoint SVGZoomEvent::previousTranslate() const
{
	if(!impl) return SVGPoint(0);
	return SVGPoint(impl->previousTranslate());
}

float SVGZoomEvent::newScale() const
{
	if(!impl) return -1;
	return impl->newScale();
}

SVGPoint SVGZoomEvent::newTranslate() const
{
	if(!impl) return SVGPoint(0);
	return SVGPoint(impl->newTranslate());
}

// vim:ts=4:noet
