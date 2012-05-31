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

#include "SVGWindow.h"
#include "SVGWindowImpl.h"
#include "SVGDocument.h"

using namespace KSVG;

SVGWindow::SVGWindow()
{
	impl = new SVGWindowImpl();
	impl->ref();
}

SVGWindow::SVGWindow(const SVGWindow &other) : impl(0)
{
	(*this) = other;
}

SVGWindow &SVGWindow::operator =(const SVGWindow &other)
{
	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGWindow::SVGWindow(SVGWindowImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGWindow::~SVGWindow()
{
	if(impl)
		impl->deref();
}

/*StyleSheet SVGWindow::defaultStyleSheet() const
{
	if(!impl) return ; // FIXME
	return impl->defaultStyleSheet();
}*/

SVGDocument SVGWindow::document() const
{
	if(!impl) return SVGDocument(0); // FIXME
	return impl->document();
}

DOM::Event SVGWindow::evt() const
{
	if(!impl) return DOM::Event(); // FIXME
	return impl->evt();
}

long SVGWindow::innerHeight() const
{
	if(!impl) return 0; // FIXME
	return impl->innerHeight();
}

long SVGWindow::innerWidth() const
{
	if(!impl) return 0; // FIXME
	return impl->innerWidth();
}

void SVGWindow::setSrc(const DOM::DOMString &src)
{
	if(impl)
		impl->setSrc(src);
}

DOM::DOMString SVGWindow::src() const
{
	if(!impl) return DOM::DOMString(); // FIXME
	return impl->src();
}

void SVGWindow::clearInterval(long interval)
{
	if(impl)
		impl->clearInterval(interval);
}

void SVGWindow::clearTimeout(long timeout)
{
	if(impl)
		impl->clearTimeout(timeout);
}

void SVGWindow::getURL(const DOM::DOMString &uri, const DOM::EventListener &callback)
{
	if(impl)
		impl->getURL(uri, callback);
}

/*DocumentFragment SVGWindow::parseXML(const DOM::DOMString &source, const Document &document)
{
	if(!impl) return ; // FIXME
	return impl->parseXML(source, document);
}*/

void SVGWindow::postURL(const DOM::DOMString &uri, const DOM::DOMString &data, const DOM::EventListener &callback, const DOM::DOMString &mimeType, const DOM::DOMString &contentEncoding)
{
	if(impl)
		impl->postURL(uri, data, callback, mimeType, contentEncoding);
}

DOM::DOMString SVGWindow::printNode(const DOM::Node &node)
{
	if(!impl) return DOM::DOMString(); // FIXME
	return impl->printNode(node);
}

long SVGWindow::setInterval(const DOM::DOMString &code, const long &delay)
{
	if(!impl) return 0; // FIXME
	return impl->setInterval(code, delay);
}

long SVGWindow::setTimeout(const DOM::DOMString &code, const long &delay)
{
	if(!impl) return 0; // FIXME
	return impl->setTimeout(code, delay);
}

void SVGWindow::alert(const DOM::DOMString &message)
{
	if(impl)
		impl->alert(message);
}

bool SVGWindow::confirm(const DOM::DOMString &message)
{
	if(!impl) return false; // FIXME
	return impl->confirm(message);
}

DOM::DOMString SVGWindow::prompt(const DOM::DOMString &message, const DOM::DOMString &_default)
{
	if(!impl) return DOM::DOMString(); // FIXME
	return impl->prompt(message, _default);
}

// vim:ts=4:noet
