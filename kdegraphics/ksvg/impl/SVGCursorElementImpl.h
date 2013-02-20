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

#ifndef SVGCursorElementImpl_H
#define SVGCursorElementImpl_H

#include "SVGTestsImpl.h"
#include "SVGElementImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGAnimatedLengthImpl;
class SVGCursorElementImpl : public SVGElementImpl,
							 public SVGURIReferenceImpl,
							 public SVGTestsImpl,
							 public SVGExternalResourcesRequiredImpl
{
public:
	SVGCursorElementImpl(DOM::ElementImpl *);
	virtual ~SVGCursorElementImpl();

	SVGAnimatedLengthImpl *x() const;
	SVGAnimatedLengthImpl *y() const;

private:
	SVGAnimatedLengthImpl *m_x;
	SVGAnimatedLengthImpl *m_y;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		X, Y
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

#endif

// vim:ts=4:noet
