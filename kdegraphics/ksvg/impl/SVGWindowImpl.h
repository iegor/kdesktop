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

#ifndef SVGWindowImpl_H
#define SVGWindowImpl_H

#include <dom/dom_string.h>
#include <dom/dom_node.h>
#include <dom/dom_misc.h>
#include <dom/dom2_events.h>

namespace KSVG
{
class SVGDocumentImpl;

class SVGWindowImpl : public DOM::DomShared
{
public:
	SVGWindowImpl();
	SVGWindowImpl(SVGDocumentImpl *doc);
	virtual ~SVGWindowImpl();

	//StyleSheet defaultStyleSheet() const;
	SVGDocumentImpl *document() const;
	DOM::Event evt() const;
	long innerHeight() const;
	long innerWidth() const;

	void setSrc(const DOM::DOMString &src);
	DOM::DOMString src() const;

	void clearInterval(long interval);
	void clearTimeout(long timeout);
	void getURL(const DOM::DOMString &uri, const DOM::EventListener &callback);
	//DocumentFragment parseXML(const DOM::DOMString &source, const Document &document);
	void postURL(const DOM::DOMString &uri, const DOM::DOMString &data, const DOM::EventListener &callback, const DOM::DOMString &mimeType, const DOM::DOMString &contentEncoding);
	DOM::DOMString printNode(const DOM::Node &node, unsigned short level = 0);
	long setInterval(const DOM::DOMString &code, const long &delay);
	long setTimeout(const DOM::DOMString &code, const long &delay);
	static void alert(const DOM::DOMString &message, const QString &title = "SVG Window");
	static bool confirm(const DOM::DOMString &message, const QString &title = "SVG Window");
	static DOM::DOMString prompt(const DOM::DOMString &message, const DOM::DOMString &_default, const QString &title = "SVG Window");

private:
	void printIndentation(QString &ret, unsigned short level, unsigned short indent = 2);

private:
	//StyleSheet m_defaultStyleSheet;
	SVGDocumentImpl *m_document;
};

}

#endif

// vim:ts=4:noet
