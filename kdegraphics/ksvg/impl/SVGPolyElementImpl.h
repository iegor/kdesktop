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

#ifndef SVGPolyElementImpl_H
#define SVGPolyElementImpl_H

#include "ksvg_lookup.h"

#include "SVGShapeImpl.h"
#include "SVGTestsImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGTransformableImpl.h"
#include "SVGAnimatedPointsImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

namespace KSVG
{

class SVGPointListImpl;
class SVGPolyElementImpl : public SVGShapeImpl,
						   public SVGTestsImpl,
						   public SVGLangSpaceImpl,
						   public SVGExternalResourcesRequiredImpl,
						   public SVGStylableImpl,
						   public SVGTransformableImpl,
						   public SVGAnimatedPointsImpl
{
public:
	SVGPolyElementImpl(DOM::ElementImpl *);
	virtual ~SVGPolyElementImpl();

	virtual SVGRectImpl *getBBox();

	virtual void drawMarkers() = 0;

protected:
	bool findOutSlope(unsigned int point, double *outSlope) const;
	bool findInSlope(unsigned int point, double *inSlope) const;

	bool m_isOpenPath;

public:
	KSVG_FORWARDGET
	KSVG_FORWARDPUT
	KSVG_NO_TAG_BRIDGE
};

}

#endif

// vim:ts=4:noet
