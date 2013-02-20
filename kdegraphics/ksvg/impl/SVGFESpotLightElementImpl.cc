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
#include "SVGFESpotLightElementImpl.h"

using namespace KSVG;

SVGFESpotLightElementImpl::SVGFESpotLightElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl)
{
	m_x = new SVGAnimatedNumberImpl();
	m_x->ref();

	m_y = new SVGAnimatedNumberImpl();
	m_y->ref();

	m_z = new SVGAnimatedNumberImpl();
	m_z->ref();

	m_pointsAtX = new SVGAnimatedNumberImpl();
	m_pointsAtX->ref();

	m_pointsAtY = new SVGAnimatedNumberImpl();
	m_pointsAtY->ref();

	m_pointsAtZ = new SVGAnimatedNumberImpl();
	m_pointsAtZ->ref();

	m_specularExponent = new SVGAnimatedNumberImpl();
	m_specularExponent->ref();

	m_limitingConeAngle = new SVGAnimatedNumberImpl();
	m_limitingConeAngle->ref();
}

SVGFESpotLightElementImpl::~SVGFESpotLightElementImpl()
{
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_z)
		m_z->deref();
	if(m_pointsAtX)
		m_pointsAtX->deref();
	if(m_pointsAtY)
		m_pointsAtY->deref();
	if(m_pointsAtZ)
		m_pointsAtZ->deref();
	if(m_specularExponent)
		m_specularExponent->deref();
	if(m_limitingConeAngle)
		m_limitingConeAngle->deref();
}

SVGAnimatedNumberImpl *SVGFESpotLightElementImpl::x() const
{
	return m_x;
}

SVGAnimatedNumberImpl *SVGFESpotLightElementImpl::y() const
{
	return m_y;
}

SVGAnimatedNumberImpl *SVGFESpotLightElementImpl::z() const
{
	return m_z;
}

SVGAnimatedNumberImpl *SVGFESpotLightElementImpl::pointsAtX() const
{
	return m_pointsAtX;
}

SVGAnimatedNumberImpl *SVGFESpotLightElementImpl::pointsAtY() const
{
	return m_pointsAtY;
}

SVGAnimatedNumberImpl *SVGFESpotLightElementImpl::pointsAtZ() const
{
	return m_pointsAtZ;
}

SVGAnimatedNumberImpl *SVGFESpotLightElementImpl::specularExponent() const
{
	return m_specularExponent;
}

SVGAnimatedNumberImpl *SVGFESpotLightElementImpl::limitingConeAngle() const
{
	return m_limitingConeAngle;
}

// vim:ts=4:noet
