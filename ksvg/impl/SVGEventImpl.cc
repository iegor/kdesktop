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

#include "SVGDocumentImpl.h"
#include "SVGEventImpl.h"

#include <kdebug.h>

using namespace KSVG;

#include "ksvg_scriptinterpreter.h"
#include "SVGEventImpl.lut.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGEventImpl::SVGEventImpl()
{
	m_canBubble = false;
	m_cancelable = false;

	m_propagationStopped = false;
	m_defaultPrevented = false;
	m_id = SVGEvent::UNKNOWN_EVENT;
	m_eventPhase = 0;
	m_createTime = QDateTime::currentDateTime();
	m_defaultHandled = false;

	m_target = 0;
	m_currentTarget = 0;
}

SVGEventImpl::SVGEventImpl(SVGEvent::EventId _id, bool canBubbleArg, bool cancelableArg)
{
	DOM::DOMString t = SVGEvent::idToType(_id);
	m_type = t.implementation();

	m_canBubble = canBubbleArg;
	m_cancelable = cancelableArg;

	m_propagationStopped = false;
	m_defaultPrevented = false;
	m_id = _id;
	m_eventPhase = 0;
	m_createTime = QDateTime::currentDateTime();
	m_defaultHandled = false;

	m_target = 0;
	m_currentTarget = 0;
}

SVGEventImpl::~SVGEventImpl()
{
}

DOM::DOMString SVGEventImpl::type() const
{
	return m_type;
}

SVGElementImpl *SVGEventImpl::target() const
{
	return m_target;
}

void SVGEventImpl::setTarget(SVGElementImpl *_target)
{
	m_target = _target;
}

SVGElementImpl *SVGEventImpl::currentTarget() const
{
	return m_currentTarget;
}

void SVGEventImpl::setCurrentTarget(SVGElementImpl *_currentTarget)
{
	m_currentTarget = _currentTarget;
}

unsigned short SVGEventImpl::eventPhase() const
{
	return m_eventPhase;
}

void SVGEventImpl::setEventPhase(unsigned short _eventPhase)
{
	m_eventPhase = _eventPhase;
}

bool SVGEventImpl::bubbles() const
{
	return m_canBubble;
}

bool SVGEventImpl::cancelable() const
{
	return m_cancelable;
}

DOM::DOMTimeStamp SVGEventImpl::timeStamp()
{
	QDateTime epoch(QDate(1970, 1, 1), QTime(0, 0));

	// ### kjs does not yet support long long (?) so the value wraps around
	return epoch.secsTo(m_createTime) * 1000 + m_createTime.time().msec();
}

void SVGEventImpl::stopPropagation()
{
	m_propagationStopped = true;
}

void SVGEventImpl::preventDefault()
{
	if(m_cancelable)
		m_defaultPrevented = true;
}

void SVGEventImpl::initEvent(const DOM::DOMString &eventTypeArg, bool canBubbleArg, bool cancelableArg)
{
	// ### ensure this is not called after we have been dispatched (also for subclasses)
	m_type = eventTypeArg.implementation();
	m_id = SVGEvent::typeToId(eventTypeArg);

	m_canBubble = canBubbleArg;
	m_cancelable = cancelableArg;
}

void SVGEventImpl::setDefaultHandled()
{
	m_defaultHandled = true;
}

