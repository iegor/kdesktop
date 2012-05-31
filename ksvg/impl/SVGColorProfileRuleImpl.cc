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

#include "SVGColorProfileRuleImpl.h"

using namespace KSVG;

SVGColorProfileRuleImpl::SVGColorProfileRuleImpl() : SVGCSSRuleImpl()
{
}

SVGColorProfileRuleImpl::~SVGColorProfileRuleImpl()
{
}

void SVGColorProfileRuleImpl::setSrc(const DOM::DOMString &src)
{
	m_src = src;
}

DOM::DOMString SVGColorProfileRuleImpl::src() const
{
	return m_src;
}

void SVGColorProfileRuleImpl::setName(const DOM::DOMString &name)
{
	m_name = name;
}

DOM::DOMString SVGColorProfileRuleImpl::name() const
{
	return m_name;
}

void SVGColorProfileRuleImpl::setRenderingIntent(unsigned short renderingIntent)
{
	m_renderingIntent = renderingIntent;
}

unsigned short SVGColorProfileRuleImpl::renderingIntent() const
{
	return m_renderingIntent;
}

// vim:ts=4:noet
