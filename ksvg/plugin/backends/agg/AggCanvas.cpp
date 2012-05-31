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

#include "AggCanvas.h"
#include "SVGShapeImpl.h"
#include "SVGPaintImpl.h"
#include "SVGPaint.h"
#include "SVGGradientElementImpl.h"
#include "SVGLinearGradientElementImpl.h"
#include "SVGRadialGradientElementImpl.h"
#include "SVGPatternElementImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include <kdebug.h>

#include "AggCanvasItems.h"
#include "GlyphTracerAgg.h"

#include "agg_vertex_iterator.h"
#include "agg_bounding_rect.h"

using namespace KSVG;

AggCanvas::AggCanvas(unsigned int width, unsigned int height) : KSVGCanvas(width, height)
{
	m_fontContext = new T2P::Converter(new T2P::GlyphTracerAgg());
}

void AggCanvas::setRenderBufferSize(int w, int h)
{
	KSVGCanvas::setRenderBufferSize(w, h);
	m_buf.attach(m_buffer, m_width, m_height, m_width * m_nrChannels);
}

void AggCanvas::setBuffer(unsigned char *buffer)
{
	KSVGCanvas::setBuffer(buffer);
	m_buf.attach(m_buffer, m_width, m_height, m_width * m_nrChannels);
}

T2P::BezierPath *AggCanvas::toBezierPath(CanvasItem *item) const
{
	// Only handle AggPath items
	return dynamic_cast<AggPath *>(item);
}

// drawing primitives
CanvasItem *AggCanvas::createRectangle(SVGRectElementImpl *rect)
{
	return new AggRectangle(this, rect);
}

CanvasItem *AggCanvas::createEllipse(SVGEllipseElementImpl *ellipse)
{
	return new AggEllipse(this, ellipse);
}

CanvasItem *AggCanvas::createCircle(SVGCircleElementImpl *circle)
{
	return new AggCircle(this, circle);
}

CanvasItem *AggCanvas::createLine(SVGLineElementImpl *line)
{
	return new AggLine(this, line);
}

CanvasItem *AggCanvas::createPolyline(SVGPolylineElementImpl *poly)
{
	return new AggPolyline(this, poly);
}

CanvasItem *AggCanvas::createPolygon(SVGPolygonElementImpl *poly)
{
	return new AggPolygon(this, poly);
}

CanvasItem *AggCanvas::createPath(SVGPathElementImpl *path)
{
	return new AggPath(this, path);
}

CanvasItem *AggCanvas::createClipPath(SVGClipPathElementImpl *)
{
	return 0;
}

CanvasItem *AggCanvas::createImage(SVGImageElementImpl *image)
{
	return new AggImage(this, image);
}

CanvasItem *AggCanvas::createCanvasMarker(SVGMarkerElementImpl *marker)
{
	return new AggMarker(this, marker);
}

CanvasItem *AggCanvas::createText(SVGTextElementImpl *text)
{
	return new AggText(this, text);
}

CanvasPaintServer *AggCanvas::createPaintServer(SVGElementImpl *pserver)
{
	AggPaintServer *result = 0;
	if(dynamic_cast<SVGLinearGradientElementImpl *>(pserver))
		result = new AggLinearGradient(dynamic_cast<SVGLinearGradientElementImpl *>(pserver));
	else if(dynamic_cast<SVGRadialGradientElementImpl *>(pserver))
		result = new AggRadialGradient(dynamic_cast<SVGRadialGradientElementImpl *>(pserver));
	else if(dynamic_cast<SVGPatternElementImpl *>(pserver))
		result = new AggPattern(dynamic_cast<SVGPatternElementImpl *>(pserver));
	return result;
}

