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

#include "SVGGlyphElement.h"
#include "SVGGlyphElementImpl.h"

using namespace KSVG;

SVGGlyphElement::SVGGlyphElement() : SVGElement(), SVGStylable()
{
	impl = 0;
}

SVGGlyphElement::SVGGlyphElement(const SVGGlyphElement &other) : SVGElement(other), SVGStylable(other), impl(0)
{
	(*this) = other;
}

SVGGlyphElement &SVGGlyphElement::operator =(const SVGGlyphElement &other)
{
	SVGElement::operator=(other);
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

SVGGlyphElement::SVGGlyphElement(SVGGlyphElementImpl *other) : SVGElement(other), SVGStylable(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGGlyphElement::~SVGGlyphElement()
{
	if(impl)
		impl->deref();
}

// vim:ts=4:noet
