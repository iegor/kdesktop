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

#ifndef SVGFETurbulenceElementImpl_H
#define SVGFETurbulenceElementImpl_H

#include "SVGElementImpl.h"
#include "SVGFilterPrimitiveStandardAttributesImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGAnimatedNumberImpl;
class SVGAnimatedIntegerImpl;
class SVGAnimatedEnumerationImpl;
class SVGFETurbulenceElementImpl : public SVGElementImpl, public SVGFilterPrimitiveStandardAttributesImpl
{
public:
	SVGFETurbulenceElementImpl(DOM::ElementImpl *);
	virtual ~SVGFETurbulenceElementImpl();

	SVGAnimatedNumberImpl *baseFrequencyX() const;
	SVGAnimatedNumberImpl *baseFrequencyY() const;
	SVGAnimatedIntegerImpl *numOctaves() const;
	SVGAnimatedNumberImpl *seed() const;
	SVGAnimatedEnumerationImpl *stitchTiles() const;
	SVGAnimatedEnumerationImpl *type() const;

private:
	SVGAnimatedNumberImpl *m_baseFrequencyX;
	SVGAnimatedNumberImpl *m_baseFrequencyY;
	SVGAnimatedIntegerImpl *m_numOctaves;
	SVGAnimatedNumberImpl *m_seed;
	SVGAnimatedEnumerationImpl *m_stitchTiles;
	SVGAnimatedEnumerationImpl *m_type;

public:
	KSVG_FORWARDGET
	KSVG_FORWARDPUT
};

}

#endif

// vim:ts=4:noet
