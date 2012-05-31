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
#include "SVGFEGaussianBlurElementImpl.h"

using namespace KSVG;

SVGFEGaussianBlurElementImpl::SVGFEGaussianBlurElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGFilterPrimitiveStandardAttributesImpl()
{
	m_in1 = new SVGAnimatedStringImpl();
	m_in1->ref();

	m_stdDeviationX = new SVGAnimatedNumberImpl();
	m_stdDeviationX->ref();

	m_stdDeviationY = new SVGAnimatedNumberImpl();
	m_stdDeviationY->ref();
}

SVGFEGaussianBlurElementImpl::~SVGFEGaussianBlurElementImpl()
{
	if(m_in1)
		m_in1->deref();
	if(m_stdDeviationX)
		m_stdDeviationX->deref();
	if(m_stdDeviationY)
		m_stdDeviationY->deref();
}

SVGAnimatedStringImpl *SVGFEGaussianBlurElementImpl::in1() const
{
	return m_in1;
}

SVGAnimatedNumberImpl *SVGFEGaussianBlurElementImpl::stdDeviationX() const
{
	return m_stdDeviationX;
}

SVGAnimatedNumberImpl *SVGFEGaussianBlurElementImpl::stdDeviationY() const
{
	return m_stdDeviationY;
}

void SVGFEGaussianBlurElementImpl::setStdDeviation(float /*stdDeviationX*/, float /*stdDeviationY*/)
{
}

// vim:ts=4:noet
