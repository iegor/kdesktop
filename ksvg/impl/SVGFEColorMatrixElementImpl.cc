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

#include "SVGAnimatedStringImpl.h"
#include "SVGAnimatedNumberListImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGFEColorMatrixElementImpl.h"

using namespace KSVG;

SVGFEColorMatrixElementImpl::SVGFEColorMatrixElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGFilterPrimitiveStandardAttributesImpl()
{
	m_in1 = new SVGAnimatedStringImpl();
	m_in1->ref();

	m_type = new SVGAnimatedEnumerationImpl();
	m_type->ref();

	m_values = new SVGAnimatedNumberListImpl();
	m_values->ref();
}

SVGFEColorMatrixElementImpl::~SVGFEColorMatrixElementImpl()
{
	if(m_in1)
		m_in1->deref();
	if(m_type)
		m_type->deref();
	if(m_values)
		m_values->deref();
}

SVGAnimatedStringImpl *SVGFEColorMatrixElementImpl::in1() const
{
	return m_in1;
}

SVGAnimatedEnumerationImpl *SVGFEColorMatrixElementImpl::type() const
{
	return m_type;
}

SVGAnimatedNumberListImpl *SVGFEColorMatrixElementImpl::values() const
{
	return m_values;
}

// vim:ts=4:noet
