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

#ifndef SVGPatternElementImpl_H
#define SVGPatternElementImpl_H

#include <qimage.h>
#include <qwmatrix.h>

#include "SVGTestsImpl.h"
#include "SVGElementImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGFitToViewBoxImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"
#include "SVGPaintServerImpl.h"
#include "LRUCache.h"

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGUnitConverter;
class SVGAnimatedLengthImpl;
class SVGAnimatedEnumerationImpl;
class SVGAnimatedTransformListImpl;
class SVGShapeImpl;
class SVGPatternElementImpl : public SVGElementImpl,
							  public SVGURIReferenceImpl,
							  public SVGTestsImpl,
							  public SVGLangSpaceImpl,
							  public SVGExternalResourcesRequiredImpl,
							  public SVGStylableImpl,
							  public SVGFitToViewBoxImpl,
							  public SVGPaintServerImpl
{
public:
	SVGPatternElementImpl(DOM::ElementImpl *);
	virtual ~SVGPatternElementImpl();

	SVGAnimatedEnumerationImpl *patternUnits() const;
	SVGAnimatedEnumerationImpl *patternContentUnits() const;
	SVGAnimatedTransformListImpl *patternTransform() const;
	SVGAnimatedLengthImpl *x() const;
	SVGAnimatedLengthImpl *y() const;
	SVGAnimatedLengthImpl *width() const;
	SVGAnimatedLengthImpl *height() const;

	virtual void setAttributes();

	virtual void createItem(KSVGCanvas *c = 0);
	virtual void removeItem(KSVGCanvas *c);

	SVGUnitConverter *converter() const { return m_converter; }

	void reference(const QString &href);
	void finalizePaintServer();

	class Tile
	{
	public:
		Tile() {}
		Tile(const QImage& image, const QWMatrix& screenToTile) : m_image(image), m_screenToTile(screenToTile) {}

		QImage image() const { return m_image; }
		const QWMatrix& screenToTile() const { return m_screenToTile; }

	private:
		QImage m_image;
		QWMatrix m_screenToTile;
	};

	Tile createTile(SVGShapeImpl *referencingElement);
	
	static void flushCachedTiles();

private:
	QImage createTile(SVGShapeImpl *referencingElement, int imageWidth, int imageHeight);

	SVGAnimatedEnumerationImpl *m_patternUnits;
	SVGAnimatedEnumerationImpl *m_patternContentUnits;
	SVGAnimatedTransformListImpl *m_patternTransform;
	SVGAnimatedLengthImpl *m_x;
	SVGAnimatedLengthImpl *m_y;
	SVGAnimatedLengthImpl *m_width;
	SVGAnimatedLengthImpl *m_height;

	SVGUnitConverter *m_converter;

	KSVGCanvas *m_canvas;	
	SVGElementImpl *m_location; // the referenced element
	MinOneLRUCache<QSize, QImage> m_tileCache;

	static QValueList<SVGPatternElementImpl *> m_patternElements;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		X, Y, Width, Height, PatternUnits, PatternContentUnits, PatternTransform
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

KSVG_REGISTER_ELEMENT(SVGPatternElementImpl, "pattern")

}

#endif

// vim:ts=4:noet
