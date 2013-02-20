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

#include "SVGCursorElement.h"
#include "SVGCursorElementImpl.h"
#include "SVGAnimatedLength.h"

using namespace KSVG;

SVGCursorElement::SVGCursorElement() : SVGElement(), SVGURIReference(), SVGTests(), SVGExternalResourcesRequired()
{
	impl = 0;
}

SVGCursorElement::SVGCursorElement(const SVGCursorElement &other) : SVGElement(other), SVGURIReference(other), SVGTests(other), SVGExternalResourcesRequired(other), impl(0)
{
	(*this) = other;
}

SVGCursorElement &SVGCursorElement::operator =(const SVGCursorElement &other)
{
	SVGElement::operator=(other);
	SVGURIReference::operator=(other);
	SVGTests::operator=(other);
	SVGExternalResourcesRequired::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGCursorElement::SVGCursorElement(SVGCursorElementImpl *other) : SVGElement(other), SVGURIReference(other), SVGTests(other), SVGExternalResourcesRequired(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGCursorElement::~SVGCursorElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedLength SVGCursorElement::x() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->x());
}

SVGAnimatedLength SVGCursorElement::y() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->y());
}

// vim:ts=4:noet
