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

#include <math.h>

#include <kdebug.h>

#include "SVGAngleImpl.h"
#include "SVGMatrixImpl.h"
#include "KSVGHelper.h"

using namespace KSVG;

#include "SVGMatrixImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGMatrixImpl::SVGMatrixImpl()
{
	KSVG_EMPTY_FLAGS
}

SVGMatrixImpl::SVGMatrixImpl(QWMatrix mat)
{
	m_mat = mat;
}

SVGMatrixImpl::SVGMatrixImpl(double a, double b, double c, double d, double e, double f)
{
	m_mat.setMatrix(a, b, c, d, e, f);
}

SVGMatrixImpl::~SVGMatrixImpl()
{
}

void SVGMatrixImpl::setA(const double &a)
{
	m_mat.setMatrix(a, m_mat.m12(), m_mat.m21(), m_mat.m22(), m_mat.dx(), m_mat.dy());
}

double SVGMatrixImpl::a() const
{
	return m_mat.m11();
}

void SVGMatrixImpl::setB(const double &b)
{
	m_mat.setMatrix(m_mat.m11(), b, m_mat.m21(), m_mat.m22(), m_mat.dx(), m_mat.dy());
}

double SVGMatrixImpl::b() const
{
	return m_mat.m12();
}

void SVGMatrixImpl::setC(const double &c)
{
	m_mat.setMatrix(m_mat.m11(), m_mat.m12(), c, m_mat.m22(), m_mat.dx(), m_mat.dy());
}

double SVGMatrixImpl::c() const
{
	return m_mat.m21();
}

void SVGMatrixImpl::setD(const double &d)
{
	m_mat.setMatrix(m_mat.m11(), m_mat.m12(), m_mat.m21(), d, m_mat.dx(), m_mat.dy());
}

double SVGMatrixImpl::d() const
{
	return m_mat.m22();
}

void SVGMatrixImpl::setE(const double &e)
{
	m_mat.setMatrix(m_mat.m11(), m_mat.m12(), m_mat.m21(), m_mat.m22(), e, m_mat.dy());
}

double SVGMatrixImpl::e() const
{
	return m_mat.dx();
}

void SVGMatrixImpl::setF(const double &f)
{
	m_mat.setMatrix(m_mat.m11(), m_mat.m12(), m_mat.m21(), m_mat.m22(), m_mat.dx(), f);
}

double SVGMatrixImpl::f() const
{
	return m_mat.dy();
}

void SVGMatrixImpl::copy(const SVGMatrixImpl *other)
{
	m_mat.setMatrix(other->m_mat.m11(), other->m_mat.m12(), other->m_mat.m21(), other->m_mat.m22(), other->m_mat.dx(), other->m_mat.dy());
}

