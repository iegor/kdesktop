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

#ifndef SVGViewSpecImpl_H
#define SVGViewSpecImpl_H

#include <dom/dom_string.h>
#include <dom/dom_misc.h>

#include "ksvg_lookup.h"

#include "SVGZoomAndPanImpl.h"
#include "SVGFitToViewBoxImpl.h"

namespace KSVG
{

class SVGElementImpl;
class SVGTransformListImpl;
class SVGViewSpecImpl : public DOM::DomShared, public SVGZoomAndPanImpl, public SVGFitToViewBoxImpl
{
public:
	SVGViewSpecImpl();
	virtual ~SVGViewSpecImpl();

	SVGTransformListImpl *transform() const;
	SVGElementImpl *viewTarget() const;
	DOM::DOMString viewBoxString() const;
	DOM::DOMString preserveAspectRatioString() const;
	DOM::DOMString transformString() const;
	DOM::DOMString viewTargetString() const;

	bool parseViewSpec(const QString &);

private:
	SVGTransformListImpl *m_transform;
	SVGElementImpl *m_viewTarget;
	DOM::DOMString m_viewBoxString;
	DOM::DOMString m_preserveAspectRatioString;
	DOM::DOMString m_transformString;
	DOM::DOMString m_viewTargetString;

public:
	KSVG_FORWARDGET
	KSVG_FORWARDPUT
};

}

#endif

// vim:ts=4:noet
