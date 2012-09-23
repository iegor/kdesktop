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

#ifndef SVGPrimitiveStandardAttributesImpl_H
#define SVGPrimitiveStandardAttributesImpl_H

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGAnimatedLengthImpl;
class SVGAnimatedStringImpl;
class SVGFilterPrimitiveStandardAttributesImpl
{
public:
	SVGFilterPrimitiveStandardAttributesImpl();
	virtual ~SVGFilterPrimitiveStandardAttributesImpl();

	SVGAnimatedLengthImpl *x() const;
	SVGAnimatedLengthImpl *y() const;
	SVGAnimatedLengthImpl *width() const;
	SVGAnimatedLengthImpl *height() const;
	SVGAnimatedStringImpl *result() const;

private:
	SVGAnimatedLengthImpl *m_x;
	SVGAnimatedLengthImpl *m_y;
	SVGAnimatedLengthImpl *m_width;
	SVGAnimatedLengthImpl *m_height;
	SVGAnimatedStringImpl *m_result;
};

}

#endif

// vim:ts=4:noet
