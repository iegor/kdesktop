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

#include <kdebug.h>

#include "SVGICCColorImpl.h"
#include "SVGNumberListImpl.h"

using namespace KSVG;

#include "SVGICCColorImpl.lut.h"
#include "ksvg_bridge.h"

SVGICCColorImpl::SVGICCColorImpl() : DOM::DomShared()
{
	m_colors = new SVGNumberListImpl();
	m_colors->ref();
}

SVGICCColorImpl::SVGICCColorImpl(const SVGICCColorImpl &other) : DOM::DomShared()
{
	(*this) = other;
}

SVGICCColorImpl::~SVGICCColorImpl()
{
	if(m_colors)
		m_colors->deref();
}

SVGICCColorImpl &SVGICCColorImpl::operator=(const SVGICCColorImpl &other)
{
	m_colorProfile = other.m_colorProfile;
	*m_colors = *(other.m_colors);

	return *this;
}

DOM::DOMString SVGICCColorImpl::colorProfile() const
{
	return m_colorProfile;
}

void SVGICCColorImpl::setColorProfile(const DOM::DOMString &colorProfile)
{
	m_colorProfile = colorProfile;
}

SVGNumberListImpl *SVGICCColorImpl::colors() const
{
	return m_colors;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGICCColorImpl::s_hashTable 3
 colorProfile		SVGICCColorImpl::ColorProfile	DontDelete
 colors				SVGICCColorImpl::Colors 		DontDelete|ReadOnly
@end
*/

Value SVGICCColorImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case ColorProfile:
			return String(m_colorProfile.string());
		case Colors:
			return m_colors->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGICCColorImpl::putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case ColorProfile:
			m_colorProfile = value.toString(exec).string();
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
