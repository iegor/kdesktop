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

#include <qxml.h>

#include <kdebug.h>

#include "SVGEvent.h"
#include "SVGEventImpl.h"
#include "SVGHelperImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"

using namespace KSVG;

#include "SVGElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_ecma.h"

SVGElementImpl::Factory *SVGElementImpl::Factory::m_instance = 0;

SVGElementImpl::SVGElementImpl(DOM::ElementImpl *impl) : DOM::DomShared(), DOM::Element(impl), SVGDOMElementBridge(static_cast<DOM::Element>(*this))
{
	KSVG_EMPTY_FLAGS

	m_ownerSVGElement = 0;
	m_viewportElement = 0;
	m_ownerDoc = 0;

	m_mouseOver = false;
	m_focus = false;

	m_eventListeners.setAutoDelete(true);
	m_attributes.setAutoDelete(true);
}

SVGElementImpl::~SVGElementImpl()
{
	if(m_ownerSVGElement)
		m_ownerSVGElement->deref();
}

void SVGElementImpl::setEventListener(int id, SVGEventListener *listener)
{
	if(listener)
		listener->ref();

	removeEventListener(id);

	if(listener)
	{
		SVGRegisteredEventListener *rl = new SVGRegisteredEventListener(static_cast<SVGEvent::EventId>(id), listener, false);
		m_eventListeners.append(rl);

		listener->deref();
	}
}

int SVGElementImpl::getEventListeners(bool local)
{
	int events = 0;

	QPtrListIterator<SVGRegisteredEventListener> it(m_eventListeners);
	for(; it.current(); ++it)
		events |= (1 << it.current()->id);

	if(local)
		return events;

	for(DOM::Node node = parentNode(); !node.isNull(); node = node.parentNode())
	{
		SVGElementImpl *element = ownerDoc()->getElementFromHandle(node.handle());

		if(element)
		{
			QPtrListIterator<SVGRegisteredEventListener> it(element->m_eventListeners);
			for(; it.current(); ++it)
				events |= (1 << it.current()->id);
		}
	}

    return events;
}

void SVGElementImpl::setupEventListeners(SVGDocumentImpl *doc, SVGDocumentImpl *newDoc)
{
	if(!doc || !newDoc)
		return;

	// Changes the document where the eventlisteners are registered
	// Needed for parseXML'ed elements with events, their listeners
	// are created in the temporary document fragment and need to be
	// registered in the main document (Niko)
	QPtrListIterator<SVGRegisteredEventListener> it(m_eventListeners);
	for(; it.current(); ++it)
	{
		SVGRegisteredEventListener *current = it.current();

		QString valueOfCurrent = newDoc->ecmaEngine()->valueOfEventListener(current->listener);
		setEventListener(current->id, doc->createEventListener(valueOfCurrent));
	}
}

bool SVGElementImpl::hasEventListener(int id, bool local)
{
	// First check if we have the listener, locally
	QPtrListIterator<SVGRegisteredEventListener> it(m_eventListeners);
	for(; it.current(); ++it)
	{
		if(it.current()->id == id)
			return true;
	}

	// We have no local listeners, if we are just interessted
	// in those listeners, then return now...
	if(local)
		return false;

	// Check every parent element
	for(DOM::Node node = parentNode(); !node.isNull(); node = node.parentNode())
	{
		SVGElementImpl *element = ownerDoc()->getElementFromHandle(node.handle());

		if(element)
		{
			QPtrListIterator<SVGRegisteredEventListener> it(element->m_eventListeners);
			for(; it.current(); ++it)
			{
				if(it.current()->id == id)
					return true;
			}
		}
	}

	return false;
}

void SVGElementImpl::removeEventListener(int id)
{
	QPtrListIterator<SVGRegisteredEventListener> it(m_eventListeners);
	for(; it.current(); ++it)
	{
		if(it.current()->id == id)
		{
			m_eventListeners.removeRef(it.current());
			break;
		}
	}
}

void SVGElementImpl::handleLocalEvents(SVGEventImpl *evt, bool useCapture)
{
	QPtrListIterator<SVGRegisteredEventListener> it(m_eventListeners);
	for(; it.current(); ++it)
	{
		if(it.current()->id == evt->id() && it.current()->useCapture == useCapture)
		{
			it.current()->listener->handleEvent(evt);
			break;
		}
	}
}

