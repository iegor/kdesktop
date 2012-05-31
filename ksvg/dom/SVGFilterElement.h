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

#ifndef SVGFilterElement_H
#define SVGFilterElement_H

#include "SVGElement.h"
#include "SVGURIReference.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGUnitTypes.h"

namespace KSVG
{

class SVGAnimatedEnumeration;
class SVGAnimatedLength;
class SVGAnimatedInteger;
class SVGFilterElementImpl;
class SVGFilterElement :	public SVGElement,
							public SVGURIReference,
							public SVGLangSpace,
							public SVGExternalResourcesRequired,
							public SVGStylable,
							public SVGUnitTypes 
{ 
public:
	SVGFilterElement();
	SVGFilterElement(const SVGFilterElement &other);
	SVGFilterElement &operator=(const SVGFilterElement &other);
	SVGFilterElement(SVGFilterElementImpl *other);
	virtual ~SVGFilterElement();

	SVGAnimatedEnumeration filterUnits() const;
	SVGAnimatedEnumeration primitiveUnits() const;
	SVGAnimatedLength x() const;
	SVGAnimatedLength y() const;
	SVGAnimatedLength width() const;
	SVGAnimatedLength height() const;
	SVGAnimatedInteger filterResX() const;
	SVGAnimatedInteger filterResY() const;

	void setFilterRes(unsigned long filterResX, unsigned long filterResY);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFilterElementImpl *handle() const { return impl; }

private:
	SVGFilterElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
