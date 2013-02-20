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

#include "SVGFEConvolveMatrixElement.h"
#include "SVGFEConvolveMatrixElementImpl.h"
#include "SVGAnimatedInteger.h"
#include "SVGAnimatedNumberList.h"
#include "SVGAnimatedNumber.h"
#include "SVGAnimatedEnumeration.h"
#include "SVGAnimatedLength.h"
#include "SVGAnimatedBoolean.h"

using namespace KSVG;

SVGFEConvolveMatrixElement::SVGFEConvolveMatrixElement() : SVGElement(), SVGFilterPrimitiveStandardAttributes()
{
	impl = 0;
}

SVGFEConvolveMatrixElement::SVGFEConvolveMatrixElement(const SVGFEConvolveMatrixElement &other) : SVGElement(other), SVGFilterPrimitiveStandardAttributes(other), impl(0)
{
	(*this) = other;
}

SVGFEConvolveMatrixElement &SVGFEConvolveMatrixElement::operator =(const SVGFEConvolveMatrixElement &other)
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

SVGFEConvolveMatrixElement::SVGFEConvolveMatrixElement(SVGFEConvolveMatrixElementImpl *other) : SVGElement(other), SVGFilterPrimitiveStandardAttributes(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGFEConvolveMatrixElement::~SVGFEConvolveMatrixElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedInteger SVGFEConvolveMatrixElement::orderX() const
{
	if(!impl) return SVGAnimatedInteger(0);
	return SVGAnimatedInteger(impl->orderX());
}

SVGAnimatedInteger SVGFEConvolveMatrixElement::orderY() const
{
	if(!impl) return SVGAnimatedInteger(0);
	return SVGAnimatedInteger(impl->orderY());
}

SVGAnimatedNumberList SVGFEConvolveMatrixElement::kernelMatrix() const
{
	if(!impl) return SVGAnimatedNumberList(0);
	return SVGAnimatedNumberList(impl->kernelMatrix());
}

SVGAnimatedNumber SVGFEConvolveMatrixElement::divisor() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->divisor());
}

SVGAnimatedNumber SVGFEConvolveMatrixElement::bias() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->bias());
}

SVGAnimatedInteger SVGFEConvolveMatrixElement::targetX() const
{
	if(!impl) return SVGAnimatedInteger(0);
	return SVGAnimatedInteger(impl->targetX());
}

SVGAnimatedInteger SVGFEConvolveMatrixElement::targetY() const
{
	if(!impl) return SVGAnimatedInteger(0);
	return SVGAnimatedInteger(impl->targetY());
}

SVGAnimatedEnumeration SVGFEConvolveMatrixElement::edgeMode() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->edgeMode());
}

SVGAnimatedLength SVGFEConvolveMatrixElement::kernelUnitLengthX() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->kernelUnitLengthX());
}

SVGAnimatedLength SVGFEConvolveMatrixElement::kernelUnitLengthY() const
{
	if(!impl) return SVGAnimatedLength(0);
	return SVGAnimatedLength(impl->kernelUnitLengthY());
}

SVGAnimatedBoolean SVGFEConvolveMatrixElement::preserveAlpha() const
{
	if(!impl) return SVGAnimatedBoolean(0);
	return SVGAnimatedBoolean(impl->preserveAlpha());
}

// vim:ts=4:noet
