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
#include "SVGImageElement.h"
#include "SVGImageElementImpl.h"
#include "SVGAnimatedLength.h"
#include "SVGAnimatedPreserveAspectRatio.h"

using namespace KSVG;

SVGImageElement::SVGImageElement() : SVGElement(), SVGTests(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable(), SVGTransformable(), SVGURIReference()
{
	impl = 0;
}

SVGImageElement::SVGImageElement(const SVGImageElement &other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGTransformable(other), SVGURIReference(other), impl(0)
{
	(*this) = other;
}

SVGImageElement &SVGImageElement::operator=(const SVGImageElement &other)
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

SVGImageElement::SVGImageElement(SVGImageElementImpl *other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGTransformable(other), SVGURIReference(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGImageElement::~SVGImageElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedLength SVGImageElement::x()
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->x());
}

SVGAnimatedLength SVGImageElement::y()
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->y());
}

SVGAnimatedLength SVGImageElement::width()
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->width());
}

SVGAnimatedLength SVGImageElement::height()
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->height());
}

SVGAnimatedPreserveAspectRatio SVGImageElement::preserveAspectRatio() const
{
	if(!impl) return SVGAnimatedPreserveAspectRatio(0);
	return SVGAnimatedPreserveAspectRatio(impl->preserveAspectRatio());
}

// vim:ts=4:noet
