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

#include "SVGMarkerElement.h"

#include "SVGRectImpl.h"
#include "SVGAngleImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGTransformImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGAnimatedRectImpl.h"
#include "SVGMarkerElementImpl.h"
#include "SVGAnimatedAngleImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGPreserveAspectRatioImpl.h"
#include "SVGAnimatedPreserveAspectRatioImpl.h"

#include "KSVGCanvas.h"

using namespace KSVG;

#include "SVGMarkerElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"
#include "ksvg_cacheimpl.h"

SVGMarkerElementImpl::SVGMarkerElementImpl(DOM::ElementImpl *impl) : SVGContainerImpl(impl), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGFitToViewBoxImpl()
{
	KSVG_EMPTY_FLAGS

	m_refX = new SVGAnimatedLengthImpl();
	m_refX->ref();

	m_refY = new SVGAnimatedLengthImpl();
	m_refY->ref();

	m_markerUnits = new SVGAnimatedEnumerationImpl();
	m_markerUnits->ref();

	m_markerWidth = new SVGAnimatedLengthImpl();
	m_markerWidth->ref();

	m_markerHeight = new SVGAnimatedLengthImpl();
	m_markerHeight->ref();

	m_orientType = new SVGAnimatedEnumerationImpl();
	m_orientType->ref();

	m_orientAngle = new SVGAnimatedAngleImpl();
	m_orientAngle->ref();
}

SVGMarkerElementImpl::~SVGMarkerElementImpl()
{
	if(m_refX)
		m_refX->deref();
	if(m_refY)
		m_refY->deref();
	if(m_markerUnits)
		m_markerUnits->deref();
	if(m_markerWidth)
		m_markerWidth->deref();
	if(m_markerHeight)
		m_markerHeight->deref();
	if(m_orientType)
		m_orientType->deref();
	if(m_orientAngle)
		m_orientAngle->deref();
}

SVGAnimatedLengthImpl *SVGMarkerElementImpl::refX() const
{
	return m_refX;
}

SVGAnimatedLengthImpl *SVGMarkerElementImpl::refY() const
{
	return m_refY;
}

SVGAnimatedEnumerationImpl *SVGMarkerElementImpl::markerUnits() const
{
	return m_markerUnits;
}

SVGAnimatedLengthImpl *SVGMarkerElementImpl::markerWidth() const
{
	return m_markerWidth;
}

SVGAnimatedLengthImpl *SVGMarkerElementImpl::markerHeight() const
{
	return m_markerHeight;
}

SVGAnimatedEnumerationImpl *SVGMarkerElementImpl::orientType() const
{
	return m_orientType;
}

SVGAnimatedAngleImpl *SVGMarkerElementImpl::orientAngle() const
{
	return m_orientAngle;
}

void SVGMarkerElementImpl::setOrientToAuto()
{
	 orientType()->setBaseVal(SVG_MARKER_ORIENT_AUTO);
}

void SVGMarkerElementImpl::setOrientToAngle(SVGAngleImpl *angle)
{
	m_orientAngle->baseVal()->setValue(angle->value());
}

void SVGMarkerElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(RefX))
		KSVG_SET_ALT_ATTRIBUTE(RefX, "0")
	
	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(RefY))
		KSVG_SET_ALT_ATTRIBUTE(RefY, "0")

	// Spec: if not specified, effect is as if a value of "3" were specified
	if(KSVG_TOKEN_NOT_PARSED(MarkerWidth))
		KSVG_SET_ALT_ATTRIBUTE(MarkerWidth, "3")
	
	// Spec: if not specified, effect is as if a value of "3" were specified
	if(KSVG_TOKEN_NOT_PARSED(MarkerHeight))
		KSVG_SET_ALT_ATTRIBUTE(MarkerHeight, "3")

	// Spec: if attribute not specified, use strokeWidth
	if(KSVG_TOKEN_NOT_PARSED(MarkerUnits))
		KSVG_SET_ALT_ATTRIBUTE(MarkerUnits, "strokeWidth")

	// Spec: if attribute not specified, use angle
	if(KSVG_TOKEN_NOT_PARSED(Orient))
		KSVG_SET_ALT_ATTRIBUTE(Orient, "angle")
}

