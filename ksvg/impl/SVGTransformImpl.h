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

#ifndef SVGTransformImpl_H
#define SVGTransformImpl_H

#include <dom/dom_misc.h>
#include <dom/dom_string.h>

#include "ksvg_lookup.h"

class QString;

namespace KSVG
{

class SVGMatrixImpl;
class SVGTransformImpl : public DOM::DomShared
{
public:
	SVGTransformImpl();
	virtual ~SVGTransformImpl();

	unsigned short type() const;

	SVGMatrixImpl *matrix() const;
	
	double angle() const;
	
	void setMatrix(SVGMatrixImpl *);
	void setTranslate(double, double);
	void setScale(double, double);
	void setRotate(double, double, double);
	void setSkewX(double);
	void setSkewY(double);

	QString toString() const;

private:
	// mop: we have to store the optional rotate stuff :( anyone with a better solution please fix that ;)
	double m_cx;
	double m_cy;

	unsigned short m_type;
	SVGMatrixImpl *m_matrix;
	double m_angle;

public:
	KSVG_GET

	enum
	{
		// Properties
		Type, Matrix, Angle,
		// Functions
		SetMatrix, SetTranslate, SetScale,
		SetRotate, SetSkewX, SetSkewY
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
};

class SVGTransformImplConstructor : public KJS::ObjectImp
{
public:
	SVGTransformImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;
	
	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGTransformImplConstructor(KJS::ExecState *exec);

}

KSVG_DEFINE_PROTOTYPE(SVGTransformImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGTransformImplProtoFunc, SVGTransformImpl)

#endif

// vim:ts=4:noet
