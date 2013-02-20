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

#ifndef SVGContainerImpl_H
#define SVGContainerImpl_H

#include "SVGShapeImpl.h"

namespace KSVG
{
enum CanvasItemUpdate;
class KSVGCanvas;

class SVGContainerImpl : public SVGShapeImpl
{
public:
	SVGContainerImpl(DOM::ElementImpl *);
	virtual ~SVGContainerImpl();

	virtual bool isContainer() const { return true; }

	virtual void createItem(KSVGCanvas *c = 0);
	virtual void removeItem(KSVGCanvas *c);

	virtual void update(CanvasItemUpdate reason, int param1, int param2);
	virtual void invalidate(KSVGCanvas *c, bool recalc);
	virtual void setReferenced(bool referenced);
	virtual void draw();

	virtual SVGRectImpl *getBBox();

public:
	KSVG_FORWARDGET
	KSVG_FORWARDPUT
};

}

#endif

// vim:ts=4:noet
