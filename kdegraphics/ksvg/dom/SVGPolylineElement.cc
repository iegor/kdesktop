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
#include "SVGPolylineElement.h"
#include "SVGPolylineElementImpl.h"
#include "SVGPointList.h"

using namespace KSVG;

SVGPolylineElement::SVGPolylineElement() : SVGElement(), SVGTests(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable(), SVGTransformable(), SVGAnimatedPoints()
{
	impl = 0;
}

SVGPolylineElement::SVGPolylineElement(const SVGPolylineElement &other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGTransformable(other), SVGAnimatedPoints(other), impl(0)
{
	(*this) = other;
}

SVGPolylineElement &SVGPolylineElement::operator=(const SVGPolylineElement &other)
{
	SVGElement::operator=(other);
	SVGTests::operator=(other);
	SVGLangSpace::operator=(other);
	SVGExternalResourcesRequired::operator=(other);
	SVGStylable::operator=(other);
	SVGTransformable::operator=(other);
	SVGAnimatedPoints::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGPolylineElement::SVGPolylineElement(SVGPolylineElementImpl *other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other), SVGTransformable(other), SVGAnimatedPoints(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGPolylineElement::~SVGPolylineElement()
{
	if(impl)
		impl->deref();
}

SVGPointList SVGPolylineElement::points()
{
	if(!impl) return SVGPointList(0);
	return SVGPointList(impl->points());
}

SVGPointList SVGPolylineElement::animatedPoints()
{
	if(!impl) return SVGPointList(0);
	return SVGPointList(impl->animatedPoints());
}
