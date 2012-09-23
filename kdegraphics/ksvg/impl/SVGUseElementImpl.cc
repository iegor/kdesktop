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

#include <qstring.h>

#include "KSVGLoader.h"
#include "KSVGCanvas.h"

#include "SVGRectImpl.h"
#include "SVGEventImpl.h"
#include "SVGHelperImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGTransformImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGUseElementImpl.h"
#include "SVGSymbolElementImpl.h"
#include "SVGTransformListImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGElementInstanceImpl.h"
#include "SVGAnimatedTransformListImpl.h"

using namespace KSVG;

#include "SVGUseElementImpl.lut.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGUseElementImpl::SVGUseElementImpl(DOM::ElementImpl *impl) : SVGShapeImpl(impl), SVGURIReferenceImpl(), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl()
{
	KSVG_EMPTY_FLAGS

	m_x = new SVGAnimatedLengthImpl();
	m_x->ref();

	m_y = new SVGAnimatedLengthImpl();
	m_y->ref();

	m_width = new SVGAnimatedLengthImpl();
	m_width->ref();

	m_height = new SVGAnimatedLengthImpl();
	m_height->ref();

	m_instanceRoot = 0;
}

SVGUseElementImpl::~SVGUseElementImpl()
{
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_width)
		m_width->deref();
	if(m_height)
		m_height->deref();
	if(m_instanceRoot)
		m_instanceRoot->deref();
}

SVGAnimatedLengthImpl *SVGUseElementImpl::x() const
{
	return m_x;
}

SVGAnimatedLengthImpl *SVGUseElementImpl::y() const
{
	return m_y;
}

SVGAnimatedLengthImpl *SVGUseElementImpl::width() const
{
	return m_width;
}

