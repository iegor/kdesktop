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

#include "ksvg_ecma.h"

#include "SVGDocument.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElement.h"
#include "SVGWindow.h"

using namespace KSVG;

SVGDocument::SVGDocument() : DOM::Document(*(new SVGDocumentImpl()))
{
	impl = reinterpret_cast<SVGDocumentImpl *>(handle());
	impl->ref();
}

SVGDocument::SVGDocument(const SVGDocument &other) : DOM::Document(other), impl(0)
{
	(*this) = other;
}

SVGDocument &SVGDocument::operator=(const SVGDocument &other)
{
	DOM::Document::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGDocument::SVGDocument(SVGDocumentImpl *other) : DOM::Document(other->handle())
{
	impl = other;

	if(impl)
		impl->ref();
}

SVGDocument::~SVGDocument()
{
	if(impl)
		impl->deref();
}

DOM::DOMString SVGDocument::title() const
{
	if(!impl) return DOM::DOMString();
	return impl->title();
}

DOM::DOMString SVGDocument::referrer() const
{
	if(!impl) return DOM::DOMString();
	return impl->referrer();
}

DOM::DOMString SVGDocument::domain() const
{
	if(!impl) return DOM::DOMString();
	return impl->domain();
}

DOM::DOMString SVGDocument::URL() const
{
	if(!impl) return DOM::DOMString();
	return impl->URL();
}

SVGWindow SVGDocument::window() const
{
	if(!impl) return SVGWindow();
	return impl->window();
}

SVGSVGElement SVGDocument::rootElement() const
{
	if(!impl) return SVGSVGElement(0);
	return SVGSVGElement(impl->rootElement());
}

SVGElement SVGDocument::createElement(const DOM::DOMString &tagName)
{
	if(!impl) return SVGElement(0);
	
	DOM::Element impl = DOM::Document::createElement(tagName);
	return SVGElement(SVGDocumentImpl::createElement(tagName, impl));
}

SVGElement SVGDocument::createElementNS(const DOM::DOMString &namespaceURI, const DOM::DOMString &qualifiedName)
{
	if(!impl) return SVGElement(0);
	
	DOM::Element impl = DOM::Document::createElementNS(namespaceURI, qualifiedName);
	return SVGElement(SVGDocumentImpl::createElement(qualifiedName, impl));
}

// Internal
KJS::Object SVGDocument::globalJSObject()
{
	if(!impl) return KJS::Object();
	return impl->ecmaEngine()->globalObject();
}

KJS::ExecState *SVGDocument::globalJSExec()
{
	if(!impl) return 0;
	return impl->ecmaEngine()->globalExec();
}
		
// vim:ts=4:noet
