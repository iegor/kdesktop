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

#ifndef SVGUseElement_H
#define SVGUseElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGTransformable.h"
#include "SVGURIReference.h"

namespace KSVG
{

class SVGAnimatedLength;
class SVGElementInstance;
class SVGUseElementImpl;
class SVGUseElement :	public SVGElement,
						public SVGTests,
						public SVGLangSpace,
						public SVGExternalResourcesRequired,
						public SVGStylable,
						public SVGTransformable,
						public SVGURIReference
{
public:
	SVGUseElement();
	SVGUseElement(const SVGUseElement &other);
	SVGUseElement &operator=(const SVGUseElement &other);
	SVGUseElement(SVGUseElementImpl *other);
	virtual ~SVGUseElement();

	SVGAnimatedLength x() const;
	SVGAnimatedLength y() const;
	SVGAnimatedLength width() const;
	SVGAnimatedLength height() const;
	SVGElementInstance instanceRoot() const;
	SVGElementInstance animatedInstanceRoot() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGUseElementImpl *handle() const { return impl; } 

private:
	SVGUseElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
