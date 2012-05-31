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

#include <dom/dom_string.h>
#include "SVGLength.h"
#include "SVGShapeImpl.h"
#include "SVGLengthImpl.h"

using namespace KSVG;

SVGLength::SVGLength()
{
	impl = new SVGLengthImpl();
	impl->ref();
}

SVGLength::SVGLength(const SVGLength &other) : impl(0)
{
	(*this) = other;
}

SVGLength &SVGLength::operator =(const SVGLength &other)
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

SVGLength::SVGLength(SVGLengthImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGLength::~SVGLength()
{
	if(impl)
		impl->deref();
}

unsigned short SVGLength::unitType() const
{
	if(!impl) return SVG_LENGTHTYPE_UNKNOWN;
	return impl->unitType();
}

void SVGLength::setValue(float value)
{
	if(impl)
	{
		impl->setValue(value);

		// Automatic updating of the shape if any value is changed, imagine:
		// SVGCircleElement c; [...] c.r().baseVal().setValue(150);
		if(impl->context() && dynamic_cast<SVGShapeImpl *>(impl->context()))
			dynamic_cast<SVGShapeImpl *>(impl->context())->update(UPDATE_TRANSFORM, 0, 0);
	}
}

float SVGLength::value() const
{
	if(!impl) return -1;
	return impl->value();
}

void SVGLength::setValueInSpecifiedUnits(float valueInSpecifiedUnits)
{
	if(impl)
		impl->setValueInSpecifiedUnits(valueInSpecifiedUnits);
}

float SVGLength::valueInSpecifiedUnits() const
{
	if(!impl) return -1;
	return impl->valueInSpecifiedUnits();
}

void SVGLength::setValueAsString(const DOM::DOMString &valueAsString)
{
	if(impl)
		impl->setValueAsString(valueAsString);
}

DOM::DOMString SVGLength::valueAsString() const
{
	if(!impl) return DOM::DOMString();
	return impl->valueAsString();
}

void SVGLength::newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits)
{
	if(impl)
		impl->newValueSpecifiedUnits(unitType, valueInSpecifiedUnits);
}

void SVGLength::convertToSpecifiedUnits(unsigned short unitType)
{
	if(impl)
		impl->convertToSpecifiedUnits(unitType);
}

SVGLength::operator float()
{
	if(!impl) return -1;
	return impl->valueInSpecifiedUnits();
}

// vim:ts=4:noet
