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

#ifndef SVGRadialGradientElementImpl_H
#define SVGRadialGradientElementImpl_H

#include "SVGGradientElementImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGAnimatedLengthImpl;
class SVGRadialGradientElementImpl : public SVGGradientElementImpl
{
public:
	SVGRadialGradientElementImpl(DOM::ElementImpl *);
	virtual ~SVGRadialGradientElementImpl();

	SVGAnimatedLengthImpl *cx() const;
	SVGAnimatedLengthImpl *cy() const;
	SVGAnimatedLengthImpl *r() const;
	SVGAnimatedLengthImpl *fx() const;
	SVGAnimatedLengthImpl *fy() const;

	virtual void setAttributes();

	virtual QMap<QString, DOM::DOMString> gradientAttributes();

private:
	SVGAnimatedLengthImpl *m_cx;
	SVGAnimatedLengthImpl *m_cy;
	SVGAnimatedLengthImpl *m_r;
	SVGAnimatedLengthImpl *m_fx;
	SVGAnimatedLengthImpl *m_fy;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		Cx, Cy, R, Fx, Fy, Href
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGRadialGradientElementImpl, "radialGradient")

}

#endif

// vim:ts=4:noet
