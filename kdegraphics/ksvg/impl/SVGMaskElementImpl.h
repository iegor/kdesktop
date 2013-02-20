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

#ifndef SVGMaskElementImpl_H
#define SVGMaskElementImpl_H

#include <qwmatrix.h>

#include "SVGTestsImpl.h"
#include "SVGBBoxTarget.h"
#include "SVGElementImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"
#include "LRUCache.h"

#include "ksvg_lookup.h"

namespace KSVG
{


class SVGUnitConverter;
class KSVGCanvas;
class SVGShapeImpl;
class SVGRectImpl;
class SVGAnimatedLengthImpl;
class SVGAnimatedEnumerationImpl;
class SVGMaskElementImpl : public SVGElementImpl,
						   public SVGTestsImpl,
						   public SVGLangSpaceImpl,
						   public SVGExternalResourcesRequiredImpl,
						   public SVGStylableImpl,
						   public SVGBBoxTarget
{
public:

	class Mask
	{
	public:
		Mask() : m_width(0), m_height(0) {}
		Mask(const QByteArray& mask, const QWMatrix& screenToMask, int width, int height);
		~Mask() {}
	
		bool isEmpty() const { return m_width == 0; }
		unsigned char value(int screenX, int screenY) const;
		QByteArray rectangle(int screenX, int screenY, int width, int height);
		QByteArray rectangle(const QRect& rect);
		void setScreenToMask(const QWMatrix& matrix) { m_screenToMask = matrix; }

	private:
		int m_width;
		int m_height;
		QByteArray m_mask;
		QWMatrix m_screenToMask;
	};

	SVGMaskElementImpl(DOM::ElementImpl *);
	virtual ~SVGMaskElementImpl();

	SVGAnimatedEnumerationImpl *maskUnits() const;
	SVGAnimatedEnumerationImpl *maskContentUnits() const;
	SVGAnimatedLengthImpl *x() const;
	SVGAnimatedLengthImpl *y() const;
	SVGAnimatedLengthImpl *width() const;
	SVGAnimatedLengthImpl *height() const;

	virtual SVGRectImpl *getBBox();
	virtual void setAttributes();
	
	SVGUnitConverter *converter() const { return m_converter; }

	Mask createMask(SVGShapeImpl *referencingElement);
	
	// Compute the mask on a given shape, taking into account all masks defined
	// on the shape's ancestors. This is a workaround for us not having a buffer
	// for container elements, so we can't mask containers directly.
	static QByteArray maskRectangle(SVGShapeImpl *shape, const QRect& screenRectangle);

private:
	class CacheKey
	{
	public:
		CacheKey()  : m_element(0), m_width(0), m_height(0) {}
		CacheKey(SVGElementImpl *element, int width, int height) : m_element(element), m_width(width), m_height(height) {}
		bool operator==(const CacheKey& other) const { return m_element == other.m_element && m_width == other.m_width && m_height == other.m_height; }

	private:
		SVGElementImpl *m_element;
		int m_width;
		int m_height;
	};

	Mask createMask(SVGShapeImpl *referencingElement, int imageWidth, int imageHeight);

	SVGAnimatedEnumerationImpl *m_maskUnits;
	SVGAnimatedEnumerationImpl *m_maskContentUnits;
	SVGAnimatedLengthImpl *m_x;
	SVGAnimatedLengthImpl *m_y;
	SVGAnimatedLengthImpl *m_width;
	SVGAnimatedLengthImpl *m_height;

	SVGUnitConverter *m_converter;
	KSVGCanvas *m_canvas;

	MinOneLRUCache<CacheKey, Mask> m_maskCache;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE
	
	enum
	{
		// Properties
		MaskUnits, MaskContentUnits, X, Y, Width, Height
	};
	
	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGMaskElementImpl, "mask")

inline unsigned char SVGMaskElementImpl::Mask::value(int screenX, int screenY) const
{
	int x, y;

	m_screenToMask.map(screenX, screenY, &x, &y);

	if(x >= 0 && x < m_width && y >= 0 && y < m_height)
		return m_mask[x + y * m_width];
	else
		return 0;
}

}

#endif

// vim:ts=4:noet
