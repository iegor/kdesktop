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

#include "SVGLangSpace.h"
#include "SVGLangSpaceImpl.h"

using namespace KSVG;

// This class can't be constructed seperately.
SVGLangSpace::SVGLangSpace()
{
	impl = 0;
}

SVGLangSpace::SVGLangSpace(const SVGLangSpace &other) : impl(0)
{
	(*this) = other;
}

SVGLangSpace &SVGLangSpace::operator=(const SVGLangSpace &other)
{
	if(impl == other.impl)
		return *this;

	impl = other.impl;

	return *this;
}

SVGLangSpace::SVGLangSpace(SVGLangSpaceImpl *other)
{
	impl = other;
}

SVGLangSpace::~SVGLangSpace()
{
	// We are not allowed to delete 'impl' as it's not refcounted.
	// delete impl;
}

void SVGLangSpace::setXmllang(const DOM::DOMString &xmllang)
{
	if(impl)
		impl->setXmllang(xmllang);
}

DOM::DOMString SVGLangSpace::xmllang() const
{
	if(!impl) return DOM::DOMString();
	return impl->xmllang();
}

void SVGLangSpace::setXmlspace(const DOM::DOMString &xmlspace)
{
	if(impl)
		impl->setXmlspace(xmlspace);
}

DOM::DOMString SVGLangSpace::xmlspace() const
{
	if(!impl) return DOM::DOMString();
	return impl->xmlspace();
}

// vim:ts=4:noet
