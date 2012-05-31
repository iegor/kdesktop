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

#ifndef SVGSymbolElementImpl_H
#define SVGSymbolElementImpl_H

#include "ksvg_lookup.h"

#include "SVGShapeImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGFitToViewBoxImpl.h"
#include "SVGTransformableImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

namespace KSVG
{

class SVGFitToViewBoxImpl;
class SVGAnimatedLengthImpl;
class SVGSymbolElementImpl : public SVGShapeImpl,
							public SVGLangSpaceImpl,
							public SVGExternalResourcesRequiredImpl,
							public SVGStylableImpl,
							public SVGFitToViewBoxImpl
{
public:
	SVGSymbolElementImpl(DOM::ElementImpl *);
	virtual ~SVGSymbolElementImpl();

	virtual bool isContainer() const { return true; }
	virtual bool directRender() { return false; }

private:
	SVGAnimatedLengthImpl *m_width;
	SVGAnimatedLengthImpl *m_height;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		Width, Height
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGSymbolElementImpl, "symbol")

}

#endif

// vim:ts=4:noet
