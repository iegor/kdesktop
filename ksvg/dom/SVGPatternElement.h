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

#ifndef SVGPatternElement_H
#define SVGPatternElement_H

#include "SVGElement.h"
#include "SVGURIReference.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGFitToViewBox.h"
#include "SVGUnitTypes.h"

namespace KSVG
{

class SVGAnimatedEnumeration;
class SVGAnimatedTransformList;
class SVGAnimatedLength;
class SVGPatternElementImpl;

/**
 * A pattern is used to fill or stroke an object using a pre-defined graphic object which can be
 * replicated ("tiled") at fixed intervals in x and y to cover the areas to be painted.
 *
 * Patterns are defined using a 'pattern' element and then referenced by properties 'fill' and 'stroke'
 * on a given graphics element to indicate that the given element shall be filled or stroked with the
 * referenced pattern.
 */
class SVGPatternElement :	public SVGElement,
							public SVGURIReference,
							public SVGTests,
							public SVGLangSpace,
							public SVGExternalResourcesRequired,
							public SVGStylable,
							public SVGFitToViewBox,
							public SVGUnitTypes
{
public:
	SVGPatternElement();
	SVGPatternElement(const SVGPatternElement &other);
	SVGPatternElement &operator=(const SVGPatternElement &other);
	SVGPatternElement(SVGPatternElementImpl *other);
	virtual ~SVGPatternElement();

	/**
	 * Defines the coordinate system for attributes <code>x</code>, <code>y</code>, <code>width</code>, <code>height</code>.
	 * If patternUnits="userSpaceOnUse", <code>x</code>, <code>y</code>, <code>width</code>, <code>height</code> represent
	 * values in the coordinate system that results from taking the current user coordinate system in place at the time when
	 * the 'pattern' element is referenced (i.e., the user coordinate system for the element referencing the 'pattern' element
	 * via a 'fill' or 'stroke' property) and then applying the transform specified by attribute patternTransform.
	 * If patternUnits="objectBoundingBox", the user coordinate system for attributes <code>x</code>, <code>y</code>, <code>width</code>,
	 * <code>height</code> is established using the bounding box of the element to which the pattern is applied and then applying
	 * the transform specified by attribute <code>patternTransform</code>.
	 * If attribute <code>patternUnits</code> is not specified, then the effect is as if a value of objectBoundingBox were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The current target coordinate system mode.
	 */
	SVGAnimatedEnumeration patternUnits() const;

	/**
	 * Defines the coordinate system for the contents of the 'pattern'. Note that this attribute has no effect
	 * if attribute <code>viewBox</code> is specified.
	 * If patternContentUnits="userSpaceOnUse", the user coordinate system for the contents of the 'pattern' element
	 * is the coordinate system that results from taking the current user coordinate system in place at the time
	 * when the 'pattern' element is referenced (i.e., the user coordinate system for the element referencing the
	 * 'pattern' element via a 'fill' or 'stroke' property) and then applying the transform specified by attribute
	 * <code>patternTransform</code>.
	 * If patternContentUnits="objectBoundingBox", the user coordinate system for the contents of the 'pattern' element
	 * is established using the bounding box of the element to which the pattern is applied and then applying the
	 * transform specified by attribute <code>patternTransform</code>.
	 * If attribute <code>patternContentUnits</code> is not specified, then the effect is as if a value of
	 * userSpaceOnUse were specified. 
	 *
	 * This attribute is animatable.
	 *
	 * @return The current content coordinate system mode.
	 */
	SVGAnimatedEnumeration patternContentUnits() const;

	/**
	 * Contains the definition of an optional additional transformation from the pattern coordinate
	 * system onto the target coordinate system (i.e., userSpaceOnUse or objectBoundingBox).
	 * This allows for things such as skewing the pattern tiles. This additional transformation
	 * matrix is post-multiplied to (i.e., inserted to the right of) any previously defined
	 * transformations, including the implicit transformation necessary to convert from object
	 * bounding box units to user space.
	 * If attribute <code>patternTransform</code> is not specified, then the effect is as if an identity transform
	 * were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The transformation list that is applied to the pattern tile.
	 */
	SVGAnimatedTransformList patternTransform() const;

	/**
	 * The attributes <code>x</code>, <code>y</code>, <code>width</code>, <code>height</code>
	 * indicate how the pattern tiles are placed and spaced.
	 * These attributes represent coordinates and values in the coordinate space specified by
	 * the combination of attributes <code>patternUnits</code> and <code>patternTransform</code>.
	 * If the attribute is not specified, the effect is as if a value of zero were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The x-axis coordinate of the pattern tile.
	 */
	SVGAnimatedLength x() const;

	/**
	 * @see x
	 * If the attribute is not specified, the effect is as if a value of zero were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The y-axis coordinate of the pattern tile.
	 */
	SVGAnimatedLength y() const;

	/**
	 * @see x
	 * A negative value is an error. A value of zero disables rendering of
	 * the element (i.e., no paint is applied).
	 *
	 * If the attribute is not specified, the effect is as if a value of zero were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The width of the pattern tile.
	 */
	SVGAnimatedLength width() const;

	/**
	 * @see x
	 * A negative value is an error. A value of zero disables rendering of
	 * the element (i.e., no paint is applied).
	 *
	 * If the attribute is not specified, the effect is as if a value of zero were specified.
	 *
	 * This attribute is animatable.
	 *
	 * @return The height coordinate of the pattern tile.
	 */
	SVGAnimatedLength height() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPatternElementImpl *handle() const { return impl; }

private:
	SVGPatternElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
