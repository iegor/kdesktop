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

#ifndef SVGComponentTransferFunctionElementImpl_H
#define SVGComponentTransferFunctionElementImpl_H

#include "SVGElementImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGAnimatedNumberImpl;
class SVGAnimatedNumberListImpl;
class SVGAnimatedEnumerationImpl;
class SVGComponentTransferFunctionElementImpl : public SVGElementImpl
{
public:
	SVGComponentTransferFunctionElementImpl(DOM::ElementImpl *);
	virtual ~SVGComponentTransferFunctionElementImpl();

	SVGAnimatedEnumerationImpl *type() const;
	SVGAnimatedNumberListImpl *tableValues() const;
	SVGAnimatedNumberImpl *slope() const;
	SVGAnimatedNumberImpl *intercept() const;
	SVGAnimatedNumberImpl *amplitude() const;
	SVGAnimatedNumberImpl *exponent() const;
	SVGAnimatedNumberImpl *offset() const;

private:
	SVGAnimatedEnumerationImpl *m_type;
	SVGAnimatedNumberListImpl *m_tableValues;
	SVGAnimatedNumberImpl *m_slope;
	SVGAnimatedNumberImpl *m_intercept;
	SVGAnimatedNumberImpl *m_amplitude;
	SVGAnimatedNumberImpl *m_exponent;
	SVGAnimatedNumberImpl *m_offset;

public:
	KSVG_FORWARDGET
	KSVG_FORWARDPUT
};

}

#endif

// vim:ts=4:noet
