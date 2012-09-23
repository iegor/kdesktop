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

#ifndef SVGColorProfileElement_H
#define SVGColorProfileElement_H

#include "SVGElement.h"
#include "SVGURIReference.h"
#include <dom/dom_string.h>

namespace KSVG
{

class SVGColorProfileElementImpl;
class SVGColorProfileElement : public SVGElement,
							   public SVGURIReference
{ 
public:
	SVGColorProfileElement();
	SVGColorProfileElement(const SVGColorProfileElement &other);
	SVGColorProfileElement &operator=(const SVGColorProfileElement &other);
	SVGColorProfileElement(SVGColorProfileElementImpl *other);
	virtual ~SVGColorProfileElement();

	void setLocal(const DOM::DOMString &local);
	DOM::DOMString local() const;

	void setName(const DOM::DOMString &name);
	DOM::DOMString name() const;

	void setRenderingIntent(unsigned short renderingIntent);
	unsigned short renderingIntent() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGColorProfileElementImpl *handle() const { return impl; }		

private:
	SVGColorProfileElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
