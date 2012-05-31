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
#include "SVGRectImpl.h"
#include "SVGPointImpl.h"
#include "SVGZoomEventImpl.h"

using namespace KSVG;

#include "SVGZoomEventImpl.lut.h"
#include "SVGSVGElementImpl.h"

using namespace KJS;

SVGZoomEventImpl::SVGZoomEventImpl(SVGEvent::EventId _id,
									bool canBubbleArg,
									bool cancelableArg,
									DOM::AbstractView &viewArg,
									long detailArg,
									float previousScale, SVGPointImpl *previousTranslate,
									float newScale, SVGPointImpl *newTranslate)
: SVGUIEventImpl(_id, canBubbleArg, cancelableArg, viewArg, detailArg), m_previousScale( previousScale ), m_newScale( newScale )
{
	m_zoomRectScreen = SVGSVGElementImpl::createSVGRect();
	m_previousTranslate = previousTranslate;
	if(m_previousTranslate)
		m_previousTranslate->ref();
	m_newTranslate = newTranslate;
	if(m_newTranslate)
		m_newTranslate->ref();
}

SVGZoomEventImpl::~SVGZoomEventImpl()
{
	if(m_zoomRectScreen)
		m_zoomRectScreen->deref();
	if(m_previousTranslate)
		m_previousTranslate->deref();
	if(m_newTranslate)
		m_newTranslate->deref();
}

SVGRectImpl *SVGZoomEventImpl::zoomRectScreen() const
{
	return m_zoomRectScreen;
}

float SVGZoomEventImpl::previousScale() const
{
	return m_previousScale;
}

SVGPointImpl *SVGZoomEventImpl::previousTranslate() const
{
	return m_previousTranslate;
}

float SVGZoomEventImpl::newScale() const
{
	return m_newScale;
}

SVGPointImpl *SVGZoomEventImpl::newTranslate() const
{
	return m_newTranslate;
}

/*
@namespace KSVG
@begin SVGZoomEventImpl::s_hashTable 7
 zoomRectScreen			SVGZoomEventImpl::ZoomRectScreen		DontDelete|ReadOnly
 previousScale			SVGZoomEventImpl::PreviousScale			DontDelete|ReadOnly
 previousTranslate		SVGZoomEventImpl::PreviousTranslate		DontDelete|ReadOnly
 newScale				SVGZoomEventImpl::NewScale				DontDelete|ReadOnly
 newTranslate			SVGZoomEventImpl::NewTranslate			DontDelete|ReadOnly
@end
*/

Value SVGZoomEventImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case PreviousScale:
			return Number(previousScale());
		case NewScale:
			return Number(newScale());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

// vim:ts=4:noet