/*
@namespace KSVG
@begin SVGEventImpl::s_hashTable 11
 type	SVGEventImpl::Type	DontDelete|ReadOnly
 target	SVGEventImpl::Target	DontDelete|ReadOnly
 currentTarget	SVGEventImpl::CurrentTarget	DontDelete|ReadOnly
 eventPhase	SVGEventImpl::EventPhase	DontDelete|ReadOnly
 bubbles	SVGEventImpl::Bubbles	DontDelete|ReadOnly
 cancelable	SVGEventImpl::Cancelable	DontDelete|ReadOnly
 timeStamp	SVGEventImpl::TimeStamp	DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGEventImplProto::s_hashTable 13
 getType	SVGEventImpl::GetType	DontDelete|Function 0
 getTarget	SVGEventImpl::GetTarget	DontDelete|Function 0
 getCurrentTarget	SVGEventImpl::GetCurrentTarget	DontDelete|Function 0
 getCurrentNode	SVGEventImpl::GetCurrentNode	DontDelete|Function 0
 getEventphase	SVGEventImpl::GetEventPhase	DontDelete|Function 0
 getBubbles	SVGEventImpl::GetBubbles	DontDelete|Function 0
 getCancelable	SVGEventImpl::GetCancelable	DontDelete|Function 0
 getTimeStamp	SVGEventImpl::GetTimeStamp	DontDelete|Function 0
 stopPropagation	SVGEventImpl::StopPropagation	DontDelete|Function 0
 preventDefault	SVGEventImpl::PreventDefault	DontDelete|Function 0
 initEvent	SVGEventImpl::InitEvent	DontDelete|Function 3
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGEvent", SVGEventImplProto, SVGEventImplProtoFunc)

Value SVGEventImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case Type:
			return String(type());
		case Target:
			return getDOMNode(exec, *target());
		case CurrentTarget:
			return getDOMNode(exec, *currentTarget());
		case EventPhase:
			return Number(eventPhase());
		case Bubbles:
			return Boolean(bubbles());
		case Cancelable:
			return Boolean(cancelable());
//		case TimeStamp: // TODO
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return KJS::Undefined();
	}
}

Value SVGEventImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGEventImpl)

	switch(id)
	{
		case SVGEventImpl::GetType:
			return String(obj->type());
		case SVGEventImpl::GetTarget:
		return getDOMNode(exec, *obj->target());
		case SVGEventImpl::GetCurrentTarget:
		case SVGEventImpl::GetCurrentNode:
			return getDOMNode(exec, *obj->currentTarget());
		case SVGEventImpl::GetEventPhase:
			return Number(obj->eventPhase());
		case SVGEventImpl::GetBubbles:
			return Boolean(obj->bubbles());
		case SVGEventImpl::GetCancelable:
			return Boolean(obj->cancelable());
//		case SVGEventImpl::GetTimeStamp: // TODO
		case SVGEventImpl::StopPropagation:
		{
			obj->stopPropagation();
			return Undefined();
		}
		case SVGEventImpl::PreventDefault:
		{
			obj->preventDefault();
			return Undefined();
		}
		case SVGEventImpl::InitEvent:
		{
			obj->initEvent(args[0].toString(exec).string(), args[1].toBoolean(exec), args[2].toBoolean(exec));
			return Undefined();
		}
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}





SVGUIEventImpl::SVGUIEventImpl() : SVGEventImpl()
{
	m_detail = 0;
}

SVGUIEventImpl::SVGUIEventImpl(SVGEvent::EventId _id, bool canBubbleArg, bool cancelableArg, DOM::AbstractView &viewArg, long detailArg)
: SVGEventImpl(_id, canBubbleArg, cancelableArg)
{
	m_view = viewArg;
	m_detail = detailArg;
}

SVGUIEventImpl::~SVGUIEventImpl()
{
}

DOM::AbstractView SVGUIEventImpl::view() const
{
	return m_view;
}

long SVGUIEventImpl::detail() const
{
	return m_detail;
}

void SVGUIEventImpl::initUIEvent(const DOM::DOMString &typeArg,
		bool canBubbleArg,
		bool cancelableArg,
		const DOM::AbstractView &viewArg,
		long detailArg)
{
	SVGEventImpl::initEvent(typeArg, canBubbleArg, cancelableArg);

	m_view = viewArg;
	m_detail = detailArg;
}

/*
@namespace KSVG
@begin SVGUIEventImpl::s_hashTable 3
 view	SVGUIEventImpl::View	DontDelete|ReadOnly
 detail	SVGUIEventImpl::Detail	DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGUIEventImplProto::s_hashTable 5
 getView		SVGUIEventImpl::GetView	DontDelete|Function 0
 getDetail		SVGUIEventImpl::GetDetail	DontDelete|Function 0
 initUIEvent	SVGUIEventImpl::InitUIEvent	DontDelete|Function 5
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGUIEvent", SVGUIEventImplProto, SVGUIEventImplProtoFunc)

Value SVGUIEventImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
//		case View: // TODO
		case Detail:
			return Number(detail());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return KJS::Undefined();
	}
}

Value SVGUIEventImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &)
{
	KSVG_CHECK_THIS(SVGUIEventImpl)

	switch(id)
	{
//		case SVGUIEventImpl::GetView: // TODO
		case SVGUIEventImpl::GetDetail:
			return Number(obj->detail());
//		case SVGUIEventImpl::InitUIEvent: // TODO
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

SVGKeyEventImpl::SVGKeyEventImpl() : SVGUIEventImpl()
{
	qKeyEvent = 0;
}

SVGKeyEventImpl::SVGKeyEventImpl(QKeyEvent *key, DOM::AbstractView &view, SVGEvent::EventId _id) : SVGUIEventImpl(_id, true, true, view, 0)
{
	qKeyEvent = new QKeyEvent(key->type(), key->key(), key->ascii(), key->state(), key->text(), key->isAutoRepeat(), key->count());

	// Events are supposed to be accepted by default in Qt!
	// This line made QLineEdit's keyevents be ignored, so they were sent to the khtmlview
	// (and e.g. space would make it scroll down)
	//qKeyEvent->ignore();

	m_detail = key->count();

	m_numPad = false;
	m_keyVal = 0;
	m_virtKeyVal = DOM_VK_UNDEFINED;
	m_inputGenerated = true;

	switch(key->key())
	{
		case Qt::Key_Enter:
			m_numPad = true;
			/* fall through */
		case Qt::Key_Return:
			m_virtKeyVal = DOM_VK_ENTER;
			break;
		case Qt::Key_NumLock:
			m_numPad = true;
			m_virtKeyVal = DOM_VK_NUM_LOCK;
			break;
		case Qt::Key_Alt:
			m_virtKeyVal = DOM_VK_RIGHT_ALT;
			// ### DOM_VK_LEFT_ALT;
			break;
		case Qt::Key_Control:
			m_virtKeyVal = DOM_VK_LEFT_CONTROL;
			// ### DOM_VK_RIGHT_CONTROL
			break;
		case Qt::Key_Shift:
			m_virtKeyVal = DOM_VK_LEFT_SHIFT;
			// ### DOM_VK_RIGHT_SHIFT
			break;
		case Qt::Key_Meta:
			m_virtKeyVal = DOM_VK_LEFT_META;
			// ### DOM_VK_RIGHT_META
			break;
		case Qt::Key_CapsLock:
			m_virtKeyVal = DOM_VK_CAPS_LOCK;
			break;
		case Qt::Key_Delete:
			m_virtKeyVal = DOM_VK_DELETE;
			break;
		case Qt::Key_End:
			m_virtKeyVal = DOM_VK_END;
			break;
		case Qt::Key_Escape:
			m_virtKeyVal = DOM_VK_ESCAPE;
			break;
		case Qt::Key_Home:
			m_virtKeyVal = DOM_VK_HOME;
			break;
		case Qt::Key_Insert:
			m_virtKeyVal = DOM_VK_INSERT;
			break;
		case Qt::Key_Pause:
			m_virtKeyVal = DOM_VK_PAUSE;
			break;
		case Qt::Key_Print:
			m_virtKeyVal = DOM_VK_PRINTSCREEN;
			break;
		case Qt::Key_ScrollLock:
			m_virtKeyVal = DOM_VK_SCROLL_LOCK;
			break;
		case Qt::Key_Left:
			m_virtKeyVal = DOM_VK_LEFT;
			break;
		case Qt::Key_Right:
			m_virtKeyVal = DOM_VK_RIGHT;
			break;
		case Qt::Key_Up:
			m_virtKeyVal = DOM_VK_UP;
			break;
		case Qt::Key_Down:
			m_virtKeyVal = DOM_VK_DOWN;
			break;
		case Qt::Key_Next:
			m_virtKeyVal = DOM_VK_PAGE_DOWN;
			break;
		case Qt::Key_Prior:
			m_virtKeyVal = DOM_VK_PAGE_UP;
			break;
		case Qt::Key_F1:
			m_virtKeyVal = DOM_VK_F1;
			break;
		case Qt::Key_F2:
			m_virtKeyVal = DOM_VK_F2;
			break;
		case Qt::Key_F3:
			m_virtKeyVal = DOM_VK_F3;
			break;
		case Qt::Key_F4:
			m_virtKeyVal = DOM_VK_F4;
			break;
		case Qt::Key_F5:
			m_virtKeyVal = DOM_VK_F5;
			break;
		case Qt::Key_F6:
			m_virtKeyVal = DOM_VK_F6;
			break;
		case Qt::Key_F7:
			m_virtKeyVal = DOM_VK_F7;
			break;
		case Qt::Key_F8:
			m_virtKeyVal = DOM_VK_F8;
			break;
		case Qt::Key_F9:
			m_virtKeyVal = DOM_VK_F9;
			break;
		case Qt::Key_F10:
			m_virtKeyVal = DOM_VK_F10;
			break;
		case Qt::Key_F11:
			m_virtKeyVal = DOM_VK_F11;
			break;
		case Qt::Key_F12:
			m_virtKeyVal = DOM_VK_F12;
			break;
		case Qt::Key_F13:
			m_virtKeyVal = DOM_VK_F13;
			break;
		case Qt::Key_F14:
			m_virtKeyVal = DOM_VK_F14;
			break;
		case Qt::Key_F15:
			m_virtKeyVal = DOM_VK_F15;
			break;
		case Qt::Key_F16:
			m_virtKeyVal = DOM_VK_F16;
			break;
		case Qt::Key_F17:
			m_virtKeyVal = DOM_VK_F17;
			break;
		case Qt::Key_F18:
			m_virtKeyVal = DOM_VK_F18;
			break;
		case Qt::Key_F19:
			m_virtKeyVal = DOM_VK_F19;
			break;
		case Qt::Key_F20:
			m_virtKeyVal = DOM_VK_F20;
			break;
		case Qt::Key_F21:
			m_virtKeyVal = DOM_VK_F21;
			break;
		case Qt::Key_F22:
			m_virtKeyVal = DOM_VK_F22;
			break;
		case Qt::Key_F23:
			m_virtKeyVal = DOM_VK_F23;
			break;
		case Qt::Key_F24:
			m_virtKeyVal = DOM_VK_F24;
			break;
		default:
			m_virtKeyVal = DOM_VK_UNDEFINED;
			break;
	}

	// m_keyVal should contain the unicode value
	// of the pressed key if available.
	if (!key->text().isNull())
		m_keyVal = key->text().unicode()[0];

	//  m_numPad = ???

	// key->state returns enum ButtonState, which is ShiftButton, ControlButton and AltButton or'ed together.
	m_modifier = key->state();

	// key->text() returns the unicode sequence as a QString
	m_outputString = DOM::DOMString(key->text());
}

