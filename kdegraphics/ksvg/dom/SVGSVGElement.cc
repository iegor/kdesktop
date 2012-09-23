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

#include "SVGRect.h"
#include "SVGSVGElement.h"
#include "SVGSVGElementImpl.h"
#include "SVGAnimatedLength.h"
#include "SVGPoint.h"
#include "SVGNumber.h"
#include "SVGAngle.h"
#include "SVGMatrix.h"
#include "SVGLength.h"
#include "SVGRect.h"
#include "SVGTransform.h"
#include "SVGViewSpec.h"
#include <kdebug.h>

using namespace KSVG;

SVGSVGElement::SVGSVGElement() : SVGElement(), SVGTests(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable(), SVGLocatable(), SVGFitToViewBox(), SVGZoomAndPan()
{
	impl = 0;
}

SVGSVGElement::SVGSVGElement(const SVGSVGElement &other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGLocatable(other), SVGFitToViewBox(other), SVGZoomAndPan(other), impl(0)
{
	(*this) = other;
}

SVGSVGElement &SVGSVGElement::operator=(const SVGSVGElement &other)
{
	SVGElement::operator=(other);
	SVGTests::operator=(other);
	SVGLangSpace::operator=(other);
	SVGExternalResourcesRequired::operator=(other);
	SVGStylable::operator=(other);
	SVGLocatable::operator=(other);
	SVGFitToViewBox::operator=(other);
	SVGZoomAndPan::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGSVGElement::SVGSVGElement(SVGSVGElementImpl *other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGLocatable(other), SVGFitToViewBox(other), SVGZoomAndPan(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGSVGElement::~SVGSVGElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedLength SVGSVGElement::x() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGSafeCreator<SVGAnimatedLength, SVGAnimatedLengthImpl>::create(impl->x());
}

SVGAnimatedLength SVGSVGElement::y() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGSafeCreator<SVGAnimatedLength, SVGAnimatedLengthImpl>::create(impl->y());
}

SVGAnimatedLength SVGSVGElement::width() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGSafeCreator<SVGAnimatedLength, SVGAnimatedLengthImpl>::create(impl->width());
}

SVGAnimatedLength SVGSVGElement::height() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGSafeCreator<SVGAnimatedLength, SVGAnimatedLengthImpl>::create(impl->height());
}

void SVGSVGElement::setContentScriptType(const DOM::DOMString &contentScriptType)
{
	if(impl)
		setContentScriptType(contentScriptType);
}

DOM::DOMString SVGSVGElement::contentScriptType() const
{
	if(!impl) return DOM::DOMString();
	return impl->contentScriptType();
}

void SVGSVGElement::setContentStyleType(const DOM::DOMString &contentStyleType)
{
	if(impl)
		setContentStyleType(contentStyleType);
}

DOM::DOMString SVGSVGElement::contentStyleType() const
{
	if(!impl) return DOM::DOMString();
	return impl->contentStyleType();
}

SVGRect SVGSVGElement::viewport() const
{
	if(!impl) return SVGRect(0);
	return SVGRect(impl->viewport());
}

float SVGSVGElement::pixelUnitToMillimeterX() const
{
	if(!impl) return -1;
	return impl->pixelUnitToMillimeterX();
}

float SVGSVGElement::pixelUnitToMillimeterY() const
{
	if(!impl) return -1;
	return impl->pixelUnitToMillimeterY();
}

float SVGSVGElement::screenPixelToMillimeterX() const
{
	if(!impl) return -1;
	return impl->screenPixelToMillimeterX();
}

float SVGSVGElement::screenPixelToMillimeterY() const
{
	if(!impl) return -1;
	return impl->screenPixelToMillimeterY();
}

void SVGSVGElement::setUseCurrentView(bool useCurrentView)
{
	if(impl)
		impl->setUseCurrentView(useCurrentView);
}

bool SVGSVGElement::useCurrentView() const
{
	if(!impl) return false;
	return impl->useCurrentView();
}

SVGViewSpec SVGSVGElement::currentView() const
{
	if(!impl) return SVGViewSpec(0);
	return impl->currentView();
}

void SVGSVGElement::setCurrentScale(float currentScale)
{
	if(impl)
		impl->setCurrentScale(currentScale);
}

float SVGSVGElement::currentScale() const
{
	if(!impl) return -1;
	return impl->currentScale();
}

SVGPoint SVGSVGElement::currentTranslate() const
{
	if(!impl) return SVGPoint(0);
	return SVGSafeCreator<SVGPoint, SVGPointImpl>::create(impl->currentTranslate());
}

unsigned long SVGSVGElement::suspendRedraw(unsigned long time)
{
	if(!impl) return 0;
	return impl->suspendRedraw(time);
}

void SVGSVGElement::unsuspendRedraw(unsigned long id)
{
	if(impl)
		impl->unsuspendRedraw(id);
}

void SVGSVGElement::unsuspendRedrawAll()
{
	if(impl)
		impl->unsuspendRedrawAll();
}

void SVGSVGElement::forceRedraw()
{
	if(impl)
		impl->forceRedraw();
}

void SVGSVGElement::pauseAnimations()
{
	if(impl)
		impl->pauseAnimations();
}

void SVGSVGElement::unpauseAnimations()
{
	if(impl)
		impl->unpauseAnimations();
}

bool SVGSVGElement::animationsPaused()
{
	if(!impl) return false;
	return impl->animationsPaused();
}

float SVGSVGElement::getCurrentTime()
{
	if(!impl) return -1;
	return impl->getCurrentTime();
}

void SVGSVGElement::setCurrentTime(float time)
{
	if(impl)
		impl->setCurrentTime(time);
}

DOM::NodeList SVGSVGElement::getIntersectionList(const SVGRect &rect,const SVGElement &referenceElement)
{
	if(!impl) return DOM::NodeList();
	return impl->getIntersectionList(rect.handle(), referenceElement.handle());
}

DOM::NodeList SVGSVGElement::getEnclosureList(const SVGRect &rect,const SVGElement &referenceElement)
{
	if(!impl) return DOM::NodeList();
	return impl->getEnclosureList(rect.handle(), referenceElement.handle());
}

bool SVGSVGElement::checkIntersection(const SVGElement &element,const SVGRect &rect)
{
	if(!impl) return false;
	return impl->checkIntersection(element.handle(), rect.handle());
}

bool SVGSVGElement::checkEnclosure(const SVGElement &element,const SVGRect &rect)
{
	if(!impl) return false;
	return impl->checkEnclosure(element.handle(), rect.handle());
}

void SVGSVGElement::deSelectAll()
{
	if(impl)
		impl->deSelectAll();
}

SVGNumber SVGSVGElement::createSVGNumber()
{
	if(!impl) return SVGNumber(0);
	return SVGSafeCreator<SVGNumber, SVGNumberImpl>::create(impl->createSVGNumber());
}

SVGLength SVGSVGElement::createSVGLength()
{
	if(!impl) return SVGLength(0);
	return SVGSafeCreator<SVGLength, SVGLengthImpl>::create(impl->createSVGLength());
}

SVGAngle SVGSVGElement::createSVGAngle()
{
	if(!impl) return SVGAngle(0);
	return SVGSafeCreator<SVGAngle, SVGAngleImpl>::create(impl->createSVGAngle());
}

SVGPoint SVGSVGElement::createSVGPoint()
{
	if(!impl) return SVGPoint(0);
	return SVGSafeCreator<SVGPoint, SVGPointImpl>::create(impl->createSVGPoint());
}

SVGMatrix SVGSVGElement::createSVGMatrix()
{
	if(!impl) return SVGMatrix(0);
	return SVGSafeCreator<SVGMatrix, SVGMatrixImpl>::create(impl->createSVGMatrix());
}

SVGRect SVGSVGElement::createSVGRect()
{
	if(!impl) return SVGRect(0);
	return SVGSafeCreator<SVGRect, SVGRectImpl>::create(impl->createSVGRect());
}

SVGTransform SVGSVGElement::createSVGTransform()
{
	if(!impl) return SVGTransform(0);
	return SVGSafeCreator<SVGTransform, SVGTransformImpl>::create(impl->createSVGTransform());
}

SVGTransform SVGSVGElement::createSVGTransformFromMatrix(const SVGMatrix &matrix)
{
	if(!impl) return SVGTransform(0);
	return SVGSafeCreator<SVGTransform, SVGTransformImpl>::create(impl->createSVGTransformFromMatrix(matrix.handle()));
}

SVGElement SVGSVGElement::getElementById(const DOM::DOMString &elementId)
{
	if(!impl) return SVGElement(0);
	return SVGSafeCreator<SVGElement, SVGElementImpl>::create(impl->getElementById(elementId));
}
 
// vim:ts=4:noet
