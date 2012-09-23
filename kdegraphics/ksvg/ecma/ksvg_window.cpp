/* This file is part of the KDE project
   Copyright (C) 2002 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2, as published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "ksvg_window.moc"
#include "ksvg_lookup.h"
#include "ksvg_ecma.h"
#include "ksvg_bridge.h"
#include "ksvg_scriptinterpreter.h"
#include <SVGEventImpl.h>
#include <SVGDocumentImpl.h>
#include <KSVGCanvas.h>
#include <SVGWindowImpl.h>
// for js constants
#include <SVGTransformImpl.h>
#include <SVGLengthImpl.h>
#include <SVGAngleImpl.h>
#include <SVGColorImpl.h>
#include <SVGPreserveAspectRatioImpl.h>
#include <SVGTextContentElementImpl.h>
#include <SVGPathSegImpl.h>
#include <SVGGradientElementImpl.h>
#include <SVGMarkerElementImpl.h>
#include <SVGTextPathElementImpl.h>
#include <SVGPaintImpl.h>
#include <SVGZoomAndPanImpl.h>

#include <kparts/part.h>
#include <assert.h>
#include <kdebug.h>
#include <qstylesheet.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kinputdialog.h>

using namespace KSVG;

#include "ksvg_window.lut.h"

// Spec on http://www.protocol7.com/svg-wiki/ow.asp?AdobeSVGViewerWindow

/*
@namespace KSVG
@begin KSVG::Window::s_hashTable 34
  closed		KSVG::Window::_Closed			DontDelete|ReadOnly
  window		KSVG::Window::_Window			DontDelete|ReadOnly
  evt			KSVG::Window::_Evt			DontDelete|ReadOnly
  document		KSVG::Window::_Document		DontDelete|ReadOnly
  svgDocument	KSVG::Window::_Document		DontDelete|ReadOnly
  innerWidth	KSVG::Window::_InnerWidth		DontDelete|ReadOnly
  innerHeight	KSVG::Window::_InnerHeight	DontDelete|ReadOnly
  setTimeout    KSVG::Window::_SetTimeout		DontDelete|Function 2
  clearTimeout  KSVG::Window::_ClearTimeout	DontDelete|Function 1
  setInterval   KSVG::Window::_SetInterval	DontDelete|Function 2
  clearInterval KSVG::Window::_ClearInterval	DontDelete|Function 1
  navigator	KSVG::Window::_Navigator	DontDelete|ReadOnly
  printNode		KSVG::Window::_PrintNode		DontDelete|Function 1

# todo navigator, status/defaultstatus, like in KJS::Window (khtml)
# todo close
# todo instancename
# todo setsrc, getsrc, reload, focus, blur, browsereval, findinstancebyname

# "Constructors" (usually repositories for constants)
  SVGTransform				KSVG::Window::_SVGTransform				DontDelete|ReadOnly
  SVGLength					KSVG::Window::_SVGLength					DontDelete|ReadOnly
  SVGAngle      			KSVG::Window::_SVGAngle					DontDelete|ReadOnly
  SVGPreserveAspectRatio	KSVG::Window::_SVGPreserveAspectRatio		DontDelete|ReadOnly
  SVGPathSeg      			KSVG::Window::_SVGPathSeg					DontDelete|ReadOnly
  SVGGradientElement		KSVG::Window::_SVGGradientElement			DontDelete|ReadOnly
  SVGMarkerElement			KSVG::Window::_SVGMarkerElement			DontDelete|ReadOnly
  SVGTextPathElement		KSVG::Window::_SVGTextPathElement			DontDelete|ReadOnly
  SVGPaint      			KSVG::Window::_SVGPaint					DontDelete|ReadOnly
  SVGTextContentElement 	KSVG::Window::_SVGTextContentElement		DontDelete|ReadOnly
  SVGZoomAndPan				KSVG::Window::_SVGZoomAndPan				DontDelete|ReadOnly
  SVGColor					KSVG::Window::_SVGColor					DontDelete|ReadOnly

# Functions
  alert						KSVG::Window::_Alert						DontDelete|Function 1
  prompt					KSVG::Window::_Prompt						DontDelete|Function 2
  confirm					KSVG::Window::_Confirm					DontDelete|Function 1
  debug						KSVG::Window::_Debug						DontDelete|Function 1
  success					KSVG::Window::_Success					DontDelete|Function 1
  getSVGViewerVersion		KSVG::Window::_GetSVGViewerVersion		DontDelete|Function 0
  getURL					KSVG::Window::_GetURL						DontDelete|Function 2
  postURL					KSVG::Window::_PostURL					DontDelete|Function 5
  parseXML					KSVG::Window::_ParseXML					DontDelete|Function 1
@end
*/

