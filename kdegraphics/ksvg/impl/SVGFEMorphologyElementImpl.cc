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
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGFEMorphologyElementImpl.h"

using namespace KSVG;

SVGFEMorphologyElementImpl::SVGFEMorphologyElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGFilterPrimitiveStandardAttributesImpl()
{
	m_in1 = new SVGAnimatedStringImpl();
	m_in1->ref();

	m_operator = new SVGAnimatedEnumerationImpl();
	m_operator->ref();

	m_radiusX = new SVGAnimatedLengthImpl();
	m_radiusX->ref();

	m_radiusY = new SVGAnimatedLengthImpl();
	m_radiusY->ref();
}

SVGFEMorphologyElementImpl::~SVGFEMorphologyElementImpl()
{
	if(m_in1)
		m_in1->deref();
	if(m_operator)
		m_operator->deref();
	if(m_radiusX)
		m_radiusX->deref();
	if(m_radiusY)
		m_radiusY->deref();
}

SVGAnimatedStringImpl *SVGFEMorphologyElementImpl::in1() const
{
	return m_in1;
}

SVGAnimatedEnumerationImpl *SVGFEMorphologyElementImpl::Operator() const
{
	return m_operator;
}

SVGAnimatedLengthImpl *SVGFEMorphologyElementImpl::radiusX() const
{
	return m_radiusX;
}

SVGAnimatedLengthImpl *SVGFEMorphologyElementImpl::radiusY() const
{
	return m_radiusY;
}

// vim:ts=4:noet
