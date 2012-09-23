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

#ifndef SVGColor_H
#define SVGColor_H

#include <dom/css_value.h>
#include <dom/dom_string.h>

namespace KSVG
{

enum
{
	SVG_COLORTYPE_UNKNOWN = 0,
	SVG_COLORTYPE_RGBCOLOR = 1,
	SVG_COLORTYPE_RGBCOLOR_ICCCOLOR = 2,
	SVG_COLORTYPE_CURRENTCOLOR = 3
};

class SVGColorImpl;
class SVGICCColor;
class SVGColor // : public css::CSSValue 
{ 
public:
	SVGColor();
	SVGColor(const SVGColor &);
	SVGColor &operator=(const SVGColor &other);
	SVGColor(SVGColorImpl *);
	~SVGColor();

	unsigned short colorType() const;

	DOM::RGBColor rgbColor() const;
	SVGICCColor iccColor() const;

	void setRGBColor(const DOM::DOMString &rgbColor);
	void setRGBColorICCColor(const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor);
	void setColor(unsigned short colorType, const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGColorImpl *handle() const { return impl; }

private:
	SVGColorImpl *impl;
};

}

#endif

// vim:ts=4:noet
