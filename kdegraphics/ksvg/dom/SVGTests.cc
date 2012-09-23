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

#include "SVGTests.h"
#include "SVGTestsImpl.h"
#include "SVGStringList.h"

using namespace KSVG;

// This class can't be constructed seperately.
SVGTests::SVGTests()
{
	impl = 0;
}

SVGTests::SVGTests(const SVGTests &other) : impl(0)
{
	(*this) = other;
}

SVGTests &SVGTests::operator=(const SVGTests &other)
{
	if(impl == other.impl)
		return *this;

	impl = other.impl;

	return *this;
}

SVGTests::SVGTests(SVGTestsImpl *other)
{
	impl = other;
}

SVGTests::~SVGTests()
{
	// We are not allowed to delete 'impl' as it's not refcounted.
	// delete impl;
}

SVGStringList SVGTests::requiredFeatures() const
{
	if(!impl) return SVGStringList(0);
	return SVGStringList(impl->requiredFeatures());
}

SVGStringList SVGTests::requiredExtensions() const
{
	if(!impl) return SVGStringList(0);
	return SVGStringList(impl->requiredExtensions());
}

SVGStringList SVGTests::systemLanguage() const
{
	if(!impl) return SVGStringList(0);
	return SVGStringList(impl->systemLanguage());
}

bool SVGTests::hasExtension(const DOM::DOMString &extension)
{
	if(!impl) return false;
	return impl->hasExtension(extension);
}

// vim:ts=4:noet
