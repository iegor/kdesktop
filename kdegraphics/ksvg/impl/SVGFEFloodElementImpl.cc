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

#include "SVGFEFloodElementImpl.h"
#include "SVGAnimatedStringImpl.h"

using namespace KSVG;

SVGFEFloodElementImpl::SVGFEFloodElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGStylableImpl(this), SVGFilterPrimitiveStandardAttributesImpl()
{
	m_in1 = new SVGAnimatedStringImpl();
	m_in1->ref();
}

SVGFEFloodElementImpl::~SVGFEFloodElementImpl()
{
	if(m_in1)
		m_in1->deref();
}

SVGAnimatedStringImpl *SVGFEFloodElementImpl::in1() const
{
	return m_in1;
}

// vim:ts=4:noet