KSVG_IMPLEMENT_PROTOFUNC(WindowFunc, Window)

const ClassInfo KSVG::Window::s_classInfo = { "Window", 0, &s_hashTable, 0 };

KSVG::Window::Window(KSVG::SVGDocumentImpl *p) : ObjectImp(), m_doc(p)
{
	winq = new WindowQObject(this);
}

KSVG::Window::~Window()
{
	delete winq;
}

KSVG::Window *KSVG::Window::retrieveActive(ExecState *exec)
{
	ValueImp *imp = exec->interpreter()->globalObject().imp();
	assert(imp);
	return static_cast<KSVG::Window*>(imp);
}

bool KSVG::Window::hasProperty(ExecState *, const Identifier &) const
{
	return true; // See KJS::KSVG::Window::hasProperty
}

Value KSVG::Window::get(ExecState *exec, const Identifier &p) const
{
	kdDebug(26004) << "KSVG::Window (" << this << ")::get " << p.qstring() << endl;
	
	if(p == "closed")
		return Boolean(m_doc.isNull());

	// we don't want any operations on a closed window
	if(m_doc.isNull())
		return Undefined();

	// Look for overrides first
	Value val = ObjectImp::get(exec, p);
	if(!val.isA(UndefinedType))
		return isSafeScript(exec) ? val : Undefined();

	// Not the right way in the long run. Should use retrieve(m_doc)...
	KSVGScriptInterpreter* interpreter = static_cast<KSVGScriptInterpreter *>(exec->interpreter());

	const HashEntry *entry = Lookup::findEntry(&KSVG::Window::s_hashTable, p);
	if(entry)
	{
		switch(entry->value)
		{
			case _Document:
				// special case, KSVGEcma::setup created it, so we don't need to do it
				return Value(interpreter->getDOMObject(m_doc->handle()));
			case _Window:
				return Value(const_cast<Window *>(this));
			case _Evt:
			{
				KSVG::SVGEventImpl *evt = interpreter->currentEvent();
				if(evt)
					return getDOMEvent(exec, evt);
				else
					return Undefined();
			}
			case _InnerWidth:
				return Number(m_doc->canvas()->width());
			case _InnerHeight:
				return Number(m_doc->canvas()->height());
			case _SetTimeout:
			case _ClearTimeout:
			case _SetInterval:
			case _ClearInterval:
			case _PrintNode:
			{
				if(isSafeScript(exec))
					return lookupOrCreateFunction<WindowFunc>(exec, p, this, entry->value, entry->params, entry->attr);
				else
					return Undefined();
			}
			case _Alert:
			case _Confirm:
			case _Debug:
			case _Success:
			case _GetSVGViewerVersion:
			case _GetURL:
			case _PostURL:
			case _ParseXML:
			case _Prompt:
				return lookupOrCreateFunction<WindowFunc>(exec, p, this, entry->value, entry->params, entry->attr);
			case _SVGTransform:
				return getSVGTransformImplConstructor(exec);
			case _SVGLength:
				return getSVGLengthImplConstructor(exec);
			case _SVGAngle:
				return getSVGAngleImplConstructor(exec);
			case _SVGColor:
				return getSVGColorImplConstructor(exec);
			case _SVGPreserveAspectRatio:
				return getSVGPreserveAspectRatioImplConstructor(exec);
			case _SVGPathSeg:
				return getSVGPathSegImplConstructor(exec);
			case _SVGGradientElement:
				return getSVGGradientElementImplConstructor(exec);
			case _SVGMarkerElement:
				return getSVGMarkerElementImplConstructor(exec);
			case _SVGTextPathElement:
				return getSVGTextPathElementImplConstructor(exec);					
			case _SVGPaint:
				return getSVGPaintImplConstructor(exec);
			case _SVGTextContentElement:
				return getSVGTextContentElementImplConstructor(exec);
			case _SVGZoomAndPan:
				return getSVGZoomAndPanImplConstructor(exec);
		}
	}

	// This isn't necessarily a bug. Some code uses if(!window.blah) window.blah=1
	// But it can also mean something isn't loaded or implemented...
	kdDebug(26004) << "KSVG::Window::get property not found: " << p.qstring() << endl;

	return Undefined();
}

