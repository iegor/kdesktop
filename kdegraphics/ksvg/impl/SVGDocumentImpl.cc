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

#include <kdebug.h>

#define USE_VALGRIND 0

#if USE_VALGRIND
#include <valgrind/calltree.h>
#endif

#include "SVGEvent.h"
#include "SVGMatrixImpl.h"
#include "SVGWindowImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.moc"
#include "SVGSVGElementImpl.h"
#include "SVGImageElementImpl.h"
#include "SVGScriptElementImpl.h"
#include "SVGTitleElementImpl.h"
#include "SVGAnimationElementImpl.h"

#include "KSVGReader.h"
#include "KSVGLoader.h"
#include "KSVGCanvas.h"
#include "CanvasItem.h"

#include <qpaintdevicemetrics.h>

using namespace KSVG;

#include "SVGDocumentImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

// A sequence of prime numbers that sets the m_elemDict's hash table size as the
// number of elements in the dictionary passes each level. This keeps the lookup
// performance high as the number of elements grows. See the QDict documentation.
unsigned int SVGDocumentImpl::elemDictHashSizes [] =
{
	101,
	211,
	401,
	809,
	1601,
	3203,
	6421,
	12809,
	25601,
	51203,
	102407,
	204803,
	409609,
	819229
};

const int SVGDocumentImpl::numElemDictHashSizes = sizeof(elemDictHashSizes) / sizeof(elemDictHashSizes[0]);

SVGDocumentImpl::SVGDocumentImpl(bool anim, bool fit, SVGImageElementImpl *parentImage) : QObject(), DOM::DomShared(), DOM::Document(), SVGDOMNodeBridge(static_cast<DOM::Node>(*this))
{
	m_animations = anim;

	m_reader = 0;
	m_loader = 0;
	m_canvas = 0;
	m_rootElement = 0;
	m_lastTarget = 0;
	m_window = 0;

	m_elemDictHashSizeIndex = 0;
	m_elemDict.resize(elemDictHashSizes[m_elemDictHashSizeIndex]);

	m_timeScheduler = new SVGTimeScheduler(this);
	m_ecmaEngine = new KSVGEcma(this);
	m_ecmaEngine->setup();

	m_finishedParsing = false;
	m_finishedLoading = false;
	m_resortZIndicesOnFinishedLoading = false;
	m_fit = fit;

	m_parentImage = parentImage;
	if(m_parentImage)
		m_parentImage->ref();
}

SVGDocumentImpl::~SVGDocumentImpl()
{
	if(rootElement() && rootElement()->hasEventListener(SVGEvent::UNLOAD_EVENT, true))
		rootElement()->dispatchEvent(SVGEvent::UNLOAD_EVENT, false, false);

	QPtrList<SVGShapeImpl> killList;

	DOM::Node node = firstChild();
	for(; !node.isNull(); node = node.nextSibling())
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(getElementFromHandle(node.handle()));
		if(shape)
			killList.append(shape);
	}

	SVGShapeImpl *rend = 0;
	for(rend = killList.first(); rend; rend = killList.next())
		delete rend;

	delete m_timeScheduler;
	delete m_ecmaEngine;
	delete m_reader;
	delete m_loader;

	if(m_window)
		m_window->deref();

	if(m_parentImage)
		m_parentImage->deref();
}

SVGWindowImpl *SVGDocumentImpl::window()
{
	if(!m_window)
	{
		m_window = new SVGWindowImpl(const_cast<SVGDocumentImpl *>(this));
		m_window->ref();
	}

	return m_window;
}

float SVGDocumentImpl::screenPixelsPerMillimeterX() const
{
	if(canvas() && canvas()->drawWindow())
	{
		QPaintDeviceMetrics metrics(canvas()->drawWindow());
		return float(metrics.width()) / float(metrics.widthMM());
	}
	else
		return 90.0;
}

float SVGDocumentImpl::screenPixelsPerMillimeterY() const
{
	if(canvas() && canvas()->drawWindow())
	{
		QPaintDeviceMetrics metrics(canvas()->drawWindow());
		return float(metrics.height()) / float(metrics.heightMM());
	}
	else
		return 90.0;
}

