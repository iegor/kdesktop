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

#ifndef SVGViewElement_H
#define SVGViewElement_H

#include "SVGElement.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGFitToViewBox.h"
#include "SVGZoomAndPan.h"

namespace KSVG
{

class SVGStringList;
class SVGViewElementImpl;
class SVGViewElement : public SVGElement,
					   public SVGExternalResourcesRequired,
					   public SVGFitToViewBox,
					   public SVGZoomAndPan
{
public:
	SVGViewElement();
	SVGViewElement(const SVGViewElement &other);
	SVGViewElement &operator=(const SVGViewElement &other);
	SVGViewElement(SVGViewElementImpl *other);
	virtual ~SVGViewElement();

	SVGStringList viewTarget() const;

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGViewElementImpl *handle() const { return impl; }

private:
	SVGViewElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
