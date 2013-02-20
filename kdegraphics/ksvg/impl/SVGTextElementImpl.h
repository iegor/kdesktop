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

#ifndef SVGTextElementImpl_H
#define SVGTextElementImpl_H

#include "ksvg_lookup.h"

#include "SVGTransformableImpl.h"
#include "SVGTextPositioningElementImpl.h"

namespace KSVG
{

class SVGRectImpl;
class SVGTextElementImpl : public SVGTextPositioningElementImpl,
						   public SVGTransformableImpl
{
public:
	SVGTextElementImpl(DOM::ElementImpl *);
	virtual ~SVGTextElementImpl();

	QString text();

	virtual long getNumberOfChars();

	virtual void createItem(KSVGCanvas *c = 0);

	virtual SVGRectImpl *getBBox();

	virtual bool prepareMouseEvent(const QPoint &p, const QPoint &a, SVGMouseEventImpl *mev);

private:
	int m_bboxX, m_bboxY, m_bboxWidth, m_bboxHeight;

public:
	KSVG_BRIDGE
	KSVG_FORWARDGET
	KSVG_FORWARDPUT
};

KSVG_REGISTER_ELEMENT(SVGTextElementImpl, "text")

}

#endif

// vim:ts=4:noet
