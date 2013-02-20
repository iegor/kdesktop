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
#include "SVGLineElement.h"
#include "SVGLineElementImpl.h"
#include "SVGAnimatedLength.h"

using namespace KSVG;

SVGLineElement::SVGLineElement() : SVGElement(), SVGTests(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable(), SVGTransformable()
{
	impl = 0;
}

SVGLineElement::SVGLineElement(const SVGLineElement &other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGTransformable(other), impl(0)
{
	(*this) = other;
}

SVGLineElement &SVGLineElement::operator=(const SVGLineElement &other)
{
	SVGElement::operator=(other);
	SVGTests::operator=(other);
	SVGLangSpace::operator=(other);
	SVGExternalResourcesRequired::operator=(other);
	SVGStylable::operator=(other);
	SVGTransformable::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGLineElement::SVGLineElement(SVGLineElementImpl *other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGTransformable(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGLineElement::~SVGLineElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedLength SVGLineElement::x1()
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->x1());
}

SVGAnimatedLength SVGLineElement::y1()
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->y1());
}

SVGAnimatedLength SVGLineElement::x2()
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->x2());
}

SVGAnimatedLength SVGLineElement::y2()
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->y2());
}

// vim:ts=4:noet
