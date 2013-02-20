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

#ifndef SVGFontElement_H
#define SVGFontElement_H

#include "SVGElement.h"
#include "SVGExternalResourcesRequired.h"
#include "SVGStylable.h"

namespace KSVG
{

class SVGFontElementImpl;
class SVGFontElement : public SVGElement,
					   public SVGExternalResourcesRequired,
					   public SVGStylable
{
public:
	SVGFontElement();
	SVGFontElement(const SVGFontElement &other);
	SVGFontElement &operator=(const SVGFontElement &other);
	SVGFontElement(SVGFontElementImpl *other);
	virtual ~SVGFontElement();

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGFontElementImpl *handle() const { return impl; }

private:
	SVGFontElementImpl *impl;
};

}

#endif

// vim:ts=4:noet
