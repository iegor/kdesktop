/*
    Copyright (C) 2001-2003 KSVG Team
    This file is part of the KDE project

	Additional copyright:
	(C) 2001 Peter Kelly <pmk@post.com>
	(C) 2001 Tobias Anton <anton@stud.fbi.fh-darmstadt.de>

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

#ifndef SVGEventImpl_H
#define SVGEventImpl_H

#include <qevent.h>
#include <qdatetime.h>

#include <dom/dom_misc.h>
#include <dom/dom_node.h>
#include <dom/dom2_views.h>
#include <dom/dom_string.h>

#include "SVGEvent.h"
#include "SVGElementImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

// @ecma-checked 07/07/02@
class SVGEventImpl : public DOM::DomShared
{
public:
	SVGEventImpl();
	SVGEventImpl(SVGEvent::EventId _id, bool canBubbleArg, bool cancelableArg);
	virtual ~SVGEventImpl();

	SVGEvent::EventId id() { return m_id; }

	DOM::DOMString type() const;

	SVGElementImpl *target() const;
	void setTarget(SVGElementImpl *_target);

	SVGElementImpl *currentTarget() const;
	void setCurrentTarget(SVGElementImpl *_currentTarget);

	unsigned short eventPhase() const;
	void setEventPhase(unsigned short _eventPhase);

	bool bubbles() const;
	bool cancelable() const;

	DOM::DOMTimeStamp timeStamp();

	void stopPropagation();
	void preventDefault();
	void initEvent(const DOM::DOMString &eventTypeArg, bool canBubbleArg, bool cancelableArg);

	virtual bool isUIEvent() { return false; }
	virtual bool isMouseEvent() { return false; }
	virtual bool isMutationEvent() { return false; }
	virtual bool isKeyEvent() { return false; }
	
	virtual DOM::DOMString eventModuleName() { return ""; }

	virtual bool propagationStopped() { return m_propagationStopped; }
	virtual bool defaultPrevented() { return m_defaultPrevented; }

	void setDefaultHandled();
	bool defaultHandled() const { return m_defaultHandled; }

protected:
	DOM::DOMString m_type;
	bool m_canBubble;
	bool m_cancelable;

	bool m_propagationStopped;
	bool m_defaultPrevented;
	bool m_defaultHandled;

	SVGEvent::EventId m_id;
	SVGElementImpl *m_currentTarget;
	SVGElementImpl *m_target;

	unsigned short m_eventPhase;
	QDateTime m_createTime;

public:
	KSVG_BASECLASS_GET

	enum
	{
		// Properties
		Type, Target, CurrentTarget, EventPhase,
		Bubbles, Cancelable, TimeStamp,
		// Functions
		GetType, GetTarget, GetCurrentTarget, GetEventPhase,
		GetBubbles, GetCancelable, GetTimeStamp,
		StopPropagation, PreventDefault, InitEvent,
		GetCurrentNode // Out-Of-Spec
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
};

class SVGEventListener : public DOM::DomShared
{
public:
	SVGEventListener() { }
	virtual ~SVGEventListener() { }

	virtual void handleEvent(SVGEventImpl *) { }
	virtual DOM::DOMString eventListenerType() { return ""; }
};

// @ecma-checked 07/07/02@
class SVGUIEventImpl : public SVGEventImpl
{
public:
	SVGUIEventImpl();
	SVGUIEventImpl(SVGEvent::EventId _id,
			bool canBubbleArg,
			bool cancelableArg,
			DOM::AbstractView &viewArg,
			long detailArg);

	virtual ~SVGUIEventImpl();

	DOM::AbstractView view() const;
	long detail() const;

	void initUIEvent(const DOM::DOMString &typeArg,
			bool canBubbleArg,
			bool cancelableArg,
			const DOM::AbstractView &viewArg,
			long detailArg);

	virtual bool isUIEvent() { return true; }
	virtual DOM::DOMString eventModuleName() { return "UIEvents"; }

protected:
	DOM::AbstractView m_view;
	long m_detail;

public:
	KSVG_GET

	enum
	{
		// Properties
		View, Detail,
		// Functions
		GetView, GetDetail, InitUIEvent
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
};

// @ecma-checked 07/07/02@
// Introduced in DOM Level 2: - internal
class SVGMouseEventImpl : public SVGUIEventImpl
{
public:
	SVGMouseEventImpl();
	SVGMouseEventImpl(SVGEvent::EventId _id,
			bool canBubbleArg,
			bool cancelableArg,
			DOM::AbstractView &viewArg,
			long detailArg,
			long screenXArg,
			long screenYArg,
			long clientXArg,
			long clientYArg,
			bool ctrlKeyArg,
			bool altKeyArg,
			bool shiftKeyArg,
			bool metaKeyArg,
			unsigned short buttonArg,
			SVGElementImpl *relatedTargetArg);

	virtual ~SVGMouseEventImpl();

	long screenX() const;
	long screenY() const;
	long clientX() const;
	long clientY() const;
	bool ctrlKey() const;
	bool shiftKey() const;
	bool altKey() const;
	bool metaKey() const;

	unsigned short button() const;

	SVGElementImpl *relatedTarget() const;

	void initMouseEvent(const DOM::DOMString &typeArg,
			bool canBubbleArg,
			bool cancelableArg,
			const DOM::AbstractView &viewArg,
			long detailArg,
			long screenXArg,
			long screenYArg,
			long clientXArg,
			long clientYArg,
			bool ctrlKeyArg,
			bool altKeyArg,
			bool shiftKeyArg,
			bool metaKeyArg,
			unsigned short buttonArg,
			SVGElementImpl *relatedTargetArg);

	virtual bool isMouseEvent() { return true; }
	virtual DOM::DOMString eventModuleName() { return "MouseEvents"; }

	// KSVG extensions
	DOM::DOMString url() const;
	void setURL(DOM::DOMString url);
	
protected:
	long m_screenX;
	long m_screenY;
	long m_clientX;
	long m_clientY;
	bool m_ctrlKey;
	bool m_altKey;
	bool m_shiftKey;
	bool m_metaKey;
	unsigned short m_button;
	SVGElementImpl *m_relatedTarget;

	// KSVG extension
	DOM::DOMString m_url;

public:
	KSVG_GET

	enum
	{
		// Properties
		ScreenX, ScreenY, ClientX, ClientY, CtrlKey,
		ShiftKey, AltKey, MetaKey, Button, RelatedTarget,
		// Functions
		GetScreenX, GetScreenY, GetClientX, GetClientY, GetCtrlKey,
		GetShiftKey, GetAltKey, GetMetaKey, GetButton, GetRelatedTarget,
		InitMouseEvent
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
};

class SVGKeyEventImpl : public SVGUIEventImpl
{
public:
	SVGKeyEventImpl();
	SVGKeyEventImpl(SVGEvent::EventId _id,
			bool canBubbleArg,
			bool cancelableArg,
			DOM::AbstractView &viewArg,
			unsigned short detailArg,
			DOM::DOMString &outputStringArg,
			unsigned long keyValArg,
			unsigned long virtKeyValArg,
			bool inputGeneratedArg,
			bool numPadArg);

	SVGKeyEventImpl(QKeyEvent *key, DOM::AbstractView &view, SVGEvent::EventId _id);

	virtual bool isKeyEvent() { return true; }

	virtual ~SVGKeyEventImpl();

	enum KeyCodes 
	{
		DOM_VK_UNDEFINED               = 0x0,
		DOM_VK_RIGHT_ALT               = 0x01,
		DOM_VK_LEFT_ALT                = 0x02,
		DOM_VK_LEFT_CONTROL            = 0x03,
		DOM_VK_RIGHT_CONTROL           = 0x04,
		DOM_VK_LEFT_SHIFT              = 0x05,
		DOM_VK_RIGHT_SHIFT             = 0x06,
		DOM_VK_LEFT_META               = 0x07,
		DOM_VK_RIGHT_META              = 0x08,
		DOM_VK_CAPS_LOCK               = 0x09,
		DOM_VK_DELETE                  = 0x0A,
		DOM_VK_END                     = 0x0B,
		DOM_VK_ENTER                   = 0x0C,
		DOM_VK_ESCAPE                  = 0x0D,
		DOM_VK_HOME                    = 0x0E,
		DOM_VK_INSERT                  = 0x0F,
		DOM_VK_NUM_LOCK                = 0x10,
		DOM_VK_PAUSE                   = 0x11,
		DOM_VK_PRINTSCREEN             = 0x12,
		DOM_VK_SCROLL_LOCK             = 0x13,
		DOM_VK_LEFT                    = 0x14,
		DOM_VK_RIGHT                   = 0x15,
		DOM_VK_UP                      = 0x16,
		DOM_VK_DOWN                    = 0x17,
		DOM_VK_PAGE_DOWN               = 0x18,
		DOM_VK_PAGE_UP                 = 0x19,
		DOM_VK_F1                      = 0x1A,
		DOM_VK_F2                      = 0x1B,
		DOM_VK_F3                      = 0x1C,
		DOM_VK_F4                      = 0x1D,
		DOM_VK_F5                      = 0x1E,
		DOM_VK_F6                      = 0x1F,
		DOM_VK_F7                      = 0x20,
		DOM_VK_F8                      = 0x21,
		DOM_VK_F9                      = 0x22,
		DOM_VK_F10                     = 0x23,
		DOM_VK_F11                     = 0x24,
		DOM_VK_F12                     = 0x25,
		DOM_VK_F13                     = 0x26,
		DOM_VK_F14                     = 0x27,
		DOM_VK_F15                     = 0x28,
		DOM_VK_F16                     = 0x29,
		DOM_VK_F17                     = 0x2A,
		DOM_VK_F18                     = 0x2B,
		DOM_VK_F19                     = 0x2C,
		DOM_VK_F20                     = 0x2D,
		DOM_VK_F21                     = 0x2E,
		DOM_VK_F22                     = 0x2F,
		DOM_VK_F23                     = 0x30,
		DOM_VK_F24                     = 0x31
	};


	bool checkModifier(unsigned long modiferArg);

	void initKeyEvent(DOM::DOMString &typeArg,
			bool canBubbleArg,
			bool cancelableArg,
			const DOM::AbstractView &viewArg,
			long detailArg,
			DOM::DOMString &outputStringArg,
			unsigned long keyValArg,
			unsigned long virtKeyValArg,
			bool inputGeneratedArg,
			bool numPadArg);

	void initModifier(unsigned long modifierArg, bool valueArg);

	bool inputGenerated() const;

	unsigned long keyVal() const;

	bool numPad() const { return m_numPad; }
	DOM::DOMString outputString() const;

	unsigned long virtKeyVal() const { return m_virtKeyVal; }

	QKeyEvent *qKeyEvent;

private:	
	unsigned long m_keyVal;
	unsigned long m_virtKeyVal;
	bool m_inputGenerated;
	DOM::DOMString m_outputString;
	bool m_numPad;

	// bitfield containing state of modifiers. not part of the dom.
	unsigned long m_modifier;

public:
	KSVG_GET
	
	enum
	{
		// Properties
		KeyVal, VirtKeyVal, OutputString,
		// Functions
		CheckModifier, GetKeyVal, GetCharCode
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
};

class SVGMutationEventImpl : public SVGEventImpl
{
public:
	SVGMutationEventImpl();
	SVGMutationEventImpl(SVGEvent::EventId _id,
			bool canBubbleArg,
			bool cancelableArg,
			SVGElementImpl *relatedNodeArg,
			const DOM::DOMString &prevValueArg,
			const DOM::DOMString &newValueArg,
			const DOM::DOMString &attrNameArg,
			unsigned short attrChangeArg);
	~SVGMutationEventImpl();

	SVGElementImpl *relatedNode() const;
	DOM::DOMString prevValue() const;
	DOM::DOMString newValue() const;
	DOM::DOMString attrName() const;
	unsigned short attrChange() const;

	void initMutationEvent(const DOM::DOMString &typeArg,
			bool canBubbleArg,
			bool cancelableArg,
			SVGElementImpl *relatedNodeArg,
			const DOM::DOMString &prevValueArg,
			const DOM::DOMString &newValueArg,
			const DOM::DOMString &attrNameArg,
			unsigned short attrChangeArg);

	virtual bool isMutationEvent() { return true; }
	virtual DOM::DOMString eventModuleName() { return "MutationEvents"; }

protected:
	SVGElementImpl *m_relatedNode;
	DOM::DOMString m_prevValue;
	DOM::DOMString m_newValue;
	DOM::DOMString m_attrName;
	unsigned short m_attrChange;

public:
	KSVG_FORWARDGET
};

class SVGRegisteredEventListener
{
public:
	SVGRegisteredEventListener(SVGEvent::EventId _id, SVGEventListener *_listener, bool _useCapture);
	~SVGRegisteredEventListener();

	bool operator==(const SVGRegisteredEventListener &other);

	SVGEvent::EventId id;
	SVGEventListener *listener;
	bool useCapture;

private:
	SVGRegisteredEventListener(const SVGRegisteredEventListener &);
	SVGRegisteredEventListener &operator=(const SVGRegisteredEventListener &);
};

}

KSVG_DEFINE_PROTOTYPE(SVGEventImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGEventImplProtoFunc, SVGEventImpl)

KSVG_DEFINE_PROTOTYPE(SVGUIEventImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGUIEventImplProtoFunc, SVGUIEventImpl)

KSVG_DEFINE_PROTOTYPE(SVGMouseEventImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGMouseEventImplProtoFunc, SVGMouseEventImpl)

KSVG_DEFINE_PROTOTYPE(SVGKeyEventImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGKeyEventImplProtoFunc, SVGKeyEventImpl)

#endif

// vim:ts=4:noet
