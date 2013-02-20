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

#include <dom/dom_exception.h>
#include <kdebug.h>
#include <klocale.h>
#include <qmap.h>
#include <ksimpleconfig.h>
#include <KSVGCanvas.h>
#include "KSVGReader.moc"
#include "SVGSVGElementImpl.h"
#include "SVGViewSpecImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGShapeImpl.h"
#include "SVGLengthImpl.h"
#include "SVGImageElementImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGUseElementImpl.h"

namespace KSVG
{

class Helper
{
public:
	static Helper *self(KSVGReader *reader = 0);

	void destroy();

	void setFit(bool bFit = true) { m_bFit = bFit; }
	bool fit() { return m_bFit; }

	SVGDocumentImpl *doc() const { return m_reader->doc(); }
	KSVGCanvas *canvas() const { return m_reader->canvas(); }

	void addSVGElement(SVGSVGElementImpl *one, DOM::NodeImpl *two) { m_svgMap.insert(two, one); }
	SVGSVGElementImpl *nextSVGElement(SVGElementImpl *elem);
	SVGSVGElementImpl *nextSVGElement(DOM::Node elem);
	void setFinished(bool error, const QString &errorDesc = "") { m_reader->setFinished(error, errorDesc); }

	// Error handling
	void setErrorDescription(const QString &err) { m_errorDesc = err; }
	QString errorDescription() { return m_errorDesc; }
	bool hasError() const { return !m_errorDesc.isEmpty(); }

	bool getURLMode() const { return m_getURLMode; }
	void setGetURLMode(bool mode) { m_getURLMode = mode; }

	QString SVGFragmentId() const { return m_SVGFragmentId; }
	void setSVGFragmentId(const QString &SVGFragmentId) { m_SVGFragmentId = SVGFragmentId; }

protected:
	Helper(KSVGReader *reader);

private:
	Helper();
	Helper(const Helper &rhs);
	Helper &operator=(const Helper &rhs);

	static Helper *m_instance;
	QMap<DOM::NodeImpl *, SVGSVGElementImpl *> m_svgMap;
	KSVGReader *m_reader;
	bool m_bFit;
	bool m_getURLMode;
	QString m_errorDesc;
	QString m_SVGFragmentId;
};

class InputHandler : public QXmlDefaultHandler
{
public:
	virtual bool startDocument();
	virtual bool endDocument();
	virtual bool startElement(const QString &namespaceURI,
	                          const QString &localName,
	                          const QString &qName,
	                          const QXmlAttributes &atts);
	virtual bool endElement(const QString &namespaceURI,
	                        const QString &localName,
	                        const QString &qName);
	virtual bool characters(const QString &ch);
	virtual bool warning(const QXmlParseException &e);
	virtual bool error(const QXmlParseException &e);
	virtual bool fatalError(const QXmlParseException &e);

private:
	DOM::Node *m_rootNode;
	DOM::Node *m_currentNode;
	DOM::Node m_parentNode;

	bool m_noRendering, m_progressive;
};

}

using namespace KSVG;

Helper *Helper::m_instance = 0;

Helper::Helper(KSVGReader *reader)
{
	m_reader = reader;
}

Helper *Helper::self(KSVGReader *reader)
{
	if(m_instance && reader != 0)
		m_instance->m_reader = reader;
	if(!m_instance)
	{
		Q_ASSERT(reader != 0);
		m_instance = new Helper(reader);
	}

	return m_instance;	
}

void Helper::destroy()
{
	m_svgMap.clear();
}

SVGSVGElementImpl *Helper::nextSVGElement(SVGElementImpl *elem)
{
	return nextSVGElement(*elem);
}

SVGSVGElementImpl *Helper::nextSVGElement(DOM::Node elem)
{
	DOM::Node foundSVG;
	DOM::Node shape = elem.parentNode();
	
	for(; !shape.isNull(); shape = shape.parentNode())
	{
		if(reinterpret_cast<DOM::Element &>(shape).nodeName() == "svg")
		{
			foundSVG = shape;
			break;
		}
	}

	SVGSVGElementImpl *svg = m_svgMap[foundSVG.handle()];
	return svg;
}

bool InputHandler::startDocument()
{
	m_rootNode = 0;
	m_currentNode = 0;
	m_noRendering = false;

	KSimpleConfig config("ksvgpluginrc");
	config.setGroup("Rendering");
	m_progressive = config.readBoolEntry("ProgressiveRendering", true);

	if(Helper::self()->canvas())
		Helper::self()->canvas()->setImmediateUpdate(m_progressive);

	return true;
}

bool InputHandler::endDocument()
{
	Helper::self()->setFinished(false);
        if (Helper::self()->canvas())
	        Helper::self()->canvas()->setImmediateUpdate(false);

	return true;
}