DOM::DOMString SVGDocumentImpl::title() const
{
	DOM::Node n;
	for(n = rootElement()->firstChild(); !n.isNull(); n = n.nextSibling())
	{
		SVGElementImpl *elem = getElementFromHandle(n.handle());
		if(dynamic_cast<SVGTitleElementImpl *>(elem))
			return elem->collectText();
	}
	return "";
}

DOM::DOMString SVGDocumentImpl::referrer() const
{
	return m_referrer;
}

DOM::DOMString SVGDocumentImpl::domain() const
{
	return m_baseURL.host();
}

DOM::DOMString SVGDocumentImpl::URL() const
{
	return m_baseURL.prettyURL();
}

void SVGDocumentImpl::setReferrer(const DOM::DOMString &referrer)
{
	// TODO : better may be to request for referrer instead of storing it
	m_referrer = referrer;
}

void SVGDocumentImpl::setRootElement(SVGSVGElementImpl *elem)
{
	m_rootElement = elem;
}

SVGSVGElementImpl *SVGDocumentImpl::rootElement() const
{
	return m_rootElement;
}

SVGElementImpl *SVGDocumentImpl::createElement(const DOM::DOMString &name, DOM::Element impl, SVGDocumentImpl *doc)
{
	DOM::ElementImpl *handle = reinterpret_cast<DOM::ElementImpl *>(impl.handle());
	SVGElementImpl *element = SVGElementImpl::Factory::self()->create(std::string(name.string().latin1()), handle);

	if(!element)
		element = new SVGElementImpl(handle);

	element->setOwnerDoc(doc);
	element->ref();
	return element;
}

bool SVGDocumentImpl::open(const ::KURL &url)
{
	if(!url.prettyURL().isEmpty())
	{
		m_baseURL = url;

		if(!m_loader)
			m_loader = new KSVGLoader();

		connect(m_loader, SIGNAL(gotResult(QIODevice *)), this, SLOT(slotSVGContent(QIODevice *)));
		m_loader->getSVGContent(url);
	}
	else
		return false;

	return true;
}

void SVGDocumentImpl::slotSVGContent(QIODevice *dev)
{
	QXmlInputSource *inputSource = new QXmlInputSource(dev);

	if(m_reader)
		delete m_reader;

	KSVGReader::ParsingArgs args;
	args.fit = m_fit;
	args.getURLMode = false;

	QString url = m_baseURL.prettyURL();
	int pos = url.find('#'); // url can become like this.svg#svgView(viewBox(63,226,74,74)), get part after '#'
	if(pos > -1)
		args.SVGFragmentId = url.mid(pos + 1);
		
	m_reader = new KSVGReader(this, m_canvas, args);
	connect(m_reader, SIGNAL(finished(bool, const QString &)), this, SLOT(slotFinishedParsing(bool, const QString &)));
	m_t.start();
	
#if USE_VALGRIND
	CALLTREE_ZERO_STATS();
#endif

	m_reader->parse(inputSource);
	delete dev;
}

void SVGDocumentImpl::parseSVG(QXmlInputSource *inputSource, bool getURLMode)
{
	if(m_reader)
		delete m_reader;

	KSVGReader::ParsingArgs args;
	args.fit = m_fit;
	args.getURLMode = getURLMode;
	m_reader = new KSVGReader(this, 0, args);
	connect(m_reader, SIGNAL(finished(bool, const QString &)), this, SLOT(slotFinishedParsing(bool, const QString &)));
	
#if USE_VALGRIND
	CALLTREE_ZERO_STATS();
#endif

	m_reader->parse(inputSource);
}

void SVGDocumentImpl::finishParsing(bool error, const QString &errorDesc)
{
	if(m_reader)
		m_reader->finishParsing(error, errorDesc);
}

