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

#ifndef SVGZoomEventImpl_H
#define SVGZoomEventImpl_H

#include "SVGEventImpl.h"
#include "ksvg_lookup.h"

namespace KSVG
{

class SVGRectImpl;
class SVGPointImpl;
class SVGZoomEventImpl : public SVGUIEventImpl
{
public:
	SVGZoomEventImpl(SVGEvent::EventId _id,
					 bool canBubbleArg,
					 bool cancelableArg,
					 DOM::AbstractView &viewArg,
					 long detailArg,
					 float previousScale, SVGPointImpl *previousTranslate,
					 float newScale, SVGPointImpl *newTranslate);
	virtual ~SVGZoomEventImpl();

	SVGRectImpl *zoomRectScreen() const;
	
	float previousScale() const;
	SVGPointImpl *previousTranslate() const;
	
	float newScale() const;
	SVGPointImpl *newTranslate() const;

private:
	SVGRectImpl *m_zoomRectScreen;

	float m_previousScale;	
	SVGPointImpl *m_previousTranslate;
	
	float m_newScale;
	SVGPointImpl *m_newTranslate;

public:
	KSVG_GET

	enum
	{
		// Properties
		ZoomRectScreen, PreviousScale, PreviousTranslate, NewScale, NewTranslate
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
};

}

#endif

// vim:ts=4:noet
