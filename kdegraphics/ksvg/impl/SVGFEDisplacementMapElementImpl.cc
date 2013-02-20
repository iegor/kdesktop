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
#include "SVGAnimatedNumberImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGFEDisplacementMapElementImpl.h"

using namespace KSVG;

SVGFEDisplacementMapElementImpl::SVGFEDisplacementMapElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGFilterPrimitiveStandardAttributesImpl()
{
	m_in1 = new SVGAnimatedStringImpl();
	m_in1->ref();

	m_in2 = new SVGAnimatedStringImpl();
	m_in2->ref();

	m_scale = new SVGAnimatedNumberImpl();
	m_scale->ref();

	m_xChannelSelector = new SVGAnimatedEnumerationImpl();
	m_xChannelSelector->ref();

	m_yChannelSelector = new SVGAnimatedEnumerationImpl();
	m_yChannelSelector->ref();
}

SVGFEDisplacementMapElementImpl::~SVGFEDisplacementMapElementImpl()
{
	if(m_in1)
		m_in1->deref();
	if(m_in2)
		m_in2->deref();
	if(m_scale)
		m_scale->deref();
	if(m_xChannelSelector)
		m_xChannelSelector->deref();
	if(m_yChannelSelector)
		m_yChannelSelector->deref();
}

SVGAnimatedStringImpl *SVGFEDisplacementMapElementImpl::in1() const
{
	return m_in1;
}

SVGAnimatedStringImpl *SVGFEDisplacementMapElementImpl::in2() const
{
	return m_in2;
}

SVGAnimatedNumberImpl *SVGFEDisplacementMapElementImpl::scale() const
{
	return m_scale;
}

SVGAnimatedEnumerationImpl *SVGFEDisplacementMapElementImpl::xChannelSelector() const
{
	return m_xChannelSelector;
}

SVGAnimatedEnumerationImpl *SVGFEDisplacementMapElementImpl::yChannelSelector() const
{
	return m_yChannelSelector;
}

// vim:ts=4:noet
