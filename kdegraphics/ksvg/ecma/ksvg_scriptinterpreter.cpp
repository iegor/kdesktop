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

#include "SVGEventImpl.h"
#include "SVGDocumentImpl.h"
#include "ksvg_scriptinterpreter.h"

using namespace KSVG;
using namespace KJS;

KSVGScriptInterpreter::KSVGScriptInterpreter(const Object &global, SVGDocumentImpl *doc) : Interpreter(global), m_document(doc)
{
	m_evt = 0;
	
	m_attributeGetMode = false;
	m_attributeSetMode = false;
}

KSVGScriptInterpreter::~KSVGScriptInterpreter()
{
	if(m_evt)
		m_evt->deref();
}

SVGDocumentImpl *KSVGScriptInterpreter::document()
{
	return m_document;
}

KSVG::SVGEventImpl *KSVGScriptInterpreter::currentEvent()
{
	return m_evt;
}

void KSVGScriptInterpreter::setCurrentEvent(KSVG::SVGEventImpl *evt)
{
	m_evt = evt;
}

KJS::ObjectImp *KSVGScriptInterpreter::getDOMObject(void *objectHandle) const
{
	return m_domObjects[objectHandle];
}

void KSVGScriptInterpreter::putDOMObject(void *objectHandle, KJS::ObjectImp *obj)
{
	m_domObjects.insert(objectHandle, obj);
}

void KSVGScriptInterpreter::removeDOMObject(void *objectHandle)
{
	m_domObjects.take(objectHandle);
}

bool KSVGScriptInterpreter::attributeGetMode()
{
	return m_attributeGetMode;
}

bool KSVGScriptInterpreter::attributeSetMode()
{
	return m_attributeSetMode;
}

void KSVGScriptInterpreter::setAttributeGetMode(bool temp)
{
	m_attributeGetMode = temp;
}

void KSVGScriptInterpreter::setAttributeSetMode(bool temp)
{
	m_attributeSetMode = temp;
}

// vim:ts=4:noet
