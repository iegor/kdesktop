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

#include "SVGAnimatedNumberImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGFECompositeElementImpl.h"
#include "SVGAnimatedEnumerationImpl.h"

using namespace KSVG;

SVGFECompositeElementImpl::SVGFECompositeElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGFilterPrimitiveStandardAttributesImpl()
{
	m_in1 = new SVGAnimatedStringImpl();
	m_in1->ref();

	m_in2 = new SVGAnimatedStringImpl();
	m_in2->ref();

	m_operator = new SVGAnimatedEnumerationImpl();
	m_operator->ref();

	m_k1 = new SVGAnimatedNumberImpl();
	m_k1->ref();

	m_k2 = new SVGAnimatedNumberImpl();
	m_k2->ref();

	m_k3 = new SVGAnimatedNumberImpl();
	m_k3->ref();

	m_k4 = new SVGAnimatedNumberImpl();
	m_k4->ref();
}

SVGFECompositeElementImpl::~SVGFECompositeElementImpl()
{
	if(m_in1)
		m_in1->deref();
	if(m_in2)
		m_in2->deref();
	if(m_operator)
		m_operator->deref();
	if(m_k1)
		m_k1->deref();
	if(m_k2)
		m_k2->deref();
	if(m_k3)
		m_k3->deref();
	if(m_k4)
		m_k4->deref();
}

SVGAnimatedStringImpl *SVGFECompositeElementImpl::in1() const
{
	return m_in1;
}

SVGAnimatedStringImpl *SVGFECompositeElementImpl::in2() const
{
	return m_in2;
}

SVGAnimatedEnumerationImpl *SVGFECompositeElementImpl::Operator() const
{
	return m_operator;
}

SVGAnimatedNumberImpl *SVGFECompositeElementImpl::k1() const
{
	return m_k1;
}

SVGAnimatedNumberImpl *SVGFECompositeElementImpl::k2() const
{
	return m_k2;
}

SVGAnimatedNumberImpl *SVGFECompositeElementImpl::k3() const
{
	return m_k3;
}

SVGAnimatedNumberImpl *SVGFECompositeElementImpl::k4() const
{
	return m_k4;
}

// vim:ts=4:noet
