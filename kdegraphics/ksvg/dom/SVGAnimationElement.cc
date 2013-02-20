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

#include "SVGAnimationElement.h"
#include "SVGAnimationElementImpl.h"

using namespace KSVG;

SVGAnimationElement::SVGAnimationElement() : SVGElement(), SVGTests(), SVGExternalResourcesRequired()
{
	impl = 0;
}

SVGAnimationElement::SVGAnimationElement(const SVGAnimationElement &other) : SVGElement(other), SVGTests(other), SVGExternalResourcesRequired(other), impl(0) 
{
	(*this) = other;
}

SVGAnimationElement &SVGAnimationElement::operator=(const SVGAnimationElement &other)
{
	SVGElement::operator=(other);
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

SVGAnimationElement::SVGAnimationElement(SVGAnimationElementImpl *other) : SVGElement(other), SVGTests(other), SVGExternalResourcesRequired(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAnimationElement::~SVGAnimationElement()
{
	if(impl)
		impl->deref();
}

SVGElement SVGAnimationElement::targetElement() const
{
	if(!impl) return SVGElement(0);
	return SVGElement(impl->targetElement());
}

float SVGAnimationElement::getStartTime()
{
	if(!impl) return -1;
	return impl->getStartTime();
}

float SVGAnimationElement::getCurrentTime()
{
	if(!impl) return -1;
	return impl->getCurrentTime();
}

float SVGAnimationElement::getSimpleDuration()
{
	if(!impl) return -1;
	return impl->getSimpleDuration();
}

// vim:ts=4:noet