SVGAnimatedLengthImpl *SVGUseElementImpl::height() const
{
	return m_height;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGUseElementImpl::s_hashTable 11
 x						SVGUseElementImpl::X					DontDelete|ReadOnly
 y						SVGUseElementImpl::Y					DontDelete|ReadOnly
 width					SVGUseElementImpl::Width				DontDelete|ReadOnly
 height					SVGUseElementImpl::Height				DontDelete|ReadOnly
 href					SVGUseElementImpl::Href					DontDelete|ReadOnly
 instanceRoot			SVGUseElementImpl::InstanceRoot			DontDelete|ReadOnly
 animatedInstanceRoot	SVGUseElementImpl::AnimatedInstanceRoot	DontDelete|ReadOnly
@end
*/

Value SVGUseElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE

	switch(token)
	{
		case X:
			if(!attributeMode)
				return m_x->cache(exec);
			else
				return Number(m_x->baseVal()->value());
		case Y:
			if(!attributeMode)
				return m_y->cache(exec);
			else
				return Number(m_y->baseVal()->value());
		case Width:
			if(!attributeMode)
				return m_width->cache(exec);
			else
				return Number(m_width->baseVal()->value());
		case Height:
			if(!attributeMode)
				return m_height->cache(exec);
			else
				return Number(m_height->baseVal()->value());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGUseElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case X:
			x()->baseVal()->setValue(value.toNumber(exec));
			break;
		case Y:
			y()->baseVal()->setValue(value.toNumber(exec));
			break;
		case Width:
			width()->baseVal()->setValue(value.toNumber(exec));
			break;
		case Height:
			height()->baseVal()->setValue(value.toNumber(exec));
			break;
		case Href:
		{
			QString url = value.toString(exec).qstring();
			href()->setBaseVal(SVGURIReferenceImpl::getTarget(url));
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

SVGRectImpl *SVGUseElementImpl::getBBox()
{
	if(m_instanceRoot)
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(m_instanceRoot->correspondingElement());
		if(KSVG_TOKEN_NOT_PARSED(Width) && KSVG_TOKEN_NOT_PARSED(Height) && shape)
			return shape->getBBox();
	}

	SVGRectImpl *ret = new SVGRectImpl();
	ret->ref();
	ret->setX(m_x->baseVal()->value());
	ret->setY(m_y->baseVal()->value());
	ret->setWidth(m_width->baseVal()->value());
	ret->setHeight(m_height->baseVal()->value());
	return ret;
}

SVGElementInstanceImpl *SVGUseElementImpl::instanceRoot() const
{
	return m_instanceRoot;
}

SVGElementInstanceImpl *SVGUseElementImpl::animatedInstanceRoot() const
{
	return m_animatedInstanceRoot;
}

void SVGUseElementImpl::createItem(KSVGCanvas *c)
{
	if(!m_instanceRoot)
	{
		// ownerSVGElement()->getElementById() is wrong here.
		// It could reference elements from other documents when using getURL (Niko)
		QString filename, id;
		DOM::DOMString url = getAttribute("href");
		if(!SVGURIReferenceImpl::parseURIReference(url.string(), filename, id))
			return;
		
		SVGElementImpl *orig;
		if(!filename.isEmpty())
		{
			KURL fragmentUrl(ownerDoc()->baseUrl(), url.string());

			id = fragmentUrl.ref();
			fragmentUrl.setRef(QString::null);

			orig = KSVGLoader::getSVGFragment(fragmentUrl, ownerDoc(), id);
		}
		else
		{
			orig = ownerDoc()->getElementByIdRecursive(ownerSVGElement(), href()->baseVal());

			if(orig == 0)
			{
				// The document will try to create this item again once the parsing has finished.
				ownerDoc()->addForwardReferencingUseElement(this);
			}
		}
		
		if(orig == 0)
			return;

		setReferencedElement(orig);

		// Create a parent, a <g>
		SVGElementImpl *parent = 0;
		DOM::Element impl = static_cast<DOM::Document *>(ownerDoc())->createElement("g");
		parent = SVGDocumentImpl::createElement("g", impl, ownerDoc());
		SVGElementImpl *clone = orig->cloneNode(true);

		// Apply the use-correction
		QString trans;
		trans += " translate(";
		trans += QString::number(x()->baseVal()->value());
		trans += " ";
		trans += QString::number(y()->baseVal()->value());
		trans += ")";

		// Apply the transform attribute and render the element
		parent->setAttributeInternal("transform", trans);
		parent->setAttribute("transform", trans);

		// Apply width/height if symbol
		if(dynamic_cast<SVGSymbolElementImpl *>(clone))
		{
			DOM::Element impl = static_cast<DOM::Document *>(ownerDoc())->createElement("svg");
			SVGElementImpl *symbolSvg = SVGDocumentImpl::createElement("svg", impl, ownerDoc());

			SVGHelperImpl::copyAttributes(orig, symbolSvg);
			
			symbolSvg->setAttribute("width", getAttribute("width"));
			symbolSvg->setAttributeInternal("width", getAttribute("width"));
			symbolSvg->setAttribute("height", getAttribute("height"));
			symbolSvg->setAttributeInternal("height", getAttribute("height"));
			DOM::Node node = clone->firstChild();
			for(; !node.isNull(); node = clone->firstChild())
				symbolSvg->appendChild(node);
				
			clone = symbolSvg;
		}
		else if(dynamic_cast<SVGSVGElementImpl *>(clone))
		{
			if(!getAttribute("width").isEmpty())
			{
				clone->setAttribute("width", getAttribute("width"));
				clone->setAttributeInternal("width", getAttribute("width"));
			}

			if(!getAttribute("height").isEmpty())
			{
				clone->setAttribute("height", getAttribute("height"));
				clone->setAttributeInternal("height", getAttribute("height"));
			}
		}

		appendChild(*parent);
		parent->appendChild(*clone);

		setupSubtree(parent, ownerSVGElement(), viewportElement());

		m_instanceRoot->setCorrespondingElement(clone);

		dynamic_cast<SVGLocatableImpl *>(parent)->updateCachedScreenCTM(screenCTM());

		// Redirect local ecma event handlers to the correspondingElement
		QPtrListIterator<SVGRegisteredEventListener> it(eventListeners());
		SVGRegisteredEventListener *eventListener;
		while((eventListener = it.current()) != 0)
		{
			++it;
			clone->setEventListener(eventListener->id, eventListener->listener);
		}
	}
	
	if(m_instanceRoot)
	{
		SVGElementImpl *element = m_instanceRoot->correspondingElement();
		element->createItem(c);
	}
}

void SVGUseElementImpl::removeItem(KSVGCanvas *c)
{
	if(m_instanceRoot)
	{
		SVGElementImpl *element = m_instanceRoot->correspondingElement();
		element->removeItem(c);
	}
}

void SVGUseElementImpl::setupSubtree(SVGElementImpl *element, SVGSVGElementImpl *ownerSVG, SVGElementImpl *viewport)
{
	element->setOwnerSVGElement(ownerSVG);
	element->setViewportElement(viewport);
	element->setAttributes();

	SVGSVGElementImpl *thisSVG = dynamic_cast<SVGSVGElementImpl *>(element);

	if(thisSVG != 0)
	{
		ownerSVG = thisSVG;
		viewport = element;
	}

	DOM::Node child = element->firstChild();
	for(; !child.isNull(); child = child.nextSibling())
	{
		SVGElementImpl *childElement = ownerDoc()->getElementFromHandle(child.handle());
		if(childElement != 0)
			setupSubtree(childElement, ownerSVG, viewport);
	}
}

void SVGUseElementImpl::setReferencedElement(SVGElementImpl *referenced)
{
	if(!referenced)
		return;
	
	if(!m_instanceRoot)
	{
		m_instanceRoot = new SVGElementInstanceImpl();
		m_instanceRoot->ref();
	}

	m_instanceRoot->setCorrespondingElement(referenced);
}

void SVGUseElementImpl::update(CanvasItemUpdate reason, int param1, int param2)
{
	if(m_instanceRoot)
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(m_instanceRoot->correspondingElement());
		if(shape)
			shape->update(reason, param1, param2);
	}
}

void SVGUseElementImpl::invalidate(KSVGCanvas *c, bool recalc)
{
	if(m_instanceRoot)
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(m_instanceRoot->correspondingElement());
		if(shape)
			shape->invalidate(c, recalc);
	}
}

void SVGUseElementImpl::setReferenced(bool referenced)
{
	if(m_instanceRoot)
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(m_instanceRoot->correspondingElement());
		if(shape)
			shape->setReferenced(referenced);
	}
}

void SVGUseElementImpl::draw()
{
	if(m_instanceRoot)
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(m_instanceRoot->correspondingElement());
		if(shape)
			shape->draw();
	}
}

// vim:ts=4:noet
