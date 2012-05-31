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

#include "SVGFitToViewBox.h"
#include "SVGFitToViewBoxImpl.h"
#include "SVGAnimatedRect.h"
#include "SVGAnimatedPreserveAspectRatio.h"

using namespace KSVG;

// This class can't be constructed seperately.
SVGFitToViewBox::SVGFitToViewBox()
{
	impl = 0;
}

SVGFitToViewBox::SVGFitToViewBox(const SVGFitToViewBox &other) : impl(0)
{
	(*this) = other;
}

SVGFitToViewBox &SVGFitToViewBox::operator=(const SVGFitToViewBox &other)
{
	if(impl == other.impl)
		return *this;

	impl = other.impl;

	return *this;
}

SVGFitToViewBox::SVGFitToViewBox(SVGFitToViewBoxImpl *other)
{
	impl = other;
}

SVGFitToViewBox::~SVGFitToViewBox()
{
	// We are not allowed to delete 'impl' as it's not refcounted.
	// delete impl;
}

SVGAnimatedRect SVGFitToViewBox::viewBox() const
{
	if(!impl) return SVGAnimatedRect(0);
	return SVGAnimatedRect(impl->viewBox());
}

SVGAnimatedPreserveAspectRatio SVGFitToViewBox::preserveAspectRatio() const
{
	if(!impl) return SVGAnimatedPreserveAspectRatio(0);
	return SVGAnimatedPreserveAspectRatio(impl->preserveAspectRatio());
}

// vim:ts=4:noet
