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

#ifndef KSVGScriptInterpreter_H
#define KSVGScriptInterpreter_H

#include <qptrdict.h>

namespace KJS
{
	class Value;
	class Object;
	class Interpreter;
}

namespace KSVG
{
	class SVGEventImpl;
	class SVGDocumentImpl;
}

class KSVGScriptInterpreter : public KJS::Interpreter
{
public:
	KSVGScriptInterpreter(const KJS::Object &global, KSVG::SVGDocumentImpl *doc);
	virtual ~KSVGScriptInterpreter();

	KSVG::SVGDocumentImpl *document();

	KJS::ObjectImp *getDOMObject(void *objectHandle) const;
	void putDOMObject(void *objectHandle, KJS::ObjectImp *obj);
	void removeDOMObject(void *objectHandle);

	KSVG::SVGEventImpl *currentEvent();
	void setCurrentEvent(KSVG::SVGEventImpl *evt);

	bool attributeGetMode();
	void setAttributeGetMode(bool temp);

	bool attributeSetMode();
	void setAttributeSetMode(bool temp);

private:
	KSVG::SVGDocumentImpl *m_document;
	KSVG::SVGEventImpl *m_evt;
	
	bool m_attributeGetMode, m_attributeSetMode;
	
	QPtrDict<KJS::ObjectImp> m_domObjects;
};

#endif

// vim:ts=4:noet
