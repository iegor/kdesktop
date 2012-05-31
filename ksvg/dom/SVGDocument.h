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

#ifndef SVGDocument_H
#define SVGDocument_H

#include <kjs/object.h>

#include <dom/dom_doc.h>
#include <dom/dom_string.h>

namespace KSVG
{

class SVGWindow;
class SVGElement;
class SVGSVGElement;
class SVGDocumentImpl;
class SVGDocument : public DOM::Document
{
public:
	SVGDocument();
	SVGDocument(const SVGDocument &other);
	SVGDocument &operator=(const SVGDocument &other);
	SVGDocument(SVGDocumentImpl *other);
	~SVGDocument();

	DOM::DOMString title() const;
	DOM::DOMString referrer() const;
	DOM::DOMString domain() const;
	DOM::DOMString URL() const;
	SVGWindow window() const;

	SVGSVGElement rootElement() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGDocumentImpl *handle() const { return impl; }
	
	SVGElement createElement(const DOM::DOMString &tagName);
	SVGElement createElementNS(const DOM::DOMString &namespaceURI, const DOM::DOMString &qualifiedName);

	KJS::Object globalJSObject();
	KJS::ExecState *globalJSExec();	

private:
	SVGDocumentImpl *impl;
};

}

#endif

// vim:ts=4:noet
