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

#ifndef SVGPaint_H
#define SVGPaint_H

#include "SVGColor.h"
#include <dom/dom_string.h>

namespace KSVG
{

enum
{
	SVG_PAINTTYPE_UNKNOWN               = 0,
	SVG_PAINTTYPE_RGBCOLOR              = 1,
	SVG_PAINTTYPE_RGBCOLOR_ICCCOLOR     = 2,
	SVG_PAINTTYPE_NONE                  = 101,
	SVG_PAINTTYPE_CURRENTCOLOR          = 102,
	SVG_PAINTTYPE_URI_NONE              = 103,
	SVG_PAINTTYPE_URI_CURRENTCOLOR      = 104,
	SVG_PAINTTYPE_URI_RGBCOLOR          = 105,
	SVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR = 106,
	SVG_PAINTTYPE_URI                   = 107
};

class SVGPaintImpl;
class SVGPaint : public SVGColor 
{ 
public:
	SVGPaint();
	SVGPaint(const SVGPaint &other);
	SVGPaint &operator=(const SVGPaint &other);
	SVGPaint(SVGPaintImpl *other);
	virtual ~SVGPaint();

	unsigned short paintType() const;
	DOM::DOMString uri() const;
	void setUri(const DOM::DOMString &uri);
	void setPaint(unsigned short paintType, const DOM::DOMString &uri, const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGPaintImpl *handle() const { return impl; }

private:
	SVGPaintImpl *impl;
};

}

#endif

// vim:ts=4:noet
