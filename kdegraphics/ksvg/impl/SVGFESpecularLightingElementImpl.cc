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
#include "SVGFESpecularLightingElementImpl.h"

using namespace KSVG;

SVGFESpecularLightingElementImpl::SVGFESpecularLightingElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGFilterPrimitiveStandardAttributesImpl()
{
	m_in1 = new SVGAnimatedStringImpl();
	m_in1->ref();

	m_surfaceScale = new SVGAnimatedNumberImpl();
	m_surfaceScale->ref();

	m_specularConstant = new SVGAnimatedNumberImpl();
	m_specularConstant->ref();

	m_specularExponent = new SVGAnimatedNumberImpl();
	m_specularExponent->ref();
}

SVGFESpecularLightingElementImpl::~SVGFESpecularLightingElementImpl()
{
	if(m_in1)
		m_in1->deref();
	if(m_surfaceScale)
		m_surfaceScale->deref();
	if(m_specularConstant)
		m_specularConstant->deref();
	if(m_specularExponent)
		m_specularExponent->deref();
}

SVGAnimatedStringImpl *SVGFESpecularLightingElementImpl::in1() const
{
	return m_in1;
}

SVGAnimatedNumberImpl *SVGFESpecularLightingElementImpl::surfaceScale() const
{
	return m_surfaceScale;
}

SVGAnimatedNumberImpl *SVGFESpecularLightingElementImpl::specularConstant() const
{
	return m_specularConstant;
}

SVGAnimatedNumberImpl *SVGFESpecularLightingElementImpl::specularExponent() const
{
	return m_specularExponent;
}

// vim:ts=4:noet
