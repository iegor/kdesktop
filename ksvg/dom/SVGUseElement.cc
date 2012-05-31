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

#include "SVGUseElement.h"
#include "SVGUseElementImpl.h"
#include "SVGElementInstance.h"
#include "SVGAnimatedLength.h"

using namespace KSVG;

SVGUseElement::SVGUseElement() : SVGElement(), SVGTests(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable(), SVGTransformable(), SVGURIReference()
{
	impl = 0;
}

SVGUseElement::SVGUseElement(const SVGUseElement &other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGTransformable(other), SVGURIReference(other), impl(0)
{
	(*this) = other;
}

SVGUseElement &SVGUseElement::operator =(const SVGUseElement &other)
{
	SVGElement::operator=(other);
	SVGTests::operator=(other);
	SVGLangSpace::operator=(other);
	SVGExternalResourcesRequired::operator=(other);
	SVGStylable::operator=(other);
	SVGTransformable::operator=(other);
	SVGURIReference::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGUseElement::SVGUseElement(SVGUseElementImpl *other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGTransformable(other), SVGURIReference(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGUseElement::~SVGUseElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedLength SVGUseElement::x() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->x());
}

SVGAnimatedLength SVGUseElement::y() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->y());
}

SVGAnimatedLength SVGUseElement::width() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->width());
}

SVGAnimatedLength SVGUseElement::height() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->height());
}

SVGElementInstance SVGUseElement::instanceRoot() const
{
	if(!impl) return SVGElementInstance(0);
	return impl->instanceRoot();
}

SVGElementInstance SVGUseElement::animatedInstanceRoot() const
{
	if(!impl) return SVGElementInstance(0);
	return impl->animatedInstanceRoot();
}

// vim:ts=4:noet
