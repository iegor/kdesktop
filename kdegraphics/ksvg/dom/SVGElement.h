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

#ifndef SVGElement_H
#define SVGElement_H

#include <dom/dom_string.h>
#include <dom/dom_element.h>

namespace KSVG
{

// Not part of the DOM specification. Internal use only!
#define dom_cast(Class, Obj) reinterpret_cast<Class##Impl *>(Obj.handle())

template<class A, class B>
class SVGSafeCreator
{
public:
	SVGSafeCreator() { }
	~SVGSafeCreator() { }

	static A create(B *impl)
	{
		if(!impl) return A(); // Don't use A(0) it'll crash!
		return A(impl);
	}			
};

class SVGSVGElement;
class SVGElementImpl;
class SVGElement : public DOM::Element
{
public:
	SVGElement();
	SVGElement(const SVGElement &other);
	SVGElement &operator=(const SVGElement &other);
	SVGElement(SVGElementImpl *other);
	~SVGElement();

	void setId(DOM::DOMString);
	DOM::DOMString id();

	void setXmlbase(DOM::DOMString);
	DOM::DOMString xmlbase();

	SVGSVGElement ownerSVGElement();
	SVGElement viewportElement();

	// We need to overwrite set/get/hasAttribute(s)
	void setAttribute(const DOM::DOMString &name, const DOM::DOMString &value);
	DOM::DOMString getAttribute(const DOM::DOMString &name);
	bool hasAttribute(const DOM::DOMString &name);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGElementImpl *handle() const { return impl; }

private:
	SVGElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
