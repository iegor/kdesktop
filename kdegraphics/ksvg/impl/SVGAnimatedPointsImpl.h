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

#ifndef SVGAnimatedPointsImpl_H
#define SVGAnimatedPointsImpl_H

#include <dom/dom_misc.h>

#include "ksvg_lookup.h"

class QString;

namespace KSVG
{

class SVGPointListImpl;
class SVGAnimatedPointsImpl : virtual public DOM::DomShared
{
public:
	SVGAnimatedPointsImpl();
	virtual ~SVGAnimatedPointsImpl();

	SVGPointListImpl *points() const;
	SVGPointListImpl *animatedPoints() const;

	static void parsePoints(QString _points, SVGPointListImpl *points);

protected:
	SVGPointListImpl *m_points;
	SVGPointListImpl *m_animatedPoints;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		Points, AnimatedPoints
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

#endif

// vim:ts=4:noet
