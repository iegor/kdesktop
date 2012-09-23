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
#include "SVGAnimatedNumberListImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGComponentTransferFunctionElementImpl.h"

using namespace KSVG;

SVGComponentTransferFunctionElementImpl::SVGComponentTransferFunctionElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl)
{
	m_type = new SVGAnimatedEnumerationImpl();
	m_type->ref();

	m_tableValues = new SVGAnimatedNumberListImpl();
	m_tableValues->ref();

	m_slope = new SVGAnimatedNumberImpl();
	m_slope->ref();

	m_intercept = new SVGAnimatedNumberImpl();
	m_intercept->ref();

	m_amplitude = new SVGAnimatedNumberImpl();
	m_amplitude->ref();

	m_exponent = new SVGAnimatedNumberImpl();
	m_exponent->ref();

	m_offset = new SVGAnimatedNumberImpl();
	m_offset->ref();
}

SVGComponentTransferFunctionElementImpl::~SVGComponentTransferFunctionElementImpl()
{
	if(m_type)
		m_type->deref();
	if(m_tableValues)
		m_tableValues->deref();
	if(m_slope)
		m_slope->deref();
	if(m_intercept)
		m_intercept->deref();
	if(m_amplitude)
		m_amplitude->deref();
	if(m_exponent)
		m_exponent->deref();
	if(m_offset)
		m_offset->deref();
}

SVGAnimatedEnumerationImpl *SVGComponentTransferFunctionElementImpl::type() const
{
	return m_type;
}

SVGAnimatedNumberListImpl *SVGComponentTransferFunctionElementImpl::tableValues() const
{
	return m_tableValues;
}

SVGAnimatedNumberImpl *SVGComponentTransferFunctionElementImpl::slope() const
{
	return m_slope;
}

SVGAnimatedNumberImpl *SVGComponentTransferFunctionElementImpl::intercept() const
{
	return m_intercept;
}

SVGAnimatedNumberImpl *SVGComponentTransferFunctionElementImpl::amplitude() const
{
	return m_amplitude;
}

SVGAnimatedNumberImpl *SVGComponentTransferFunctionElementImpl::exponent() const
{
	return m_exponent;
}

SVGAnimatedNumberImpl *SVGComponentTransferFunctionElementImpl::offset() const
{
	return m_offset;
}

// vim:ts=4:noet
