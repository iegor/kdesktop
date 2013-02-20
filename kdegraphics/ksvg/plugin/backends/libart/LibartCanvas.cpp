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

#include "LibartCanvas.h"
#include "SVGMatrixImpl.h"
#include "SVGRectImpl.h"
#include "SVGPaintImpl.h"
#include "SVGShapeImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGStringListImpl.h"
#include "SVGPatternElementImpl.h"
#include "SVGGradientElementImpl.h"
#include "SVGLinearGradientElementImpl.h"
#include "SVGRadialGradientElementImpl.h"
#include "SVGClipPathElementImpl.h"
#include "SVGTextPositioningElementImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedLengthListImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGMarkerElementImpl.h"
#include "SVGMaskElementImpl.h"

#include <kdebug.h>
#include <kglobal.h>
#include <kgenericfactory.h>

#include "SVGPaint.h"

#include <qdatetime.h>
#include <qstring.h>
#include <qimage.h>

#include "KSVGHelper.h"
#include "KSVGTextChunk.h"
#include "LibartCanvasItems.h"

#include <libart_lgpl/art_rgb.h>
#include <libart_lgpl/art_affine.h>
#include <libart_lgpl/art_alphagamma.h>
#include <libart_lgpl/art_rgb_svp.h>
#include <libart_lgpl/art_svp.h>
#include <libart_lgpl/art_svp_ops.h>
#include <libart_lgpl/art_svp_intersect.h>
#include <libart_lgpl/art_rect_svp.h>
#include <libart_lgpl/art_svp_vpath.h>

#include <libs/art_support/art_misc.h>
#include <libs/art_support/art_rgba_svp.h>

#include <Font.h>
#include "BezierPathLibart.h"
#include "GlyphTracerLibart.h"

#include <fontconfig/fontconfig.h>

ArtSVP *art_svp_from_rect(int x0, int y0, int x1, int y1)
{
	ArtVpath vpath[] =
	{
		{ ART_MOVETO, x0, y0 },
		{ ART_LINETO, x0, y1 },
		{ ART_LINETO, x1, y1 },
		{ ART_LINETO, x1, y0 },
		{ ART_LINETO, x0, y0 },
		{ ART_END, 0, 0}
	};

	return art_svp_from_vpath(vpath);
} 

ArtSVP *art_svp_from_irect(ArtIRect *bbox)
{
	return art_svp_from_rect(bbox->x0, bbox->y0, bbox->x1, bbox->y1);
} 

ArtSVP *art_svp_from_qrect(const QRect& rect)
{
	return art_svp_from_rect(rect.left(), rect.top(), rect.right() + 1, rect.bottom() + 1);
} 

using namespace KSVG;

LibartCanvas::LibartCanvas(unsigned int width, unsigned int height) : KSVGCanvas(width, height)
{
	m_fontContext = new T2P::Converter(new T2P::GlyphTracerLibart());
}

T2P::BezierPath *LibartCanvas::toBezierPath(CanvasItem *item) const
{
	LibartPath *path = dynamic_cast<LibartPath *>(item);
	if(!path)
		return 0;

	// Only handle LibartPath items
	//T2P::BezierPathLibart *result = new T2P::BezierPathLibart(path->m_array.data());
	return path;
}

// drawing primitives
CanvasItem *LibartCanvas::createRectangle(SVGRectElementImpl *rect)
{
	return new LibartRectangle(this, rect);
}

CanvasItem *LibartCanvas::createEllipse(SVGEllipseElementImpl *ellipse)
{
	return new LibartEllipse(this, ellipse);
}

CanvasItem *LibartCanvas::createCircle(SVGCircleElementImpl *circle)
{
	return new LibartCircle(this, circle);
}

CanvasItem *LibartCanvas::createLine(SVGLineElementImpl *line)
{
	return new LibartLine(this, line);
}

CanvasItem *LibartCanvas::createPolyline(SVGPolylineElementImpl *poly)
{
	return new LibartPolyline(this, poly);
}

CanvasItem *LibartCanvas::createPolygon(SVGPolygonElementImpl *poly)
{
	return new LibartPolygon(this, poly);
}

CanvasItem *LibartCanvas::createPath(SVGPathElementImpl *path)
{
	return new LibartPath(this, path);
}

CanvasItem *LibartCanvas::createClipPath(SVGClipPathElementImpl *clippath)
{
	CanvasClipPath *result = new LibartClipPath(this, clippath);
	QString index = clippath->id().string();
	m_clipPaths.insert(index, result);
	return result;
}

CanvasItem *LibartCanvas::createImage(SVGImageElementImpl *image)
{
	return new LibartImage(this, image);
}

