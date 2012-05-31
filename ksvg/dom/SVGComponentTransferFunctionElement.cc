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

#include "SVGComponentTransferFunctionElement.h"
#include "SVGComponentTransferFunctionElementImpl.h"
#include "SVGAnimatedNumber.h"
#include "SVGAnimatedNumberList.h"
#include "SVGAnimatedEnumeration.h"

using namespace KSVG;

SVGComponentTransferFunctionElement::SVGComponentTransferFunctionElement() : SVGElement()
{
	impl = 0;
}

SVGComponentTransferFunctionElement::SVGComponentTransferFunctionElement(const SVGComponentTransferFunctionElement &other) : SVGElement(other), impl(0)
{
	(*this) = other;
}

SVGComponentTransferFunctionElement &SVGComponentTransferFunctionElement::operator =(const SVGComponentTransferFunctionElement &other)
{
	SVGElement::operator=(other);

	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGComponentTransferFunctionElement::SVGComponentTransferFunctionElement(SVGComponentTransferFunctionElementImpl *other) : SVGElement(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGComponentTransferFunctionElement::~SVGComponentTransferFunctionElement()
{
	if(impl)
		impl->deref();
}

SVGAnimatedEnumeration SVGComponentTransferFunctionElement::type() const
{
	if(!impl) return SVGAnimatedEnumeration(0);
	return SVGAnimatedEnumeration(impl->type());
}

SVGAnimatedNumberList SVGComponentTransferFunctionElement::tableValues() const
{
	if(!impl) return SVGAnimatedNumberList(0);
	return SVGAnimatedNumberList(impl->tableValues());
}

SVGAnimatedNumber SVGComponentTransferFunctionElement::slope() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->slope());
}

SVGAnimatedNumber SVGComponentTransferFunctionElement::intercept() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->intercept());
}

SVGAnimatedNumber SVGComponentTransferFunctionElement::amplitude() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->amplitude());
}

SVGAnimatedNumber SVGComponentTransferFunctionElement::exponent() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->exponent());
}

SVGAnimatedNumber SVGComponentTransferFunctionElement::offset() const
{
	if(!impl) return SVGAnimatedNumber(0);
	return SVGAnimatedNumber(impl->offset());
}

// vim:ts=4:noet