SVGKeyEventImpl::SVGKeyEventImpl(SVGEvent::EventId _id,
		bool canBubbleArg,
		bool cancelableArg,
		DOM::AbstractView &viewArg,
		unsigned short detailArg,
		DOM::DOMString &outputStringArg,
		unsigned long keyValArg,
		unsigned long virtKeyValArg,
		bool inputGeneratedArg,
		bool numPadArg)
: SVGUIEventImpl(_id, canBubbleArg, cancelableArg, viewArg, detailArg)
{
	qKeyEvent = 0;
	m_keyVal = keyValArg;
	m_virtKeyVal = virtKeyValArg;
	m_inputGenerated = inputGeneratedArg;
	m_outputString = outputStringArg;
	m_numPad = numPadArg;
	m_modifier = 0;
}

SVGKeyEventImpl::~SVGKeyEventImpl()
{
	delete qKeyEvent;
}

bool SVGKeyEventImpl::checkModifier(unsigned long modifierArg)
{
	return ((m_modifier && modifierArg) == modifierArg);
}

void SVGKeyEventImpl::initKeyEvent(DOM::DOMString &typeArg,
		bool canBubbleArg,
		bool cancelableArg,
		const DOM::AbstractView &viewArg,
		long detailArg,
		DOM::DOMString &outputStringArg,
		unsigned long keyValArg,
		unsigned long virtKeyValArg,
		bool inputGeneratedArg,
		bool numPadArg)
{
	SVGUIEventImpl::initUIEvent(typeArg, canBubbleArg, cancelableArg, viewArg, detailArg);

	m_outputString = outputStringArg;
	m_keyVal = keyValArg;
	m_virtKeyVal = virtKeyValArg;
	m_inputGenerated = inputGeneratedArg;
	m_numPad = numPadArg;
}

