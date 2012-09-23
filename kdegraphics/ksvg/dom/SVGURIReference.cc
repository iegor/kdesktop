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

#include "SVGURIReference.h"
#include "SVGURIReferenceImpl.h"
#include "SVGAnimatedString.h"

using namespace KSVG;

SVGURIReference::SVGURIReference()
{
	impl = new SVGURIReferenceImpl();
}

SVGURIReference::SVGURIReference(const SVGURIReference &other)
{
	impl = other.impl;
}

SVGURIReference &SVGURIReference::operator=(const SVGURIReference &other)
{
	if(impl == other.impl)
		return *this;

	delete impl;
	impl = other.impl;

	return *this;
}

SVGURIReference::SVGURIReference(SVGURIReferenceImpl *other)
{
	impl = other;
}

SVGURIReference::~SVGURIReference()
{
}

SVGAnimatedString SVGURIReference::href() const
{
	if(!impl) return SVGAnimatedString(0);
	return SVGAnimatedString(impl->href());
}

// vim:ts=4:noet
