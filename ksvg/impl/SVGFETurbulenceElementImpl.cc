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
#include "SVGAnimatedIntegerImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGFETurbulenceElementImpl.h"

using namespace KSVG;

SVGFETurbulenceElementImpl::SVGFETurbulenceElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGFilterPrimitiveStandardAttributesImpl()
{
	m_baseFrequencyX = new SVGAnimatedNumberImpl();
	m_baseFrequencyX->ref();

	m_baseFrequencyY = new SVGAnimatedNumberImpl();
	m_baseFrequencyY->ref();

	m_numOctaves = new SVGAnimatedIntegerImpl();
	m_numOctaves->ref();

	m_seed = new SVGAnimatedNumberImpl();
	m_seed->ref();

	m_stitchTiles = new SVGAnimatedEnumerationImpl();
	m_stitchTiles->ref();

	m_type = new SVGAnimatedEnumerationImpl();
	m_type->ref();
}

SVGFETurbulenceElementImpl::~SVGFETurbulenceElementImpl()
{
	if(m_baseFrequencyX)
		m_baseFrequencyX->deref();

	if(m_baseFrequencyY)
		m_baseFrequencyY->deref();

	if(m_numOctaves)
		m_numOctaves->deref();

	if(m_seed)
		m_seed->deref();

	if(m_stitchTiles)
		m_stitchTiles->deref();

	if(m_type)
		m_type->deref();
}

SVGAnimatedNumberImpl *SVGFETurbulenceElementImpl::baseFrequencyX() const
{
	return m_baseFrequencyX;
}

SVGAnimatedNumberImpl *SVGFETurbulenceElementImpl::baseFrequencyY() const
{
	return m_baseFrequencyY;
}

SVGAnimatedIntegerImpl *SVGFETurbulenceElementImpl::numOctaves() const
{
	return m_numOctaves;
}

SVGAnimatedNumberImpl *SVGFETurbulenceElementImpl::seed() const
{
	return m_seed;
}

SVGAnimatedEnumerationImpl *SVGFETurbulenceElementImpl::stitchTiles() const
{
	return m_stitchTiles;
}

SVGAnimatedEnumerationImpl *SVGFETurbulenceElementImpl::type() const
{
	return m_type;
}

// vim:ts=4:noet
