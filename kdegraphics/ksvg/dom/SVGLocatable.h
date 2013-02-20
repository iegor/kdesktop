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

#ifndef SVGLocatable_H
#define SVGLocatable_H

namespace KSVG
{

class SVGElement;
class SVGMatrix;
class SVGRect;
class SVGLocatableImpl;

/**
 * Interface SVGLocatable is for all elements which either have a <code>transform</code>
 * attribute or don't have a <code>transform</code> attribute but whose content can have
 * a bounding box in current user space.
 */
class SVGLocatable
{ 
public:
	SVGLocatable(const SVGLocatable &other);
	SVGLocatable &operator=(const SVGLocatable &other);
	SVGLocatable(SVGLocatableImpl *other);
	virtual ~SVGLocatable();

	/**
	 * The element which established the current viewport. Often, the nearest
	 * ancestor 'svg' element. Null if the current element is the outermost 'svg' element. 
	 */	
	SVGElement nearestViewportElement() const;

	/**
	 * The farthest ancestor 'svg' element. Null if the current element is
	 * the outermost 'svg' element. 
	 */
	SVGElement farthestViewportElement() const;

	/**
	 * Returns the tight bounding box in current user space (i.e., after application of
	 * the <code>transform</code> attribute, if any) on the geometry of all contained graphics
	 * elements, exclusive of stroke-width and filter effects).
	 *
	 * @return An SVGRect object that defines the bounding box.
	 */
	SVGRect getBBox();

	/**
	 * Returns the transformation matrix from current user units (i.e., after application of
	 * the <code>transform</code> attribute, if any) to the viewport coordinate system for
	 * the nearestViewportElement.
	 *
	 * @return An SVGMatrix object that defines the CTM.
	 */
	SVGMatrix getCTM();

	/**
	 * Returns the transformation matrix from current user units (i.e., after application of
	 * the <code>transform</code> attribute, if any) to the parent user agent's notice of a "pixel".
	 * For display devices, ideally this represents a physical screen pixel. For other devices or
	 * environments where physical pixel sizes are not known, then an algorithm similar to the
	 * CSS2 definition of a "pixel" can be used instead.
	 *
	 * @return An SVGMatrix object that defines the given transformation matrix.
	 */
	SVGMatrix getScreenCTM();

	/**
	 * Returns the transformation matrix from the user coordinate system on the current
	 * element (after application of the <code>transform</code> attribute, if any) to the
	 * user coordinate system on parameter element (after application of its
	 * <code>transform</code> attribute, if any). 
	 *
	 * @param element   The target element.
	 *
	 * @return An SVGMatrix object that defines the transformation.
	 */
	SVGMatrix getTransformToElement(const SVGElement &element);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGLocatableImpl *handle() const { return impl; }

protected:
	SVGLocatable();

private:
	SVGLocatableImpl *impl;
};

}

#endif

// vim:ts=4:noet
