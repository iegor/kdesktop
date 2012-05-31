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

#ifndef SVGGElement_H
#define SVGGElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGTransformable.h"

namespace KSVG
{

class SVGGElementImpl;

/**
 * The 'g' element is a container element for grouping together related graphics elements.
 *
 * Grouping constructs, when used in conjunction with the @ref desc and @ref title elements, provide
 * information about document structure and semantics. Documents that are rich in structure may be
 * rendered graphically, as speech, or as braille, and thus promote accessibility.
 *
 * A group of elements, as well as individual objects, can be given a name using the <code> id </code> attribute.
 * Named groups are needed for several purposes such as animation and re-usable objects.
 *
 * A 'g' element can contain other 'g' elements nested within it, to an arbitrary depth.
 */
class SVGGElement : public SVGElement,
					public SVGTests,
					public SVGLangSpace,
					public SVGExternalResourcesRequired,
					public SVGStylable,
					public SVGTransformable
{ 
public:
	SVGGElement();
	SVGGElement(const SVGGElement &other);
	SVGGElement &operator=(const SVGGElement &other);
	SVGGElement(SVGGElementImpl *other);
	~SVGGElement();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGGElementImpl *handle() const { return impl; }

private:
	SVGGElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
