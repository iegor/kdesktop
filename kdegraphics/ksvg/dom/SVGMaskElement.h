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

#ifndef SVGMaskElement_H
#define SVGMaskElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGUnitTypes.h"

namespace KSVG
{

class SVGAnimatedEnumeration;
class SVGAnimatedLength;
class SVGMaskElementImpl;
class SVGMaskElement : public SVGElement,
					   public SVGTests,
					   public SVGLangSpace,
					   public SVGExternalResourcesRequired,
					   public SVGStylable,
					   public SVGUnitTypes
{ 
public:
	SVGMaskElement();
	SVGMaskElement(const SVGMaskElement &other);
	SVGMaskElement &operator=(const SVGMaskElement &other);
	SVGMaskElement(SVGMaskElementImpl *other);
	virtual ~SVGMaskElement();

	SVGAnimatedEnumeration maskUnits() const;
	SVGAnimatedEnumeration maskContentUnits() const;
	SVGAnimatedLength x() const;
	SVGAnimatedLength y() const;
	SVGAnimatedLength width() const;
	SVGAnimatedLength height() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGMaskElementImpl *handle() const { return impl; }

private:
	SVGMaskElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
