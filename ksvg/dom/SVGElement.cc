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

#include "SVGSVGElement.h"
#include "SVGElement.h"
#include "SVGElementImpl.h"

using namespace KSVG;

// There is no way to create a SVGElement this way! Use SVGDocument.
SVGElement::SVGElement() : DOM::Element()
{
	impl = 0; // new SVGElementImpl(ownerDocument().createElement().handle());
}

SVGElement::SVGElement(const SVGElement &other) : DOM::Element(other), impl(0)
{
	(*this) = other;
}

SVGElement &SVGElement::operator=(const SVGElement &other) 
{
	// Baseclass assignement operators always first (Niko)
	DOM::Element::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGElement::SVGElement(SVGElementImpl *other) : DOM::Element(other->handle())
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGElement::~SVGElement()
{
	if(impl)
		impl->deref();
}

void SVGElement::setId(DOM::DOMString value)
{
	if(impl)
		impl->setId(value);
}

DOM::DOMString SVGElement::id()
{
	if(!impl) return DOM::DOMString();
	return impl->id();
}

void SVGElement::setXmlbase(DOM::DOMString value)
{
	if(impl)
		impl->setXmlbase(value);
}

DOM::DOMString SVGElement::xmlbase()
{
	if(!impl) return DOM::DOMString();
	return impl->xmlbase();
}

SVGSVGElement SVGElement::ownerSVGElement()
{
	if(!impl) return SVGSVGElement(0);
	return impl->ownerSVGElement();
}

SVGElement SVGElement::viewportElement()
{
	if(!impl) return SVGElement(0);
	return impl->viewportElement();
}

void SVGElement::setAttribute(const DOM::DOMString &name, const DOM::DOMString &value)
{
	if(impl)
		impl->setAttributeInternal(name, value);
}

DOM::DOMString SVGElement::getAttribute(const DOM::DOMString &name)
{
	if(!impl) return DOM::DOMString();
	return impl->getAttributeInternal(name);
}

bool SVGElement::hasAttribute(const DOM::DOMString &name)
{
	if(!impl) return false;
	return impl->hasAttribute(name);
}
		  
// vim:ts=4:noet
