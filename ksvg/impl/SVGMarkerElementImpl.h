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

#ifndef SVGMarkerElementImpl_H
#define SVGMarkerElementImpl_H

#include "ksvg_lookup.h"

#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGContainerImpl.h"
#include "SVGFitToViewBoxImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"
#include "KSVGHelper.h"

namespace KSVG
{

class SVGAngleImpl;
class SVGAnimatedAngleImpl;
class SVGAnimatedLengthImpl;
class SVGAnimatedEnumerationImpl;
class SVGMarkerElementImpl : public SVGContainerImpl,
							 public SVGLangSpaceImpl,
							 public SVGExternalResourcesRequiredImpl,
							 public SVGStylableImpl,
							 public SVGFitToViewBoxImpl
{
public:
	SVGMarkerElementImpl(DOM::ElementImpl *);
	virtual ~SVGMarkerElementImpl();

	SVGAnimatedLengthImpl *refX() const;
	SVGAnimatedLengthImpl *refY() const;
	SVGAnimatedEnumerationImpl *markerUnits() const;
	SVGAnimatedLengthImpl *markerWidth() const;
	SVGAnimatedLengthImpl *markerHeight() const;
	SVGAnimatedEnumerationImpl *orientType() const;
	SVGAnimatedAngleImpl *orientAngle() const;

	void setOrientToAuto();
	void setOrientToAngle(SVGAngleImpl *angle);

	virtual void setAttributes();
	virtual void createItem(KSVGCanvas *c = 0);
	
	void draw(SVGShapeImpl *referencingElement, double x, double y, double lwidth, double angle);
	
	KSVGPolygon clipShape() const { return m_clipShape; }

	virtual bool directRender() { return false; }
	virtual bool isContainer() const { return false; }

private:
	SVGAnimatedLengthImpl *m_refX;
	SVGAnimatedLengthImpl *m_refY;
	SVGAnimatedEnumerationImpl *m_markerUnits;
	SVGAnimatedLengthImpl *m_markerWidth;
	SVGAnimatedLengthImpl *m_markerHeight;
	SVGAnimatedEnumerationImpl *m_orientType;
	SVGAnimatedAngleImpl *m_orientAngle;
	
	KSVGPolygon m_clipShape;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE
	
	enum
	{
		// Properties
		RefX, RefY, MarkerUnits, MarkerWidth, MarkerHeight, OrientType, OrientAngle,
		Orient, ViewBox, PreserveAspectRatio,
		// Functions
		SetOrientToAuto, SetOrientToAngle
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGMarkerElementImplConstructor : public KJS::ObjectImp
{
public:
	SVGMarkerElementImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;

	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGMarkerElementImplConstructor(KJS::ExecState *exec);

KSVG_REGISTER_ELEMENT(SVGMarkerElementImpl, "marker")

}

KSVG_DEFINE_PROTOTYPE(SVGMarkerElementImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGMarkerElementImplProtoFunc, SVGMarkerElementImpl)

#endif

// vim:ts=4:noet
