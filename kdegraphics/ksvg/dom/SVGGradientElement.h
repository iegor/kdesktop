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

#ifndef SVGGradientElement_H
#define SVGGradientElement_H

#include "SVGElement.h"
#include "SVGURIReference.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGUnitTypes.h"

namespace KSVG
{

enum
{
	SVG_SPREADMETHOD_UNKNOWN = 0,
	SVG_SPREADMETHOD_PAD     = 1,
	SVG_SPREADMETHOD_REFLECT = 2,
	SVG_SPREADMETHOD_REPEAT  = 3
};

class SVGAnimatedEnumeration;
class SVGAnimatedTransformList;
class SVGGradientElementImpl;
class SVGGradientElement :	public SVGElement,
							public SVGURIReference,
							public SVGExternalResourcesRequired,
							public SVGStylable,
							public SVGUnitTypes
{
public:
	SVGGradientElement(const SVGGradientElement &other);
	SVGGradientElement &operator=(const SVGGradientElement &other);
	SVGGradientElement(SVGGradientElementImpl *other);
	virtual ~SVGGradientElement();

	SVGAnimatedEnumeration gradientUnits() const;
	SVGAnimatedTransformList gradientTransform() const;
	SVGAnimatedEnumeration spreadMethod() const;

protected:
	SVGGradientElement();

private:
	SVGGradientElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
