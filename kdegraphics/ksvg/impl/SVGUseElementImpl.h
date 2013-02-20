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

#ifndef SVGUseElementImpl_H
#define SVGUseElementImpl_H

#include "ksvg_lookup.h"

#include "SVGTestsImpl.h"
#include "SVGShapeImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGTransformableImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

namespace KSVG
{

class SVGAnimatedLengthImpl;
class SVGElementInstanceImpl;
class SVGUseElementImpl : public SVGShapeImpl,
						  public SVGURIReferenceImpl,
						  public SVGTestsImpl,
						  public SVGLangSpaceImpl,
						  public SVGExternalResourcesRequiredImpl,
						  public SVGStylableImpl,
						  public SVGTransformableImpl
{
public:
	SVGUseElementImpl(DOM::ElementImpl *);
	virtual ~SVGUseElementImpl();

	SVGAnimatedLengthImpl *x() const;
	SVGAnimatedLengthImpl *y() const;
	SVGAnimatedLengthImpl *width() const;
	SVGAnimatedLengthImpl *height() const;

	SVGElementInstanceImpl *instanceRoot() const;
	SVGElementInstanceImpl *animatedInstanceRoot() const;

	virtual void createItem(KSVGCanvas *c);
	virtual void removeItem(KSVGCanvas *c);
	virtual void update(CanvasItemUpdate reason, int param1, int param2);
	virtual void invalidate(KSVGCanvas *c, bool recalc);
	virtual void setReferenced(bool referenced);
	virtual void draw();

	virtual SVGRectImpl *getBBox();

	void setReferencedElement(SVGElementImpl *);

private:
	SVGAnimatedLengthImpl *m_x;
	SVGAnimatedLengthImpl *m_y;
	SVGAnimatedLengthImpl *m_width;
	SVGAnimatedLengthImpl *m_height;
	SVGElementInstanceImpl *m_instanceRoot;
	SVGElementInstanceImpl *m_animatedInstanceRoot;

	void setupSubtree(SVGElementImpl *element, SVGSVGElementImpl *ownerSVG, SVGElementImpl *viewport);

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		X, Y, Width, Height, Href, InstanceRoot, AnimatedInstanceRoot
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGUseElementImpl, "use")

}

#endif

// vim:ts=4:noet
