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

#include "SVGMarkerElement.h"
#include "SVGMarkerElementImpl.h"
#include "SVGAnimatedLength.h"
#include "SVGAnimatedAngle.h"
#include "SVGAngle.h"
#include "SVGAnimatedEnumeration.h"

using namespace KSVG;

SVGMarkerElement::SVGMarkerElement() : SVGElement(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable(), SVGFitToViewBox()
{
	impl = 0;
}

SVGMarkerElement::SVGMarkerElement(const SVGMarkerElement &other) : SVGElement(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGFitToViewBox(other), impl(0)
{
	(*this) = other;
}

SVGMarkerElement &SVGMarkerElement::operator =(const SVGMarkerElement &other)
{
	SVGElement::operator=(other);
	SVGLangSpace::operator=(other);
	SVGExternalResourcesRequired::operator=(other);
	SVGStylable::operator=(other);
	SVGFitToViewBox::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGMarkerElement::SVGMarkerElement(SVGMarkerElementImpl *other) : SVGElement(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGFitToViewBox(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGMarkerElement::~SVGMarkerElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedLength SVGMarkerElement::refX() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->refX());
}

SVGAnimatedLength SVGMarkerElement::refY() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->refY());
}

SVGAnimatedEnumeration SVGMarkerElement::markerUnits() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->markerUnits());
}

SVGAnimatedLength SVGMarkerElement::markerWidth() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->markerWidth());
}

SVGAnimatedLength SVGMarkerElement::markerHeight() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->markerHeight());
}

SVGAnimatedEnumeration SVGMarkerElement::orientType() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->orientType());
}

SVGAnimatedAngle SVGMarkerElement::orientAngle() const
{
	if(!impl) return SVGAnimatedAngle(0);
	return SVGAnimatedAngle(impl->orientAngle());
}

void SVGMarkerElement::setOrientToAuto()
{
	if(impl)
		impl->setOrientToAuto();
}

void SVGMarkerElement::setOrientToAngle(const SVGAngle &angle)
{
	if(impl)
		impl->setOrientToAngle(angle.handle());
}

// vim:ts=4:noet
