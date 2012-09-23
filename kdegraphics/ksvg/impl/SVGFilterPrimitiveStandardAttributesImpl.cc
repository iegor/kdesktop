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

#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGFilterPrimitiveStandardAttributesImpl.h"

using namespace KSVG;

SVGFilterPrimitiveStandardAttributesImpl::SVGFilterPrimitiveStandardAttributesImpl()
{
	m_x = new SVGAnimatedLengthImpl();
	m_x->ref();

	m_y = new SVGAnimatedLengthImpl();
	m_y->ref();

	m_width = new SVGAnimatedLengthImpl();
	m_width->ref();

	m_height = new SVGAnimatedLengthImpl();
	m_height->ref();

	m_result = new SVGAnimatedStringImpl();
	m_result->ref();
}

SVGFilterPrimitiveStandardAttributesImpl::~SVGFilterPrimitiveStandardAttributesImpl()
{
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_width)
		m_width->deref();
	if(m_height)
		m_height->deref();
	if(m_result)
		m_result->deref();
}

SVGAnimatedLengthImpl *SVGFilterPrimitiveStandardAttributesImpl::x() const
{
	return m_x;
}

SVGAnimatedLengthImpl *SVGFilterPrimitiveStandardAttributesImpl::y() const
{
	return m_y;
}

SVGAnimatedLengthImpl *SVGFilterPrimitiveStandardAttributesImpl::width() const
{
	return m_width;
}

SVGAnimatedLengthImpl *SVGFilterPrimitiveStandardAttributesImpl::height() const
{
	return m_height;
}

SVGAnimatedStringImpl *SVGFilterPrimitiveStandardAttributesImpl::result() const
{
	return m_result;
}

// vim:ts=4:noet
