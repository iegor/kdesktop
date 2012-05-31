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

#include "SVGAnimateMotionElement.h"
#include "SVGAnimateMotionElementImpl.h"

using namespace KSVG;

SVGAnimateMotionElement::SVGAnimateMotionElement() : SVGAnimationElement()
{
	impl = 0;
}

SVGAnimateMotionElement::SVGAnimateMotionElement(const SVGAnimateMotionElement &other) : SVGAnimationElement(), impl(0)
{
	(*this) = other;
}

SVGAnimateMotionElement &SVGAnimateMotionElement::operator =(const SVGAnimateMotionElement &other)
{
	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGAnimateMotionElement::SVGAnimateMotionElement(SVGAnimateMotionElementImpl *other) : SVGAnimationElement()
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAnimateMotionElement::~SVGAnimateMotionElement()
{
	if(impl)
		impl->deref();
}

// vim:ts=4:noet
