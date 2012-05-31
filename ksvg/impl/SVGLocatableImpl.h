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

#ifndef SVGLocatableImpl_H
#define SVGLocatableImpl_H

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGRectImpl;
class SVGMatrixImpl;
class SVGElementImpl;
class SVGLocatableImpl
{
public:
	SVGLocatableImpl();
	virtual ~SVGLocatableImpl();

	SVGElementImpl *nearestViewportElement() const { return m_nearestViewportElement; }
	SVGElementImpl *farthestViewportElement() const{ return m_farthestViewportElement; }

	virtual SVGRectImpl *getBBox();
	virtual SVGMatrixImpl *getCTM();
	virtual SVGMatrixImpl *getScreenCTM();

	// Faster access for when a new copy isn't required.
	const SVGMatrixImpl *screenCTM() const { return m_cachedScreenCTM; }

	SVGMatrixImpl *getTransformToElement(SVGElementImpl *element);

	// The local transformations concatenated together. 0 if
	// there are no local transformations.
	virtual const SVGMatrixImpl *localMatrix() { return 0; }

	bool cachedScreenCTMIsValid() const { return m_cachedScreenCTMIsValid; }
	void invalidateCachedMatrices() { m_cachedScreenCTMIsValid = false; }

	// If the cached matrix is invalid, update it and update any child elements
	// recursively. Otherwise, check child elements recursively.
	virtual void checkCachedScreenCTM(const SVGMatrixImpl *parentScreenCTM);
	// Update the cached matrix, and update child element cached matrices,
	// recursively.
	virtual void updateCachedScreenCTM(const SVGMatrixImpl *parentScreenCTM);

	// Called immediately after the screen ctm has been updated.
	virtual void onScreenCTMUpdated() {}

protected:
	SVGElementImpl *m_nearestViewportElement;
	SVGElementImpl *m_farthestViewportElement;
	SVGMatrixImpl *m_cachedScreenCTM;
	bool m_cachedScreenCTMIsValid;

public:
	KSVG_BASECLASS_GET

	enum
	{
		// Properties
		NearestViewportElement, FarthestViewportElement,
		// Functions
		GetBBox, GetCTM, GetScreenCTM, GetTransformToElement
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
};

}

KSVG_DEFINE_PROTOTYPE(SVGLocatableImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGLocatableImplProtoFunc, SVGLocatableImpl)

#endif

// vim:ts=4:noet