void SVGElementImpl::defaultEventHandler(SVGEventImpl *)
{
}

/*
@namespace KSVG
@begin SVGElementImpl::s_hashTable 23
 id						SVGElementImpl::ElementId 		DontDelete
 ownerSVGElement		SVGElementImpl::OwnerSvgElement	DontDelete|ReadOnly
 viewportElement		SVGElementImpl::ViewportElement	DontDelete|ReadOnly
 xmlbase				SVGElementImpl::XmlBase			DontDelete
 base					SVGElementImpl::XmlBase			DontDelete
 onmouseup				SVGElementImpl::OnMouseUp		DontDelete
 onmousedown			SVGElementImpl::OnMouseDown		DontDelete
 onmousemove			SVGElementImpl::OnMouseMove		DontDelete
 onmouseover			SVGElementImpl::OnMouseOver		DontDelete
 onmouseout				SVGElementImpl::OnMouseOut		DontDelete
 onclick				SVGElementImpl::OnClick			DontDelete
 onmouseclick			SVGElementImpl::OnClick			DontDelete
 onactivate				SVGElementImpl::OnActivate		DontDelete
 onkeydown				SVGElementImpl::OnKeyDown		DontDelete
 onkeyup				SVGElementImpl::OnKeyUp			DontDelete
 onkeypress				SVGElementImpl::OnKeyPress		DontDelete
 onload 				SVGElementImpl::OnLoad			DontDelete
 onfocusin				SVGElementImpl::OnFocusIn		DontDelete
 onfocusout				SVGElementImpl::OnFocusOut		DontDelete
 onerror				SVGElementImpl::OnError			DontDelete
 onabort				SVGElementImpl::OnAbort			DontDelete
@end
@namespace KSVG
@begin SVGElementImplProto::s_hashTable 5
 getStyle				SVGElementImpl::GetStyle			DontDelete|Function 0
 setProperty			SVGElementImpl::SetProperty			DontDelete|Function 2
 getPropertyValue		SVGElementImpl::GetPropertyValue	DontDelete|Function 1
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGElement", SVGElementImplProto, SVGElementImplProtoFunc)

Value SVGElementImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case ElementId:
			return String(id().string());
		case XmlBase:
			return String(xmlbase().string());
		case OwnerSvgElement:
			return getDOMNode(exec, *ownerSVGElement());
		case ViewportElement:
			return getDOMNode(exec, *viewportElement());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int)
{
    switch(token)
    {
		case ElementId:
			setId(value.toString(exec).string());
			break;
		case XmlBase:
			setXmlbase(value.toString(exec).string());
			break;
		case OnMouseUp:
			setEventListener(SVGEvent::MOUSEUP_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnMouseDown:
			setEventListener(SVGEvent::MOUSEDOWN_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnMouseMove:
			setEventListener(SVGEvent::MOUSEMOVE_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnMouseOver:
			setEventListener(SVGEvent::MOUSEOVER_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnMouseOut:
			setEventListener(SVGEvent::MOUSEOUT_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnClick:
			setEventListener(SVGEvent::CLICK_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnActivate:
			setEventListener(SVGEvent::DOMACTIVATE_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnKeyDown:
			setEventListener(SVGEvent::KEYDOWN_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnKeyUp:
			setEventListener(SVGEvent::KEYUP_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnKeyPress:
			setEventListener(SVGEvent::KEYPRESS_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnLoad:
			setEventListener(SVGEvent::LOAD_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnFocusIn:
			setEventListener(SVGEvent::DOMFOCUSIN_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnFocusOut:
			setEventListener(SVGEvent::DOMFOCUSOUT_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnError:
			setEventListener(SVGEvent::ERROR_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
		case OnAbort:
			setEventListener(SVGEvent::ABORT_EVENT, m_ownerDoc->createEventListener(value.toString(exec).string()));
			break;
    	default:
	        kdWarning() << k_funcinfo << "unhandled token " << token << endl;
    }
}

Value SVGElementImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGElementImpl)

	switch(id)
	{
		case SVGElementImpl::GetStyle:
			return obj->cache(exec);
		case SVGElementImpl::SetProperty:
		{
			DOM::DOMString attribute = args[0].toString(exec).qstring().lower();
			DOM::DOMString value = args[1].toString(exec).qstring();
			obj->setAttribute(attribute, value);
			break;
		}
		case SVGElementImpl::GetPropertyValue:
			return String(obj->getAttribute(args[0].toString(exec).qstring()));
		default:
			break;
	}
	
	return Undefined();
}

QDict<DOM::DOMString> &SVGElementImpl::attributes()
{
	return m_attributes;
}

// khtml overrides
void SVGElementImpl::setAttribute(const DOM::DOMString &name, const DOM::DOMString &value)
{
	m_attributes.replace(name.string(), new DOM::DOMString(value));
}

// Changes internal value. This will have no effect on getAttribute().
void SVGElementImpl::setAttributeInternal(const DOM::DOMString &name, const DOM::DOMString &value)
{
	ExecState *exec = ownerDoc()->ecmaEngine()->globalExec();

	static_cast<KSVGScriptInterpreter *>(exec->interpreter())->setAttributeSetMode(true);
	bridge(exec)->put(exec, Identifier(UString(name)), String(value), KJS::Internal);
	static_cast<KSVGScriptInterpreter *>(exec->interpreter())->setAttributeSetMode(false);
}

// This gets the actual attribute as set in the svg source
DOM::DOMString SVGElementImpl::getAttribute(const DOM::DOMString &name) const
{
	DOM::DOMString *result = m_attributes[name.string()];
	if(result)
		return *result;
	else
		return DOM::DOMString();
}

// This gets the internal, real-time value. This means it can return a default value
// for an attribute even if its not explicitly set in the svg source.
DOM::DOMString SVGElementImpl::getAttributeInternal(const DOM::DOMString &name)
{
	ExecState *exec = ownerDoc()->ecmaEngine()->globalExec();

	DOM::DOMString retVal;
	
	static_cast<KSVGScriptInterpreter *>(exec->interpreter())->setAttributeGetMode(true);
	retVal = bridge(exec)->get(exec, Identifier(UString(name))).toString(exec).string();
	static_cast<KSVGScriptInterpreter *>(exec->interpreter())->setAttributeGetMode(false);

	return retVal;
}

bool SVGElementImpl::hasAttribute(const DOM::DOMString &name)
{
	return m_attributes.find(name.string()) != 0;
}

bool SVGElementImpl::hasAttributes()
{
	return m_attributes.count() > 0;
}

void SVGElementImpl::setApplyAttribute(const QString &name, const QString &value)
{
	if(hasAttribute(name))
	{
		QString cur = getAttribute(name).string();
		cur = cur.simplifyWhiteSpace();

		if(!cur.endsWith(";"))
			cur += "; ";

		cur += value;

		setAttribute(name, cur);
	}
	else
		setAttribute(name, value);
}

void SVGElementImpl::setId(DOM::DOMString id)
{
	setAttribute("id", id);

	if(ownerDoc() && ownerDoc()->rootElement() && !id.isEmpty())
		ownerDoc()->rootElement()->addToIdMap(id.string(), this);
	else if(m_ownerSVGElement && !id.isEmpty())
		m_ownerSVGElement->addToIdMap(id.string(), this);
}

DOM::DOMString SVGElementImpl::id() const
{
	return getAttribute("id");
}

void SVGElementImpl::setXmlbase(DOM::DOMString xmlbase)
{
	setAttribute("xml:base", xmlbase);
}

DOM::DOMString SVGElementImpl::xmlbase() const
{
	return getAttribute("xml:base");
}

void SVGElementImpl::setOwnerSVGElement(SVGSVGElementImpl *owner)
{
	if(m_ownerSVGElement)
		m_ownerSVGElement->deref();

	m_ownerSVGElement = owner;

	if(m_ownerSVGElement)
		m_ownerSVGElement->ref();
}

void SVGElementImpl::setViewportElement(SVGElementImpl *viewport)
{
	if(m_viewportElement)
		m_viewportElement->deref();
	
	m_viewportElement = viewport;
	
	if(m_viewportElement)
		m_viewportElement->ref();
}

SVGSVGElementImpl *SVGElementImpl::ownerSVGElement() const
{
	return m_ownerSVGElement;
}

SVGElementImpl *SVGElementImpl::viewportElement() const
{
	return m_viewportElement;
}

void SVGElementImpl::setAttributes(const QXmlAttributes &attrs)
{
	for(int i = 0; i < attrs.length(); i++)
	{
		setAttribute(attrs.localName(i), attrs.value(i));
		setAttributeInternal(attrs.localName(i), attrs.value(i));
	}

	setAttributes();
}

void SVGElementImpl::setAttributes()
{
	// Finalize style
	SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(this);

	if(style)
		style->processStyle();
}

void SVGElementImpl::setAttributes(bool deep)
{
	// Finalize style
	SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(this);

	if(style)
		style->processStyle();

	if(deep)
	{
		if(hasChildNodes())
		{
			DOM::Node n;
			for(n = firstChild(); !n.isNull(); n = n.nextSibling())
			{
				SVGElementImpl *elem = ownerDoc()->getElementFromHandle(n.handle());
				if(elem)
					elem->setAttributes(true);
			}
		}
	}
}

bool SVGElementImpl::prepareMouseEvent(const QPoint &p, const QPoint &a, SVGMouseEventImpl *mev)
{
	SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(this);
	if(shape)
		return shape->prepareMouseEvent(p, a, mev);

	return false;
}

bool SVGElementImpl::dispatchEvent(int id, bool canBubbleArg, bool cancelableArg)
{	
	SVGEventImpl *evt = new SVGEventImpl(static_cast<SVGEvent::EventId>(id), canBubbleArg, cancelableArg);

	evt->ref();
	bool ret = dispatchEvent(evt, true);
	evt->deref();

	return ret;
}

bool SVGElementImpl::dispatchEvent(SVGEventImpl *evt, bool tempEvent)
{
	evt->setTarget(this);

	// Find out, where to send to -> collect parent nodes
	QPtrList<SVGElementImpl> nodeChain;

	for(DOM::Element e = *this; !e.isNull(); e = e.parentNode())
		nodeChain.prepend(ownerDoc()->getElementFromHandle(e.handle()));

	// Trigger any capturing event handlers on our way down
	evt->setEventPhase(DOM::Event::CAPTURING_PHASE);

	QPtrListIterator<SVGElementImpl> it(nodeChain);
	for(; it.current() && it.current() != this && !evt->propagationStopped(); ++it)
	{
		evt->setCurrentTarget(it.current());

		if(it.current())
			it.current()->handleLocalEvents(evt, true);
	}

	// Dispatch to the actual target node
	it.toLast();
	if(!evt->propagationStopped())
	{
		evt->setEventPhase(DOM::Event::AT_TARGET);
		evt->setCurrentTarget(it.current());

		if(it.current())
			it.current()->handleLocalEvents(evt, false);
	}

	--it;
	
	// Bubble up again
	if(evt->bubbles())
	{
		evt->setEventPhase(DOM::Event::BUBBLING_PHASE);
		for(; it.current() && !evt->propagationStopped(); --it)
		{
			evt->setCurrentTarget(it.current());

			if(it.current())
				it.current()->handleLocalEvents(evt, false);
		}
	}

	evt->setCurrentTarget(0);
	evt->setEventPhase(0); // I guess this is correct, the spec does not seem to say
						   // anything about the default event handler phase.
	
	if(evt->bubbles())
	{
		// now we call all default event handlers (this is not part of DOM - it is internal to ksvg)
		it.toLast();
		for(; it.current() && !evt->propagationStopped() && !evt->defaultPrevented() && !evt->defaultHandled(); --it)
			it.current()->defaultEventHandler(evt);
	}

	// If tempEvent is true, this means that the DOM implementation will not be storing a reference to the event, i.e.
	// there is no way to retrieve it from javascript if a script does not already have a reference to it in a variable.
	// So there is no need for the interpreter to keep the event in its cache
	if(tempEvent)
		ownerDoc()->ecmaEngine()->finishedWithEvent(evt);

	return !evt->defaultPrevented(); // ### what if defaultPrevented was called before dispatchEvent?
}

bool SVGElementImpl::dispatchKeyEvent(QKeyEvent *ke)
{
	DOM::AbstractView temp;

	SVGEvent::EventId evtId = SVGEvent::UNKNOWN_EVENT;

	if(ke->type() == QEvent::KeyRelease && !ke->isAutoRepeat())
		evtId = SVGEvent::KEYUP_EVENT;
	else if(ke->isAutoRepeat())
		evtId = SVGEvent::KEYPRESS_EVENT;
	else if(ke->type() == QEvent::KeyPress)
		evtId = SVGEvent::KEYDOWN_EVENT;

	if(evtId == SVGEvent::KEYUP_EVENT && hasEventListener(SVGEvent::DOMACTIVATE_EVENT, false))
		dispatchEvent(SVGEvent::DOMACTIVATE_EVENT, true, true);

	if(!hasEventListener(evtId, false))
		return false;

	SVGEventImpl *evt = new SVGKeyEventImpl(ke, temp, evtId);

	evt->ref();
	bool ret = dispatchEvent(evt, true);
	evt->deref();

	// Rerender now! Once! (Niko)
	ownerDoc()->rerender();

	return ret;
}

bool SVGElementImpl::dispatchMouseEvent(int id, bool canBubbleArg, bool cancelableArg, long detailArg, long screenXArg, long screenYArg, long clientXArg, long clientYArg, bool ctrlKeyArg, bool altKeyArg, bool shiftKeyArg, bool metaKeyArg, unsigned short buttonArg, SVGElementImpl *relatedTargetArg)
{
	DOM::AbstractView temp;

	SVGEventImpl *evt = new SVGMouseEventImpl(static_cast<SVGEvent::EventId>(id),
											  canBubbleArg, cancelableArg, temp, detailArg,
											  screenXArg, screenYArg,
											  clientXArg, clientYArg,
											  ctrlKeyArg, altKeyArg, shiftKeyArg, metaKeyArg,
											  buttonArg, relatedTargetArg);

	evt->ref();
	bool ret = dispatchEvent(evt, true);
	evt->deref();

	return ret;
}

void SVGElementImpl::setOwnerDoc(SVGDocumentImpl *doc)
{
	if(m_ownerDoc)
		m_ownerDoc->removeFromElemDict(handle());

	m_ownerDoc = doc;

	if(m_ownerDoc)
		m_ownerDoc->addToElemDict(handle(), this);
}

SVGDocumentImpl *SVGElementImpl::ownerDoc() const
{
	return m_ownerDoc;
}

SVGElementImpl *SVGElementImpl::cloneNode(bool deep)
{
	DOM::Element impl = static_cast<DOM::Document *>(ownerDoc())->createElementNS("", tagName());
	SVGElementImpl *newElement = SVGDocumentImpl::createElement(tagName(), impl.cloneNode(false), ownerDoc());
	newElement->setOwnerSVGElement(ownerSVGElement());
	newElement->setViewportElement(viewportElement());

	SVGHelperImpl::copyAttributes(this, newElement);

	// Recalc style
	//newElement->setAttributes();

	if(deep)
		cloneChildNodes(newElement);

	return newElement;
}

void SVGElementImpl::cloneChildNodes(SVGElementImpl *clone)
{
	DOM::Node n;
	for(n = firstChild(); !n.isNull(); n = n.nextSibling())
	{
		SVGElementImpl *elem = ownerDoc()->getElementFromHandle(n.handle());
		if(elem)
			clone->appendChild(*elem->cloneNode(true));
		else if(n.nodeType() == DOM::Node::TEXT_NODE)
			clone->appendChild(n.cloneNode(true));
	}
}

void SVGElementImpl::gotError(const QString &errorDesc)
{
	if(ownerDoc())
	{
		ownerDoc()->finishParsing(true, errorDesc);
		if(hasEventListener(SVGEvent::ERROR_EVENT, true))
			dispatchEvent(SVGEvent::ERROR_EVENT, false, false);
	}
}

QString SVGElementImpl::collectText()
{
	QString text;

	if(hasChildNodes())
	{
		DOM::Node node = firstChild();

		for(; !node.isNull(); node = node.nextSibling())
		{
			if(node.nodeType() == DOM::Node::TEXT_NODE)
			{
				DOM::Text textNode = node;
				text += textNode.data().string();
			}
		}
	}

	return text;
}

// vim:ts=4:noet