void KSVG::Window::put(ExecState* exec, const Identifier &propertyName, const Value &value, int attr)
{
	// Called by an internal KJS call (e.g. InterpreterImp's constructor) ?
	// If yes, save time and jump directly to ObjectImp.
	if((attr != None && attr != DontDelete)
		// Same thing if we have a local override (e.g. "var location")
		|| (ObjectImp::getDirect(propertyName) && isSafeScript(exec)))
	{
		ObjectImp::put( exec, propertyName, value, attr );
		return;
	}

	const HashEntry *entry = Lookup::findEntry(&KSVG::Window::s_hashTable, propertyName);
	if(entry)
	{
#ifdef KJS_VERBOSE
		kdDebug(26004) << "Window(" << this << ")::put " << propertyName.qstring() << endl;
#endif
		//switch(entry->value)
		//{
		// ...
		//}
	}
	
	if(isSafeScript(exec))
		ObjectImp::put(exec, propertyName, value, attr);
}

bool KSVG::Window::isSafeScript(ExecState *exec) const
{
	if(m_doc.isNull()) // part deleted ? can't grant access
	{
		kdDebug(26004) << "KSVG::Window::isSafeScript: accessing deleted part !" << endl;
		return false;
	}
	
	KSVG::SVGDocumentImpl *activePart = static_cast<KSVGScriptInterpreter *>(exec->interpreter())->document();
	
	if(!activePart)
	{
		kdDebug(26004) << "KSVG::Window::isSafeScript: current interpreter's part is 0L!" << endl;
		return false;
	}
	
	if(activePart == m_doc) // Not calling from another frame, no problem.
		return true;

	return false;
}

void KSVG::Window::clear(ExecState *exec)
{
	kdDebug(26004) << "KSVG::Window::clear " << this << endl;
	delete winq;
	winq = new WindowQObject(this);;
	
	// Get rid of everything, those user vars could hold references to DOM nodes
	deleteAllProperties(exec);
	
	// Really delete those properties, so that the DOM nodes get deref'ed
	// KJS::Collector::collect();
	
	// Now recreate a working global object for the next URL that will use us
	Interpreter *interpreter = exec->interpreter();
	interpreter->initGlobalObject();
}

