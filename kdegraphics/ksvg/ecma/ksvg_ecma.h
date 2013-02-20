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

#ifndef KSVGEcma_H
#define KSVGEcma_H

#include <qptrlist.h>

#include "ksvg_bridge.h"

namespace DOM
{
	class Node;
	class DOMString;
}

namespace KJS
{
	class Value;
	class Object;
	class UString;
	class ExecState;
	class Completion;
}

namespace KSVG
{
	class Window;
	class SVGEventImpl;
	class SVGDocumentImpl;
	class SVGEventListener;
	class SVGDOMNodeBridge;
}

class QVariant;

class KSVGEcmaEventListener;
class KSVGScriptInterpreter;

typedef KSVGBridge<KSVG::SVGEventImpl> KSVGEcmaEvent;

// Helpers
namespace KSVG
{
	KJS::Value getDOMNode(KJS::ExecState *, DOM::Node);
	KJS::Value getDOMEvent(KJS::ExecState *, KSVG::SVGEventImpl *);
	KJS::Value getString(DOM::DOMString);
	
	QVariant valueToVariant(KJS::ExecState *, const KJS::Value &);
	
	DOM::Node toNode(const KJS::Value &);
	
	// This one is part of generateddata.cpp
	SVGDOMNodeBridge *toNodeBridge(const KJS::ObjectImp *);
}

class KSVGEcma
{
public:
	KSVGEcma(KSVG::SVGDocumentImpl *doc);
	~KSVGEcma();

	void setup();
	bool initialized();

	KJS::Completion evaluate(const KJS::UString &code, const KJS::Value &thisV);

	KJS::Object globalObject();
	KJS::ExecState *globalExec();

	KSVGScriptInterpreter *interpreter() { return m_interpreter; }

	KSVG::SVGEventListener *createEventListener(DOM::DOMString type);
	QString valueOfEventListener(KSVG::SVGEventListener *listener) const;
	void addEventListener(KSVGEcmaEventListener *listener);
	void removeEventListener(KSVGEcmaEventListener *listener);
	bool hasEventListeners();

	void finishedWithEvent(KSVG::SVGEventImpl *event);

	KJS::Value getUrl(KJS::ExecState *exec, ::KURL url);
	void postUrl(KJS::ExecState *exec, ::KURL url, const QString &data, const QString &mimeType, const QString &contentEncoding, KJS::Object &callBackFunction);

private:
	bool m_init, m_hasListeners;
	
	KSVG::SVGDocumentImpl *m_doc;

	KSVG::Window *m_window;
	KSVGScriptInterpreter *m_interpreter;
	QPtrList<KSVGEcmaEventListener> m_ecmaEventListeners;

};

#endif

// vim:ts=4:noet
