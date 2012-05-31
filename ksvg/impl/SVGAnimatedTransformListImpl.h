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

#ifndef SVGAnimatedTransformListImpl_H
#define SVGAnimatedTransformListImpl_H

#include <dom/dom_misc.h>

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGTransformListImpl;
class SVGAnimatedTransformListImpl : public DOM::DomShared
{
public:
	SVGAnimatedTransformListImpl();
	virtual ~SVGAnimatedTransformListImpl();

	SVGTransformListImpl *baseVal() const;
	SVGTransformListImpl *animVal() const;

private:
	SVGTransformListImpl *m_baseVal;
	SVGTransformListImpl *m_animVal;

public:
	KSVG_GET

	enum
    {
		// Properties
		BaseVal, AnimVal
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
};

}

#endif

// vim:ts=4:noet