CanvasItem *LibartCanvas::createCanvasMarker(SVGMarkerElementImpl *marker)
{
	return new LibartMarker(this, marker);
}

CanvasItem *LibartCanvas::createText(SVGTextElementImpl *text)
{
	return new LibartText(this, text);
}

CanvasPaintServer *LibartCanvas::createPaintServer(SVGElementImpl *pserver)
{
	LibartPaintServer *result;
	if(dynamic_cast<SVGLinearGradientElementImpl *>(pserver))
		result = new LibartLinearGradient(dynamic_cast<SVGLinearGradientElementImpl *>(pserver));
	else if(dynamic_cast<SVGRadialGradientElementImpl *>(pserver))
		result = new LibartRadialGradient(dynamic_cast<SVGRadialGradientElementImpl *>(pserver));
	else if(dynamic_cast<SVGPatternElementImpl *>(pserver))
		result = new LibartPattern(dynamic_cast<SVGPatternElementImpl *>(pserver));
	return result;
}

void LibartCanvas::drawImage(QImage image, SVGStylableImpl *style, const SVGMatrixImpl *matrix, const KSVGPolygon& clippingPolygon)
{
	SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(style);

	if(shape)
	{
		if(image.depth() != 32)
			image = image.convertDepth(32);

		ArtSVP *imageBorder = svpFromPolygon(clippingPolygon);
		ArtSVP *clipSvp = clipSingleSVP(imageBorder, shape);

		ArtDRect bbox;
		art_drect_svp(&bbox, clipSvp);

		// clamp to viewport
		int x0 = int(bbox.x0);
		int y0 = int(bbox.y0);

		// Use inclusive coords for x1/y1 for clipToBuffer
		int x1 = int(ceil(bbox.x1)) - 1;
		int y1 = int(ceil(bbox.y1)) - 1;

		if(x0 < int(m_width) && y0 < int(m_height) && x1 >= 0 && y1 >= 0)
		{
			clipToBuffer(x0, y0, x1, y1);

			QRect screenBBox(x0, y0, x1 - x0 + 1, y1 - y0 + 1);

			QByteArray mask = SVGMaskElementImpl::maskRectangle(shape, screenBBox);

			double affine[6];
			KSVGHelper::matrixToAffine(matrix, affine);

			ksvg_art_rgb_affine_clip(clipSvp, m_buffer + x0 * nrChannels() + y0 * rowStride(), x0, y0, x1 + 1, y1 + 1, rowStride(), nrChannels(), image.bits(), image.width(), image.height(), image.width() * 4, affine, int(style->getOpacity() * 255), (const art_u8 *)mask.data());
		}

		art_svp_free(imageBorder);
		art_svp_free(clipSvp);
	}
}

ArtSVP *LibartCanvas::clippingRect(const QRect &rect, const SVGMatrixImpl *ctm)
{
	ArtVpath *vec = allocVPath(6);
	// Order of points in clipping rectangle must be counter-clockwise
	bool flip = ((ctm->a() * ctm->d()) < (ctm->b() * ctm->c()));

	vec[0].code = ART_MOVETO;
	vec[0].x = rect.x();
	vec[0].y = rect.y();

	vec[1].code = ART_LINETO;
	vec[1].x = rect.x() + (flip ? rect.width() : 0);
	vec[1].y = rect.y() + (flip ? 0 : rect.height());

	vec[2].code = ART_LINETO;
	vec[2].x = rect.x() + rect.width();
	vec[2].y = rect.y() + rect.height();

	vec[3].code = ART_LINETO;
	vec[3].x = rect.x() + (flip ? 0 : rect.width());
	vec[3].y = rect.y() + (flip ? rect.height() : 0);

	vec[4].code = ART_LINETO;
	vec[4].x = rect.x();
	vec[4].y = rect.y();

	vec[5].code = ART_END;

	double affine[6];
	KSVGHelper::matrixToAffine(ctm, affine);

	ArtVpath *temp = art_vpath_affine_transform(vec, affine);
	art_free(vec);

	ArtSVP *result = art_svp_from_vpath(temp);
	art_free(temp);
	return result;
}

