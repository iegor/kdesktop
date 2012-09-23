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

#include "SVGExternalResourcesRequired.h"
#include "SVGExternalResourcesRequiredImpl.h"
#include "SVGAnimatedBoolean.h"

using namespace KSVG;

// This class can't be constructed seperately.
SVGExternalResourcesRequired::SVGExternalResourcesRequired()
{
	impl = 0;
}

SVGExternalResourcesRequired::SVGExternalResourcesRequired(const SVGExternalResourcesRequired &other) : impl(0)
{
	(*this) = other;
}

SVGExternalResourcesRequired &SVGExternalResourcesRequired::operator=(const SVGExternalResourcesRequired &other)
{
	if(impl == other.impl)
		return *this;

	impl = other.impl;

	return *this;
}

SVGExternalResourcesRequired::SVGExternalResourcesRequired(SVGExternalResourcesRequiredImpl *other)
{
	impl = other;
}

SVGExternalResourcesRequired::~SVGExternalResourcesRequired()
{
	// We are not allowed to delete 'impl' as it's not refcounted.
	// delete impl;
}

SVGAnimatedBoolean SVGExternalResourcesRequired::externalResourcesRequired() const
{
	if(!impl) return SVGAnimatedBoolean(0);
	return SVGAnimatedBoolean(impl->externalResourcesRequired());
}

// vim:ts=4:noet
