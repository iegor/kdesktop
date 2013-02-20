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

#ifndef SVGTextContentElement_H
#define SVGTextContentElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"

namespace KSVG
{

enum 
{
	LENGTHADJUST_UNKNOWN = 0,
	LENGTHADJUST_SPACING = 1,
	LENGTHADJUST_SPACINGANDGLYPHS = 2
};
	
class SVGAnimatedLength;
class SVGAnimatedEnumeration;
class SVGTextContentElementImpl;
class SVGTextContentElement : public SVGElement,
							  public SVGTests,
							  public SVGLangSpace,
							  public SVGExternalResourcesRequired,
							  public SVGStylable
{
public:
	SVGTextContentElement(const SVGTextContentElement &);
	SVGTextContentElement &operator=(const SVGTextContentElement &other);
	SVGTextContentElement(SVGTextContentElementImpl *);
	~SVGTextContentElement();

	SVGAnimatedLength textLength() const;
	SVGAnimatedEnumeration lengthAdjust() const;
	long getNumberOfChars();
	float getComputedTextLength();
//	float getSubStringLength(const unsigned long &charnum, const unsigned long &nchars);
//	SVGPoint getStartPositionOfChar(const unsigned long &charnum);
//	SVGPoint getEndPositionOfChar(const unsigned long &charnum);
//	SVGRect getExtentOfChar(const unsigned long &charnum);
//	float getRotationOfChar(const unsigned long &charnum);
//	long getCharNumAtPosition(const SVGPoint &point);
//	void selectSubString(const unsigned long &charnum, const unsigned long &nchars);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGTextContentElementImpl *handle() const { return impl; }

protected:
	SVGTextContentElement();

private:
	SVGTextContentElementImpl *impl;
};

}

#endif
