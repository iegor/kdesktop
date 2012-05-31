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

#ifndef SVGRect_H
#define SVGRect_H

namespace KSVG
{

/**
 * Rectangles are defined as consisting of a (x,y) coordinate pair
 * identifying a minimum X value, a minimum Y value, and a width and
 * height, which are usually constrained to be non-negative. 
 */
class SVGRectImpl;
class SVGRect 
{ 
public:
	SVGRect();
	SVGRect(const SVGRect &);
	SVGRect &operator=(const SVGRect &);
	SVGRect(SVGRectImpl *);
	~SVGRect();

	/**
	 * Corresponds to attribute <code>x</code> on the given element. 
	 */
	void setX(float x);
	float x() const;

	/**
	 * Corresponds to attribute <code>y</code> on the given element. 
	 */	
	void setY(float y);
	float y() const;

	/**
	 * Corresponds to attribute <code>width</code> on the given element. 
	 */
	void setWidth(float width);
	float width() const;

	/**
	 * Corresponds to attribute <code>height</code> on the given element. 
	 */	
	void setHeight(float height);
	float height() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGRectImpl *handle() const { return impl; }

private:
	SVGRectImpl *impl;
};

}

#endif

// vim:ts=4:noet
