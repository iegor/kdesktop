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

#ifndef SVGViewSpec_H
#define SVGViewSpec_H

#include "SVGZoomAndPan.h"
#include "SVGFitToViewBox.h"
#include <dom/dom_string.h>

namespace KSVG
{

class SVGTransformList;
class SVGElement;
class SVGViewSpecImpl;
class SVGViewSpec :	public SVGZoomAndPan,
					public SVGFitToViewBox
{
public:
	SVGViewSpec();
	SVGViewSpec(const SVGViewSpec &other);
	SVGViewSpec &operator=(const SVGViewSpec &other);
	SVGViewSpec(SVGViewSpecImpl *other);
	virtual ~SVGViewSpec();

	SVGTransformList transform() const;
	SVGElement viewTarget() const;

	DOM::DOMString viewBoxString() const;
	DOM::DOMString preserveAspectRatioString() const;
	DOM::DOMString transformString() const;
	DOM::DOMString viewTargetString() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGViewSpecImpl *handle() const { return impl; }

private:
	SVGViewSpecImpl *impl;
};

}

#endif

// vim:ts=4:noet
