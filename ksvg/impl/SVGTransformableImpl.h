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

#ifndef SVGTransformableImpl_H
#define SVGTransformableImpl_H

#include <qstring.h>

#include "SVGLocatableImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGMatrixImpl;
class SVGAnimatedTransformListImpl;

class SVGTransformableImpl : public SVGLocatableImpl
{
public:
	SVGTransformableImpl();
	SVGTransformableImpl(const SVGTransformableImpl &);
	virtual ~SVGTransformableImpl();

	SVGTransformableImpl &operator=(const SVGTransformableImpl &);

	SVGAnimatedTransformListImpl *transform() const;

	virtual SVGMatrixImpl *getCTM();

	// The local transformations concatenated together. 0 if
	// there are no local transformations.
	virtual const SVGMatrixImpl *localMatrix() { return m_localMatrix; }

private:
	SVGAnimatedTransformListImpl *m_transform;
	SVGMatrixImpl *m_localMatrix;

	void updateLocalMatrix();

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		Transform
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

#endif

// vim:ts=4:noet