SVGMatrixImpl *SVGMatrixImpl::postMultiply(const SVGMatrixImpl *secondMatrix)
{
	QWMatrix temp(secondMatrix->a(), secondMatrix->b(), secondMatrix->c(), secondMatrix->d(), secondMatrix->e(), secondMatrix->f());
	m_mat *= temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::inverse()
{
	m_mat = m_mat.invert();
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::postTranslate(const double &x,  const double &y)
{
	// Could optimise these.
	QWMatrix temp;
	temp.translate(x, y);
	m_mat *= temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::postScale(const double &scaleFactor)
{
	QWMatrix temp;
	temp.scale(scaleFactor, scaleFactor);
	m_mat *= temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::postScaleNonUniform(const double &scaleFactorX, const double &scaleFactorY)
{
	QWMatrix temp;
	temp.scale(scaleFactorX, scaleFactorY);
	m_mat *= temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::postRotate(const double &angle)
{
	QWMatrix temp;
	temp.rotate(angle);
	m_mat *= temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::postRotateFromVector(const double &x, const double &y)
{
	QWMatrix temp;
	temp.rotate(SVGAngleImpl::todeg(atan2(y, x)));
	m_mat *= temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::postFlipX()
{
	QWMatrix temp(-1.0F, 0.0F, 0.0F, 1.0F, 0.0F, 0.0F);
	m_mat *= temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::postFlipY()
{
	QWMatrix temp(1.0F, 0.0F, 0.0F, -1.0F, 0.0F, 0.0F);
	m_mat *= temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::postSkewX(const double &angle)
{
	QWMatrix temp;
	temp.shear(tan(SVGAngleImpl::torad(angle)), 0.0F);
	m_mat *= temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::postSkewY(const double &angle)
{
	QWMatrix temp;
	temp.shear(0.0F, tan(SVGAngleImpl::torad(angle)));
	m_mat *= temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::multiply(const SVGMatrixImpl *secondMatrix)
{
	QWMatrix temp(secondMatrix->a(), secondMatrix->b(), secondMatrix->c(), secondMatrix->d(), secondMatrix->e(), secondMatrix->f());
	temp *= m_mat;
	m_mat = temp;
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::translate(const double &x,  const double &y)
{
	m_mat.translate(x, y);
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::scale(const double &scaleFactor)
{
	m_mat.scale(scaleFactor, scaleFactor);
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::scaleNonUniform(const double &scaleFactorX, const double &scaleFactorY)
{
	m_mat.scale(scaleFactorX, scaleFactorY);
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::rotate(const double &angle)
{
	m_mat.rotate(angle);
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::rotateFromVector(const double &x, const double &y)
{
	m_mat.rotate(SVGAngleImpl::todeg(atan2(y, x)));
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::flipX()
{
	m_mat.scale(-1.0f, 1.0f);
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::flipY()
{
	m_mat.scale(1.0f, -1.0f);
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::skewX(const double &angle)
{
	m_mat.shear(tan(SVGAngleImpl::torad(angle)), 0.0F);
	return this;
}

SVGMatrixImpl *SVGMatrixImpl::skewY(const double &angle)
{
	m_mat.shear(0.0F, tan(SVGAngleImpl::torad(angle)));
	return this;
}

void SVGMatrixImpl::setMatrix(QWMatrix mat)
{
	m_mat = mat;
}

QWMatrix &SVGMatrixImpl::qmatrix()
{
	return m_mat;
}

const QWMatrix &SVGMatrixImpl::qmatrix() const
{
	return m_mat;
}

void SVGMatrixImpl::reset()
{
	m_mat.reset();
}

void SVGMatrixImpl::removeScale(double *xScale, double *yScale)
{
	double sx = sqrt(a()*a() + b()*b());
	double sy = sqrt(c()*c() + d()*d());

	// Remove the scaling from the matrix.

	setA(a()/sx);
	setB(b()/sx);
	setC(c()/sy);
	setD(d()/sy);

	*xScale = sx;
	*yScale = sy;
}

KSVGPolygon SVGMatrixImpl::map(const KSVGPolygon& polygon) const
{
	KSVGPolygon mapped;

	for(unsigned int i = 0; i < polygon.numPoints(); i++)
	{
		double x, y;

		m_mat.map(polygon.point(i).x(), polygon.point(i).y(), &x, &y);
		mapped.addPoint(x, y);
	}

	return mapped;
}

KSVGPolygon SVGMatrixImpl::inverseMap(const KSVGPolygon& polygon) const
{
	QWMatrix inverse = m_mat.invert();
	KSVGPolygon mapped;

	for(unsigned int i = 0; i < polygon.numPoints(); i++)
	{
		double x, y;

		inverse.map(polygon.point(i).x(), polygon.point(i).y(), &x, &y);
		mapped.addPoint(x, y);
	}

	return mapped;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGMatrixImpl::s_hashTable 7
 a					SVGMatrixImpl::A	DontDelete
 b					SVGMatrixImpl::B	DontDelete
 c					SVGMatrixImpl::C	DontDelete
 d					SVGMatrixImpl::D	DontDelete
 e					SVGMatrixImpl::E	DontDelete
 f					SVGMatrixImpl::F	DontDelete
@end
@namespace KSVG
@begin SVGMatrixImplProto::s_hashTable 13
 inverse			SVGMatrixImpl::Inverse			DontDelete|Function 0
 multiply			SVGMatrixImpl::Multiply			DontDelete|Function 1
 translate			SVGMatrixImpl::Translate		DontDelete|Function 2
 scale				SVGMatrixImpl::Scale			DontDelete|Function 1
 rotate				SVGMatrixImpl::Rotate			DontDelete|Function 1
 rotateFromVector	SVGMatrixImpl::RotateFromVector	DontDelete|Function 2
 scaleNonUniform	SVGMatrixImpl::ScaleNonUniform	DontDelete|Function 2
 flipX				SVGMatrixImpl::FlipX			DontDelete|Function 0
 flipY				SVGMatrixImpl::FlipY			DontDelete|Function 0
 skewX				SVGMatrixImpl::SkewX			DontDelete|Function 1
 skewY				SVGMatrixImpl::SkewY			DontDelete|Function 1
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGMatrix", SVGMatrixImplProto, SVGMatrixImplProtoFunc)

Value SVGMatrixImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case A:
			return Number(a());
		case B:
			return Number(b());
		case C:
			return Number(c());
		case D:
			return Number(d());
		case E:
			return Number(e());
		case F:
			return Number(f());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return KJS::Undefined();
	}
}


void SVGMatrixImpl::putValueProperty(ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case A:
			setA(value.toNumber(exec));
			break;
		case B:
			setB(value.toNumber(exec));
			break;
		case C:
			setC(value.toNumber(exec));
			break;
		case D:
			setD(value.toNumber(exec));
			break;
		case E:
			setE(value.toNumber(exec));
			break;
		case F:
			setF(value.toNumber(exec));
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			break;
	}
}

Value SVGMatrixImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGMatrixImpl)

	switch(id)
	{
		case SVGMatrixImpl::Inverse:
			return obj->inverse()->cache(exec);
			break;
		case SVGMatrixImpl::Multiply:
			return obj->multiply(static_cast<KSVGBridge<SVGMatrixImpl> *>(args[0].imp())->impl())->cache(exec);
			break;
		case SVGMatrixImpl::Translate:
			return obj->translate(args[0].toNumber(exec), args[1].toNumber(exec))->cache(exec);
			break;
		case SVGMatrixImpl::Scale:
			return obj->scale(args[0].toNumber(exec))->cache(exec);
			break;
		case SVGMatrixImpl::Rotate:
			return obj->rotate(args[0].toNumber(exec))->cache(exec);
			break;
		case SVGMatrixImpl::RotateFromVector:
			return obj->rotateFromVector(args[0].toNumber(exec), args[1].toNumber(exec))->cache(exec);
			break;
		case SVGMatrixImpl::ScaleNonUniform:
			return obj->scaleNonUniform(args[0].toNumber(exec), args[1].toNumber(exec))->cache(exec);
			break;
		case SVGMatrixImpl::FlipX:
			return obj->flipX()->cache(exec);
			break;
		case SVGMatrixImpl::FlipY:
			return obj->flipY()->cache(exec);
			break;
		case SVGMatrixImpl::SkewX:
			return obj->skewX(args[0].toNumber(exec))->cache(exec);
			break;
		case SVGMatrixImpl::SkewY:
			return obj->skewY(args[0].toNumber(exec))->cache(exec);;
			break;
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

// vim:ts=4:noet
