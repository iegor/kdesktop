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

#include "SVGColorProfileRule.h"
#include "SVGColorProfileRuleImpl.h"
#include "SVGRenderingIntent.h"

using namespace KSVG;

SVGColorProfileRule::SVGColorProfileRule() : SVGCSSRule()
{
	impl = new SVGColorProfileRuleImpl();
	impl->ref();
}

SVGColorProfileRule::SVGColorProfileRule(const SVGColorProfileRule &other) : SVGCSSRule(), impl(0)
{
	(*this) = other;
}

SVGColorProfileRule &SVGColorProfileRule::operator=(const SVGColorProfileRule &other)
{
	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGColorProfileRule::SVGColorProfileRule(SVGColorProfileRuleImpl *other) : SVGCSSRule(other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGColorProfileRule::~SVGColorProfileRule()
{
	if(impl)
		impl->deref();
}

void SVGColorProfileRule::setSrc(const DOM::DOMString &src)
{
	if(impl)
		impl->setSrc(src);
}

DOM::DOMString SVGColorProfileRule::src() const
{
	if(!impl) return DOM::DOMString();
	return impl->src();
}

void SVGColorProfileRule::setName(const DOM::DOMString &name)
{
	if(impl)
		impl->setName(name);
}

DOM::DOMString SVGColorProfileRule::name() const
{
	if(!impl) return DOM::DOMString();
	return impl->name();
}

void SVGColorProfileRule::setRenderingIntent(unsigned short renderingIntent)
{
	if(impl)
		impl->setRenderingIntent(renderingIntent);
}

unsigned short SVGColorProfileRule::renderingIntent() const
{
	if(!impl) return RENDERING_INTENT_UNKNOWN;
	return impl->renderingIntent();
}

// vim:ts=4:noet
