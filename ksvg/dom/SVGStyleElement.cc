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

#include "SVGStyleElement.h"
#include "SVGStyleElementImpl.h"

using namespace KSVG;

SVGStyleElement::SVGStyleElement() : SVGElement()
{
	impl = 0;
}

SVGStyleElement::SVGStyleElement(const SVGStyleElement &other) : SVGElement(other), impl(0)
{
	(*this) = other;
}

SVGStyleElement &SVGStyleElement::operator =(const SVGStyleElement &other)
{
	SVGElement::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGStyleElement::SVGStyleElement(SVGStyleElementImpl *other) : SVGElement(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGStyleElement::~SVGStyleElement()
{
	if(impl)
		impl->deref();
}

void SVGStyleElement::setXmlspace(const DOM::DOMString &xmlspace)
{
	if(impl)
		impl->setXmlspace(xmlspace);
}

DOM::DOMString SVGStyleElement::xmlspace() const
{
	if(!impl) return DOM::DOMString();
	return impl->xmlspace();
}

void SVGStyleElement::setType(const DOM::DOMString &type)
{
	if(impl)
		impl->setType(type);
}

DOM::DOMString SVGStyleElement::type() const
{
	if(!impl) return DOM::DOMString();
	return impl->type();
}

void SVGStyleElement::setMedia(const DOM::DOMString &media)
{
	if(impl)
		impl->setMedia(media);
}

DOM::DOMString SVGStyleElement::media() const
{
	if(!impl) return DOM::DOMString();
	return impl->media();
}

void SVGStyleElement::setTitle(const DOM::DOMString &title)
{
	if(impl)
		impl->setTitle(title);
}

DOM::DOMString SVGStyleElement::title() const
{
	if(!impl) return DOM::DOMString();
	return impl->title();
}

// vim:ts=4:noet