ArtSVP *LibartCanvas::clipSingleSVP(ArtSVP *svp, SVGShapeImpl *shape)
{
	ArtSVP *clippedSvp = copy_svp(svp);
	SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(shape);

	if(style)
	{
		QString clipPathRef = style->getClipPath();

		if(!clipPathRef.isEmpty())
		{
			CanvasClipPath *clipPath = m_clipPaths[clipPathRef];

			if(clipPath)
			{
				LibartClipPath *lclip = dynamic_cast<LibartClipPath *>(clipPath);
				reinterpret_cast<SVGClipPathElementImpl *>(clipPath->element())->setBBoxTarget(shape);

				lclip->init();

				if(lclip->clipSVP())
				{
					ArtSVP *s = art_svp_intersect(lclip->clipSVP(), clippedSvp);
					art_svp_free(clippedSvp);
					clippedSvp = s;
				}
			}
		}
	}

	SVGSVGElementImpl *svg = dynamic_cast<SVGSVGElementImpl *>(shape);

	// Clip outer svg, unless width and height not set
	if(svg && (!svg->isRootElement() || !svg->getAttribute("width").isEmpty() || !svg->getAttribute("height").isEmpty()) && !svg->getOverflow())
	{
		ArtSVP *svgClip = clippingRect(svg->clip(), svg->screenCTM());
		ArtSVP *s = art_svp_intersect(svgClip, clippedSvp);
		art_svp_free(clippedSvp);
		art_svp_free(svgClip);
		clippedSvp = s;
	}

	if(dynamic_cast<SVGPatternElementImpl *>(shape) != 0)
	{
		// TODO: inherit clipping paths into tile space
	}
	else if(dynamic_cast<SVGMarkerElementImpl *>(shape) != 0)
	{
		SVGMarkerElementImpl *marker = static_cast<SVGMarkerElementImpl *>(shape);

		if(!marker->clipShape().isEmpty())
		{
			ArtSVP *clipShape = svpFromPolygon(marker->clipShape());
			ArtSVP *s = art_svp_intersect(clipShape, clippedSvp);
			art_svp_free(clipShape);
			art_svp_free(clippedSvp);
			clippedSvp = s;
		}

		// TODO: inherit clipping paths into marker space
	}
	else
	{
		SVGElementImpl *element = dynamic_cast<SVGElementImpl *>(shape);
		DOM::Node parentNode = element->parentNode();

		if(!parentNode.isNull())
		{
			SVGElementImpl *parent = element->ownerDoc()->getElementFromHandle(parentNode.handle());

			if(parent)
			{
				SVGShapeImpl *parentShape = dynamic_cast<SVGShapeImpl *>(parent);

				if(parentShape)
				{
					// Clip against ancestor clipping paths
					ArtSVP *parentClippedSvp = clipSingleSVP(clippedSvp, parentShape);
					art_svp_free(clippedSvp);
					clippedSvp = parentClippedSvp;
				}
			}
		}
	}

	return clippedSvp;
}

void LibartCanvas::drawSVP(ArtSVP *svp, art_u32 color, QByteArray mask, QRect screenBBox)
{
	int x0 = screenBBox.left();
	int y0 = screenBBox.top();
	int x1 = screenBBox.right();
	int y1 = screenBBox.bottom();

	if(m_nrChannels == 3)
	{
		if(mask.data())
			art_ksvg_rgb_svp_alpha_mask(svp, x0, y0, x1 + 1, y1 + 1, color, m_buffer + x0 * 3 + y0 * 3 * m_width, m_width * 3, 0, (art_u8 *)mask.data());
		else
			art_rgb_svp_alpha(svp, x0, y0, x1 + 1, y1 + 1, color, m_buffer + x0 * 3 + y0 * 3 * m_width, m_width * 3, 0);
	}
	else
		art_ksvg_rgba_svp_alpha(svp, x0, y0, x1 + 1, y1 + 1, color, m_buffer + x0 * 4 + y0 * 4 * m_width, m_width * 4, 0, (art_u8 *)mask.data());
}

ArtSVP *LibartCanvas::copy_svp(const ArtSVP *svp)
{
	// No API to copy an SVP so do so without accessing the data structure directly.
	ArtVpath *vec = allocVPath(1);

	vec[0].code = ART_END;

	ArtSVP *empty = art_svp_from_vpath(vec);
	art_free(vec);

	ArtSVP *result = art_svp_union(empty, svp);
	art_svp_free(empty);

	return result;
}

ArtSVP *LibartCanvas::svpFromPolygon(const KSVGPolygon& polygon)
{
	if(polygon.numPoints() > 2)
	{
		ArtVpath *points = new ArtVpath[polygon.numPoints() + 2];

		points[0].code = ART_MOVETO;
		points[0].x = polygon.point(0).x();
		points[0].y = polygon.point(0).y();

		unsigned int i;

		for(i = 1; i < polygon.numPoints(); i++)
		{
			points[i].code = ART_LINETO;
			points[i].x = polygon.point(i).x();
			points[i].y = polygon.point(i).y();
		}

		points[i].code = ART_LINETO;
		points[i].x = polygon.point(0).x();
		points[i].y = polygon.point(0).y();

		points[i + 1].code = ART_END;

		ArtSVP *svp = art_svp_from_vpath(points);
		delete [] points;

		return svp;
	}
	else
		return 0;
} 

// vim:ts=4:noet