bool InputHandler::characters(const QString &ch)
{
	kdDebug(26001) << "InputHandler::characters, read " << ch << endl;

	if(ch.simplifyWhiteSpace().isEmpty())
		return true;

	QString t = ch;

	SVGSVGElementImpl *root = Helper::self()->nextSVGElement(*m_currentNode);
	if(root)
	{
		SVGElementImpl *element = root->ownerDoc()->getElementFromHandle(m_currentNode->handle());
		SVGLangSpaceImpl *langSpace = dynamic_cast<SVGLangSpaceImpl *>(element);
		
		if(langSpace)
			t = langSpace->handleText(ch);
	}

	if(!t.isEmpty())
	{
		DOM::Text impl = static_cast<DOM::Document *>(Helper::self()->doc())->createTextNode(t);
		m_currentNode->appendChild(impl);
	}

	return true;
}

bool InputHandler::startElement(const QString &namespaceURI, const QString &, const QString &qName, const QXmlAttributes &attrs)
{
	kdDebug(26001) << "InputHandler::startElement, namespaceURI " << namespaceURI << " qName " << qName << endl;

	SVGElementImpl *newElement = 0;
	SVGSVGElementImpl *svg = 0;

	if(qName == "svg")
	{
		DOM::Element impl = static_cast<DOM::Document *>(Helper::self()->doc())->createElementNS(namespaceURI, qName);
		newElement = SVGDocumentImpl::createElement(qName, impl, Helper::self()->doc());
		svg = dynamic_cast<SVGSVGElementImpl *>(newElement);

		Helper::self()->addSVGElement(svg, impl.handle());

		// Need this before we can find our ownerSVGElement (AP)
		if(m_currentNode != 0)
			m_currentNode->appendChild(*svg);
		else
		// TODO: Those set/get attribute callls have NO effect anymore
		// Convert to the new system, Rob? (Niko)
		{
			if(Helper::self()->fit())
			{	// handle fitting of svg into small drawing area(thumb)
				// TODO : aspectratio? and what about svgs that dont provide width and height?
				if(svg->getAttribute("viewBox").string().isEmpty())
				{
					SVGLengthImpl *width = SVGSVGElementImpl::createSVGLength();
					SVGLengthImpl *height = SVGSVGElementImpl::createSVGLength();
					width->setValueAsString(svg->getAttribute("width").string());
					height->setValueAsString(svg->getAttribute("height").string());
					QString viewbox = QString("0 0 %1 %2").arg(width->value()).arg(height->value());
					//kdDebug(26001) << "VIEWBOX : " << viewbox.latin1() << endl;
					svg->setAttribute("viewBox", viewbox);
					width->deref();
					height->deref();
				}
				svg->setAttribute("width", QString::number(Helper::self()->canvas()->width()));
				svg->setAttribute("height", QString::number(Helper::self()->canvas()->height()));
			}

			if(!Helper::self()->SVGFragmentId().isEmpty())
			{
				if(svg->currentView()->parseViewSpec(Helper::self()->SVGFragmentId()))
					svg->setUseCurrentView(true);
			}
		}

		if(m_rootNode == 0)
		{
			Helper::self()->doc()->appendChild(*svg);
			Helper::self()->doc()->setRootElement(svg);

			m_rootNode = svg;
		}
	}
	else
	{
		if(!m_rootNode && !Helper::self()->getURLMode())
		{
			Helper::self()->setErrorDescription(i18n("A legal svg document requires a <svg> root element"));
			return false;
		}

		DOM::Element impl = static_cast<DOM::Document *>(Helper::self()->doc())->createElementNS(namespaceURI, qName);
		newElement = SVGDocumentImpl::createElement(qName, impl, Helper::self()->doc());

		// m_currentNode == 0 if we are dynamically extending the dom (parsexml...)
		// and the file doesn't have a root element
		if(m_currentNode != 0)
			m_currentNode->appendChild(*newElement);
		else
			Helper::self()->doc()->appendChild(*newElement);

		// Special logics:
		if(qName == "switch" || qName == "pattern" || qName == "mask")
			m_noRendering = true;
	}

	newElement->setOwnerSVGElement(Helper::self()->nextSVGElement(newElement));
	newElement->setViewportElement(newElement->ownerSVGElement());

	newElement->setAttributes(attrs);

	if(svg && svg->ownerSVGElement() == 0)
	{
		SVGImageElementImpl *parentImage = Helper::self()->doc()->parentImage();

		if(parentImage)
		{
			// We're being displayed in a document via an 'image' element. Set
			// us up to fit into it's rectangle.
			parentImage->setupSVGElement(svg);
		}
	}

	SVGLocatableImpl *locatable = dynamic_cast<SVGLocatableImpl *>(newElement);

	if(locatable)
	{
		// Set up the cached screenCTM
		SVGLocatableImpl *locatableParent = 0;
		DOM::Node parentNode = newElement->parentNode();

		if(!parentNode.isNull())
		{
			SVGElementImpl *parent = Helper::self()->doc()->getElementFromHandle(parentNode.handle());

			if(parent)
				locatableParent = dynamic_cast<SVGLocatableImpl *>(parent);
		}

		SVGMatrixImpl *parentMatrix = 0;

		if(locatableParent)
			parentMatrix = locatableParent->getScreenCTM();
		else
			parentMatrix = SVGSVGElementImpl::createSVGMatrix();

		locatable->updateCachedScreenCTM(parentMatrix);
		parentMatrix->deref();
	}

	m_currentNode = newElement;
	return !Helper::self()->hasError();
}

