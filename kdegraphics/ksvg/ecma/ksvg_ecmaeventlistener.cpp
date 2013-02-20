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

#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"

#include "ksvg_ecma.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_ecmaeventlistener.h"

using namespace KSVG;
using namespace KJS;

KSVGEcmaEventListener::KSVGEcmaEventListener(KJS::Object _listener, QString _type, KSVGEcma *_ecma) : SVGEventListener()
{
	m_listener = _listener;
	m_remove = true;
	m_type = _type;
	m_ecma = _ecma;

	m_ecma->addEventListener(this);
}

KSVGEcmaEventListener::~KSVGEcmaEventListener()
{
	if(m_remove)
		m_ecma->removeEventListener(this);
}

void KSVGEcmaEventListener::forbidRemove()
{
	m_remove = false;	
}

void KSVGEcmaEventListener::handleEvent(SVGEventImpl *evt)
{
	if(m_ecma && m_listener.implementsCall())
	{
		KSVGScriptInterpreter *interpreter = m_ecma->interpreter();
		ExecState *exec = m_ecma->globalExec();

		// Append 'evt'
		List args;
		args.append(getDOMEvent(exec, evt));

		// Set current event
		interpreter->setCurrentEvent(evt);

		// Call it!
		Object thisObj = Object::dynamicCast(getDOMNode(exec, *evt->currentTarget()));
		Value retval = m_listener.call(exec, thisObj, args);

		interpreter->setCurrentEvent(0);
		
		if(exec->hadException())
		{
			exec->clearException();

			// onerror support
			SVGSVGElementImpl *rootElement = static_cast<KSVGScriptInterpreter *>(exec->interpreter())->document()->rootElement();
			if(rootElement)
				rootElement->dispatchEvent(SVGEvent::ERROR_EVENT, true, false);
		}
		else
		{
			QVariant ret = valueToVariant(exec, retval);
			if(ret.type() == QVariant::Bool && ret.toBool() == false)
				evt->preventDefault();
		}
	}
}

DOM::DOMString KSVGEcmaEventListener::eventListenerType()
{
	return "KSVGEcmaEventListener - " + m_type;
}

// vim:ts=4:noet
