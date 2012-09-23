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

#ifndef SVGEllipseElementImpl_H
#define SVGEllipseElementImpl_H

#include "ksvg_lookup.h"

#include "SVGShapeImpl.h"
#include "SVGTestsImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGTransformableImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

namespace KSVG
{

class SVGRectImpl;
class SVGAnimatedLengthImpl;
class SVGEllipseElementImpl : public SVGShapeImpl,
							  public SVGTestsImpl,
							  public SVGLangSpaceImpl,
							  public SVGExternalResourcesRequiredImpl,
							  public SVGStylableImpl,
							  public SVGTransformableImpl
{
public:
	SVGEllipseElementImpl(DOM::ElementImpl *);
	virtual ~SVGEllipseElementImpl();

	SVGAnimatedLengthImpl *cx();
	SVGAnimatedLengthImpl *cy();
	SVGAnimatedLengthImpl *rx();
	SVGAnimatedLengthImpl *ry();

	virtual void createItem(KSVGCanvas *c = 0);
	virtual void setAttributes();

	virtual SVGRectImpl *getBBox();

private:
	SVGAnimatedLengthImpl *m_cx;
	SVGAnimatedLengthImpl *m_cy;
	SVGAnimatedLengthImpl *m_rx;
	SVGAnimatedLengthImpl *m_ry;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		Cx, Cy, Rx, Ry
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGEllipseElementImpl, "ellipse")

}

#endif

// vim:ts=4:noet
