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

#include "SVGTextContentElement.h"
#include "SVGTextContentElementImpl.h"
#include "SVGAnimatedEnumeration.h"
#include "SVGAnimatedLength.h"

using namespace KSVG;

SVGTextContentElement::SVGTextContentElement() : SVGElement(), SVGTests(), SVGLangSpace(), SVGExternalResourcesRequired(), SVGStylable()
{
	impl = new SVGTextContentElementImpl(0);
}

SVGTextContentElement::SVGTextContentElement(const SVGTextContentElement &other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other)
{
	impl = other.impl;
}

SVGTextContentElement &SVGTextContentElement::operator=(const SVGTextContentElement &other)
{
	SVGElement::operator=(other);
	SVGTests::operator=(other);
	SVGLangSpace::operator=(other);
	SVGExternalResourcesRequired::operator=(other);
	SVGStylable::operator=(other);

	if(impl == other.impl)
		return *this;

	delete impl;
	impl = other.impl;

	return *this;
}

SVGTextContentElement::SVGTextContentElement(SVGTextContentElementImpl *other) : SVGElement(other), SVGTests(other), SVGLangSpace(other), SVGExternalResourcesRequired(other), SVGStylable(other)
{
	impl = other;
}

SVGTextContentElement::~SVGTextContentElement()
{
}


SVGAnimatedLength SVGTextContentElement::textLength() const
{
	return impl->textLength();
}

SVGAnimatedEnumeration SVGTextContentElement::lengthAdjust() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->lengthAdjust());
}

long SVGTextContentElement::getNumberOfChars()
{
	return impl->getNumberOfChars();
}

float SVGTextContentElement::getComputedTextLength()
{
	return impl->getComputedTextLength();
}
/*
float SVGTextContentElement::getSubStringLength(const unsigned long &charnum, const unsigned long &nchars)
{
	return impl->getSubStringLength(charnum, nchars);
}

SVGPoint SVGTextContentElement::getStartPositionOfChar(const unsigned long &charnum)
{
	return impl->getStartPositionOfChar(charnum);
}

SVGPoint SVGTextContentElement::getEndPositionOfChar(const unsigned long &charnum)
{
	return impl->getEndPositionOfChar(charnum);
}

SVGRect SVGTextContentElement::getExtentOfChar(const unsigned long &charnum)
{
	return impl->getExtentOfChar(charnum);
}

float SVGTextContentElement::getRotationOfChar(const unsigned long &charnum)
{
	return impl->getRotationOfChar(charnum);
}

long SVGTextContentElement::getCharNumAtPosition(const SVGPoint &point)
{
	return impl->getCharNumAtPosition(point);
}

void SVGTextContentElement::selectSubString(const unsigned long &charnum, const unsigned long &nchars)
{
	return impl->selectSubString(charnum, nchars);
}
*/