bool InputHandler::endElement(const QString &, const QString &, const QString &qName)
{
	kdDebug(26001) << "InputHandler::endElement, qName " << qName << endl;

	bool haveCanvas = Helper::self()->canvas();

	SVGSVGElementImpl *root = Helper::self()->nextSVGElement(*m_currentNode);
	SVGElementImpl *element = root ? root->ownerDoc()->getElementFromHandle(m_currentNode->handle()) : Helper::self()->doc()->getElementFromHandle(m_currentNode->handle());
	SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(element);
	SVGTestsImpl *tests = dynamic_cast<SVGTestsImpl *>(element);
	SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(element);

	if(qName != "script" && !m_noRendering && !Helper::self()->getURLMode())
	{
		if(!root)
		{
			if(haveCanvas)
			{
				if(!m_progressive)
					Helper::self()->canvas()->update();

				Helper::self()->canvas()->blit();

				QValueList<SVGUseElementImpl *> forwardReferencingUseElements = Helper::self()->doc()->forwardReferencingUseElements();

				if(!forwardReferencingUseElements.isEmpty())
				{
					// Create the elements again now that we have parsed the whole document.
					QValueList<SVGUseElementImpl *>::iterator it;

					Helper::self()->canvas()->setImmediateUpdate(false);

					for(it = forwardReferencingUseElements.begin(); it != forwardReferencingUseElements.end(); it++)
						(*it)->createItem(Helper::self()->canvas());

					// The newly created items will need to be moved into their correct z-order positions.
					Helper::self()->doc()->resortZIndicesOnFinishedLoading();
				}
			}

			return true; // we have reached the bottom
		}

		if(haveCanvas && (tests ? tests->ok() : true))
		{
			if((shape && !shape->isContainer()) || (!shape && element))
				element->createItem();
		}
	}

	// Special logics:
	if(qName == "switch" || qName == "pattern" || qName == "mask")
	{
		m_noRendering = false;
		bool ok = tests ? tests->ok() : true;

		if(haveCanvas && element && style && ok && style->getDisplay() && style->getVisible() && qName == "pattern" || (shape && shape->directRender()))
			element->createItem();
	}

	m_parentNode = m_currentNode->parentNode();	// this is needed since otherwise we get temporary vars
	m_currentNode = &m_parentNode;
	
	return true;
}

bool InputHandler::warning(const QXmlParseException &e)
{
	kdDebug(26001) << "[" << e.lineNumber() << ":" << e.columnNumber() << "]: WARNING: " << e.message() << endl;
	return true;
}

bool InputHandler::error(const QXmlParseException &e)
{
	kdDebug(26001) << "[" << e.lineNumber() << ":" << e.columnNumber() << "]: ERROR: " << e.message() << endl;
	return true;
}

bool InputHandler::fatalError(const QXmlParseException &e)
{
	QString error;
	
	if(Helper::self()->hasError())
	{
		error = Helper::self()->errorDescription();
		Helper::self()->setErrorDescription(QString::null);
	}
	else
		error = QString("[%1:%2]: FATAL ERROR: %3").arg(e.lineNumber()).arg(e.columnNumber()).arg(e.message());

	kdDebug(26001) << "InputHandler::fatalError, " << error << endl;

	Helper::self()->setFinished(true, error);
	return true;
}

struct KSVGReader::Private
{
	QXmlSimpleReader *reader;
	InputHandler *inputHandler;
	SVGDocumentImpl *doc;
	KSVGCanvas *canvas;
};

KSVGReader::KSVGReader(SVGDocumentImpl *doc, KSVGCanvas *canvas, ParsingArgs args) : QObject(), d(new Private)
{
	d->doc = doc;
	d->canvas = canvas;

	d->reader = new QXmlSimpleReader();
	d->inputHandler = new InputHandler();

	Helper::self(this);
	Helper::self()->setFit(args.fit);
	Helper::self()->setGetURLMode(args.getURLMode);
	Helper::self()->setSVGFragmentId(args.SVGFragmentId);

	d->reader->setContentHandler(d->inputHandler);
	d->reader->setErrorHandler(d->inputHandler);
}

KSVGReader::~KSVGReader()
{
	Helper::self()->destroy();

	delete d->reader;
	delete d->inputHandler;
	delete d;
}

void KSVGReader::parse(QXmlInputSource *source)
{
	d->reader->parse(source);
}

void KSVGReader::finishParsing(bool, const QString &errorDesc)
{
	Helper::self()->setErrorDescription(errorDesc);
}

void KSVGReader::setFinished(bool error, const QString &errorDesc)
{
	kdDebug(26001) << "KSVGReader::setFinished" << endl;
	emit finished(error, errorDesc);
}

SVGDocumentImpl *KSVGReader::doc()
{
	return d->doc;
}

KSVG::KSVGCanvas *KSVGReader::canvas()
{
	return d->canvas;
}

// vim:ts=4:noet