void SVGDocumentImpl::slotFinishedParsing(bool error, const QString &errorDesc)
{
	kdDebug(26000) << k_funcinfo << "total time : " << m_t.elapsed() << endl;

#if USE_VALGRIND
	CALLTREE_DUMP_STATS();
#endif

	if(m_animations)
		m_timeScheduler->startAnimations();

	if(m_canvas && !error && rootElement())
		executeScripts();

	m_finishedParsing = true;

	emit finishedParsing(error, errorDesc);
	if(!error)
		emit finishedRendering();

	checkFinishedLoading();
}

void SVGDocumentImpl::newImageJob(SVGImageElementImpl *image)
{
	kdDebug(26002) << "SVGDocumentImpl::newImageJob, " << image << endl;
	m_loader->newImageJob(image, m_baseURL);
}

void SVGDocumentImpl::notifyImageLoading(SVGImageElementImpl *image)
{
	m_imagesLoading.append(image);
}

void SVGDocumentImpl::notifyImageLoaded(SVGImageElementImpl *image)
{
	m_imagesLoading.remove(image);

	if(m_imagesLoading.isEmpty())
		checkFinishedLoading();
}

void SVGDocumentImpl::checkFinishedLoading()
{
	if(m_finishedParsing && m_imagesLoading.isEmpty())
	{
		m_finishedLoading = true;

		if(m_resortZIndicesOnFinishedLoading)
		{
			// Only resort if we're the 'outermost' document, i.e. we're not an svg image
			// inside another document. We could resort as each image finishes loading, but it
			// slows down the parsing phase.
			if(m_parentImage == 0 && m_canvas && m_rootElement)
			{
				m_canvas->setElementItemZIndexRecursive(m_rootElement, 0);
				m_canvas->update();
			}
		}

		emit finishedLoading();
	}
}

void SVGDocumentImpl::addForwardReferencingUseElement(SVGUseElementImpl *use)
{
	if(!m_forwardReferencingUseElements.contains(use))
		m_forwardReferencingUseElements.append(use);
}

void SVGDocumentImpl::slotPaint()
{
    rerender();
}

void SVGDocumentImpl::rerender()
{
	m_canvas->update();
	emit finishedRendering();
}

void SVGDocumentImpl::attach(KSVG::KSVGCanvas *c)
{
	m_canvas = c;
}

void SVGDocumentImpl::detach()
{
	m_canvas = 0;
}

KSVG::KSVGCanvas *SVGDocumentImpl::canvas() const
{
	return m_canvas;
}

void SVGDocumentImpl::syncCachedMatrices()
{
	if(rootElement())
	{
		SVGMatrixImpl *parentMatrix = SVGSVGElementImpl::createSVGMatrix();
		rootElement()->checkCachedScreenCTM(parentMatrix);
		parentMatrix->deref();
	}
}

void SVGDocumentImpl::executeScriptsRecursive(DOM::Node start)
{
	DOM::Node node = start.firstChild();

	for(; !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *element = getElementFromHandle(node.handle());

		SVGContainerImpl *container = dynamic_cast<SVGContainerImpl *>(element);
		if(container)
			executeScriptsRecursive(node);

		SVGScriptElementImpl *script = dynamic_cast<SVGScriptElementImpl *>(element);

		if(script)
			script->executeScript(DOM::Node());
	}
}

bool SVGDocumentImpl::executeScriptsRecursiveCheck(DOM::Node start)
{
	bool test = true;

	DOM::Node node = start.firstChild();

	for(; !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *element = getElementFromHandle(node.handle());

		SVGContainerImpl *container = dynamic_cast<SVGContainerImpl *>(element);
		if(container)
		{
			if(!executeScriptsRecursiveCheck(node))
				return false;
		}

		SVGScriptElementImpl *script = dynamic_cast<SVGScriptElementImpl *>(element);

		if(script)
		{
			if(!script->canExecuteScript())
			{
				test = false;
				break;
			}
		}
	}

	return test;
}

void SVGDocumentImpl::executeScripts()
{
	bool test = executeScriptsRecursiveCheck(*rootElement());
	
	if(!test)
		QTimer::singleShot(50, this, SLOT(executeScripts()));
	else
	{
		executeScriptsRecursive(*rootElement());
		
		// mop: only rerender if an loadevent has been found
		if(dispatchRecursiveEvent(SVGEvent::LOAD_EVENT, lastChild()))
			m_canvas->update();
	}
}

