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

#ifndef SVGClipPathElement_H
#define SVGClipPathElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGTransformable.h"
#include "SVGUnitTypes.h"

namespace KSVG
{

class SVGAnimatedEnumeration;
class SVGClipPathElementImpl;
class SVGClipPathElement : public SVGElement,
						   public SVGTests,
						   public SVGLangSpace,
						   public SVGExternalResourcesRequired,
						   public SVGStylable,
						   public SVGTransformable,
						   public SVGUnitTypes 
{ 
public:
	SVGClipPathElement();
	SVGClipPathElement(const SVGClipPathElement &other);
	SVGClipPathElement &operator=(const SVGClipPathElement &other);
	SVGClipPathElement(SVGClipPathElementImpl *other);
	virtual ~SVGClipPathElement();

	SVGAnimatedEnumeration clipPathUnits() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGClipPathElementImpl *handle() const { return impl; }

private:
	SVGClipPathElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
