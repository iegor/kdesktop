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

#include "SVGFilterElementImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedIntegerImpl.h"
#include "SVGAnimatedEnumerationImpl.h"

using namespace KSVG;

SVGFilterElementImpl::SVGFilterElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGURIReferenceImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this)
{
	m_filterUnits = new SVGAnimatedEnumerationImpl();
	m_filterUnits->ref();

	m_primitiveUnits = new SVGAnimatedEnumerationImpl();
	m_primitiveUnits->ref();

	m_x = new SVGAnimatedLengthImpl();
	m_x->ref();

	m_y = new SVGAnimatedLengthImpl();
	m_y->ref();

	m_width = new SVGAnimatedLengthImpl();
	m_width->ref();

	m_height = new SVGAnimatedLengthImpl();
	m_height->ref();

	m_filterResX = new SVGAnimatedIntegerImpl();
	m_filterResX->ref();

	m_filterResY = new SVGAnimatedIntegerImpl();
	m_filterResY->ref();
}

SVGFilterElementImpl::~SVGFilterElementImpl()
{
	if(m_filterUnits)
		m_filterUnits->deref();
	if(m_primitiveUnits)
		m_primitiveUnits->deref();
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_width)
		m_width->deref();
	if(m_height)
		m_height->deref();
	if(m_filterResX)
		m_filterResX->deref();
	if(m_filterResY)
		m_filterResY->deref();
}

SVGAnimatedEnumerationImpl *SVGFilterElementImpl::filterUnits() const
{
	return m_filterUnits;
}

SVGAnimatedEnumerationImpl *SVGFilterElementImpl::primitiveUnits() const
{
	return m_primitiveUnits;
}

SVGAnimatedLengthImpl *SVGFilterElementImpl::x() const
{
	return m_x;
}

SVGAnimatedLengthImpl *SVGFilterElementImpl::y() const
{
	return m_y;
}

SVGAnimatedLengthImpl *SVGFilterElementImpl::width() const
{
	return m_width;
}

SVGAnimatedLengthImpl *SVGFilterElementImpl::height() const
{
	return m_height;
}

SVGAnimatedIntegerImpl *SVGFilterElementImpl::filterResX() const
{
	return m_filterResX;
}

SVGAnimatedIntegerImpl *SVGFilterElementImpl::filterResY() const
{
	return m_filterResY;
}

void SVGFilterElementImpl::setFilterRes(unsigned long /*filterResX*/, unsigned long /*filterResY*/)
{
}

// vim:ts=4:noet
