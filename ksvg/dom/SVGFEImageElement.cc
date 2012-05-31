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

#include "SVGFEImageElement.h"
#include "SVGFEImageElementImpl.h"

using namespace KSVG;

SVGFEImageElement::SVGFEImageElement() : SVGElement(), SVGURIReference(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable(), SVGFilterPrimitiveStandardAttributes()
{
	impl = 0;
}

SVGFEImageElement::SVGFEImageElement(const SVGFEImageElement &other) : SVGElement(other), SVGURIReference(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGFilterPrimitiveStandardAttributes(other), impl(0)
{
	(*this) = other;
}

SVGFEImageElement &SVGFEImageElement::operator =(const SVGFEImageElement &other)
{
	SVGElement::operator=(other);
	SVGURIReference::operator=(other);
	SVGLangSpace::operator=(other);
	SVGExternalResourcesRequired::operator=(other);
	SVGStylable::operator=(other);
        SVGFilterPrimitiveStandardAttributes::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGFEImageElement::SVGFEImageElement(SVGFEImageElementImpl *other) : SVGElement(other), SVGURIReference(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGFilterPrimitiveStandardAttributes(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGFEImageElement::~SVGFEImageElement()
{
	if(impl)
		impl->deref();
}

// vim:ts=4:noet