// Dispatches a non-cancelable, non-bubbles event to every child
bool SVGDocumentImpl::dispatchRecursiveEvent(SVGEvent::EventId id, DOM::Node start)
{
	bool eventExecuted = false;
	
	// Iterate the tree, backwards, and dispatch the event to every child
	DOM::Node node = start;
	for(; !node.isNull(); node = node.previousSibling())
	{
		SVGElementImpl *element = getElementFromHandle(node.handle());
	
		if(element && element->hasChildNodes())
		{
			// Dispatch to all children
			eventExecuted = dispatchRecursiveEvent(id, element->lastChild()) ? true : eventExecuted;

			// Dispatch, locally
			if(element->hasEventListener(id, true))
			{
				element->dispatchEvent(id, false, false);
				eventExecuted = true;
			}
		}
		else if(element && element->hasEventListener(id, true))
		{
			element->dispatchEvent(id, false, false);
			eventExecuted = true;
		}
	}
	
	return eventExecuted;
}

SVGEventListener *SVGDocumentImpl::createEventListener(DOM::DOMString type)
{
	return m_ecmaEngine->createEventListener(type);
}

SVGElementImpl *SVGDocumentImpl::recursiveSearch(DOM::Node start, const DOM::DOMString &id)
{
	DOM::Node node = start.firstChild();
	for(; !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *test = getElementFromHandle(node.handle());

		// Look in containers
		SVGContainerImpl *container = dynamic_cast<SVGContainerImpl *>(test);
		if(container)
		{
			SVGElementImpl *found = recursiveSearch(node, id);
			if(found)
				return found;
		}

		// Look in SVGSVGElementImpl's
		SVGSVGElementImpl *svgtest = dynamic_cast<SVGSVGElementImpl *>(test);
		if(svgtest)
		{
			SVGElementImpl *element = svgtest->getElementById(id);
			if(element)
				return element;
		}
	}

	return 0;
}

