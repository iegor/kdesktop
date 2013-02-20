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

#include "SVGTransformImpl.h"
#include "SVGTransform.h"
#include "SVGMatrix.h"

using namespace KSVG;

SVGTransform::SVGTransform()
{
	impl = new SVGTransformImpl();
	impl->ref();
}

SVGTransform::SVGTransform(const SVGTransform &other) : impl(0)
{
	(*this) = other;
}

SVGTransform &SVGTransform::operator=(const SVGTransform &other)
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

SVGTransform::SVGTransform(SVGTransformImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGTransform::~SVGTransform()
{
}

unsigned short SVGTransform::type() const
{
	if(!impl) return SVG_TRANSFORM_UNKNOWN;
	return impl->type();
}

SVGMatrix SVGTransform::matrix() const
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->matrix());
}

double SVGTransform::angle() const
{
	if(!impl) return -1;
	return impl->angle();
}

void SVGTransform::setMatrix(SVGMatrix matrix)
{
	if(impl)
		impl->setMatrix(matrix.handle());
}

void SVGTransform::setTranslate(double tx, double ty)
{
	if(impl)
		impl->setTranslate(tx, ty);
}

void SVGTransform::setScale(double sx, double sy)
{
	if(impl)
		impl->setScale(sx, sy);
}

void SVGTransform::setRotate(double angle, double cx, double cy)
{
	if(impl)
		impl->setRotate(angle, cx, cy);
}

void SVGTransform::setSkewX(double angle)
{
	if(impl)
		impl->setSkewX(angle);
}

void SVGTransform::setSkewY(double angle)
{
	if(impl)
		impl->setSkewY(angle);
}

// vim:ts=4:noet
