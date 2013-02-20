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


	This file includes excerpts from the Scalable Vector Graphics
    (SVG) 1.0 Specification (Proposed Recommendation)
    http://www.w3.org/TR/SVG

    Copyright © 2001 World Wide Web Consortium, (Massachusetts
    Institute of Technology, Institut National de Recherche en
    Informatique et en Automatique, Keio University).
    All Rights Reserved.
*/

#ifndef SVGDefsElement_H
#define SVGDefsElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGTransformable.h"

namespace KSVG
{

class SVGDefsElementImpl;

/**
 * The 'defs' element is a container element for referenced elements. For understandability and accessibility
 * reasons, it is recommended that, whenever possible, referenced elements be defined inside of a 'defs'.
 *
 * The content model for 'defs' is the same as for the <code> g </code> element; thus, any element that can be a child of
 * a <code> g </code> can also be a child of a 'defs', and vice versa.
 */
class SVGDefsElement : public SVGElement,
					   public SVGTests,
					   public SVGLangSpace,
					   public SVGExternalResourcesRequired,
					   public SVGStylable,
					   public SVGTransformable
{
public:
	SVGDefsElement();
	SVGDefsElement(const SVGDefsElement &other);
	SVGDefsElement &operator=(const SVGDefsElement &other);
	SVGDefsElement(SVGDefsElementImpl *other);
	virtual ~SVGDefsElement();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGDefsElementImpl *handle() const { return impl; }

private:
	SVGDefsElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
