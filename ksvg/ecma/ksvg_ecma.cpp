/*
    Copyright (C) 2002-2003 KSVG Team
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

#include <kdebug.h>

#include <qvariant.h>

#include <dom/dom2_events.h>

#include "SVGEcma.h"

#include "SVGDocumentImpl.h"

#include "ksvg_ecma.h"
#include "ksvg_window.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_ecmaeventlistener.h"

#include "KSVGLoader.h"

using namespace KSVG;
using namespace KJS;

class AsyncStatus : public ObjectImp
{
public:
	AsyncStatus() : ObjectImp() { }

	virtual bool implementsCall() const { return true; }
	virtual Value call(ExecState *exec, Object &thisObj, const List &args);
};

Value AsyncStatus::call(ExecState *exec, Object &, const List &args)
{
	kdDebug(26004) << "[AsyncStatus] " << args[0].toString(exec).ascii() << endl;

	if(args[0].toString(exec) == "success")
		return Number(1);
	else
		return Undefined();
}

KSVGEcma::KSVGEcma(SVGDocumentImpl *doc) : m_doc(doc)
{
	m_init = false;
	m_hasListeners = false;

	m_window = 0;
	m_interpreter = 0;

	m_ecmaEventListeners.setAutoDelete(true);
}

KSVGEcma::~KSVGEcma()
{
	// We are 0 soon so event listeners may NOT call us
	QPtrListIterator<KSVGEcmaEventListener> it(m_ecmaEventListeners);
	for(; it.current(); ++it)
		it.current()->forbidRemove();

	if(m_interpreter)
		delete m_interpreter;
}

bool KSVGEcma::initialized()
{
	return m_init;
}

void KSVGEcma::setup()
{
	if(m_init)
		return;

	m_init = true;

	// Create handler for js calls
	m_window = new KSVG::Window(m_doc);
	Object globalObject(m_window);

	// Create code interpreter
	m_interpreter = new KSVGScriptInterpreter(globalObject, m_doc);

	// Set object prototype for global object
	m_window->setPrototype(m_interpreter->builtinObjectPrototype());

	// Create bridge for document. Could be done on demand now, though
	KSVGBridge<SVGDocumentImpl> *documentRequest = new KSVGBridge<SVGDocumentImpl>(m_interpreter->globalExec(), m_doc);
	documentRequest->ref();

	m_interpreter->putDOMObject(m_doc->handle(), documentRequest);
}

Completion KSVGEcma::evaluate(const UString &code, const Value &thisV)
{
#ifdef KJS_VERBOSE
	kdDebug(6070) << "KSVGEcma::evaluate " << code.qstring() << endl;
#endif
	
	return m_interpreter->evaluate(code, thisV);
}

Object KSVGEcma::globalObject()
{
	return m_interpreter->globalObject();
}

ExecState *KSVGEcma::globalExec()
{
	return m_interpreter->globalExec();
}

SVGEventListener *KSVGEcma::createEventListener(DOM::DOMString type)
{
	QPtrListIterator<KSVGEcmaEventListener> it(m_ecmaEventListeners);

	for(; it.current(); ++it)
	{
		if(it.current()->type() == type.string())
			return static_cast<SVGEventListener *>(it.current());
	}

	setup();

	Object constr = m_interpreter->builtinFunction();

	List args;
	args.append(String("event"));
	args.append(String(type.string()));

	Object obj = constr.construct(m_interpreter->globalExec(), args);

	// Note that the KSVGEcmaEventListener constructor adds itself to the m_ecmaEventListeners list
	KSVGEcmaEventListener *event = new KSVGEcmaEventListener(obj, type.string(), this);
	event->ref();

	// addEventListener() is called by KSVGEcmaListeners ctor, so it's
	// safe to check to count of the eventListeners list (Niko)
	if(m_ecmaEventListeners.count() > 0)
		m_hasListeners = true;

	return event;
}

QString KSVGEcma::valueOfEventListener(SVGEventListener *listener) const
{
	KSVGEcmaEventListener *event = static_cast<KSVGEcmaEventListener *>(listener);
	if(!event)
		return QString::null;

	return event->type();
}

void KSVGEcma::addEventListener(KSVGEcmaEventListener *listener)
{
	m_ecmaEventListeners.append(listener);
}

void KSVGEcma::removeEventListener(KSVGEcmaEventListener *listener)
{
	m_ecmaEventListeners.take(m_ecmaEventListeners.find(listener));

	if(m_ecmaEventListeners.count() == 0)
		m_hasListeners = false;
}

bool KSVGEcma::hasEventListeners()
{
	return m_hasListeners;
}

void KSVGEcma::finishedWithEvent(SVGEventImpl *event)
{
	KSVGScriptInterpreter *interpreter = static_cast<KSVGScriptInterpreter *>(globalExec()->interpreter());
	interpreter->removeDOMObject(event);
}

Value KSVGEcma::getUrl(ExecState *exec, ::KURL url)
{
	Object *status = new Object(new AsyncStatus());

	// FIXME: Security issue, allows local testing of getURL(), REMOVE BEFORE RELEASE! (Niko)
	QString svgDocument = KSVGLoader::getUrl(url, true);
	if(svgDocument.length() > 0)
	{
		status->put(exec, Identifier("success"), Boolean(true));
		status->put(exec, Identifier("content"), String(svgDocument));
	}
	else
	{
		status->put(exec, Identifier("success"), Boolean(false));
		status->put(exec, Identifier("content"), String(""));
	}

    return Value(*status);
}

void KSVGEcma::postUrl(ExecState *exec, ::KURL url, const QString &data, const QString &mimeType, const QString &contentEncoding, Object &callBackFunction)
{
	Object *status = new Object(new AsyncStatus());
	status->put(exec, Identifier("content"), String(""));
    status->put(exec, Identifier("success"), Boolean(false));

	QByteArray byteArray;
	QDataStream ds(byteArray, IO_WriteOnly);
	ds << data;

	// Support gzip compression
	if(contentEncoding == "gzip" || contentEncoding == "deflate")
		byteArray = qCompress(byteArray);

	KSVGLoader *loader = new KSVGLoader();
	loader->postUrl(url, byteArray, mimeType, exec, callBackFunction, *status);
	delete loader;
}

// Helpers
Value KSVG::getDOMNode(ExecState *exec, DOM::Node n)
{
	ObjectImp *ret = 0;
	if(n.isNull())
		return Null();

	KSVGScriptInterpreter *interpreter = static_cast<KSVGScriptInterpreter *>(exec->interpreter());

	ObjectImp *request = interpreter->getDOMObject(n.handle());
	if(request)
		return Value(request);

	SVGElementImpl *elem = 0;

	switch(n.nodeType())
	{
		case DOM::Node::ELEMENT_NODE:
			elem = interpreter->document()->getElementFromHandle(n.handle());
			if(!elem)
			{
				// Lookup different document, if possible
				SVGDocumentImpl *different = interpreter->document()->getDocumentFromHandle(n.ownerDocument().handle());

				if(!different)
					return Null();

				elem = different->getElementFromHandle(n.handle());

				if(!elem)
					return Null();
			}

			// The generated bridge() function does not ref the ret itself
			ret = elem->bridge(exec);
			ret->ref();
			break;
		case DOM::Node::TEXT_NODE:
			ret = new KSVGRWBridge<SVGDOMTextBridge>(exec, new SVGDOMTextBridge(n));
			ret->ref();
			break;
		default:
			ret = new KSVGBridge<SVGDOMNodeBridge>(exec, new SVGDOMNodeBridge(n));
			ret->ref();
			break;
	}

	interpreter->putDOMObject(n.handle(), ret);

	return Value(ret);
}

Value KSVG::getDOMEvent(ExecState *exec, SVGEventImpl *e)
{
    return e->cache(exec);
}

Value KSVG::getString(DOM::DOMString s)
{
	if(s.isNull())
		return Null();
	else
		return String(s);
}

DOM::Node KSVG::toNode(const Value &val)
{
	Object obj = Object::dynamicCast(val);
	if(obj.isNull())
		return DOM::Node();

	SVGDOMNodeBridge *bridge = toNodeBridge(static_cast<ObjectImp *>(obj.imp()));

	if(bridge)
		return bridge->impl();

	return DOM::Node();
}

QVariant KSVG::valueToVariant(ExecState *exec, const Value &val)
{
	QVariant res;

	switch(val.type())
	{
		case BooleanType:
			res = QVariant(val.toBoolean(exec), 0);
			break;
		case NumberType:
			res = QVariant(val.toNumber(exec));
			break;
		case StringType:
			res = QVariant(val.toString(exec).qstring());
			break;
		default:
			// everything else will be 'invalid'
			break;
	}

	return res;
}

// vim:ts=4:noet
