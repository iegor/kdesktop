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

#ifndef SVGAnimationElement_H
#define SVGAnimationElement_H

#include "SVGElement.h"
#include "SVGTests.h"
#include "SVGExternalResourcesRequired.h"

namespace KSVG
{

class SVGAnimationElementImpl;
class SVGAnimationElement : public SVGElement,
							public SVGTests,
							public SVGExternalResourcesRequired
{
public:
	SVGAnimationElement();
	SVGAnimationElement(const SVGAnimationElement &other);
	SVGAnimationElement &operator=(const SVGAnimationElement &other);
	SVGAnimationElement(SVGAnimationElementImpl *other);
	virtual ~SVGAnimationElement();

	SVGElement targetElement() const;

	float getStartTime();
	float getCurrentTime();
	float getSimpleDuration();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGAnimationElementImpl *handle() const { return impl; }

private:
	SVGAnimationElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