void SVGMarkerElementImpl::createItem(KSVGCanvas *c)
{
	if(!c)
		c = ownerDoc()->canvas();

	if(!m_item)
	{
		m_item = c->createCanvasMarker(this);
		c->insert(m_item);
	}
}

void SVGMarkerElementImpl::draw(SVGShapeImpl *referencingElement, double x, double y, double lwidth, double angle)
{
	SVGMatrixImpl *mtx = dynamic_cast<SVGLocatableImpl *>(referencingElement)->getScreenCTM();

	// move to dest
	mtx->translate(x, y);

	// scale by linewidth if marker units == strokewidth
	if(markerUnits()->baseVal() == SVG_MARKERUNITS_STROKEWIDTH)
		mtx->scale(lwidth);

	// select appropriate rotation
	if(orientType()->baseVal() == SVG_MARKER_ORIENT_AUTO)
		mtx->rotate(angle);
	else
		mtx->rotate(orientAngle()->baseVal()->value());

	SVGRectImpl *viewBoxRect = viewBox()->baseVal();
	SVGMatrixImpl *pres = preserveAspectRatio()->baseVal()->getCTM(viewBoxRect->x(), viewBoxRect->y(),
																	viewBoxRect->width(), viewBoxRect->height(),
																	0, 0, markerWidth()->baseVal()->value(),
																	markerHeight()->baseVal()->value());

	// viewbox stuff
	mtx->multiply(pres);

	// Get the vertex position in viewbox coordinates. The vertex is at (0, 0) in viewport coordinates.
	double vertexX, vertexY;
	pres->qmatrix().invert().map(0, 0, &vertexX, &vertexY);

	// Translate so that the vertex is at (refX, refY) in viewbox coordinates.
	mtx->translate(vertexX - refX()->baseVal()->value(), vertexY - refY()->baseVal()->value());

	if(getOverflow())
		m_clipShape.clear();
	else
	{
		KSVGRectangle viewport;

		if(hasAttribute("viewBox"))
		{
			// Get the viewport ((0, 0) - (markerWidth, markerHeight)) in viewbox coordinates.
			double w, h;
			pres->qmatrix().invert().map(markerWidth()->baseVal()->value(), markerHeight()->baseVal()->value(), &w, &h);

			viewport = KSVGRectangle(vertexX, vertexY, w - vertexX, h - vertexY);
		}
		else
			viewport = KSVGRectangle(0, 0, markerWidth()->baseVal()->value(), markerHeight()->baseVal()->value());

		// Transform to screen coordinates.
		m_clipShape = mtx->map(viewport);
	}

	pres->deref();

	DOM::Node node = firstChild();
	for(; !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *element = ownerDoc()->getElementFromHandle(node.handle());
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(element);
		SVGTestsImpl *tests = dynamic_cast<SVGTestsImpl *>(element);
		SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(element);

		bool ok = tests ? tests->ok() : true;
		if(element && shape && style && ok && style->getVisible() && style->getDisplay())
		{
			SVGLocatableImpl *locatable = dynamic_cast<SVGLocatableImpl *>(element);
			if(locatable)
				locatable->updateCachedScreenCTM(mtx);

			shape->update(UPDATE_TRANSFORM);
			shape->setReferenced(true);
			shape->draw();
			shape->setReferenced(false);
		}
	}

	mtx->deref();
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGMarkerElementImpl::s_hashTable 11
 refX					SVGMarkerElementImpl::RefX					DontDelete|ReadOnly
 refY					SVGMarkerElementImpl::RefY					DontDelete|ReadOnly
 markerUnits			SVGMarkerElementImpl::MarkerUnits			DontDelete|ReadOnly
 markerWidth			SVGMarkerElementImpl::MarkerWidth			DontDelete|ReadOnly
 markerHeight			SVGMarkerElementImpl::MarkerHeight			DontDelete|ReadOnly
 orientType				SVGMarkerElementImpl::OrientType			DontDelete|ReadOnly
 orientAngle			SVGMarkerElementImpl::OrientAngle			DontDelete|ReadOnly
 orient					SVGMarkerElementImpl::Orient				DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGMarkerElementImplProto::s_hashTable 3
 setOrientToAuto		SVGMarkerElementImpl::SetOrientToAuto		DontDelete|Function 0
 setOrientToAngle		SVGMarkerElementImpl::SetOrientToAngle		DontDelete|Function 0
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGMarkerElement", SVGMarkerElementImplProto, SVGMarkerElementImplProtoFunc)

Value SVGMarkerElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE
		
	switch(token)
	{
		case RefX:
			if(!attributeMode)
				return m_refX->cache(exec);
			else
				return Number(m_refX->baseVal()->value());
		case RefY:
			if(!attributeMode)
				return m_refY->cache(exec);
			else
				return Number(m_refY->baseVal()->value());
		case MarkerUnits:
			if(!attributeMode)
				return m_markerUnits->cache(exec);
			else
				return Number(m_markerUnits->baseVal());
		case MarkerWidth:
			if(!attributeMode)
				return m_markerWidth->cache(exec);
			else
				return Number(m_markerWidth->baseVal()->value());
		case MarkerHeight:
			if(!attributeMode)
				return m_markerHeight->cache(exec);
			else
				return Number(m_markerHeight->baseVal()->value());
		case OrientType:
			if(!attributeMode)
				return m_orientType->cache(exec);
			else
				return Number(m_orientType->baseVal());
		case OrientAngle:
			if(!attributeMode)
				return m_orientAngle->cache(exec);
			else
				return Number(m_orientAngle->baseVal()->value());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGMarkerElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case RefX:
			refX()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case RefY:
			refY()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case MarkerWidth:
			markerWidth()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case MarkerHeight:
			markerHeight()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case MarkerUnits:
			if(value.toString(exec).qstring() == "userSpaceOnUse")
				markerUnits()->setBaseVal(SVG_MARKERUNITS_USERSPACEONUSE);
			else
				markerUnits()->setBaseVal(SVG_MARKERUNITS_STROKEWIDTH);
			break;
		case Orient:
		{
			QString param = value.toString(exec).qstring();
			
			if(param == "auto")
				orientType()->setBaseVal(SVG_MARKER_ORIENT_AUTO);
			else
			{
				orientType()->setBaseVal(SVG_MARKER_ORIENT_ANGLE);
				m_orientAngle->baseVal()->setValueAsString(param);
			}
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

Value SVGMarkerElementImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGMarkerElementImpl)

	switch(id)
	{
 		case SVGMarkerElementImpl::SetOrientToAuto:
			obj->setOrientToAuto();
 			return Undefined();
#ifdef __GNUC__
#warning FIXME cache stuff
#endif
 		case SVGMarkerElementImpl::SetOrientToAngle:
			obj->setOrientToAngle(static_cast<KSVGBridge<SVGAngleImpl> *>(args[0].imp())->impl());
 			return Undefined();
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

// CONSTANTS

/*
@namespace KSVG
@begin SVGMarkerElementImplConstructor::s_hashTable 7
 SVG_MARKERUNITS_UNKNOWN		KSVG::SVG_MARKERUNITS_UNKNOWN     		DontDelete|ReadOnly
 SVG_MARKERUNITS_USERSPACEONUSE KSVG::SVG_MARKERUNITS_USERSPACEONUSE	DontDelete|ReadOnly
 SVG_MARKERUNITS_STROKEWIDTH    KSVG::SVG_MARKERUNITS_STROKEWIDTH		DontDelete|ReadOnly
 SVG_MARKER_ORIENT_UNKNOWN     	KSVG::SVG_MARKER_ORIENT_UNKNOWN			DontDelete|ReadOnly
 SVG_MARKER_ORIENT_AUTO			KSVG::SVG_MARKER_ORIENT_AUTO			DontDelete|ReadOnly
 SVG_MARKER_ORIENT_ANGLE		KSVG::SVG_MARKER_ORIENT_ANGLE			DontDelete|ReadOnly
@end
*/

using namespace KJS;

Value SVGMarkerElementImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token);
}

Value KSVG::getSVGMarkerElementImplConstructor(ExecState *exec)
{
	return cacheGlobalBridge<SVGMarkerElementImplConstructor>(exec, "[[svgmarkerelement.constructor]]");
}

// vim:ts=4:noet
