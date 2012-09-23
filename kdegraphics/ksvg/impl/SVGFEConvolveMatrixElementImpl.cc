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
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedIntegerImpl.h"
#include "SVGAnimatedBooleanImpl.h"
#include "SVGAnimatedIntegerImpl.h"
#include "SVGAnimatedNumberListImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGFEConvolveMatrixElementImpl.h"

using namespace KSVG;

SVGFEConvolveMatrixElementImpl::SVGFEConvolveMatrixElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGFilterPrimitiveStandardAttributesImpl()
{
	m_orderX = new SVGAnimatedIntegerImpl();
	m_orderX->ref();

	m_orderY = new SVGAnimatedIntegerImpl();
	m_orderY->ref();

	m_kernelMatrix = new SVGAnimatedNumberListImpl();
	m_kernelMatrix->ref();

	m_divisor = new SVGAnimatedNumberImpl();
	m_divisor->ref();

	m_bias = new SVGAnimatedNumberImpl();
	m_bias->ref();

	m_targetX = new SVGAnimatedIntegerImpl();
	m_targetX->ref();

	m_targetY = new SVGAnimatedIntegerImpl();
	m_targetY->ref();

	m_edgeMode = new SVGAnimatedEnumerationImpl();
	m_edgeMode->ref();

	m_kernelUnitLengthX = new SVGAnimatedLengthImpl();
	m_kernelUnitLengthX->ref();

	m_kernelUnitLengthY = new SVGAnimatedLengthImpl();
	m_kernelUnitLengthY->ref();

	m_preserveAlpha = new SVGAnimatedBooleanImpl();
	m_preserveAlpha->ref();
}

SVGFEConvolveMatrixElementImpl::~SVGFEConvolveMatrixElementImpl()
{
	if(m_orderX)
		m_orderX->deref();
	if(m_orderY)
		m_orderY->deref();
	if(m_kernelMatrix)
		m_kernelMatrix->deref();
	if(m_divisor)
		m_divisor->deref();
	if(m_bias)
		m_bias->deref();
	if(m_targetX)
		m_targetX->deref();
	if(m_targetY)
		m_targetY->deref();
	if(m_edgeMode)
		m_edgeMode->deref();
	if(m_kernelUnitLengthX)
		m_kernelUnitLengthX->deref();
	if(m_kernelUnitLengthY)
		m_kernelUnitLengthY->deref();
	if(m_preserveAlpha)
		m_preserveAlpha->deref();
}

SVGAnimatedIntegerImpl *SVGFEConvolveMatrixElementImpl::orderX() const
{
	return m_orderX;
}

SVGAnimatedIntegerImpl *SVGFEConvolveMatrixElementImpl::orderY() const
{
	return m_orderY;
}

SVGAnimatedNumberListImpl *SVGFEConvolveMatrixElementImpl::kernelMatrix() const
{
	return m_kernelMatrix;
}

SVGAnimatedNumberImpl *SVGFEConvolveMatrixElementImpl::divisor() const
{
	return m_divisor;
}

SVGAnimatedNumberImpl *SVGFEConvolveMatrixElementImpl::bias() const
{
	return m_bias;
}

SVGAnimatedIntegerImpl *SVGFEConvolveMatrixElementImpl::targetX() const
{
	return m_targetX;
}

SVGAnimatedIntegerImpl *SVGFEConvolveMatrixElementImpl::targetY() const
{
	return m_targetY;
}

SVGAnimatedEnumerationImpl *SVGFEConvolveMatrixElementImpl::edgeMode() const
{
	return m_edgeMode;
}

SVGAnimatedLengthImpl *SVGFEConvolveMatrixElementImpl::kernelUnitLengthX() const
{
	return m_kernelUnitLengthX;
}

SVGAnimatedLengthImpl *SVGFEConvolveMatrixElementImpl::kernelUnitLengthY() const
{
	return m_kernelUnitLengthY;
}

SVGAnimatedBooleanImpl *SVGFEConvolveMatrixElementImpl::preserveAlpha() const
{
	return m_preserveAlpha;
}

// vim:ts=4:noet
