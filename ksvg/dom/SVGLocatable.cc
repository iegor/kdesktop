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

#include "SVGLocatable.h"
#include "SVGLocatableImpl.h"
#include "SVGElement.h"
#include "SVGRect.h"
#include "SVGMatrix.h"

using namespace KSVG;

// This class can't be constructed seperately.
SVGLocatable::SVGLocatable()
{
	impl = 0;
}

SVGLocatable::SVGLocatable(const SVGLocatable &other)
{
	(*this) = other;
}

SVGLocatable &SVGLocatable::operator=(const SVGLocatable &other)
{
	if(impl == other.impl)
		return *this;

	impl = other.impl;

	return *this;
}

SVGLocatable::SVGLocatable(SVGLocatableImpl *other)
{
	impl = other;
}

SVGLocatable::~SVGLocatable()
{
	// We are not allowed to delete 'impl' as it's not refcounted.
	// delete impl;
}

SVGElement SVGLocatable::nearestViewportElement() const
{
	if(!impl) return SVGElement(0);
	return impl->nearestViewportElement();
}

SVGElement SVGLocatable::farthestViewportElement() const
{
	if(!impl) return SVGElement(0);
	return impl->farthestViewportElement();
}

SVGRect SVGLocatable::getBBox()
{
	if(!impl) return SVGRect(0);
	return SVGRect(impl->getBBox()); // TODO: Check correctness
}

SVGMatrix SVGLocatable::getCTM()
{
	if(!impl) return  SVGMatrix(0);
	return SVGMatrix(impl->getCTM());
}

SVGMatrix SVGLocatable::getScreenCTM()
{
	if(!impl) return  SVGMatrix(0);
	return SVGMatrix(impl->getScreenCTM());
}

SVGMatrix SVGLocatable::getTransformToElement(const SVGElement &element)
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->getTransformToElement(element.handle()));
}

// vim:ts=4:noet
