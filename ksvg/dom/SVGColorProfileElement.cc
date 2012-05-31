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

#include "SVGColorProfileElement.h"
#include "SVGColorProfileElementImpl.h"
#include "SVGRenderingIntent.h"

using namespace KSVG;

SVGColorProfileElement::SVGColorProfileElement() : SVGElement(), SVGURIReference()
{
	impl = 0;
}

SVGColorProfileElement::SVGColorProfileElement(const SVGColorProfileElement &other) : SVGElement(other), SVGURIReference(other), impl(0)
{
	(*this) = other;
}

SVGColorProfileElement &SVGColorProfileElement::operator=(const SVGColorProfileElement &other)
{
	SVGElement::operator=(other);
	SVGURIReference::operator=(other);
	
	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGColorProfileElement::SVGColorProfileElement(SVGColorProfileElementImpl *other) : SVGElement(other), SVGURIReference(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGColorProfileElement::~SVGColorProfileElement()
{
	if(impl)
		impl->deref();
}

void SVGColorProfileElement::setLocal(const DOM::DOMString &local)
{
	if(impl)
		impl->setLocal(local);
}

DOM::DOMString SVGColorProfileElement::local() const
{
	if(!impl) return DOM::DOMString();
	return impl->local();
}

void SVGColorProfileElement::setName(const DOM::DOMString &name)
{
	if(impl)
		impl->setName(name);
}

DOM::DOMString SVGColorProfileElement::name() const
{
	if(!impl) return DOM::DOMString();
	return impl->name();
}

void SVGColorProfileElement::setRenderingIntent(unsigned short renderingIntent)
{
	if(impl)
		impl->setRenderingIntent(renderingIntent);
}

unsigned short SVGColorProfileElement::renderingIntent() const
{
	if(!impl) return RENDERING_INTENT_UNKNOWN;
	return impl->renderingIntent();
}

// vim:ts=4:noet
