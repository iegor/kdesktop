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

#ifndef SVGTests_H
#define SVGTests_H

#include <dom/dom_string.h>

namespace KSVG
{

class SVGStringList;
class SVGTestsImpl;
class SVGTests 
{ 
public:
	SVGTests(const SVGTests &other);
	SVGTests &operator=(const SVGTests &other);
	SVGTests(SVGTestsImpl *other);
	~SVGTests();

	SVGStringList requiredFeatures() const;
	SVGStringList requiredExtensions() const;
	SVGStringList systemLanguage() const;

	bool hasExtension(const DOM::DOMString &extension);

	// Internal! - NOT PART OF THE SVG SPECIFICATION
	SVGTestsImpl *handle() const { return impl; }

protected:
	SVGTests();

private:
	SVGTestsImpl *impl;
};

}

#endif

// vim:ts=4:noet
