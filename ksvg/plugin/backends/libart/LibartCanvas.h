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
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef LIBARTCANVAS_H
#define LIBARTCANVAS_H

#include "CanvasItem.h"
#include "KSVGCanvas.h"

#include <libart_lgpl/art_svp.h>
#include <libart_lgpl/art_vpath.h>
#include <libart_lgpl/art_config.h>

#include <Glyph.h>
#include <Converter.h>
#include "GlyphTracerLibart.h"

class QString;
class QImage;

struct _ArtSVP;

namespace KSVG
{

class LibartPaintServer;
class SVGElementImpl;
class SVGStylableImpl;
class SVGSVGElementImpl;
class KSVGPolygon;
class LibartCanvas : public KSVGCanvas
{
public:
	LibartCanvas(unsigned int width, unsigned int height);

	void drawSVP(_ArtSVP *svp, art_u32 color, QByteArray mask, QRect screenBBox);
	void drawImage(QImage image, SVGStylableImpl *style, const SVGMatrixImpl *matrix, const KSVGPolygon& clippingPolygon);

	virtual T2P::BezierPath *toBezierPath(CanvasItem *item) const;

	// creating canvas items
	virtual CanvasItem *createRectangle(SVGRectElementImpl *rect);
	virtual CanvasItem *createEllipse(SVGEllipseElementImpl *ellipse);
	virtual CanvasItem *createCircle(SVGCircleElementImpl *circle);
	virtual CanvasItem *createLine(SVGLineElementImpl *line);
	virtual CanvasItem *createPolyline(SVGPolylineElementImpl *poly);
	virtual CanvasItem *createPolygon(SVGPolygonElementImpl *poly);
	virtual CanvasItem *createPath(SVGPathElementImpl *path);
	virtual CanvasItem *createClipPath(SVGClipPathElementImpl *clippath);
	virtual CanvasItem *createImage(SVGImageElementImpl *image);
	virtual CanvasItem *createCanvasMarker(SVGMarkerElementImpl *marker);
	virtual CanvasItem *createText(SVGTextElementImpl *text);
	virtual CanvasPaintServer *createPaintServer(SVGElementImpl *pserver);

	_ArtSVP *clippingRect(const QRect &rect, const SVGMatrixImpl *ctm);
	_ArtSVP *svpFromPolygon(const KSVGPolygon& polygon);

	static ArtSVP *copy_svp(const ArtSVP *svp);

	_ArtSVP *clipSingleSVP(_ArtSVP *svp, SVGShapeImpl *clipShape);
};

}

#endif

// vim:ts=4:noet
