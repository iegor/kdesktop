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

#include "SVGAngle.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

SVGAngle::SVGAngle()
{
	impl = new SVGAngleImpl();
	impl->ref();
}

SVGAngle::SVGAngle(const SVGAngle &other) : impl(0)
{
	(*this) = other;
}

SVGAngle &SVGAngle::operator =(const SVGAngle &other)
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

SVGAngle::SVGAngle(SVGAngleImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGAngle::~SVGAngle()
{
	if(impl)
		impl->deref();
}

unsigned short SVGAngle::unitType() const
{
	if(!impl) return SVG_ANGLETYPE_UNKNOWN;
	return impl->unitType();
}

void SVGAngle::setValue(float value)
{
	if(impl)
		impl->setValue(value);
}

float SVGAngle::value() const
{
	if(!impl) return -1;
	return impl->value();
}

void SVGAngle::setValueInSpecifiedUnits(float valueInSpecifiedUnits)
{
	if(impl)
		impl->setValueInSpecifiedUnits(valueInSpecifiedUnits);
}

float SVGAngle::valueInSpecifiedUnits() const
{
	if(!impl) return -1;
	return impl->valueInSpecifiedUnits();
}

void SVGAngle::setValueAsString(const DOM::DOMString &valueAsString)
{
	if(impl)
		impl->setValueAsString(valueAsString);
}

DOM::DOMString SVGAngle::valueAsString() const
{
	if(!impl) return DOM::DOMString();
	return impl->valueAsString();
}

void SVGAngle::newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits)
{
	if(impl)
		impl->newValueSpecifiedUnits(unitType, valueInSpecifiedUnits);
}

void SVGAngle::convertToSpecifiedUnits(unsigned short unitType)
{
	if(impl)
		impl->convertToSpecifiedUnits(unitType);
}

// vim:ts=4:noet
