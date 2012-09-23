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

#ifndef SVGDescElementImpl_H
#define SVGDescElementImpl_H

#include "ksvg_lookup.h"

#include "SVGElementImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"

namespace KSVG
{

class SVGDescElementImpl : public SVGElementImpl,
						   public SVGLangSpaceImpl,
						   public SVGStylableImpl
{
public:
	SVGDescElementImpl(DOM::ElementImpl *impl);
	virtual ~SVGDescElementImpl();

	virtual void createItem(KSVGCanvas *c = 0);

public:
	KSVG_BRIDGE
	KSVG_FORWARDGET
	KSVG_FORWARDPUT
};

KSVG_REGISTER_ELEMENT(SVGDescElementImpl, "desc")

}

#endif

// vim:ts=4:noet
