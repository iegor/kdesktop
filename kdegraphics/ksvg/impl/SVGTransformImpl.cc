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

#include <kdebug.h>

#include "SVGTransform.h"

#include "SVGMatrixImpl.h"
#include "SVGTransformImpl.h"
#include "SVGSVGElementImpl.h"

using namespace KSVG;

#include "SVGTransformImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_cacheimpl.h"

SVGTransformImpl::SVGTransformImpl()
{
	m_matrix = SVGSVGElementImpl::createSVGMatrix();

	m_type = SVG_TRANSFORM_UNKNOWN;
	m_angle = 0;
}

SVGTransformImpl::~SVGTransformImpl()
{
	if(m_matrix)
		m_matrix->deref();
}

unsigned short SVGTransformImpl::type() const
{
	return m_type;
}

SVGMatrixImpl *SVGTransformImpl::matrix() const
{
	return m_matrix;
}

double SVGTransformImpl::angle() const
{
	return m_angle;
}

void SVGTransformImpl::setMatrix(SVGMatrixImpl *matrix)
{
	if(!matrix)
		return;

	m_type = SVG_TRANSFORM_MATRIX;
	m_angle = 0;

	m_matrix->deref();
	m_matrix = matrix;
	m_matrix->ref();
}

void SVGTransformImpl::setTranslate(double tx, double ty)
{
	m_type = SVG_TRANSFORM_TRANSLATE;
	m_angle = 0;
	m_matrix->reset();
	m_matrix->translate(tx, ty);
}

void SVGTransformImpl::setScale(double sx, double sy)
{
	m_type = SVG_TRANSFORM_SCALE;
	m_angle = 0;
	m_matrix->reset();
	m_matrix->scaleNonUniform(sx, sy);
}

void SVGTransformImpl::setRotate(double angle, double cx, double cy)
{
	m_type = SVG_TRANSFORM_ROTATE;
	// mop: evil...fix that...needed to make toString() work correctly
	m_cx = cx;
	m_cy = cy;
	m_angle = angle;
	m_matrix->reset();
	m_matrix->translate(cx, cy);
	m_matrix->rotate(angle);
	m_matrix->translate(-cx, -cy);
}

void SVGTransformImpl::setSkewX(double angle)
{
	m_type = SVG_TRANSFORM_SKEWX;
	m_angle = angle;
	m_matrix->reset();
	m_matrix->skewX(angle);
}

void SVGTransformImpl::setSkewY(double angle)
{
	m_type = SVG_TRANSFORM_SKEWY;
	m_angle = angle;
	m_matrix->reset();
	m_matrix->skewY(angle);
}

QString SVGTransformImpl::toString() const
{
     	switch (m_type)
	{
		case SVG_TRANSFORM_UNKNOWN:
			return QString();
		case SVG_TRANSFORM_MATRIX:
			return QString("matrix(" + QString::number(m_matrix->a()) + " " + QString::number(m_matrix->b()) + " " + QString::number(m_matrix->c()) + " " + QString::number(m_matrix->d()) + " " + QString::number(m_matrix->e()) + " " + QString::number(m_matrix->f()) + ")");
		case SVG_TRANSFORM_TRANSLATE:
			return QString("translate(" + QString::number(m_matrix->e()) + " " + QString::number(m_matrix->f()) + ")");
		case SVG_TRANSFORM_SCALE:
			return QString("scale(" + QString::number(m_matrix->a()) + " " + QString::number(m_matrix->d()) + ")");
		case SVG_TRANSFORM_ROTATE:
			return QString("rotate(" + QString::number(m_angle) + " " + QString::number(m_cx) + " " + QString::number(m_cy) + ")");
		case SVG_TRANSFORM_SKEWX:
			return QString("skewX(" + QString::number(m_angle) + ")");
		case SVG_TRANSFORM_SKEWY:
			return QString("skewY(" + QString::number(m_angle) + ")");
		default:
			kdWarning() << "Unknown transform type " << m_type << endl;
			return QString();	
	}
}

// ECMA binding

/*
@namespace KSVG
@begin SVGTransformImpl::s_hashTable 5
 type			SVGTransformImpl::Type			DontDelete|ReadOnly
 matrix			SVGTransformImpl::Matrix		DontDelete|ReadOnly
 angle			SVGTransformImpl::Angle			DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGTransformImplProto::s_hashTable 7
 setMatrix		SVGTransformImpl::SetMatrix		DontDelete|Function 1
 setTranslate	SVGTransformImpl::SetTranslate	DontDelete|Function 2
 setScale		SVGTransformImpl::SetScale		DontDelete|Function 2
 setRotate		SVGTransformImpl::SetRotate		DontDelete|Function 3
 setSkewX		SVGTransformImpl::SetSkewX		DontDelete|Function 1
 setSkewY		SVGTransformImpl::SetSkewY		DontDelete|Function 1
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGTransform", SVGTransformImplProto, SVGTransformImplProtoFunc)

Value SVGTransformImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case Type:
			return Number(m_type);
		case Matrix:
			return m_matrix->cache(exec);
		case Angle:
			return Number(m_angle);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

Value SVGTransformImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGTransformImpl)

	switch(id)
	{
		case SVGTransformImpl::SetMatrix:
			obj->setMatrix(static_cast<KSVGBridge<SVGMatrixImpl> *>(args[0].imp())->impl());
			break;
		case SVGTransformImpl::SetTranslate:
			obj->setTranslate(args[0].toNumber(exec), args[1].toNumber(exec));
			break;
		case SVGTransformImpl::SetScale:
			obj->setScale(args[0].toNumber(exec), args[1].toNumber(exec));
			break;
		case SVGTransformImpl::SetRotate:
			obj->setRotate(args[0].toNumber(exec), args[1].toNumber(exec), args[2].toNumber(exec));
			break;
		case SVGTransformImpl::SetSkewX:
			obj->setSkewX(args[0].toNumber(exec));
			break;
		case SVGTransformImpl::SetSkewY:
			obj->setSkewY(args[0].toNumber(exec));
			break;
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

/*
@namespace KSVG
@begin SVGTransformImplConstructor::s_hashTable 11
 SVG_TRANSFORM_UNKNOWN		KSVG::SVG_TRANSFORM_UNKNOWN		DontDelete|ReadOnly
 SVG_TRANSFORM_MATRIX		KSVG::SVG_TRANSFORM_MATRIX		DontDelete|ReadOnly
 SVG_TRANSFORM_TRANSLATE	KSVG::SVG_TRANSFORM_TRANSLATE	DontDelete|ReadOnly
 SVG_TRANSFORM_SCALE		KSVG::SVG_TRANSFORM_SCALE		DontDelete|ReadOnly
 SVG_TRANSFORM_ROTATE		KSVG::SVG_TRANSFORM_ROTATE		DontDelete|ReadOnly
 SVG_TRANSFORM_SKEWX		KSVG::SVG_TRANSFORM_SKEWX		DontDelete|ReadOnly
 SVG_TRANSFORM_SKEWY		KSVG::SVG_TRANSFORM_SKEWY		DontDelete|ReadOnly
@end
*/

Value SVGTransformImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token);
}

Value KSVG::getSVGTransformImplConstructor(ExecState *exec)
{
	return cacheGlobalBridge<SVGTransformImplConstructor>(exec, "[[svgtransform.constructor]]");
}

// vim:ts=4:noet
