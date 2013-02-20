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

#ifndef SVGFilterElementImpl_H
#define SVGFilterElementImpl_H

#include "SVGElementImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGAnimatedLengthImpl;
class SVGAnimatedIntegerImpl;
class SVGAnimatedEnumerationImpl;
class SVGFilterElementImpl : public SVGElementImpl,
							 public SVGURIReferenceImpl,
							 public SVGLangSpaceImpl,
							 public SVGExternalResourcesRequiredImpl,
							 public SVGStylableImpl
{
public:
	SVGFilterElementImpl(DOM::ElementImpl *);
	virtual ~SVGFilterElementImpl();

	SVGAnimatedEnumerationImpl *filterUnits() const;
	SVGAnimatedEnumerationImpl *primitiveUnits() const;
	SVGAnimatedLengthImpl *x() const;
	SVGAnimatedLengthImpl *y() const;
	SVGAnimatedLengthImpl *width() const;
	SVGAnimatedLengthImpl *height() const;
	SVGAnimatedIntegerImpl *filterResX() const;
	SVGAnimatedIntegerImpl *filterResY() const;
	void setFilterRes(unsigned long filterResX, unsigned long filterResY);

private:
	SVGAnimatedEnumerationImpl *m_filterUnits;
	SVGAnimatedEnumerationImpl *m_primitiveUnits;
	SVGAnimatedLengthImpl *m_x;
	SVGAnimatedLengthImpl *m_y;
	SVGAnimatedLengthImpl *m_width;
	SVGAnimatedLengthImpl *m_height;
	SVGAnimatedIntegerImpl *m_filterResX;
	SVGAnimatedIntegerImpl *m_filterResY;
public:
	KSVG_FORWARDGET
	KSVG_FORWARDPUT
};

}

#endif

// vim:ts=4:noet
