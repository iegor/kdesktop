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

#include "SVGPatternElement.h"
#include "SVGPatternElementImpl.h"
#include "SVGAnimatedTransformList.h"
#include "SVGAnimatedLength.h"
#include "SVGAnimatedEnumeration.h"

using namespace KSVG;

SVGPatternElement::SVGPatternElement() : SVGElement(), SVGURIReference(), SVGTests(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable(), SVGFitToViewBox(), SVGUnitTypes()
{
	impl = 0;
}

SVGPatternElement::SVGPatternElement(const SVGPatternElement &other) : SVGElement(other), SVGURIReference(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGFitToViewBox(other), SVGUnitTypes(), impl(0)
{
	(*this) = other;
}

SVGPatternElement &SVGPatternElement::operator =(const SVGPatternElement &other)
{
	SVGElement::operator=(other);
	SVGTests::operator=(other);
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

SVGPatternElement::SVGPatternElement(SVGPatternElementImpl *other) : SVGElement(other), SVGURIReference(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGFitToViewBox(other), SVGUnitTypes()
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGPatternElement::~SVGPatternElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedEnumeration SVGPatternElement::patternUnits() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->patternUnits());
}

SVGAnimatedEnumeration SVGPatternElement::patternContentUnits() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->patternContentUnits());
}

SVGAnimatedTransformList SVGPatternElement::patternTransform() const
{
	if(!impl) return SVGAnimatedTransformList(0);
	return SVGAnimatedTransformList(impl->patternTransform());
}

SVGAnimatedLength SVGPatternElement::x() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->x());
}

SVGAnimatedLength SVGPatternElement::y() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->y());
}

SVGAnimatedLength SVGPatternElement::width() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->width());
}

SVGAnimatedLength SVGPatternElement::height() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->height());
}

// vim:ts=4:noet
