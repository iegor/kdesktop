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

#include "SVGWindowImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGEvent.h"
#include "KSVGCanvas.h"
#include <ksvg_ecma.h>
#include <ksvg_scriptinterpreter.h>

#include <qstylesheet.h>

#include <kurl.h>
#include <kmessagebox.h>
#include <kinputdialog.h>
#include <klocale.h>

using namespace KSVG;


SVGWindowImpl::SVGWindowImpl()
{
	m_document = 0;
}

SVGWindowImpl::SVGWindowImpl(SVGDocumentImpl *doc)
{
	m_document = doc;
	if(m_document)
		m_document->ref();
}

SVGWindowImpl::~SVGWindowImpl()
{
	if(m_document)
		m_document->deref();
}

/*StyleSheet SVGWindowImpl::defaultStyleSheet() const
{
	return m_defaultStyleSheet;
}*/

SVGDocumentImpl *SVGWindowImpl::document() const
{
	return m_document;
}

DOM::Event SVGWindowImpl::evt() const
{
	return KSVG::SVGEvent(m_document->ecmaEngine()->interpreter()->currentEvent());
}

long SVGWindowImpl::innerHeight() const
{
	return m_document ? int(m_document->canvas()->height()) : -1;
}

long SVGWindowImpl::innerWidth() const
{
	return m_document ? int(m_document->canvas()->width()) : -1;
}

void SVGWindowImpl::setSrc(const DOM::DOMString &/*src*/)
{
	// TODO : make KURL, load and parse doc
}

DOM::DOMString SVGWindowImpl::src() const
{
	if(!m_document)
		return DOM::DOMString();
	return m_document->baseUrl().prettyURL();
}

void SVGWindowImpl::clearInterval(long /*interval*/)
{
}

void SVGWindowImpl::clearTimeout(long /*timeout*/)
{
}

void SVGWindowImpl::getURL(const DOM::DOMString &/*uri*/, const DOM::EventListener &/*callback*/)
{
}

/*DocumentFragment SVGWindowImpl::parseXML(const DOM::DOMString &source, const Document &document)
{
}*/

void SVGWindowImpl::postURL(const DOM::DOMString &/*uri*/, const DOM::DOMString &/*data*/, const DOM::EventListener &/*callback*/, const DOM::DOMString &/*mimeType*/, const DOM::DOMString &/*contentEncoding*/)
{
}

DOM::DOMString SVGWindowImpl::printNode(const DOM::Node &node, unsigned short level)
{
	QString ret;
	if(node.isNull()) return ret;
	SVGElementImpl *elem = m_document->getElementFromHandle(node.handle());
	if(node.nodeType() == DOM::Node::DOCUMENT_NODE)
	{
		ret +=  "<\?xml version=\"1.0\" encoding=\"UTF-8\"\?><!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n" + printNode(node.firstChild()).string() + "\n";
	}
	else if(node.nodeType() == DOM::Node::TEXT_NODE)
	{
		printIndentation(ret, level);
		ret += node.nodeValue().string();
	}
	else if(elem)
	{
		printIndentation(ret, level);
		ret += "<" + elem->tagName().string();
		// handle attrs
		QDictIterator<DOM::DOMString> it(elem->attributes());
		for(;it.current(); ++it)
		         ret += " " + it.currentKey() + "=\"" + it.current()->string() + '\"';
		if(elem->firstChild().isNull()) // no children
			ret += " />\n";
		else // handle children
		{
			ret += ">\n";
			for(DOM::Node child = node.firstChild();!child.isNull();child = child.nextSibling())
				ret += printNode(child, level + 1).string();
			printIndentation(ret, level);
			ret += "</" + elem->tagName().string() + ">\n";
		}
	}
	return ret;
}

void SVGWindowImpl::printIndentation(QString &ret, unsigned short level, unsigned short indent)
{
	for(int i = 0;i < indent * level;i++)
		ret += " ";
}

long SVGWindowImpl::setInterval(const DOM::DOMString &/*code*/, const long &/*delay*/)
{
	return 0;
}

long SVGWindowImpl::setTimeout(const DOM::DOMString &/*code*/, const long &/*delay*/)
{
	return 0;
}

void SVGWindowImpl::alert(const DOM::DOMString &message, const QString &title)
{
	KMessageBox::error(0L, QStyleSheet::convertFromPlainText(message.string()), title);
}

bool SVGWindowImpl::confirm(const DOM::DOMString &message, const QString &title)
{
	return KMessageBox::warningContinueCancel(0L, QStyleSheet::convertFromPlainText(message.string()), title, KStdGuiItem::ok()) == KMessageBox::Continue;
}

DOM::DOMString SVGWindowImpl::prompt(const DOM::DOMString &message, const DOM::DOMString &_default, const QString &)
{
	bool ok;
	QString str;
	str = KInputDialog::getText(i18n("Prompt"), QStyleSheet::convertFromPlainText(message.string()), _default.string(), &ok);
	if(ok)
		return str;
	else
		return "";
}

// vim:ts=4:noet
