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

#ifndef SVGLinearGradientElementImpl_H
#define SVGLinearGradientElementImpl_H

#include "SVGGradientElementImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGAnimatedLengthImpl;
class SVGLinearGradientElementImpl : public SVGGradientElementImpl
{
public:
	SVGLinearGradientElementImpl(DOM::ElementImpl *);
	virtual ~SVGLinearGradientElementImpl();

	SVGAnimatedLengthImpl *x1() const;
	SVGAnimatedLengthImpl *y1() const;
	SVGAnimatedLengthImpl *x2() const;
	SVGAnimatedLengthImpl *y2() const;

	virtual void setAttributes();

	virtual QMap<QString, DOM::DOMString> gradientAttributes();

private:
	SVGAnimatedLengthImpl *m_x1;
	SVGAnimatedLengthImpl *m_y1;
	SVGAnimatedLengthImpl *m_x2;
	SVGAnimatedLengthImpl *m_y2;
	
public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		X1, X2, Y1, Y2, Href
	};
	
	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGLinearGradientElementImpl, "linearGradient")

}

#endif

// vim:ts=4:noet
