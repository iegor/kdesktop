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

#ifndef SVGGradientElementImpl_H
#define SVGGradientElementImpl_H

#include <qmap.h>

#include "SVGElementImpl.h"
#include "SVGStylableImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"
#include "SVGPaintServerImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGUnitConverter;
class SVGAnimatedEnumerationImpl;
class SVGAnimatedTransformListImpl;
class SVGGradientElementImpl : public SVGElementImpl,
							   public SVGURIReferenceImpl,
							   public SVGExternalResourcesRequiredImpl,
							   public SVGStylableImpl,
							   public SVGPaintServerImpl
{
public:
	SVGGradientElementImpl(DOM::ElementImpl *);
	virtual ~SVGGradientElementImpl();

	SVGAnimatedEnumerationImpl *gradientUnits() const;
	SVGAnimatedTransformListImpl *gradientTransform() const;
	SVGAnimatedEnumerationImpl *spreadMethod() const;

	virtual void setAttributes();

	virtual void createItem(KSVGCanvas *c = 0);
	virtual void removeItem(KSVGCanvas *c);

	SVGUnitConverter *converter() const { return m_converter; }

	// Returns the gradient element that holds the stops for this gradient,
	// taking into account indirection through href.
	SVGGradientElementImpl *stopsSource();

	// Returns the linear/radial gradient attributes set on this element,
	// taking into account indirection through href.
	virtual QMap<QString, DOM::DOMString> gradientAttributes() = 0;
	
protected:
	void setAttributesFromHref();

private:
	SVGAnimatedEnumerationImpl *m_gradientUnits;
	SVGAnimatedTransformListImpl *m_gradientTransform;
	SVGAnimatedEnumerationImpl *m_spreadMethod;

	SVGUnitConverter *m_converter;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_NO_TAG_BRIDGE

	enum
	{
		// Properties
		GradientUnits, GradientTransform, SpreadMethod
	};
	
	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);	
};

class SVGGradientElementImplConstructor : public KJS::ObjectImp
{
public:
	SVGGradientElementImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;
	
	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGGradientElementImplConstructor(KJS::ExecState *exec);

}

#endif

// vim:ts=4:noet
