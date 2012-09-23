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

#ifndef SVGMarkerElement_H
#define SVGMarkerElement_H

#include "SVGElement.h"
#include "SVGLangSpace.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"
#include "SVGFitToViewBox.h"

namespace KSVG
{

enum
{
	SVG_MARKERUNITS_UNKNOWN = 0,
	SVG_MARKERUNITS_USERSPACEONUSE = 1,
	SVG_MARKERUNITS_STROKEWIDTH = 2
};

enum
{
	SVG_MARKER_ORIENT_UNKNOWN      = 0,
	SVG_MARKER_ORIENT_AUTO         = 1,
	SVG_MARKER_ORIENT_ANGLE        = 2
};

class SVGAnimatedLength;
class SVGAnimatedAngle;
class SVGAngle;
class SVGAnimatedEnumeration;
class SVGMarkerElementImpl;
class SVGMarkerElement : public SVGElement,
						 public SVGLangSpace,
						 public SVGExternalResourcesRequired,
						 public SVGStylable,
						 public SVGFitToViewBox 
{ 
public:
	SVGMarkerElement();
	SVGMarkerElement(const SVGMarkerElement &other);
	SVGMarkerElement &operator=(const SVGMarkerElement &other);
	SVGMarkerElement(SVGMarkerElementImpl *other);
	virtual ~SVGMarkerElement();

	SVGAnimatedLength refX() const;
	SVGAnimatedLength refY() const;
	SVGAnimatedEnumeration markerUnits() const;
	SVGAnimatedLength markerWidth() const;
	SVGAnimatedLength markerHeight() const;
	SVGAnimatedEnumeration orientType() const;
	SVGAnimatedAngle orientAngle() const;

	void setOrientToAuto();
	void setOrientToAngle(const SVGAngle &angle);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGMarkerElementImpl *handle() const { return impl; }

private:
	SVGMarkerElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
