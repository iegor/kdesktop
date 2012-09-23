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

#ifndef SVGElementImpl_H
#define SVGElementImpl_H

#include <map>
#include <string>

#include <qdict.h>
#include <qptrlist.h>

#include <dom/dom_string.h>

#include "SVGEcma.h"

#include "ksvg_lookup.h"

class QKeyEvent;
class QXmlAttributes;

namespace KJS
{
	class Value;
	class UString;
	class ExecState;
}

namespace KSVG
{

class KSVGCanvas;
class SVGEventImpl;
class SVGDocumentImpl;
class SVGEventListener;
class SVGSVGElementImpl;
class SVGMouseEventImpl;
class SVGRegisteredEventListener;
class SVGElementImpl : virtual public DOM::DomShared,
							   public DOM::Element,
							   public SVGDOMElementBridge
{
public:
	SVGElementImpl(DOM::ElementImpl *impl);
	virtual ~SVGElementImpl();

	// Attribute setting
	// "Override" khtml's functions with ours
	void setAttribute(const DOM::DOMString &name, const DOM::DOMString &value);
	void setAttributeInternal(const DOM::DOMString &name, const DOM::DOMString &value);
	DOM::DOMString getAttribute(const DOM::DOMString &name) const;
	DOM::DOMString getAttributeInternal(const DOM::DOMString &name);
	bool hasAttribute(const DOM::DOMString &name);
	bool hasAttributes();

	QDict<DOM::DOMString> &attributes();
	void setApplyAttribute(const QString &name, const QString &value);

	void setId(DOM::DOMString);
	DOM::DOMString id() const;

	void setXmlbase(DOM::DOMString);
	DOM::DOMString xmlbase() const;

	void setOwnerSVGElement(SVGSVGElementImpl *owner);
	void setViewportElement(SVGElementImpl *viewport);

	SVGSVGElementImpl *ownerSVGElement() const;
	SVGElementImpl *viewportElement() const;

	void setAttributes(const QXmlAttributes &);
	virtual void setAttributes();
	void setAttributes(bool deep);

	QString collectText();

	void setOwnerDoc(SVGDocumentImpl *doc);
	SVGDocumentImpl *ownerDoc() const;

	virtual SVGElementImpl *cloneNode(bool deep);
	void cloneChildNodes(SVGElementImpl *clone);

	// Event stuff
	bool dispatchEvent(int id, bool canBubbleArg, bool cancelableArg);
	bool dispatchEvent(SVGEventImpl *evt, bool tempEvent);
	bool dispatchMouseEvent(int id, bool canBubbleArg, bool cancelableArg, long detailArg, long screenXArg, long screenYArg, long clientXArg, long clientYArg, bool ctrlKeyArg, bool altKeyArg, bool shiftKeyArg, bool metaKeyArg, unsigned short buttonArg, SVGElementImpl *relatedTargetArg);
	bool dispatchKeyEvent(QKeyEvent *ke);

	void setEventListener(int id, SVGEventListener *listener);
	bool hasEventListener(int id, bool local);
	int getEventListeners(bool local);
	void removeEventListener(int id);
	void setupEventListeners(SVGDocumentImpl *doc, SVGDocumentImpl *newDoc);

	// Use with care! Internal only.
	const QPtrList<SVGRegisteredEventListener> &eventListeners() { return m_eventListeners; }
	
	void handleLocalEvents(SVGEventImpl *evt, bool useCapture);
	virtual void defaultEventHandler(SVGEventImpl *evt);

	bool mouseOver() { return m_mouseOver; }
	void setMouseOver(bool v) { m_mouseOver = v; }

	bool focus() { return m_focus; }
	void setFocus(bool v) { m_focus = v; }

	virtual bool prepareMouseEvent(const QPoint &, const QPoint &, SVGMouseEventImpl *);

	virtual void createItem(KSVGCanvas *c = 0) { Q_UNUSED(c); }
	virtual void removeItem(KSVGCanvas *c) { Q_UNUSED(c); }

	// Element creation
	typedef SVGElementImpl *(*FactoryFn)(DOM::ElementImpl *impl);
	class Factory
	{
	public:
		static Factory *self()
		{
			if(!m_instance)
				m_instance = new Factory();

			return m_instance;
		}

		void announce(SVGElementImpl::FactoryFn factoryFn, const std::string &tag)
		{
			if(m_elementMap.find(tag) == m_elementMap.end())
				m_elementMap[tag] = factoryFn;
		}

		SVGElementImpl *create(const std::string &tag, DOM::ElementImpl *impl) const
		{
			ElementMap::const_iterator it = m_elementMap.find(tag);
			if(it != m_elementMap.end())
				return it->second(impl);
			return 0;
		}

	private:
		Factory() { }
		Factory(const Factory &rhs);
		Factory &operator=(const Factory &rhs);

		static Factory *m_instance;

		typedef std::map<std::string, SVGElementImpl::FactoryFn> ElementMap;
		mutable ElementMap m_elementMap;
	};

	template <class T>
	class Registrar
	{
	public:
		Registrar(const std::string &tag)
		{
			SVGElementImpl::Factory::self()->announce(&factoryFn, tag);
		}

		static SVGElementImpl *factoryFn(DOM::ElementImpl *impl)
		{
			return new T(impl);
		}
	};

#define KSVG_REGISTER_ELEMENT(Class, Tag) \
	static SVGElementImpl::Registrar<Class> Class##Registrar(Tag);

protected:
	void gotError(const QString &errorDesc);

private:
	SVGSVGElementImpl *m_ownerSVGElement;
	SVGElementImpl *m_viewportElement;
	SVGDocumentImpl *m_ownerDoc;

	bool m_mouseOver : 1;
	bool m_focus : 1;

	QPtrList<SVGRegisteredEventListener> m_eventListeners;
	QDict<DOM::DOMString> m_attributes;

public:
	KSVG_BASECLASS_GET
	KSVG_PUT

	virtual KJS::ObjectImp *bridge(KJS::ExecState *exec) const; // NEEDS TO BE VIRTUAL HERE

	enum
	{
		// Properties
		ElementId, OwnerSvgElement, ViewportElement, XmlBase,
		OnMouseUp, OnMouseDown, OnMouseMove, OnMouseOver, OnMouseOut,
		OnClick, OnActivate, OnKeyDown, OnKeyUp, OnKeyPress, OnLoad,
		OnFocusIn, OnFocusOut, OnError, OnAbort,
		// Functions
		GetStyle,
		// these do not really belong here, but the css doesnt work yet...
		SetProperty, GetPropertyValue
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

KSVG_DEFINE_PROTOTYPE(SVGElementImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGElementImplProtoFunc, SVGElementImpl)

#endif

// vim:ts=4:noet
