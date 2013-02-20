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

#include "SVGGradientElement.h"
#include "SVGGradientElementImpl.h"
#include "SVGStopElementImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGSVGElementImpl.h"

#include "KSVGCanvas.h"
#include "CanvasItems.h"
#include "SVGHelperImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGAnimatedTransformListImpl.h"
#include "SVGTransformListImpl.h"
#include "SVGUnitConverter.h"

using namespace KSVG;

#include "SVGGradientElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"
#include "ksvg_cacheimpl.h"

SVGGradientElementImpl::SVGGradientElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGURIReferenceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGPaintServerImpl()
{
	KSVG_EMPTY_FLAGS

	m_gradientUnits = new SVGAnimatedEnumerationImpl();
	m_gradientUnits->ref();

	m_gradientTransform = new SVGAnimatedTransformListImpl();
	m_gradientTransform->ref();

	m_spreadMethod = new SVGAnimatedEnumerationImpl();
	m_spreadMethod->ref();

	m_converter = new SVGUnitConverter();
}

SVGGradientElementImpl::~SVGGradientElementImpl()
{
	if(m_gradientUnits)
		m_gradientUnits->deref();
	if(m_gradientTransform)
		m_gradientTransform->deref();
	if(m_spreadMethod)
		m_spreadMethod->deref();
	delete m_converter;
}

SVGAnimatedEnumerationImpl *SVGGradientElementImpl::gradientUnits() const
{
	return m_gradientUnits;
}

SVGAnimatedTransformListImpl *SVGGradientElementImpl::gradientTransform() const
{
	return m_gradientTransform;
}

SVGAnimatedEnumerationImpl *SVGGradientElementImpl::spreadMethod() const
{
	return m_spreadMethod;
}

/*
@namespace KSVG
@begin SVGGradientElementImpl::s_hashTable 5
 gradientUnits		SVGGradientElementImpl::GradientUnits		DontDelete|ReadOnly
 gradientTransform	SVGGradientElementImpl::GradientTransform	DontDelete|ReadOnly
 spreadMethod		SVGGradientElementImpl::SpreadMethod		DontDelete|ReadOnly
@end
*/

Value SVGGradientElementImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case GradientUnits:
			return m_gradientUnits->cache(exec);
		case GradientTransform:
			return m_gradientTransform->cache(exec);
		case SpreadMethod:
			return m_spreadMethod->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGGradientElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case GradientUnits:
			if(value.toString(exec).qstring() == "userSpaceOnUse")
				m_gradientUnits->setBaseVal(SVGGradientElement::SVG_UNIT_TYPE_USERSPACEONUSE);
			else
				m_gradientUnits->setBaseVal(SVGGradientElement::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);
			break;
		case GradientTransform:
			m_gradientTransform->baseVal()->clear();
			SVGHelperImpl::parseTransformAttribute(m_gradientTransform->baseVal(), value.toString(exec).qstring());
			break;
		case SpreadMethod:
		{
			QString spreadMethod = value.toString(exec).qstring();

			if(spreadMethod == "repeat")
				m_spreadMethod->setBaseVal(SVG_SPREADMETHOD_REPEAT);
			else if(spreadMethod == "reflect")
				m_spreadMethod->setBaseVal(SVG_SPREADMETHOD_REFLECT);
			else
				m_spreadMethod->setBaseVal(SVG_SPREADMETHOD_PAD);
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

void SVGGradientElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();
	setAttributesFromHref();

	// Spec: if attribute not specified, use "pad"
	if(KSVG_TOKEN_NOT_PARSED(SpreadMethod))
		KSVG_SET_ALT_ATTRIBUTE(SpreadMethod, "pad")

	// Spec: if attribute not specified, use objectBoundingBox
	if(KSVG_TOKEN_NOT_PARSED(GradientUnits))
		KSVG_SET_ALT_ATTRIBUTE(GradientUnits, "objectBoundingBox")
}

void SVGGradientElementImpl::setAttributesFromHref()
{
	QString _href = SVGURIReferenceImpl::getTarget(href()->baseVal().string());

	if(!_href.isEmpty())
	{
		SVGGradientElementImpl *refGradient = dynamic_cast<SVGGradientElementImpl *>(ownerSVGElement()->getElementById(_href));

		if(refGradient)
		{
			QMap<QString, DOM::DOMString> refAttributes = refGradient->gradientAttributes();
			QMap<QString, DOM::DOMString>::iterator it;

			for(it = refAttributes.begin(); it != refAttributes.end(); ++it)
			{
				QString name = it.key();
				DOM::DOMString value = it.data();

				if(!hasAttribute(name))
				{
					setAttribute(name, value);
					setAttributeInternal(name, value);
				}
			}
		}
	}
}

SVGGradientElementImpl *SVGGradientElementImpl::stopsSource()
{
	// Spec:
	// If this element has no defined gradient stops, and the referenced element does
	// (possibly due to its own href attribute), then this element inherits the gradient stop from the referenced element.
	// Inheritance can be indirect to an arbitrary level; thus, if the referenced element inherits attribute or gradient stops
	// due to its own href attribute, then the current element can inherit those attributes or gradient stops. (mop)
	bool haveStops = false;

	for(DOM::Node node = firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGStopElementImpl *stop = dynamic_cast<SVGStopElementImpl *>(ownerDoc()->getElementFromHandle(node.handle()));

		if(stop)
		{
			haveStops = true;
			break;
		}
	}

	SVGGradientElementImpl *source = this;

	if(!haveStops)
	{
		QString _href = SVGURIReferenceImpl::getTarget(href()->baseVal().string());
		
		if(!_href.isEmpty())
		{
			SVGGradientElementImpl *refGradient = dynamic_cast<SVGGradientElementImpl *>(ownerSVGElement()->getElementById(_href));

			if(refGradient)
				source = refGradient->stopsSource();
		}
	}

	return source;
}

void SVGGradientElementImpl::createItem(KSVGCanvas *c)
{
	if(!c)
		c = ownerDoc()->canvas();

	if(!m_paintServer)
		m_paintServer = c->createPaintServer(this);
}

void SVGGradientElementImpl::removeItem(KSVGCanvas *)
{
	delete m_paintServer;
	m_paintServer = 0;
}

/*
@namespace KSVG
@begin SVGGradientElementImplConstructor::s_hashTable 5
 SVG_SPREADMETHOD_UNKNOWN    KSVG::SVG_SPREADMETHOD_UNKNOWN     DontDelete|ReadOnly
 SVG_SPREADMETHOD_PAD        KSVG::SVG_SPREADMETHOD_PAD			DontDelete|ReadOnly
 SVG_SPREADMETHOD_REFLECT    KSVG::SVG_SPREADMETHOD_REFLECT		DontDelete|ReadOnly
 SVG_SPREADMETHOD_REPEAT     KSVG::SVG_SPREADMETHOD_REPEAT		DontDelete|ReadOnly
@end
*/

using namespace KJS;

Value SVGGradientElementImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token);
}

Value KSVG::getSVGGradientElementImplConstructor(ExecState *exec)
{
	return cacheGlobalBridge<SVGGradientElementImplConstructor>(exec, "[[svggradientelement.constructor]]");
}

// vim:ts=4:noet
