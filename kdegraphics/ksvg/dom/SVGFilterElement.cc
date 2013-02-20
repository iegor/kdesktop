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

#include "SVGFilterElement.h"
#include "SVGFilterElementImpl.h"
#include "SVGAnimatedEnumeration.h"
#include "SVGAnimatedInteger.h"
#include "SVGAnimatedLength.h"

using namespace KSVG;

SVGFilterElement::SVGFilterElement() : SVGElement(), SVGURIReference(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable(), SVGUnitTypes()
{
	impl = 0;
}

SVGFilterElement::SVGFilterElement(const SVGFilterElement &other) : SVGElement(other), SVGURIReference(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGUnitTypes(), impl(0)
{
	(*this) = other;
}

SVGFilterElement &SVGFilterElement::operator =(const SVGFilterElement &other)
{
	SVGElement::operator=(other);
	SVGURIReference::operator=(other);
	SVGLangSpace::operator=(other);
	SVGExternalResourcesRequired::operator=(other);
	SVGStylable::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGFilterElement::SVGFilterElement(SVGFilterElementImpl *other) : SVGElement(other), SVGURIReference(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGUnitTypes()
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGFilterElement::~SVGFilterElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedEnumeration SVGFilterElement::filterUnits() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->filterUnits());
}

SVGAnimatedEnumeration SVGFilterElement::primitiveUnits() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->primitiveUnits());
}

SVGAnimatedLength SVGFilterElement::x() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->x());
}

SVGAnimatedLength SVGFilterElement::y() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->y());
}

SVGAnimatedLength SVGFilterElement::width() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->width());
}

SVGAnimatedLength SVGFilterElement::height() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->height());
}

SVGAnimatedInteger SVGFilterElement::filterResX() const
{
	if(!impl) return SVGAnimatedInteger(0);
	return SVGAnimatedInteger(impl->filterResX());
}

SVGAnimatedInteger SVGFilterElement::filterResY() const
{
	if(!impl) return SVGAnimatedInteger(0);
	return SVGAnimatedInteger(impl->filterResY());
}

void SVGFilterElement::setFilterRes(unsigned long filterResX, unsigned long filterResY)
{
	if(impl)
		impl->setFilterRes(filterResX, filterResY);
}

// vim:ts=4:noet
