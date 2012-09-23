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

#include "SVGMatrixImpl.h"
#include "SVGMatrix.h"

using namespace KSVG;

SVGMatrix::SVGMatrix()
{
	impl = new SVGMatrixImpl();
	impl->ref();
}

SVGMatrix::SVGMatrix(double a, double b, double c, double d, double e, double f)
{
	impl = new SVGMatrixImpl(a, b, c, d, e, f);
	impl->ref();
}

SVGMatrix::SVGMatrix(const SVGMatrix &other) : impl(0)
{
	(*this) = other;
}

SVGMatrix &SVGMatrix::operator=(const SVGMatrix &other)
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

SVGMatrix::SVGMatrix(SVGMatrixImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGMatrix::~SVGMatrix()
{
	if(impl)
		impl->deref();
}

void SVGMatrix::setA(const double &a)
{
	if(impl)
		impl->setA(a);
}

double SVGMatrix::a() const
{
	if(!impl) return -1;
	return impl->a();
}

void SVGMatrix::setB(const double &b)
{
	if(impl)
		impl->setB(b);
}

double SVGMatrix::b() const
{
	if(!impl) return -1;
	return impl->b();
}

void SVGMatrix::setC(const double &c)
{
	if(impl)
		impl->setC(c);
}

double SVGMatrix::c() const
{
	if(!impl) return -1;
	return impl->c();
}

void SVGMatrix::setD(const double &d)
{
	if(impl)
		impl->setD(d);
}

double SVGMatrix::d() const
{
	if(!impl) return -1;
	return impl->d();
}

void SVGMatrix::setE(const double &e)
{
	if(impl)
		impl->setE(e);
}

double SVGMatrix::e() const
{
	if(!impl) return -1;
	return impl->e();
}

void SVGMatrix::setF(const double &f)
{
	if(impl)
		impl->setF(f);
}

double SVGMatrix::f() const
{
	if(!impl) return -1;
	return impl->f();
}

SVGMatrix SVGMatrix::multiply(SVGMatrix &secondMatrix)
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->multiply(secondMatrix.handle()));
}

SVGMatrix SVGMatrix::inverse()
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->inverse());
}

SVGMatrix SVGMatrix::translate(const double &x, const double &y)
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->translate(x, y));
}

SVGMatrix SVGMatrix::scale(const double &scaleFactor)
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->scale(scaleFactor));
}

SVGMatrix SVGMatrix::scaleNonUniform(const double &scaleFactorX, const double &scaleFactorY)
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->scaleNonUniform(scaleFactorX, scaleFactorY));
}

SVGMatrix SVGMatrix::rotate(const double &angle)
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->rotate(angle));
}

SVGMatrix SVGMatrix::rotateFromVector(const double &x, const double &y)
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->rotateFromVector(x, y));
}

SVGMatrix SVGMatrix::flipX()
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->flipX());
}

SVGMatrix SVGMatrix::flipY()
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->flipY());
}

SVGMatrix SVGMatrix::skewX(const double &angle)
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->skewX(angle));
}

SVGMatrix SVGMatrix::skewY(const double &angle)
{
	if(!impl) return SVGMatrix(0);
	return SVGMatrix(impl->skewY(angle));
}

// vim:ts=4:noet
