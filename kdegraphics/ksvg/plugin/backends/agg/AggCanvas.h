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

#ifndef AGGCANVAS_H
#define AGGCANVAS_H

#include "KSVGCanvas.h"

#include "agg_basics.h"
#include "agg_rendering_buffer.h"
#include "agg_path_storage.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_u.h"
#include "agg_renderer_scanline.h"
#include "agg_pixfmt_rgb24.h"

namespace KSVG
{

class AggPaintServer;
class SVGElementImpl;
class AggCanvas : public KSVGCanvas
{
public:
	AggCanvas(unsigned int width, unsigned int height);

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

	virtual void setRenderBufferSize(int w, int h);

	agg::rendering_buffer &buf() { return m_buf; }

	float zoom() const { return m_zoom; }

	agg::rasterizer_scanline_aa<> m_ras;

protected:
	virtual void setBuffer(unsigned char *buffer);

protected:
	agg::rendering_buffer m_buf;
};

};

#endif