Value WindowFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	if(!thisObj.inherits(&KSVG::Window::s_classInfo))
	{
		Object err = Error::create(exec, TypeError);
		exec->setException(err);
		return err;
	}
	
	Window *window = static_cast<Window *>(thisObj.imp());
	Value v = args[0];
	UString s = v.toString(exec);
	QString str = s.qstring();

	switch(id)
	{
		case KSVG::Window::_Alert:
			SVGWindowImpl::alert(DOM::DOMString(str), "Javascript");
			return Undefined();
		case KSVG::Window::_Confirm:
			return Boolean(SVGWindowImpl::confirm(DOM::DOMString(str), "Javascript"));
		case KSVG::Window::_Debug:
			kdDebug(26004) << "[Debug] " << str << endl;
			return Undefined();
		case KSVG::Window::_Success:
			//if(args[0].toString(exec) == "success") // ? Did you really mean that ?
			return Number(1);
			//return Undefined();
		case KSVG::Window::_GetSVGViewerVersion:
			return String("0.1"); // I cant really find a central place for the version nr, so... (Rob)
		case KSVG::Window::_ClearTimeout:
		case KSVG::Window::_ClearInterval:
			(const_cast<Window *>(window))->clearTimeout(v.toInt32(exec));
			return Undefined();
		case KSVG::Window::_PrintNode:
			return String(const_cast<Window *>(window)->doc()->window()->printNode(toNode(args[0])));
		case KSVG::Window::_GetURL:
		{
			KURL url((const_cast<Window *>(window))->doc()->baseUrl(), args[0].toString(exec).qstring());
			Value asyncStatus = (const_cast<Window *>(window))->doc()->ecmaEngine()->getUrl(exec, url);
			Object callBackFunction = Object::dynamicCast(args[1]);
			List callBackArgs;

			callBackArgs.append(asyncStatus);
			callBackFunction.call(exec, callBackFunction, callBackArgs);

			return Undefined();
		}
		case KSVG::Window::_PostURL:
		{
			KURL url((const_cast<Window *>(window))->doc()->baseUrl(), args[0].toString(exec).qstring());
			QString data = args[1].toString(exec).qstring();
			QString mimeType = args[3].toString(exec).qstring();
			QString contentEncoding = args[4].toString(exec).qstring();
			Object callBackFunction = Object::dynamicCast(args[2]);
			
			(const_cast<Window *>(window))->doc()->ecmaEngine()->postUrl(exec, url, data, mimeType, contentEncoding, callBackFunction);

			return Undefined();
		};
		case KSVG::Window::_ParseXML:
		{
			SVGDocumentImpl *doc = new SVGDocumentImpl();
			doc->ref();
			doc->attach(0);

			// So we can find it later...
			(const_cast<Window *>(window))->doc()->addToDocumentDict(doc->handle(), doc);

			// Also add the current doc to the new doc!
			SVGDocumentImpl *curDoc = (const_cast<Window *>(window))->doc();
			doc->addToDocumentDict(curDoc->handle(), curDoc);

			QXmlInputSource *svgFragment = new QXmlInputSource();
			svgFragment->setData(args[0].toString(exec).qstring());

			doc->parseSVG(svgFragment, true);

			DOM::DocumentFragment fragment = doc->createDocumentFragment();
			DOM::Node node = doc->firstChild();
			for(; !node.isNull(); node = node.nextSibling())
					fragment.appendChild(node);


//			fragment = *doc; // Should copy the nodes into here...

			return (new SVGDOMDocumentFragmentBridge(fragment))->cache(exec);
		}
		case KSVG::Window::_Prompt:
		{
		    // mop: from khtml. do we need that?
		    // part->xmlDocImpl()->updateRendering();
		    bool ok;
		    QString str2;
		    if (args.size() >= 2)
			str2 = KInputDialog::getText(i18n("Prompt"),
				QStyleSheet::convertFromPlainText(str),
				args[1].toString(exec).qstring(), &ok);
		    else
			str2 = KInputDialog::getText(i18n("Prompt"),
				QStyleSheet::convertFromPlainText(str),
				QString::null, &ok);
		    if ( ok )
			return String(str2);
		    else
			return Null();
		}
		case KSVG::Window::_SetInterval:
		if(args.size() >= 2 && v.isA(StringType))
		{
			int i = args[1].toInt32(exec);
			int r = (const_cast<Window *>(window))->installTimeout(s, i, false);
			return Number(r);
		}
		else if(args.size() >= 2 && !Object::dynamicCast(v).isNull() && Object::dynamicCast(v).implementsCall())
		{
			Value func = args[0];
			int i = args[1].toInt32(exec);
			int r = (const_cast<Window *>(window))->installTimeout(s, i, false);
			return Number(r);
		}
		else
			return Undefined();
		case KSVG::Window::_SetTimeout:
		if (args.size() == 2 && v.isA(StringType))
		{
			int i = args[1].toInt32(exec);
			int r = (const_cast<Window *>(window))->installTimeout(s, i, true /*single shot*/);
			return Number(r);
		}
		else if(args.size() >= 2 && v.isA(ObjectType) && Object::dynamicCast(v).implementsCall())
		{
			Value func = args[0];
			int i = args[1].toInt32(exec);
			int r = (const_cast<Window *>(window))->installTimeout(s, i, false);
			return Number(r);
		}
		else
			return Undefined();
	}
	
	return Undefined();
}

