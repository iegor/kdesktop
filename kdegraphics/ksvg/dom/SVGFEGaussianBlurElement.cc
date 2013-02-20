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

#include "SVGFEGaussianBlurElement.h"
#include "SVGFEGaussianBlurElementImpl.h"
#include "SVGAnimatedString.h"
#include "SVGAnimatedNumber.h"

using namespace KSVG;

SVGFEGaussianBlurElement::SVGFEGaussianBlurElement() : SVGElement(), SVGFilterPrimitiveStandardAttributes()
{
	impl = 0;
}

SVGFEGaussianBlurElement::SVGFEGaussianBlurElement(const SVGFEGaussianBlurElement &other) : SVGElement(other), SVGFilterPrimitiveStandardAttributes(other), impl(0)
{
	(*this) = other;
}

SVGFEGaussianBlurElement &SVGFEGaussianBlurElement::operator =(const SVGFEGaussianBlurElement &other)
{
	SVGElement::operator=(other);
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

SVGFEGaussianBlurElement::SVGFEGaussianBlurElement(SVGFEGaussianBlurElementImpl *other) : SVGElement(other), SVGFilterPrimitiveStandardAttributes(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGFEGaussianBlurElement::~SVGFEGaussianBlurElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedString SVGFEGaussianBlurElement::in1() const
{
	if(!impl) return SVGAnimatedString(0);
	return SVGAnimatedString(impl->in1());
}

SVGAnimatedNumber SVGFEGaussianBlurElement::stdDeviationX() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->stdDeviationX());
}

SVGAnimatedNumber SVGFEGaussianBlurElement::stdDeviationY() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->stdDeviationY());
}

void SVGFEGaussianBlurElement::setStdDeviation(float stdDeviationX, float stdDeviationY)
{
	if(impl)
		impl->setStdDeviation(stdDeviationX, stdDeviationY);
}

// vim:ts=4:noet
