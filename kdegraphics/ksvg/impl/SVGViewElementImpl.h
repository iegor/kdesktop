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

#ifndef SVGViewElementImpl_H
#define SVGViewElementImpl_H

#include "SVGElementImpl.h"
#include "SVGZoomAndPanImpl.h"
#include "SVGFitToViewBoxImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGStringListImpl;
class SVGViewElementImpl : public SVGElementImpl,
						   public SVGExternalResourcesRequiredImpl,
						   public SVGFitToViewBoxImpl,
						   public SVGZoomAndPanImpl
{
public:
	SVGViewElementImpl(DOM::ElementImpl *);
	virtual ~SVGViewElementImpl();

	SVGStringListImpl *viewTarget() const;

private:
	SVGStringListImpl *m_viewTarget;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		ViewTarget
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGViewElementImpl, "view")

}

#endif

// vim:ts=4:noet
