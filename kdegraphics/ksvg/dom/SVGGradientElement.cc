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
#include "SVGAnimatedTransformList.h"
#include "SVGAnimatedEnumeration.h"

using namespace KSVG;

SVGGradientElement::SVGGradientElement() : SVGElement(), SVGURIReference(), SVGExternalResourcesRequired(), SVGStylable(), SVGUnitTypes()
{
	impl = 0;
}

SVGGradientElement::SVGGradientElement(const SVGGradientElement &other) : SVGElement(other), SVGURIReference(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGUnitTypes(other), impl(0)
{
	(*this) = other;
}

SVGGradientElement &SVGGradientElement::operator =(const SVGGradientElement &other)
{
	SVGElement::operator=(other);
	SVGURIReference::operator=(other);
	SVGExternalResourcesRequired::operator=(other);
	SVGStylable::operator=(other);

	if(impl == other.impl)
		return *this;

	impl = other.impl;

	return *this;
}

SVGGradientElement::SVGGradientElement(SVGGradientElementImpl *other) : SVGElement(other), SVGURIReference(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGUnitTypes()
{
	impl = other;
}

SVGGradientElement::~SVGGradientElement()
{
}

SVGAnimatedEnumeration SVGGradientElement::gradientUnits() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->gradientUnits());
}

SVGAnimatedTransformList SVGGradientElement::gradientTransform() const
{
	if(!impl) return SVGAnimatedTransformList(0);
	return SVGAnimatedTransformList(impl->gradientTransform());
}

SVGAnimatedEnumeration SVGGradientElement::spreadMethod() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->spreadMethod());
}

// vim:ts=4:noet
