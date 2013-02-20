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

#ifndef SVGFEConvolveMatrixElementImpl_H
#define SVGFEConvolveMatrixElementImpl_H

#include "SVGElementImpl.h"
#include "SVGFilterPrimitiveStandardAttributesImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGAnimatedNumberImpl;
class SVGAnimatedLengthImpl;
class SVGAnimatedIntegerImpl;
class SVGAnimatedIntegerImpl;
class SVGAnimatedBooleanImpl;
class SVGAnimatedNumberListImpl;
class SVGAnimatedEnumerationImpl;
class SVGFEConvolveMatrixElementImpl : public SVGElementImpl,
									   public SVGFilterPrimitiveStandardAttributesImpl
{
public:
	SVGFEConvolveMatrixElementImpl(DOM::ElementImpl *);
	virtual ~SVGFEConvolveMatrixElementImpl();

	SVGAnimatedIntegerImpl *orderX() const;
	SVGAnimatedIntegerImpl *orderY() const;
	SVGAnimatedNumberListImpl *kernelMatrix() const;
	SVGAnimatedNumberImpl *divisor() const;
	SVGAnimatedNumberImpl *bias() const;
	SVGAnimatedIntegerImpl *targetX() const;
	SVGAnimatedIntegerImpl *targetY() const;
	SVGAnimatedEnumerationImpl *edgeMode() const;
	SVGAnimatedLengthImpl *kernelUnitLengthX() const;
	SVGAnimatedLengthImpl *kernelUnitLengthY() const;
	SVGAnimatedBooleanImpl *preserveAlpha() const;

private:
	SVGAnimatedIntegerImpl *m_orderX;
	SVGAnimatedIntegerImpl *m_orderY;
	SVGAnimatedNumberListImpl *m_kernelMatrix;
	SVGAnimatedNumberImpl *m_divisor;
	SVGAnimatedNumberImpl *m_bias;
	SVGAnimatedIntegerImpl *m_targetX;
	SVGAnimatedIntegerImpl *m_targetY;
	SVGAnimatedEnumerationImpl *m_edgeMode;
	SVGAnimatedLengthImpl *m_kernelUnitLengthX;
	SVGAnimatedLengthImpl *m_kernelUnitLengthY;
	SVGAnimatedBooleanImpl *m_preserveAlpha;

public:
	KSVG_FORWARDGET
	KSVG_FORWARDPUT
};

}

#endif

// vim:ts=4:noet