void SVGKeyEventImpl::initModifier(unsigned long modifierArg, bool valueArg)
{
	if(valueArg)
		m_modifier |= modifierArg;
	else
		m_modifier &= (modifierArg ^ 0xFFFFFFFF);
}

bool SVGKeyEventImpl::inputGenerated() const
{
	return m_inputGenerated;
}

unsigned long SVGKeyEventImpl::keyVal() const
{
	return m_keyVal;
}

DOM::DOMString SVGKeyEventImpl::outputString() const
{
	return m_outputString;
}

/*
@namespace KSVG
@begin SVGKeyEventImpl::s_hashTable 7
 keyVal			SVGKeyEventImpl::KeyVal			DontDelete|ReadOnly
 keyCode			SVGKeyEventImpl::KeyVal			DontDelete|ReadOnly
 charCode			SVGKeyEventImpl::KeyVal			DontDelete|ReadOnly
 outputString	SVGKeyEventImpl::OutputString	DontDelete|ReadOnly
 virtKeyVal		SVGKeyEventImpl::VirtKeyVal		DontDelete|ReadOnly
# todo visibleOutputGenerated numPad
@end
@namespace KSVG
@begin SVGKeyEventImplProto::s_hashTable 7
 checkModifier			SVGKeyEventImpl::CheckModifier		DontDelete|Function 1
 getKeyCode				SVGKeyEventImpl::GetKeyVal			DontDelete|Function 0
 getCharCode			SVGKeyEventImpl::GetKeyVal			DontDelete|Function 0
 getKeyVal				SVGKeyEventImpl::GetKeyVal			DontDelete|Function 0
 getCharCode			SVGKeyEventImpl::GetCharCode		DontDelete|Function 0
# todo initModifier
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGKeyEvent", SVGKeyEventImplProto, SVGKeyEventImplProtoFunc)

Value SVGKeyEventImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case KeyVal:
			return Number(keyVal());
		case VirtKeyVal:
			return Number(virtKeyVal());
		case OutputString:
			return String(outputString());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

Value SVGKeyEventImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGKeyEventImpl)

	switch(id)
	{
		case SVGKeyEventImpl::CheckModifier:
			return Boolean((static_cast<KSVGBridge<SVGKeyEventImpl> *>(static_cast<ObjectImp *>(thisObj.imp()))->impl())->checkModifier(args[0].toUInt32(exec)));
		case SVGKeyEventImpl::GetKeyVal:
		case SVGKeyEventImpl::GetCharCode:
			return Number((static_cast<KSVGBridge<SVGKeyEventImpl> *>(static_cast<ObjectImp *>(thisObj.imp()))->impl())->keyVal());
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}



// -----------------------------------------------------------------------------

SVGMouseEventImpl::SVGMouseEventImpl() : SVGUIEventImpl()
{
	m_screenX = 0;
	m_screenY = 0;
	m_clientX = 0;
	m_clientY = 0;
	m_ctrlKey = false;
	m_altKey = false;
	m_shiftKey = false;
	m_metaKey = false;
	m_button = 0;
}

SVGMouseEventImpl::SVGMouseEventImpl(SVGEvent::EventId _id,
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
		SVGElementImpl *relatedTargetArg)
: SVGUIEventImpl(_id, canBubbleArg, cancelableArg, viewArg, detailArg)
{
	m_screenX = screenXArg;
	m_screenY = screenYArg;
	m_clientX = clientXArg;
	m_clientY = clientYArg;
	m_ctrlKey = ctrlKeyArg;
	m_altKey = altKeyArg;
	m_shiftKey = shiftKeyArg;
	m_metaKey = metaKeyArg;
	m_button = buttonArg;
	m_relatedTarget = relatedTargetArg;
}

SVGMouseEventImpl::~SVGMouseEventImpl()
{
}

long SVGMouseEventImpl::screenX() const
{
	return m_screenX;
}

long SVGMouseEventImpl::screenY() const
{
	return m_screenY;
}

long SVGMouseEventImpl::clientX() const
{
	return m_clientX;
}

long SVGMouseEventImpl::clientY() const
{
	return m_clientY;
}

bool SVGMouseEventImpl::ctrlKey() const
{
	return m_ctrlKey;
}

bool SVGMouseEventImpl::shiftKey() const
{
	return m_shiftKey;
}

bool SVGMouseEventImpl::altKey() const
{
	return m_altKey;
}

bool SVGMouseEventImpl::metaKey() const
{
	return m_metaKey;
}

unsigned short SVGMouseEventImpl::button() const
{
	return m_button;
}

SVGElementImpl *SVGMouseEventImpl::relatedTarget() const
{
	return m_relatedTarget;
}

DOM::DOMString SVGMouseEventImpl::url() const
{
	return m_url;
}

void SVGMouseEventImpl::setURL(DOM::DOMString url)
{
	m_url = url;
}

void SVGMouseEventImpl::initMouseEvent(const DOM::DOMString &typeArg,
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
		SVGElementImpl *relatedTargetArg)
{
	SVGUIEventImpl::initUIEvent(typeArg, canBubbleArg, cancelableArg, viewArg, detailArg);

	m_screenX = screenXArg;
	m_screenY = screenYArg;
	m_clientX = clientXArg;
	m_clientY = clientYArg;
	m_ctrlKey = ctrlKeyArg;
	m_altKey = altKeyArg;
	m_shiftKey = shiftKeyArg;
	m_metaKey = metaKeyArg;
	m_button = buttonArg;
	m_relatedTarget = relatedTargetArg;
}

/*
@namespace KSVG
@begin SVGMouseEventImpl::s_hashTable 11
 screenX	SVGMouseEventImpl::ScreenX	DontDelete|ReadOnly
 screenY	SVGMouseEventImpl::ScreenY	DontDelete|ReadOnly
 clientX	SVGMouseEventImpl::ClientX	DontDelete|ReadOnly
 clientY	SVGMouseEventImpl::ClientY	DontDelete|ReadOnly
 ctrlKey	SVGMouseEventImpl::CtrlKey	DontDelete|ReadOnly
 shiftKey	SVGMouseEventImpl::ShiftKey	DontDelete|ReadOnly
 altKey	SVGMouseEventImpl::AltKey	DontDelete|ReadOnly
 metaKey	SVGMouseEventImpl::MetaKey	DontDelete|ReadOnly
 button	SVGMouseEventImpl::Button	DontDelete|ReadOnly
 relatedTarget	SVGMouseEventImpl::RelatedTarget	DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGMouseEventImplProto::s_hashTable 13
 getScreenX	SVGMouseEventImpl::GetScreenX	DontDelete|Function 0
 getScreenY	SVGMouseEventImpl::GetScreenY	DontDelete|Function 0
 getClientX	SVGMouseEventImpl::GetClientX	DontDelete|Function 0
 getClientY	SVGMouseEventImpl::GetClientY	DontDelete|Function 0
 getCtrlKey	SVGMouseEventImpl::GetCtrlKey	DontDelete|Function 0
 getShiftKey	SVGMouseEventImpl::GetShiftKey	DontDelete|Function 0
 getAltKey	SVGMouseEventImpl::GetAltKey	DontDelete|Function 0
 getMetaKey	SVGMouseEventImpl::GetMetaKey	DontDelete|Function 0
 getButton	SVGMouseEventImpl::GetButton	DontDelete|Function 0
 getRelatedTarget	SVGMouseEventImpl::GetRelatedTarget	DontDelete|Function 0
 initMouseEvent	SVGMouseEventImpl::InitMouseEvent	DontDelete|Function 15
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGMouseEvent", SVGMouseEventImplProto, SVGMouseEventImplProtoFunc)

Value SVGMouseEventImpl::getValueProperty(ExecState *exec, int token) const
{
	kdDebug(26004) << k_funcinfo << endl;
	switch(token)
	{
		case ScreenX:
			return Number(screenX());
		case ScreenY:
			return Number(screenY());
		case ClientX:
			return Number(clientX());
		case ClientY:
			return Number(clientY());
		case CtrlKey:
			return Number(ctrlKey());
		case ShiftKey:
			return Number(shiftKey());
		case AltKey:
			return Number(altKey());
		case MetaKey:
			return Number(metaKey());
		case Button:
			return Number(button());
		case RelatedTarget:
			return getDOMNode(exec, *relatedTarget());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return KJS::Undefined();
	}
}

Value SVGMouseEventImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &)
{
	kdDebug(26004) << k_funcinfo << endl;
	KSVG_CHECK_THIS(SVGMouseEventImpl)

	switch(id)
	{
		case SVGMouseEventImpl::GetScreenX:
			return Number(obj->screenX());
		case SVGMouseEventImpl::GetScreenY:
			return Number(obj->screenY());
		case SVGMouseEventImpl::GetClientX:
			return Number(obj->clientX());
		case SVGMouseEventImpl::GetClientY:
			return Number(obj->clientY());
		case SVGMouseEventImpl::GetCtrlKey:
			return Number(obj->ctrlKey());
		case SVGMouseEventImpl::GetShiftKey:
			return Number(obj->shiftKey());
		case SVGMouseEventImpl::GetAltKey:
			return Number(obj->altKey());
		case SVGMouseEventImpl::GetMetaKey:
			return Number(obj->metaKey());
		case SVGMouseEventImpl::GetButton:
			return Number(obj->button());
		case SVGMouseEventImpl::GetRelatedTarget:
			return getDOMNode(exec, *obj->relatedTarget());
//		case SVGMouseEventImpl::InitMouseEvent: // TODO
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}




SVGMutationEventImpl::SVGMutationEventImpl() : SVGEventImpl()
{
	m_attrChange = 0;
}

SVGMutationEventImpl::SVGMutationEventImpl(SVGEvent::EventId _id,
		bool canBubbleArg,
		bool cancelableArg,
		SVGElementImpl *relatedNodeArg,
		const DOM::DOMString &prevValueArg,
		const DOM::DOMString &newValueArg,
		const DOM::DOMString &attrNameArg,
		unsigned short attrChangeArg)
: SVGEventImpl(_id, canBubbleArg, cancelableArg)
{
	m_relatedNode = relatedNodeArg;
	m_prevValue = prevValueArg.implementation();
	m_newValue = newValueArg.implementation();
	m_attrName = attrNameArg.implementation();
	m_attrChange = attrChangeArg;
}

SVGMutationEventImpl::~SVGMutationEventImpl()
{
}

SVGElementImpl *SVGMutationEventImpl::relatedNode() const
{
	return m_relatedNode;
}

DOM::DOMString SVGMutationEventImpl::prevValue() const
{
	return m_prevValue;
}

DOM::DOMString SVGMutationEventImpl::newValue() const
{
	return m_newValue;
}

DOM::DOMString SVGMutationEventImpl::attrName() const
{
	return m_attrName;
}

unsigned short SVGMutationEventImpl::attrChange() const
{
	return m_attrChange;
}

void SVGMutationEventImpl::initMutationEvent(const DOM::DOMString &typeArg,
		bool canBubbleArg,
		bool cancelableArg,
		SVGElementImpl *relatedNodeArg,
		const DOM::DOMString &prevValueArg,
		const DOM::DOMString &newValueArg,
		const DOM::DOMString &attrNameArg,
		unsigned short attrChangeArg)
{
	SVGEventImpl::initEvent(typeArg, canBubbleArg, cancelableArg);

	m_relatedNode = relatedNodeArg;
	m_prevValue = prevValueArg.implementation();
	m_newValue = newValueArg.implementation();
	m_attrName = attrNameArg.implementation();
	m_attrChange = attrChangeArg;
}





SVGRegisteredEventListener::SVGRegisteredEventListener(SVGEvent::EventId _id, SVGEventListener *_listener, bool _useCapture)
{
	id = _id;
	listener = _listener;
	useCapture = _useCapture;

	listener->ref();
}

SVGRegisteredEventListener::~SVGRegisteredEventListener()
{
	listener->deref();
}

bool SVGRegisteredEventListener::operator==(const SVGRegisteredEventListener &other)
{
	return (id == other.id &&
			listener == other.listener &&
			useCapture == other.useCapture);
}

// vim:ts=4:noet
