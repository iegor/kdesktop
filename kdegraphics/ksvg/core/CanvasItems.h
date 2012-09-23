/*
    Copyright (C) 2003 KSVG Team
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
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef CANVASITEMS_H
#define CANVASITEMS_H

#include <qptrlist.h>
#include "CanvasItem.h"
#include "SVGTextElementImpl.h"
#include "SVGTextPathElementImpl.h"

#include "svgpathparser.h"
#include "SVGBBoxTarget.h"

namespace T2P
{
	class GlyphSet;
	class BezierPath;
	class GlyphLayoutParams;
}

namespace KSVG
{

class KSVGCanvas;
class KSVGTextChunk;
class SVGPathParser;
class SVGMatrixImpl;
class SVGMarkerElementImpl;
class SVGClipPathElementImpl;
class SVGTextContentElementImpl;
class SVGTextPathElementImpl;

#define CANVAS_CLASS(Prefix, Class, Postfix, Member) \
class Canvas##Class : public CanvasItem \
{ \
public: \
	Canvas##Class(Prefix##Class##Postfix *Member) : CanvasItem(), m_##Member(Member) { } \
	virtual ~Canvas##Class() { } \
	virtual SVGElementImpl *element() { return reinterpret_cast<SVGElementImpl *>(m_##Member); } \
protected: \
	Prefix##Class##Postfix *m_##Member; \
};

CANVAS_CLASS(SVG, ClipPath, ElementImpl, clipPath)

class CanvasMarker : public CanvasItem
{
public:
	CanvasMarker(SVGMarkerElementImpl *marker) : CanvasItem(), m_marker(marker) {}
	virtual ~CanvasMarker() {}
	virtual SVGElementImpl *element() { return reinterpret_cast<SVGElementImpl *>(m_marker); }

	virtual void draw(SVGShapeImpl *obj, double x, double y, double lwidth = 1.0, double angle = 0.0)
	{
		Q_UNUSED(obj); Q_UNUSED(x); Q_UNUSED(y); Q_UNUSED(lwidth); Q_UNUSED(angle);
	}

protected:
	SVGMarkerElementImpl *m_marker;
};

class MarkerHelper
{
protected:
	void doStartMarker(SVGShapeImpl *shape, SVGStylableImpl *style, double x, double y, double angle = 0.0);
	void doMidMarker(SVGShapeImpl *shape, SVGStylableImpl *style, double x, double y, double angle = 0.0);
	void doEndMarker(SVGShapeImpl *shape, SVGStylableImpl *style, double x, double y, double angle = 0.0);

private:
	void doMarker(SVGShapeImpl *shape, SVGStylableImpl *style, double x, double y, double angle, const QString &marker);
};

class CanvasText : public CanvasItem
{
public:
	CanvasText(SVGTextElementImpl *text);
	virtual ~CanvasText();

	KSVGTextChunk *createTextChunk(KSVGCanvas *canvas, const SVGMatrixImpl *screenCTM, int &curx, int &cury, int &endx, int &endy);
	virtual SVGElementImpl *element() const { return m_text; }

	virtual void renderCallback(SVGTextContentElementImpl *element, const SVGMatrixImpl *screenCTM, T2P::GlyphSet *glyph, T2P::GlyphLayoutParams *params, double anchor) const = 0;
	void createGlyphs(KSVGTextChunk *textChunk, KSVGCanvas *canvas, const SVGMatrixImpl *screenCTM, int curx, int cury, int &endx, int &endy, T2P::BezierPath *bpath = 0) const;

	virtual void addTextDecoration(SVGTextContentElementImpl *element, double x, double y, double w, double h) const = 0;

private:
	void handleTSpan(KSVGCanvas *canvas, const SVGMatrixImpl *screenCTM, int &curx, int &cury, int &endx, int &endy, SVGElementImpl *element, KSVGTextChunk *textChunk, T2P::BezierPath *bpath);

protected:
	SVGTextElementImpl *m_text;
};

class CanvasPaintServer : public SVGBBoxTarget
{
public:
	CanvasPaintServer() : SVGBBoxTarget() { m_finalized = false; }
	virtual ~CanvasPaintServer() {}

	void setFinalized() { m_finalized = true; }
	void resetFinalized() { m_finalized = false; }
	bool finalized() { return m_finalized; }

	virtual void finalizePaintServer() = 0;
	virtual void reference(const QString &href) = 0;

private:
	bool m_finalized;
};

}

#endif

// vim:ts=4:noet
