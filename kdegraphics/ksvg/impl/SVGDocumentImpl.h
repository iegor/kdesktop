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

#ifndef SVGDocumentImpl_H
#define SVGDocumentImpl_H

#include <kurl.h>

#include <qxml.h>
#include <qobject.h>
#include <qptrdict.h>
#include <qptrlist.h>
#include <qdatetime.h>
#include <qstringlist.h>

#include <kjs/object.h>

#include <dom/dom_doc.h>
#include <dom/dom_misc.h>
#include <dom/dom_string.h>

#include "ksvg_lookup.h"

#include "SVGEvent.h"
#include "SVGTimeScheduler.h"

class KSVGEcma;
class KSVGRequest;

namespace KJS
{
	class Value;
	class UString;
	class ExecState;
	class Identifier;
}

namespace KSVG
{

class SVGTimer;
class KSVGLoader;
class KSVGReader;
class KSVGCanvas;
class SVGImageElementImpl;
class SVGSVGElementImpl;
class SVGWindowImpl;
class SVGScriptElementImpl;
class SVGDescElementImpl;
class SVGTitleElementImpl;
class SVGUseElementImpl;
class SVGDocumentImpl : public QObject,
						public DOM::DomShared,
						public DOM::Document,
						public SVGDOMNodeBridge
{
Q_OBJECT
public:
	SVGDocumentImpl(bool anim = true, bool bFit = false, SVGImageElementImpl *parentImage = 0);
	virtual ~SVGDocumentImpl();

	float screenPixelsPerMillimeterX() const;
	float screenPixelsPerMillimeterY() const;

	DOM::DOMString title() const;
	DOM::DOMString referrer() const;
	DOM::DOMString domain() const;
	DOM::DOMString URL() const;

	void setReferrer(const DOM::DOMString &referrer);

	void setRootElement(SVGSVGElementImpl *);
	SVGSVGElementImpl *rootElement() const;

	SVGImageElementImpl *parentImage() const { return m_parentImage; }

	SVGWindowImpl *window();

	static SVGElementImpl *createElement(const DOM::DOMString &name, DOM::Element impl, SVGDocumentImpl *doc = 0);

	bool open(const KURL &url);
	void rerender();

	void attach(KSVG::KSVGCanvas *p);
	void detach();

	bool ready() { return m_finishedParsing; }

	KURL baseUrl() { return m_baseURL; }
	KSVGCanvas *canvas() const;

	// Ecma stuff
	KSVGEcma *ecmaEngine() { return m_ecmaEngine; }

	void parseSVG(QXmlInputSource *inputSource, bool getURLMode = false);

	virtual bool implementsCall() const { return true; }

	void executeScriptsRecursive(DOM::Node start);
	bool executeScriptsRecursiveCheck(DOM::Node start);

	bool dispatchRecursiveEvent(SVGEvent::EventId id, DOM::Node start);

	SVGElementImpl *getElementByIdRecursive(SVGSVGElementImpl *start, const DOM::DOMString &elementId, bool dontSearch = false);

	// Event stuff
	SVGEventListener *createEventListener(DOM::DOMString type);

	void setLastTarget(SVGElementImpl *elem) { m_lastTarget = elem; }
	SVGElementImpl *lastTarget() { return m_lastTarget; }

	// Animation stuff
	SVGTimeScheduler *timeScheduler() const { return m_timeScheduler; }

	// Internal
	SVGElementImpl *getElementFromHandle(DOM::NodeImpl *handle) const;
	void addToElemDict(DOM::NodeImpl *handle, SVGElementImpl *obj);
	void removeFromElemDict(DOM::NodeImpl *handle);

	SVGDocumentImpl *getDocumentFromHandle(DOM::NodeImpl *handle) const;
	void addToDocumentDict(DOM::NodeImpl *handle, SVGDocumentImpl *obj);

	SVGElementImpl *recursiveSearch(DOM::Node start, const DOM::DOMString &id);

	void finishParsing(bool error, const QString &errorDesc);

	void newImageJob(SVGImageElementImpl *);

	void notifyImageLoading(SVGImageElementImpl *image);
	void notifyImageLoaded(SVGImageElementImpl *image);
	void resortZIndicesOnFinishedLoading() { m_resortZIndicesOnFinishedLoading = true; }
	
	void addForwardReferencingUseElement(SVGUseElementImpl *use);
	QValueList<SVGUseElementImpl *> forwardReferencingUseElements() const { return m_forwardReferencingUseElements; }

	// Traverse the element hierarchy and update any cached matrices that are
	// no longer valid.
	void syncCachedMatrices();

public slots:
	void slotPaint();
	void executeScripts();

private slots:
	void slotSVGContent(QIODevice *);
	void slotFinishedParsing(bool error, const QString &errorDesc);

// KDE invents public signals :)
#undef signals
#define signals public
signals:
	void gotDescription(const QString &);
	void gotTitle(const QString &);
	void gotURL(const QString &);

	void finishedParsing(bool error, const QString &errorDesc);
	void finishedRendering();
	void finishedLoading();

private:	
	void checkFinishedLoading();

	bool m_finishedParsing;
	bool m_finishedLoading;
	bool m_animations;

	SVGSVGElementImpl *m_rootElement;

	SVGTimeScheduler *m_timeScheduler;

	// Set if this document is being displayed by an 'image' element reference
	// rather than as the main document, 0 otherwise.
	SVGImageElementImpl *m_parentImage;

	KSVGReader *m_reader;
	KSVGLoader *m_loader;
	KSVGCanvas *m_canvas;

	KSVGEcma *m_ecmaEngine;

	QPtrDict<SVGElementImpl> m_elemDict;
	QPtrDict<SVGDocumentImpl> m_documentDict;

	static uint elemDictHashSizes[];
	static const int numElemDictHashSizes;
	int m_elemDictHashSizeIndex;

	SVGWindowImpl *m_window;
	SVGElementImpl *m_lastTarget;

	KURL m_baseURL;

	DOM::DOMString m_referrer;

	bool m_fit;

	QTime m_t;
	
	QValueList<SVGImageElementImpl *> m_imagesLoading;
	bool m_resortZIndicesOnFinishedLoading;

	QValueList<SVGUseElementImpl *> m_forwardReferencingUseElements;

public:
	KSVG_BASECLASS_GET
	KSVG_PUT

	enum
	{
		// Properties
		Title, Referrer, Domain, Url,
		DocType, Implementation, RootElement, DocumentElement,
		// Functions
		CreateTextNode, CreateElement, CreateElementNS,
		GetElementById, GetElementsByTagName, GetElementsByTagNameNS
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
};

}

KSVG_DEFINE_PROTOTYPE(SVGDocumentImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGDocumentImplProtoFunc, SVGDocumentImpl)

#endif

// vim:ts=4:noet