int KSVG::Window::installTimeout(const UString &handler, int t, bool singleShot)
{
	return winq->installTimeout(handler, t, singleShot);
}

void KSVG::Window::clearTimeout(int timerId)
{
	winq->clearTimeout(timerId);
}

////////////////////// ScheduledAction ////////////////////////

ScheduledAction::ScheduledAction(Object _func, List _args, bool _singleShot)
{
	func = _func;
	args = _args;
	isFunction = true;
	singleShot = _singleShot;
}

ScheduledAction::ScheduledAction(QString _code, bool _singleShot)
{
	code = _code;
	isFunction = false;
	singleShot = _singleShot;
}

ScheduledAction::~ScheduledAction()
{
}

void ScheduledAction::execute(Window *window)
{
	Q_ASSERT(window->doc());
	
	KSVGScriptInterpreter *interpreter = window->doc()->ecmaEngine()->interpreter();
	if(isFunction)
	{
		if(func.implementsCall())
		{
			ExecState *exec = interpreter->globalExec();
			Q_ASSERT(window == interpreter->globalObject().imp());
			Object obj(window);
			func.call(exec, obj, args); // note that call() creates its own execution state for the func call
		}
	}
	else
	{
		interpreter->evaluate(code);
		window->doc()->rerender();
	}
}

////////////////////// WindowQObject ////////////////////////

WindowQObject::WindowQObject(Window *w) : parent(w)
{
}

WindowQObject::~WindowQObject()
{
	parentDestroyed(); // reuse same code
}

void WindowQObject::parentDestroyed()
{
	killTimers();
	
	QMapIterator<int, ScheduledAction *> it;
	for(it = scheduledActions.begin(); it != scheduledActions.end(); ++it)
	{
		ScheduledAction *action = *it;
		delete action;
	}
	
	scheduledActions.clear();
}

int WindowQObject::installTimeout(const UString &handler, int t, bool singleShot)
{
	int id = startTimer(t);
	ScheduledAction *action = new ScheduledAction(handler.qstring(), singleShot);
	scheduledActions.insert(id, action);
	return id;
}

int WindowQObject::installTimeout(const Value &func, List args, int t, bool singleShot)
{
	Object objFunc = Object::dynamicCast(func);
	int id = startTimer(t);
	scheduledActions.insert(id, new ScheduledAction(objFunc, args, singleShot));
	return id;
}

void WindowQObject::clearTimeout(int timerId, bool delAction)
{
	killTimer(timerId);
	
	if(delAction)
	{
		QMapIterator<int, ScheduledAction *> it = scheduledActions.find(timerId);
		if(it != scheduledActions.end())
		{
			ScheduledAction *action = *it;
			scheduledActions.remove(it);
			delete action;
		}
	}
}

void WindowQObject::timerEvent(QTimerEvent *e)
{
	QMapIterator<int, ScheduledAction *> it = scheduledActions.find(e->timerId());
	if(it != scheduledActions.end())
	{
		ScheduledAction *action = *it;
		bool singleShot = action->singleShot;

		// remove single shots installed by setTimeout()
		if(singleShot)
		{
			clearTimeout(e->timerId(), false);
			scheduledActions.remove(it);
		}
		
		if(parent->doc())
			action->execute(parent);
		
		// It is important to test singleShot and not action->singleShot here - the
		// action could have been deleted already if not single shot and if the
		// JS code called by execute() calls clearTimeout().
		if(singleShot)
			delete action;
	}
	else
		kdWarning(6070) << "WindowQObject::timerEvent this=" << this << " timer " << e->timerId() << " not found (" << scheduledActions.count() << " actions in map)" << endl;
}

void WindowQObject::timeoutClose()
{
}