/*
@namespace KSVG
@begin	SVGDocumentImpl::s_hashTable 9
 title				SVGDocumentImpl::Title				DontDelete|ReadOnly
 referrer			SVGDocumentImpl::Referrer			DontDelete|ReadOnly
 domain				SVGDocumentImpl::Domain				DontDelete|ReadOnly
 URL				SVGDocumentImpl::Url				DontDelete|ReadOnly
 doctype			SVGDocumentImpl::DocType			DontDelete|ReadOnly
 implementation		SVGDocumentImpl::Implementation		DontDelete|ReadOnly
 rootElement		SVGDocumentImpl::RootElement		DontDelete|ReadOnly
 documentElement	SVGDocumentImpl::DocumentElement	DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGDocumentImplProto::s_hashTable 7
 createTextNode			SVGDocumentImpl::CreateTextNode			DontDelete|Function 1
 createElement			SVGDocumentImpl::CreateElement			DontDelete|Function 1
 createElementNS		SVGDocumentImpl::CreateElementNS		DontDelete|Function 2
 getElementById			SVGDocumentImpl::GetElementById			DontDelete|Function 1
 getElementsByTagName	SVGDocumentImpl::GetElementsByTagName	DontDelete|Function 1
 getElementsByTagNameNS	SVGDocumentImpl::GetElementsByTagNameNS	DontDelete|Function 2
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGDocument", SVGDocumentImplProto, SVGDocumentImplProtoFunc)

Value SVGDocumentImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case Title:
			return String(title().string());
		case Referrer:
			return String(referrer().string());
		case Domain:
			return String(domain().string());
		case Url:
			return String(URL().string());
		case DocType:
			return getDOMNode(exec, doctype());
		case Implementation:
			return (new SVGDOMDOMImplementationBridge(implementation()))->cache(exec);
		case RootElement:
		case DocumentElement:
			return m_rootElement->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

Value SVGDocumentImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGDocumentImpl)

	switch(id)
	{
		case SVGDocumentImpl::CreateTextNode:
			return getDOMNode(exec, obj->createTextNode(args[0].toString(exec).string()));
		case SVGDocumentImpl::CreateElement:
		case SVGDocumentImpl::CreateElementNS:
		{
			SVGElementImpl *newElement = 0;

			if(id == SVGDocumentImpl::CreateElement)
				newElement = obj->createElement(args[0].toString(exec).qstring(), static_cast<DOM::Document *>(obj)->createElement(args[0].toString(exec).string()), obj);
			else if(id == SVGDocumentImpl::CreateElementNS)
				newElement = obj->createElement(args[1].toString(exec).qstring(), static_cast<DOM::Document *>(obj)->createElementNS(args[0].toString(exec).string(), args[1].toString(exec).string()), obj);

			newElement->setOwnerSVGElement(obj->rootElement()); // FIXME: Correct in all situations?
			newElement->setViewportElement(obj->rootElement());
			newElement->setAttributes();

			return getDOMNode(exec, *newElement);
		}
		case SVGDocumentImpl::GetElementById:
		{
			Value ret;

			SVGElementImpl *element = obj->rootElement()->getElementById(args[0].toString(exec).string());

			if(element)
				ret = getDOMNode(exec, *element);
			else
			{
				element = obj->recursiveSearch(*obj, args[0].toString(exec).string());
				if(!element)
					return Null();

				ret = getDOMNode(exec, *element);
			}

			return ret;
		}
		case SVGDocumentImpl::GetElementsByTagName:
			return (new SVGDOMNodeListBridge(obj->getElementsByTagName(args[0].toString(exec).string())))->cache(exec);
		case SVGDocumentImpl::GetElementsByTagNameNS:
			return (new SVGDOMNodeListBridge(obj->getElementsByTagNameNS(args[0].toString(exec).string(), args[1].toString(exec).string())))->cache(exec);
		default:
			break;
	}

	return KJS::Undefined();
}

SVGElementImpl *SVGDocumentImpl::getElementFromHandle(DOM::NodeImpl *handle) const
{
	return m_elemDict[handle];
}

void SVGDocumentImpl::addToElemDict(DOM::NodeImpl *handle, SVGElementImpl *obj)
{
	m_elemDict.insert(handle, obj);

	if(m_elemDict.count()>m_elemDict.size() && m_elemDictHashSizeIndex<numElemDictHashSizes-1)
	{
		// Increase the hash table size to maintain good lookup speed.
		m_elemDictHashSizeIndex++;
		m_elemDict.resize(elemDictHashSizes[m_elemDictHashSizeIndex]);
	}
}

void SVGDocumentImpl::removeFromElemDict(DOM::NodeImpl *handle)
{
	m_elemDict.remove(handle);
}

SVGDocumentImpl *SVGDocumentImpl::getDocumentFromHandle(DOM::NodeImpl *handle) const
{
	return m_documentDict[handle];
}

void SVGDocumentImpl::addToDocumentDict(DOM::NodeImpl *handle, SVGDocumentImpl *obj)
{
	m_documentDict.insert(handle, obj);
}

SVGElementImpl *SVGDocumentImpl::getElementByIdRecursive(SVGSVGElementImpl *start, const DOM::DOMString &elementId, bool dontSearch)
{
	SVGElementImpl *element = 0;
	
	// #1 Look in passed SVGSVGElementImpl
	if(start)
	{
		element = start->getElementById(elementId);

		if(element)
			return element;
	}
	
	// #2 Search in all child SVGSVGElementImpl's 
	element = recursiveSearch(*this, elementId);

	if(element)
		return element;

	// #3 Search in other documents
	if(!dontSearch)
	{
		QPtrDictIterator<SVGDocumentImpl> it(m_documentDict);
		for(; it.current(); ++it)
		{
			SVGElementImpl *temp = it.current()->getElementByIdRecursive(0, elementId, true);
			if(temp)
				return temp;
		}
	}

	return element;
}

// vim:ts=4:noet
