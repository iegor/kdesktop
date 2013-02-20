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

#include <cfloat>

#include <qimage.h>
#include <qwmatrix.h>

#include "SVGPaint.h"
#include "SVGRectImpl.h"
#include "SVGAngleImpl.h"
#include "SVGPaintImpl.h"
#include "SVGUnitTypes.h"
#include "SVGHelperImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGPointListImpl.h"
#include "SVGMarkerElement.h"
#include "SVGMarkerElementImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGPathSegListImpl.h"
#include "SVGAnimatedRectImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGImageElementImpl.h"
#include "SVGAnimatedAngleImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGPolygonElementImpl.h"
#include "SVGClipPathElementImpl.h"
#include "SVGPolylineElementImpl.h"
#include "SVGAnimatedLengthListImpl.h"
#include "SVGAnimatedNumberImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGPreserveAspectRatioImpl.h"
#include "SVGAnimatedPreserveAspectRatioImpl.h"
#include "SVGGradientElementImpl.h"
#include "SVGGradientElement.h"
#include "SVGLinearGradientElementImpl.h"
#include "SVGRadialGradientElementImpl.h"
#include "SVGPatternElementImpl.h"
#include "SVGPatternElement.h"
#include "SVGStopElementImpl.h"
#include "SVGStylableImpl.h"
#include "SVGAnimatedTransformListImpl.h"
#include "SVGTransformListImpl.h"
#include "SVGUnitConverter.h"
#include "SVGTextPathElementImpl.h"
#include "SVGMaskElementImpl.h"

#include "KSVGHelper.h"
#include "LibartCanvasItems.h"
#include "KSVGTextChunk.h"

#include "art_misc.h"
#include "art_render_misc.h"
#include "BezierPathLibart.h"
#include "Point.h"

#include <dom/dom_node.h>


#include <libart_lgpl/art_vpath.h>
#include <libart_lgpl/art_bpath.h>
#include <libart_lgpl/art_affine.h>
#include <libart_lgpl/art_svp_ops.h>
#include <libart_lgpl/art_svp_point.h>
#include <libart_lgpl/art_vpath_svp.h>
#include <libart_lgpl/art_svp_intersect.h>
#include <libart_lgpl/art_svp_vpath.h>
#include <libart_lgpl/art_svp_vpath_stroke.h>
#include <libart_lgpl/art_rect_svp.h>
#include <libart_lgpl/art_vpath_dash.h>
#include <libart_lgpl/art_render.h>
#include <libart_lgpl/art_rect_svp.h>
#include <libart_lgpl/art_render_gradient.h>
#include <libart_lgpl/art_render_svp.h>
#include <libart_lgpl/art_render_mask.h>

using namespace KSVG;

LibartShape::LibartShape(LibartCanvas *c, SVGStylableImpl *style) : m_canvas(c), m_style(style)
{
	m_fillSVP = 0;
	m_strokeSVP = 0;
	m_fillPainter = 0;
	m_strokePainter = 0;
}

LibartShape::~LibartShape()
{
	freeSVPs();
	delete m_fillPainter;
	delete m_strokePainter;
}

QRect LibartShape::bbox() const
{
	QRect rect;
	if(m_strokeSVP || m_fillSVP)
	{
		ArtIRect *irect = new ArtIRect();
		ArtVpath *vpath = art_vpath_from_svp(m_strokeSVP ? m_strokeSVP : m_fillSVP);
		art_vpath_bbox_irect(vpath, irect);
		art_free(vpath);

		rect.setX(irect->x0);
		rect.setY(irect->y0);
		rect.setWidth(irect->x1 - irect->x0);
		rect.setHeight(irect->y1 - irect->y0);

		delete irect;
	}

	return rect;
}

bool LibartShape::isVisible(SVGShapeImpl *shape)
{
	return m_referenced || (m_style->getVisible() && m_style->getDisplay() && shape->directRender());
}

bool LibartShape::fillContains(const QPoint &p)
{
	if(m_fillSVP)
		return art_svp_point_wind(m_fillSVP, p.x(), p.y()) != 0;
	else
		return false;
}

bool LibartShape::strokeContains(const QPoint &p)
{
	if(m_strokeSVP)
		return art_svp_point_wind(m_strokeSVP, p.x(), p.y()) != 0;
	else
		return false;
}

void LibartShape::update(CanvasItemUpdate reason, int param1, int param2)
{
	if(reason == UPDATE_STYLE)
	{
		if(!m_fillPainter || !m_strokePainter)
			LibartShape::init();
		if(m_fillPainter)
			m_fillPainter->update(m_style);
		if(m_strokePainter)
			m_strokePainter->update(m_style);
		m_canvas->invalidate(this, false);
	}
	else if(reason == UPDATE_TRANSFORM)
	{
		reset();
		m_canvas->invalidate(this, true);
	}
	else if(reason == UPDATE_ZOOM)
		reset();
	else if(reason == UPDATE_PAN)
	{
		if(m_fillSVP)
			ksvg_art_svp_move(m_fillSVP, param1, param2);
		if(m_strokeSVP)
			ksvg_art_svp_move(m_strokeSVP, param1, param2);
	}
	else if(reason == UPDATE_LINEWIDTH)
	{
		if(m_strokeSVP)
		{
			art_svp_free(m_strokeSVP);
			m_strokeSVP = 0;
		}
		init();
		m_canvas->invalidate(this, true);
	}
}

void LibartShape::draw(SVGShapeImpl *shape)
{
	if(!m_referenced && (!m_style->getVisible() || !m_style->getDisplay() || !shape->directRender()))
		return;

	bool fillOk = m_fillSVP && m_style->isFilled();
	bool strokeOk = m_strokeSVP && m_style->isStroked() && m_style->getStrokeWidth()->baseVal()->value() > 0; // Spec: A zero value causes no stroke to be painted.

	if(fillOk || strokeOk)
	{
		if(m_fillPainter && m_fillSVP)
			m_fillPainter->draw(m_canvas, m_fillSVP, m_style, shape);

		if(m_strokePainter && m_strokeSVP)
			m_strokePainter->draw(m_canvas, m_strokeSVP, m_style, shape);
	}
}

void LibartShape::init(const SVGMatrixImpl *)
{
}

void LibartPainter::update(SVGStylableImpl *style)
{
	if(paintType(style) != SVG_PAINTTYPE_URI)
	{
		QColor qcolor;
		if(paintType(style) == SVG_PAINTTYPE_CURRENTCOLOR)
			qcolor = style->getColor()->rgbColor().color();
		else
			qcolor = color(style);

		short _opacity = static_cast<short>(opacity(style) * 255 + 0.5);

		// Spec: clamping
		_opacity = _opacity < 0 ? 0 : _opacity;
		_opacity = _opacity > 255 ? 255 : _opacity;

		m_color = KSVGHelper::toArtColor(qcolor, _opacity);
	}
}

void LibartPainter::draw(LibartCanvas *canvas, _ArtSVP *svp, SVGStylableImpl *style, SVGShapeImpl *shape)
{
	ArtSVP *clippedSvp = canvas->clipSingleSVP(svp, shape);

	// Clipping
	ArtDRect bbox;
	art_drect_svp(&bbox, clippedSvp);

	// clamp to viewport
	int x0 = int(bbox.x0);
	int y0 = int(bbox.y0);

	// Use inclusive coords for x1/y1 for clipToBuffer
	int x1 = int(ceil(bbox.x1)) - 1;
	int y1 = int(ceil(bbox.y1)) - 1;

	if(x0 < int(canvas->width()) && y0 < int(canvas->height()) && x1 > -1 && y1 > -1)
	{
		canvas->clipToBuffer(x0, y0, x1, y1);

		QRect screenBBox(x0, y0, x1 - x0 + 1, y1 - y0 + 1);

		QByteArray mask = SVGMaskElementImpl::maskRectangle(shape, screenBBox);

		if(paintType(style) == SVG_PAINTTYPE_URI)
		{
			LibartPaintServer *pserver = static_cast<LibartPaintServer *>(SVGPaintServerImpl::paintServer(shape->ownerDoc(), paintUri(style)));

			if(pserver)
			{
				pserver->setBBoxTarget(shape);
				if(!pserver->finalized())
					pserver->finalizePaintServer();
				pserver->render(canvas, clippedSvp, opacity(style), mask, screenBBox);
			}
		}
		else
			canvas->drawSVP(clippedSvp, m_color, mask, screenBBox);
	}

	art_svp_free(clippedSvp);
}

LibartStrokePainter::LibartStrokePainter(SVGStylableImpl *style)
{
	update(style);
}

LibartFillPainter::LibartFillPainter(SVGStylableImpl *style)
{
	update(style);
}

void LibartShape::init()
{
	if(m_style->isFilled())
	{
		if(m_fillPainter == 0)
			m_fillPainter = new LibartFillPainter(m_style);
	}
	else
	{
		delete m_fillPainter;
		m_fillPainter = 0;
	}

	// Spec: A zero value causes no stroke to be painted.
	if(m_style->isStroked() && m_style->getStrokeWidth()->baseVal()->value() > 0)
	{
		if(m_strokePainter == 0)
			m_strokePainter = new LibartStrokePainter(m_style);
	}
	else
	{
		delete m_strokePainter;
		m_strokePainter = 0;
	}
}

void LibartShape::initClipItem()
{
	init();
}

ArtSVP *LibartShape::clipSVP()
{
	return m_fillSVP;
}

void LibartShape::freeSVPs()
{
	if(m_fillSVP)
		art_svp_free(m_fillSVP);
	if(m_strokeSVP)
		art_svp_free(m_strokeSVP);

	m_fillSVP = 0;
	m_strokeSVP = 0;
}

void LibartShape::calcClipSVP(ArtVpath *vec, SVGStylableImpl *style, const SVGMatrixImpl *matrix, _ArtSVP **clipSVP)
{
	double affine[6];
	KSVGHelper::matrixToAffine(matrix, affine);

	if(!style)
	{
		art_free(vec);
		return;
	}

	ArtVpath *vtemp = art_vpath_affine_transform(vec, affine);
	art_free(vec);
	vec = vtemp;

	ArtSVP *temp;
	ArtSvpWriter *swr;
	temp = art_svp_from_vpath(vec);

	if(style->getClipRule() == RULE_EVENODD)
		swr = art_svp_writer_rewind_new(ART_WIND_RULE_ODDEVEN);
	else
		swr = art_svp_writer_rewind_new(ART_WIND_RULE_NONZERO);

	art_svp_intersector(temp, swr);
	*clipSVP = art_svp_writer_rewind_reap(swr);

	art_svp_free(temp);
	art_free(vec);
}

void LibartShape::calcSVPs(ArtVpath *vec, SVGStylableImpl *style, const SVGMatrixImpl *matrix, ArtSVP **strokeSVP, ArtSVP **fillSVP)
{
	if(style)
	{
		double affine[6];
		KSVGHelper::matrixToAffine(matrix, affine);

		ArtVpath *temp = art_vpath_affine_transform(vec, affine);
		art_free(vec);
		vec = temp;
		calcSVPInternal(vec, style, affine, strokeSVP, fillSVP);
	}
	else
		art_free(vec);
}

void LibartShape::calcSVPs(ArtBpath *bpath, SVGStylableImpl *style, const SVGMatrixImpl *matrix, ArtSVP **strokeSVP, ArtSVP **fillSVP)
{
	if(style)
	{
		double affine[6];
		KSVGHelper::matrixToAffine(matrix, affine);

		ArtBpath *temp = art_bpath_affine_transform(bpath, affine);
		ArtVpath *vec = ksvg_art_bez_path_to_vec(temp, 0.25);
		art_free(temp);
		calcSVPInternal(vec, style, affine, strokeSVP, fillSVP);
	}
}

void LibartShape::calcSVPInternal(ArtVpath *vec, SVGStylableImpl *style, double *affine, ArtSVP **strokeSVP, ArtSVP **fillSVP)
{
	ArtSVP *svp;

	// Filling
	{
		ArtSvpWriter *swr;
		ArtSVP *temp;
		temp = art_svp_from_vpath(vec);

		if(style->getFillRule() == RULE_EVENODD)
			swr = art_svp_writer_rewind_new(ART_WIND_RULE_ODDEVEN);
		else
			swr = art_svp_writer_rewind_new(ART_WIND_RULE_NONZERO);

		art_svp_intersector(temp, swr);
		svp = art_svp_writer_rewind_reap(swr);

		*fillSVP = svp;
		art_svp_free(temp);
	}

	// Stroking
	if(style->isStroked() || style->getStrokeColor()->paintType() == SVG_PAINTTYPE_URI)
	{
		double ratio = art_affine_expansion(affine);

		unsigned int dashLength;
		if(style->getDashArray() && (dashLength = style->getDashArray()->baseVal()->numberOfItems()) > 0)
		{
			// HACK: libart will hang in art_vpath_dash() if passed an array with only zeroes.
			bool allZeroes = true;

			// there are dashes to be rendered
			ArtVpathDash dash;
			dash.offset = int(style->getDashOffset()->baseVal()->value()) * ratio;
			dash.n_dash = dashLength;
			double *dashes = new double[dashLength];
			for(unsigned int i = 0; i < dashLength; i++)
			{
				dashes[i] = style->getDashArray()->baseVal()->getItem(i)->value() * ratio;
				if(dashes[i] != 0.0)
					allZeroes = false;
			}
			dash.dash = dashes;

			if(!allZeroes)
			{
				// get the dashed VPath and use that for the stroke render operation
				ArtVpath *vec2 = art_vpath_dash(vec, &dash);
				art_free(vec);
				vec = vec2;
			}

			// reset the dashes
			delete [] dashes;
		}

		double penWidth = style->getStrokeWidth()->baseVal()->value() * ratio;
		svp = art_svp_vpath_stroke(vec, (ArtPathStrokeJoinType)style->getJoinStyle(), (ArtPathStrokeCapType)style->getCapStyle(), penWidth, style->getStrokeMiterlimit(), 0.25);

		*strokeSVP = svp;
	}
	art_free(vec);
}

// #####

LibartRectangle::LibartRectangle(LibartCanvas *c, SVGRectElementImpl *rect)
: LibartShape(c, rect), m_rect(rect)
{
	init();
}

void LibartRectangle::draw()
{
	if(isVisible())
		LibartShape::draw(m_rect);
}

bool LibartRectangle::isVisible()
{
	// Spec: a value of zero disables rendering
	return LibartShape::isVisible(m_rect) && m_rect->width()->baseVal()->value() > 0 && m_rect->height()->baseVal()->value() > 0;
}

void LibartRectangle::init()
{
	init(m_rect->screenCTM());
}

void LibartRectangle::init(const SVGMatrixImpl *screenCTM)
{
	LibartShape::init();
	double x = m_rect->x()->baseVal()->value();
	double y = m_rect->y()->baseVal()->value();
	double width = m_rect->width()->baseVal()->value();
	double height = m_rect->height()->baseVal()->value();
	double rx = m_rect->rx()->baseVal()->value();
	double ry = m_rect->ry()->baseVal()->value();

	// Spec: If there is no rx or ry specified, draw a normal rect
	if(rx == -1 && ry == -1)
	{
		ArtVpath *vec = allocVPath(6);

		vec[0].code = ART_MOVETO;
		vec[0].x = x;
		vec[0].y = y;

		vec[1].code = ART_LINETO;
		vec[1].x = x;
		vec[1].y = y + height;

		vec[2].code = ART_LINETO;
		vec[2].x = x + width;
		vec[2].y = y + height;

		vec[3].code = ART_LINETO;
		vec[3].x = x + width;
		vec[3].y = y;

		vec[4].code = ART_LINETO;
		vec[4].x = x;
		vec[4].y = y;

		vec[5].code = ART_END;

		if(m_context == NORMAL)
			calcSVPs(vec, m_rect, screenCTM, &m_strokeSVP, &m_fillSVP);
		else
			calcClipSVP(vec, m_rect, screenCTM, &m_fillSVP);
	}
	else
	{
		ArtVpath *res;
		ArtBpath *vec = allocBPath(10);

		int i = 0;

		// Spec: If rx isn't specified, but ry, set rx to ry
		if(rx == -1)
			rx = ry;

		// Spec: If ry isn't specified, but rx, set ry to rx
		if(ry == -1)
			ry = rx;

		// Spec: If rx is greater than half of the width of the rectangle
		//       then set rx to half of the width
		if(rx > width / 2)
			rx = width / 2;

		// Spec: If ry is greater than half of the height of the rectangle
		//       then set ry to half of the height
		if(ry > height / 2)
			ry = height / 2;

		vec[i].code = ART_MOVETO_OPEN;
		vec[i].x3 = x + rx;
		vec[i].y3 = y;

		i++;

		vec[i].code = ART_CURVETO;
		vec[i].x1 = x + rx * (1 - 0.552);
		vec[i].y1 = y;
		vec[i].x2 = x;
		vec[i].y2 = y + ry * (1 - 0.552);
		vec[i].x3 = x;
		vec[i].y3 = y + ry;

		i++;

		if(ry < height / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x;
			vec[i].y3 = y + height - ry;

			i++;
		}
		vec[i].code = ART_CURVETO;
		vec[i].x1 = x;
		vec[i].y1 = y + height - ry * (1 - 0.552);
		vec[i].x2 = x + rx * (1 - 0.552);
		vec[i].y2 = y + height;
		vec[i].x3 = x + rx;
		vec[i].y3 = y + height;

		i++;

		if(rx < width / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x + width - rx;
			vec[i].y3 = y + height;

			i++;
		}

		vec[i].code = ART_CURVETO;
		vec[i].x1 = x + width - rx * (1 - 0.552);
		vec[i].y1 = y + height;
		vec[i].x2 = x + width;
		vec[i].y2 = y + height - ry * (1 - 0.552);
		vec[i].x3 = x + width;

		vec[i].y3 = y + height - ry;

		i++;

		if(ry < height / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x + width;
			vec[i].y3 = y + ry;

			i++;
		}
		vec[i].code = ART_CURVETO;
		vec[i].x1 = x + width;
		vec[i].y1 = y + ry * (1 - 0.552);
		vec[i].x2 = x + width - rx * (1 - 0.552);
		vec[i].y2 = y;
		vec[i].x3 = x + width - rx;
		vec[i].y3 = y;

		i++;

		if(rx < width / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x + rx;
			vec[i].y3 = y;

			i++;
		}

		vec[i].code = ART_END;

		res = ksvg_art_bez_path_to_vec(vec, 0.25);
		if(m_context == NORMAL)
			calcSVPs(res, m_rect, screenCTM, &m_strokeSVP, &m_fillSVP);
		else
			calcClipSVP(res, m_rect, screenCTM, &m_fillSVP);
		art_free(vec);
	 }
}

// #####

LibartEllipse::LibartEllipse(LibartCanvas *c, SVGEllipseElementImpl *ellipse)
: LibartShape(c, ellipse), m_ellipse(ellipse)
{
	init();
}
	
void LibartEllipse::draw()
{
	if(isVisible())
		LibartShape::draw(m_ellipse);
}

bool LibartEllipse::isVisible()
{
	// Spec: dont render when rx and/or ry is zero
	return LibartShape::isVisible(m_ellipse) && m_ellipse->rx()->baseVal()->value() > 0 && m_ellipse->ry()->baseVal()->value() > 0;
}

void LibartEllipse::init()
{
	init(m_ellipse->screenCTM());
}

void LibartEllipse::init(const SVGMatrixImpl *screenCTM)
{
	LibartShape::init();
	ArtBpath *temp = allocBPath(6);

	double x1, y1, x2, y2, x3, y3;
	double len = 0.55228474983079356;
	double rx = m_ellipse->rx()->baseVal()->value();
	double ry = m_ellipse->ry()->baseVal()->value();
	double cx = m_ellipse->cx()->baseVal()->value();
	double cy = m_ellipse->cy()->baseVal()->value();
	double cos4[] = {1.0, 0.0, -1.0, 0.0, 1.0};
	double sin4[] = {0.0, 1.0, 0.0, -1.0, 0.0};
	int i = 0;

	temp[i].code = ART_MOVETO;
	temp[i].x3 = cx + rx;
	temp[i].y3 = cy;

	i++;

	while(i < 5)
	{
		x1 = cos4[i-1] + len * cos4[i];
		y1 = sin4[i-1] + len * sin4[i];
		x2 = cos4[i] + len * cos4[i-1];
		y2 = sin4[i] + len * sin4[i-1];
		x3 = cos4[i];
		y3 = sin4[i];

		temp[i].code = ART_CURVETO;
		temp[i].x1 = cx + x1 * rx;
		temp[i].y1 = cy + y1 * ry;
		temp[i].x2 = cx + x2 * rx;
		temp[i].y2 = cy + y2 * ry;
		temp[i].x3 = cx + x3 * rx;
		temp[i].y3 = cy + y3 * ry;

		i++;
	}

	temp[i].code = ART_END;

	if(m_context == NORMAL)
		calcSVPs(temp, m_ellipse, screenCTM, &m_strokeSVP, &m_fillSVP);
	else
		calcClipSVP(ksvg_art_bez_path_to_vec(temp, 0.25), m_ellipse, screenCTM, &m_fillSVP);
	art_free(temp);
}

// #####

LibartCircle::LibartCircle(LibartCanvas *c, SVGCircleElementImpl *circle)
: LibartShape(c, circle), m_circle(circle)
{
	init();
}

void LibartCircle::draw()
{
	// Spec: a value of zero disables rendering
	if(isVisible())
		LibartShape::draw(m_circle);
}

bool LibartCircle::isVisible()
{
	// Spec: dont render when rx and/or ry is zero
	return LibartShape::isVisible(m_circle) && m_circle->r()->baseVal()->value() > 0;
}

void LibartCircle::init()
{
	init(m_circle->screenCTM());
}

void LibartCircle::init(const SVGMatrixImpl *screenCTM)
{
	LibartShape::init();
	ArtBpath *temp = allocBPath(6);

	double x1, y1, x2, y2, x3, y3;
	double len = 0.55228474983079356;
	double r = m_circle->r()->baseVal()->value();
	double cx = m_circle->cx()->baseVal()->value();
	double cy = m_circle->cy()->baseVal()->value();
	double cos4[] = {1.0, 0.0, -1.0, 0.0, 1.0};
	double sin4[] = {0.0, 1.0, 0.0, -1.0, 0.0};
	int i = 0;

	temp[i].code = ART_MOVETO;
	temp[i].x3 = cx + r;
	temp[i].y3 = cy;

	i++;

	while(i < 5)
	{
		x1 = cos4[i-1] + len * cos4[i];
		y1 = sin4[i-1] + len * sin4[i];
		x2 = cos4[i] + len * cos4[i-1];
		y2 = sin4[i] + len * sin4[i-1];
		x3 = cos4[i];
		y3 = sin4[i];

		temp[i].code = ART_CURVETO;
		temp[i].x1 = cx + x1 * r;
		temp[i].y1 = cy + y1 * r;
		temp[i].x2 = cx + x2 * r;
		temp[i].y2 = cy + y2 * r;
		temp[i].x3 = cx + x3 * r;
		temp[i].y3 = cy + y3 * r;

		i++;
	}

	temp[i].code = ART_END;

	if(m_context == NORMAL)
		calcSVPs(temp, m_circle, screenCTM, &m_strokeSVP, &m_fillSVP);
	else
		calcClipSVP(ksvg_art_bez_path_to_vec(temp, 0.25), m_circle, screenCTM, &m_fillSVP);
	art_free(temp);
}

// #####

LibartLine::LibartLine(LibartCanvas *c, SVGLineElementImpl *line)
: LibartShape(c, line), MarkerHelper(), m_line(line)
{
	init();
}

LibartLine::~LibartLine()
{
}

void LibartLine::draw()
{
	LibartShape::draw(m_line);

	if(m_line->hasMarkers())
	{
		double x1 = m_line->x1()->baseVal()->value();
		double y1 = m_line->y1()->baseVal()->value();
		double x2 = m_line->x2()->baseVal()->value();
		double y2 = m_line->y2()->baseVal()->value();
		double slope = SVGAngleImpl::todeg(atan2(y2 - y1, x2 - x1));

		if(m_line->hasStartMarker())
			doStartMarker(m_line, m_line, x1, y1, slope);
		if(m_line->hasEndMarker())
			doEndMarker(m_line, m_line, x2, y2, slope);
	}
}

bool LibartLine::isVisible()
{
	return LibartShape::isVisible(m_line);
}

void LibartLine::init()
{
	init(m_line->screenCTM());
}

void LibartLine::init(const SVGMatrixImpl *screenCTM)
{
	LibartShape::init();
	ArtVpath *vec;

	vec = allocVPath(3);

	vec[0].code = ART_MOVETO_OPEN;
	vec[0].x = m_line->x1()->baseVal()->value();
	vec[0].y = m_line->y1()->baseVal()->value();

	vec[1].code = ART_LINETO;
	vec[1].x = m_line->x2()->baseVal()->value();
	vec[1].y = m_line->y2()->baseVal()->value();

	// A subpath consisting of a moveto and lineto to the same exact location or a subpath consisting of a moveto
	// and a closepath will be stroked only if the 'stroke-linecap' property is set to "round", producing a circle
	// centered at the given point.
	if(vec[1].x == vec[0].x && vec[1].y == vec[0].y && m_line->getCapStyle() == PATH_STROKE_CAP_ROUND)
		vec[1].x += .5;

	vec[2].code = ART_END;

	if(m_context == NORMAL)
	{
		calcSVPs(vec, m_line, screenCTM, &m_strokeSVP, &m_fillSVP);
		art_svp_free(m_fillSVP);
		m_fillSVP = 0;
	}
	else
		calcClipSVP(vec, m_line, screenCTM, &m_fillSVP);
}

// #####
LibartPoly::LibartPoly(LibartCanvas *c, SVGPolyElementImpl *poly)
: LibartShape(c, poly), MarkerHelper(), m_poly(poly)
{
}

LibartPoly::~LibartPoly()
{
}

void LibartPoly::init()
{
	init(m_poly->screenCTM());
}

void LibartPoly::draw()
{
	LibartShape::draw(m_poly);

	if(m_poly->hasMarkers())
		m_poly->drawMarkers();
}

bool LibartPoly::isVisible()
{
	return LibartShape::isVisible(m_poly);
}

// #####
LibartPolyline::LibartPolyline(LibartCanvas *c, SVGPolylineElementImpl *poly)
: LibartPoly(c, poly)
{
	LibartPoly::init();
}

LibartPolyline::~LibartPolyline()
{
}

void LibartPolyline::init(const SVGMatrixImpl *screenCTM)
{
	LibartShape::init();
	unsigned int numberOfPoints = m_poly->points()->numberOfItems();

	if(numberOfPoints < 1)
		return;

	ArtVpath *polyline = allocVPath(2 + numberOfPoints);

	polyline[0].code = ART_MOVETO_OPEN;
	polyline[0].x = m_poly->points()->getItem(0)->x();
	polyline[0].y = m_poly->points()->getItem(0)->y();

	unsigned int index;
	for(index = 1; index < numberOfPoints; index++)
	{
		polyline[index].code = ART_LINETO;
		polyline[index].x = m_poly->points()->getItem(index)->x();
		polyline[index].y = m_poly->points()->getItem(index)->y();
	}

	// A subpath consisting of a moveto and lineto to the same exact location or a subpath consisting of a moveto
	// and a closepath will be stroked only if the 'stroke-linecap' property is set to "round", producing a circle
	// centered at the given point.
	if(numberOfPoints == 2 && polyline[1].x == polyline[0].x && polyline[1].y == polyline[0].y && m_poly->getCapStyle() == PATH_STROKE_CAP_ROUND)
		polyline[1].x += .5;


	if(m_poly->isFilled()) // if the polyline must be filled, inform libart that it should not be closed.
	{
		polyline[index].code = (ArtPathcode) ART_END2;
		polyline[index].x = m_poly->points()->getItem(0)->x();
		polyline[index++].y = m_poly->points()->getItem(0)->y();
	}

	polyline[index].code = ART_END;
	if(m_context == NORMAL)
		calcSVPs(polyline, m_poly, screenCTM, &m_strokeSVP, &m_fillSVP);
	else
		calcClipSVP(polyline, m_poly, screenCTM, &m_fillSVP);
}

// #####

LibartPolygon::LibartPolygon(LibartCanvas *c, SVGPolygonElementImpl *poly)
: LibartPoly(c, poly)
{
	LibartPoly::init();
}

LibartPolygon::~LibartPolygon()
{
}

void LibartPolygon::init(const SVGMatrixImpl *screenCTM)
{
	LibartShape::init();
	unsigned int numberOfPoints = m_poly->points()->numberOfItems();

	if(numberOfPoints < 1)
		return;

	ArtVpath *polygon = allocVPath(2 + numberOfPoints);

	polygon[0].code = ART_MOVETO;
	polygon[0].x = m_poly->points()->getItem(0)->x();
	polygon[0].y = m_poly->points()->getItem(0)->y();

	unsigned int index;
	for(index = 1; index < numberOfPoints; index++)
	{
		polygon[index].code = ART_LINETO;
		polygon[index].x = m_poly->points()->getItem(index)->x();
		polygon[index].y = m_poly->points()->getItem(index)->y();
	}

	polygon[index].code = ART_LINETO;
	polygon[index].x = m_poly->points()->getItem(0)->x();
	polygon[index].y = m_poly->points()->getItem(0)->y();

	index++;
	polygon[index].code = ART_END;

	if(m_context == NORMAL)
		calcSVPs(polygon, m_poly, screenCTM, &m_strokeSVP, &m_fillSVP);
	else
		calcClipSVP(polygon, m_poly, screenCTM, &m_fillSVP);
}

// #####

LibartPath::LibartPath(LibartCanvas *c, SVGPathElementImpl *path)
: LibartShape(c, path), MarkerHelper(), T2P::BezierPathLibart(), ::SVGPathParser(), m_path(path)
{
	reset();
}

LibartPath::~LibartPath()
{
}

void LibartPath::reset()
{
	m_array.resize(0);
	LibartShape::reset();
}

void LibartPath::draw()
{
	LibartShape::draw(m_path);

	if(m_path->hasMarkers())
	{
		SVGPathElementImpl::MarkerData markers = m_path->markerData();
		int numMarkers = markers.numMarkers();

		if(m_path->hasStartMarker())
			doStartMarker(m_path, m_path, markers.marker(0).x, markers.marker(0).y, markers.marker(0).angle);

		for(int i = 1; i < numMarkers - 1; i++)
		{
			if(m_path->hasMidMarker())
				doMidMarker(m_path, m_path, markers.marker(i).x, markers.marker(i).y, markers.marker(i).angle);
		}

		if(m_path->hasEndMarker())
			doEndMarker(m_path, m_path, markers.marker(numMarkers - 1).x, markers.marker(numMarkers - 1).y, markers.marker(numMarkers - 1).angle);
	}
}

bool LibartPath::isVisible()
{
	return LibartShape::isVisible(m_path);
}

void LibartPath::init()
{
	init(m_path->screenCTM());
}

void LibartPath::init(const SVGMatrixImpl *screenCTM)
{
	LibartShape::init();
	if(m_array.count() > 0)
	{
		if(m_context == NORMAL)
			calcSVPs(m_array.data(), m_path, screenCTM, &m_strokeSVP, &m_fillSVP);
		else
			calcClipSVP(ksvg_art_bez_path_to_vec(m_array.data(), 0.25), m_path, screenCTM, &m_fillSVP);
	}
	else if(!m_path->getAttribute("d").string().isEmpty())
	{
		parseSVG(m_path->getAttribute("d").string(), true);

		int index = m_array.count();
		double curx = m_array[index - 1].x3;
		double cury = m_array[index - 1].y3;

		// Find last subpath
		int find = -1;
		for(int i = index - 1; i >= 0; i--)
		{
			if(m_array[i].code == ART_MOVETO_OPEN || m_array[i].code == ART_MOVETO)
			{
				find = i;
				break;
			}
		}

		// Fix a problem where the .svg file used floats as values... (sofico.svg)
		if(curx != m_array[find].x3 && cury != m_array[find].y3)
		{
			if((int) curx == (int) m_array[find].x3 && (int) cury == (int) m_array[find].y3)
			{
				ensureSpace(m_array, index)

				m_array[index].code = ART_LINETO;
				m_array[index].x3 = m_array[find].x3;
				m_array[index].y3 = m_array[find].y3;

				curx = m_array[find].x3;
				cury = m_array[find].y3;

				index++;
			}
		}

		// handle filled paths that are not closed explicitly
		if(m_path->getFillColor()->paintType() != SVG_PAINTTYPE_NONE)
		{
			if((int) curx != (int) m_array[find].x3 || (int) cury != (int) m_array[find].y3)
			{
				ensureSpace(m_array, index)

				m_array[index].code = (ArtPathcode)ART_END2;
				m_array[index].x3 = m_array[find].x3;
				m_array[index].y3 = m_array[find].y3;

				curx = m_array[find].x3;
				cury = m_array[find].y3;

				index++;
			}
		}

		// close
		ensureSpace(m_array, index)

		m_array[index].code = ART_END;

		// A subpath consisting of a moveto and lineto to the same exact location or a subpath consisting of a moveto
		// and a closepath will be stroked only if the 'stroke-linecap' property is set to "round", producing a circle
		// centered at the given point.
		if(index == 2 && m_array[1].code == ART_LINETO && m_array[1].x3 == m_array[0].x3 && m_array[1].y3 == m_array[0].y3 && m_path->getCapStyle() == PATH_STROKE_CAP_ROUND)
			m_array[1].x3 += .5;

		// There are pure-moveto paths which reference paint servers *bah*
		// Do NOT render them
		bool render = false;
		for(int i = index; i >= 0; i--)
		{
			if(m_array[i].code != ART_MOVETO_OPEN && m_array[i].code != ART_MOVETO && !(m_array[i].code >= ART_END))
			{
				render = true;
				break;
			}
		}

		if(render && m_context == NORMAL)
			calcSVPs(m_array.data(), m_path, screenCTM, &m_strokeSVP, &m_fillSVP);
		else
			calcClipSVP(ksvg_art_bez_path_to_vec(m_array.data(), 0.25), m_path, screenCTM, &m_fillSVP);
	}
}

void LibartPath::svgMoveTo(double x1, double y1, bool closed, bool)
{
	int index = m_array.count();

	if(index > 0 && !closed)
	{
		// Find last subpath
		int find = -1;
		for(int i = index - 1; i >= 0; i--)
		{
			if(m_array[i].code == ART_MOVETO_OPEN || m_array[i].code == ART_MOVETO)
			{
				find = i;
				break;
			}
		}
		
		ensureSpace(m_array, index)

		m_array[index].code = (ArtPathcode) ART_END2;
		m_array[index].x3 = m_array[find].x3;
		m_array[index].y3 = m_array[find].y3;

		index++;
	}

	ensureSpace(m_array, index)

	m_array[index].code = (index == 0) ? ART_MOVETO : ART_MOVETO_OPEN;
	m_array[index].x3 = x1;
	m_array[index].y3 = y1;
}

void LibartPath::svgLineTo(double x1, double y1, bool)
{
	int index = m_array.count();

	ensureSpace(m_array, index)

	m_array[index].code = ART_LINETO;
	m_array[index].x3 = x1;
	m_array[index].y3 = y1;
}

void LibartPath::svgCurveToCubic(double x1, double y1, double x2, double y2, double x3, double y3, bool)
{
	int index = m_array.count();

	ensureSpace(m_array, index)

	m_array[index].code = ART_CURVETO;
	m_array[index].x1 = x1;
	m_array[index].y1 = y1;
	m_array[index].x2 = x2;
	m_array[index].y2 = y2;
	m_array[index].x3 = x3;
	m_array[index].y3 = y3;
}

void LibartPath::svgClosePath()
{
	int index = m_array.count();
	double curx = m_array[index - 1].x3;
	double cury = m_array[index - 1].y3;

	int find = -1;
	for(int i = index - 1; i >= 0; i--)
	{
		if(m_array[i].code == ART_MOVETO_OPEN || m_array[i].code == ART_MOVETO)
		{
			find = i;
			break;
		}
	}

	if(find != -1)
	{
		if(m_array[find].x3 != curx || m_array[find].y3 != cury)
		{
			ensureSpace(m_array, index)

			m_array[index].code = ART_LINETO;
			m_array[index].x3 = m_array[find].x3;
			m_array[index].y3 = m_array[find].y3;
		}
	}
}

// #####

LibartClipPath::LibartClipPath(LibartCanvas *c, SVGClipPathElementImpl *clipPath)
: CanvasClipPath(clipPath), m_canvas(c)
{
	m_clipSVP = 0;
	m_clipItems.setAutoDelete(true);
}

LibartClipPath::~LibartClipPath()
{
	if(m_clipSVP)
		art_svp_free(m_clipSVP);
}

void LibartClipPath::update(CanvasItemUpdate, int, int)
{
	if(m_clipSVP)
		art_svp_free(m_clipSVP);
	m_clipSVP = 0;
}

void LibartClipPath::init()
{
	SVGMatrixImpl *clipMatrix = 0;
	
	// Start with referencing element's coordinate system
	SVGLocatableImpl *locatableReferrer = dynamic_cast<SVGLocatableImpl *>(m_clipPath->getBBoxTarget());
	if(locatableReferrer)
		clipMatrix = locatableReferrer->getScreenCTM();
	else
		clipMatrix = SVGSVGElementImpl::createSVGMatrix();

	if(m_clipPath->clipPathUnits()->baseVal() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX && m_clipPath->getBBoxTarget())
	{
		SVGRectImpl *rect = m_clipPath->getBBoxTarget()->getBBox();

		clipMatrix->translate(rect->qrect().x(), rect->qrect().y());
		clipMatrix->scaleNonUniform(rect->qrect().width(), rect->qrect().height());

		rect->deref();
	}

	// Add transformations on the clipPath element itself
	if(m_clipPath->localMatrix())
		clipMatrix->multiply(m_clipPath->localMatrix());

	if(m_clipSVP)
	{
		art_svp_free(m_clipSVP);
		m_clipSVP = 0;
	}

	DOM::Node node = m_clipPath->firstChild();
	for(; !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *element = m_clipPath->ownerDoc()->getElementFromHandle(node.handle());
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(element);
		SVGTestsImpl *tests = dynamic_cast<SVGTestsImpl *>(element);

		bool ok = tests ? tests->ok() : true;

		if(element && shape && ok && !shape->isContainer())
		{
			LibartClipItem *clipElement = dynamic_cast<LibartClipItem *>(shape->item());
			
			if(dynamic_cast<LibartText *>(shape->item()))
			{
				// The cast to a clipElement above is failing when it is valid. But only
				// in the plugin - svgdisplay works fine. What's going on? (Adrian)
				clipElement = dynamic_cast<LibartText *>(shape->item());
			}

			if(clipElement)
			{
				clipElement->setRenderContext(CLIPPING);

				// Push coordinate system down to children.
				SVGLocatableImpl *locatable = dynamic_cast<SVGLocatableImpl *>(shape);
				if(locatable)
					locatable->updateCachedScreenCTM(clipMatrix);

				clipElement->initClipItem();

				ArtSVP *one = clipElement->clipSVP();
				if(!one)
					break;

				if(m_clipSVP == 0)
					m_clipSVP = LibartCanvas::copy_svp(one);
				else
				{
					ArtSVP *svp_union = art_svp_union(m_clipSVP, one);
					art_svp_free(m_clipSVP);
					m_clipSVP = svp_union;
				}
			}
		}
	}

	clipMatrix->deref();
}

void LibartClipPath::draw()
{
}

ArtSVP *LibartClipPath::clipSVP()
{
	return m_clipSVP;
}

// #####

LibartImage::LibartImage(LibartCanvas *c, SVGImageElementImpl *image)
: m_canvas(c), m_image(image)
{
}

LibartImage::~LibartImage()
{
}

void LibartImage::draw()
{
	if(isVisible())
	{
		SVGMatrixImpl *ctm = m_image->scaledImageMatrix();
		QImage image = m_image->scaledImage();
		KSVGPolygon clippingPolygon = m_image->clippingShape();

		m_canvas->drawImage(image, m_image, ctm, clippingPolygon);

		ctm->deref();
	}
}

bool LibartImage::isVisible()
{
	return (m_referenced || (m_image->getVisible() && m_image->getDisplay() && m_image->directRender())) && m_image->image();
}

void LibartImage::init()
{
}

QRect LibartImage::bbox() const
{
	QRect bbox(static_cast<int>(m_image->x()->baseVal()->value()),
			   static_cast<int>(m_image->y()->baseVal()->value()),
			   static_cast<int>(m_image->width()->baseVal()->value()),
			   static_cast<int>(m_image->height()->baseVal()->value()));

	
	return SVGHelperImpl::fromUserspace(m_image, bbox);
}

// #####

LibartMarker::LibartMarker(LibartCanvas *c, SVGMarkerElementImpl *marker)
: CanvasMarker(marker), m_canvas(c)
{
}

LibartMarker::~LibartMarker()
{
}

void LibartMarker::init()
{
}

void LibartMarker::draw()
{
}

// #####

LibartText::LibartText(LibartCanvas *c, SVGTextElementImpl *text)
: CanvasText(text), m_canvas(c)
{
	m_drawFillItems.setAutoDelete(true);
	m_drawStrokeItems.setAutoDelete(true);
	m_fillPainters.setAutoDelete(true);
	m_strokePainters.setAutoDelete(true);

	init();
}

LibartText::~LibartText()
{
	clearSVPs();
}

LibartText::SVPElement::~SVPElement()
{
	if(svp)
		art_svp_free(svp);
}

QRect LibartText::bbox() const
{
	QRect result, rect;

	QPtrListIterator<SVPElement> it1(m_drawFillItems);
	QPtrListIterator<SVPElement> it2(m_drawStrokeItems);

	SVPElement *fill = it1.current(), *stroke = it2.current();
	while(fill != 0 || stroke != 0)
	{
		ArtIRect *irect = new ArtIRect();
		ArtVpath *vpath = art_vpath_from_svp((stroke && stroke->svp) ? stroke->svp : fill->svp);
		art_vpath_bbox_irect(vpath, irect);
		art_free(vpath);

		rect.setX(irect->x0);
		rect.setY(irect->y0);
		rect.setWidth(irect->x1 - irect->x0);
		rect.setHeight(irect->y1 - irect->y0);

		delete irect;

		result = result.unite(rect);

		fill = ++it1;
		stroke = ++it2;
	}
	return result;
}

bool LibartText::fillContains(const QPoint &p)
{
	QPtrListIterator<SVPElement> it(m_drawFillItems);

	SVPElement *fill = it.current();
	while(fill && fill->svp)
	{
		if(fill->svp && art_svp_point_wind(fill->svp, p.x(), p.y()) != 0)
			return true;

		fill = ++it;
	}

	return false;
}

bool LibartText::strokeContains(const QPoint &p)
{
	QPtrListIterator<SVPElement> it(m_drawStrokeItems);

	SVPElement *stroke = it.current();
	while(stroke && stroke->svp)
	{
		if(stroke->svp && art_svp_point_wind(stroke->svp, p.x(), p.y()) != 0)
			return true;

		stroke = ++it;
	}

	return false;
}

void LibartText::update(CanvasItemUpdate reason, int param1, int param2)
{
	if(reason == UPDATE_STYLE)
	{
		QPtrListIterator<SVPElement> it1(m_drawFillItems);
		QPtrListIterator<SVPElement> it2(m_drawStrokeItems);
		SVPElement *fill = it1.current(), *stroke = it2.current();
		while(fill != 0 || stroke != 0)
		{
			SVGTextContentElementImpl *text = fill ? fill->element : stroke->element;

			bool fillOk = fill && fill->svp && text->isFilled();
			bool strokeOk = stroke && stroke->svp && text->isStroked() && text->getStrokeWidth()->baseVal()->value() > 0; // Spec: A zero value causes no stroke to be painted.
			if(fillOk || strokeOk)
			{
				if(m_fillPainters.find(text))
					m_fillPainters[text]->update(text);

				if(m_strokePainters.find(text))
					m_strokePainters[text]->update(text);
			}
			fill = ++it1;
			stroke = ++it2;
		}
		m_canvas->invalidate(this, false);
	}
	else if(reason == UPDATE_TRANSFORM)
	{
		clearSVPs();
		init();
		m_canvas->invalidate(this, true);
	}
	else if(reason == UPDATE_ZOOM)
	{
		clearSVPs();
		init();
	}
	else if(reason == UPDATE_PAN)
	{
		QPtrListIterator<SVPElement> it1(m_drawFillItems);
		QPtrListIterator<SVPElement> it2(m_drawStrokeItems);

		double affine[6];
		KSVGHelper::matrixToAffine(m_text->screenCTM(), affine);

		SVPElement *fill = it1.current(), *stroke = it2.current();
		while(fill != 0 || stroke != 0)
		{
			SVGTextContentElementImpl *text = fill ? fill->element : stroke->element;

			bool fillOk = fill && fill->svp && text->isFilled();
			bool strokeOk = stroke && stroke->svp && text->isStroked() && text->getStrokeWidth()->baseVal()->value() > 0; // Spec: A zero value causes no stroke to be painted.

			if(fillOk)
				ksvg_art_svp_move(fill->svp, param1, param2);
			if(strokeOk)
				ksvg_art_svp_move(stroke->svp, param1, param2);
			fill = ++it1;
			stroke = ++it2;
		}
	}
	/*
	else if(reason == UPDATE_LINEWIDTH)
	{
	}*/
}

void LibartText::draw()
{
	QPtrListIterator<SVPElement> it1(m_drawFillItems);
	QPtrListIterator<SVPElement> it2(m_drawStrokeItems);

	SVPElement *fill = it1.current(), *stroke = it2.current();
	while(fill != 0 || stroke != 0)
	{
		SVGTextContentElementImpl *text = fill ? fill->element : stroke->element;
		if(!text || !text->getVisible() || !text->getDisplay() || !text->directRender())
			return;

		bool fillOk = fill && fill->svp && text->isFilled();
		bool strokeOk = stroke && stroke->svp && text->isStroked() && text->getStrokeWidth()->baseVal()->value() > 0; // Spec: A zero value causes no stroke to be painted.

		if(fillOk || strokeOk)
		{
			if(fillOk && m_fillPainters.find(text))
				m_fillPainters[text]->draw(m_canvas, fill->svp, text, text);

			if(strokeOk && m_strokePainters.find(text))
				m_strokePainters[text]->draw(m_canvas, stroke->svp, text, text);
		}
		fill = ++it1;
		stroke = ++it2;
	}
}

bool LibartText::isVisible()
{
	bool foundVisible = false;
	QPtrListIterator<SVPElement> it1(m_drawFillItems);
	QPtrListIterator<SVPElement> it2(m_drawStrokeItems);

	SVPElement *fill = it1.current(), *stroke = it2.current();
	while(fill != 0 || stroke != 0)
	{
		SVGTextContentElementImpl *text = fill ? fill->element : stroke->element;
		if(text && text->getVisible() && text->getDisplay() && text->directRender())
		{
			foundVisible = true;
			break;
		}

		fill = ++it1;
		stroke = ++it2;
	}

	return foundVisible;
}

void LibartText::init()
{
	init(m_text->screenCTM());
}

void LibartText::renderCallback(SVGTextContentElementImpl *element, const SVGMatrixImpl *screenCTM, T2P::GlyphSet *glyph, T2P::GlyphLayoutParams *params, double anchor) const
{
	unsigned int glyphCount = glyph->glyphCount(); // Don't call it n times in the for loop
	for(unsigned int i = 0; i < glyphCount; i++)
	{
		T2P::GlyphAffinePair *glyphAffine = glyph->set()[i];
		ArtBpath *bezier = static_cast<const T2P::BezierPathLibart *>(glyphAffine->transformatedPath())->m_array.data();
		ArtBpath *result = bezier;

		// text-anchor support
		if(anchor != 0)
		{
			double correct[6];

			if(!params->tb())
				art_affine_translate(correct, -anchor, 0);
			else
				art_affine_translate(correct, 0, -anchor);

			ArtBpath *temp = art_bpath_affine_transform(result, correct);
			//art_free(result);
			result = temp;
		}

		ArtSVP *fillSVP = 0, *strokeSVP = 0;

		if(m_context == NORMAL)
			LibartShape::calcSVPs(result, m_text, screenCTM, &strokeSVP, &fillSVP);
		else
			LibartShape::calcClipSVP(ksvg_art_bez_path_to_vec(result, 0.25), m_text, screenCTM, &fillSVP);

		SVPElement *fillElement = new SVPElement();
		fillElement->svp = fillSVP;
		fillElement->element = element;

		SVPElement *strokeElement = new SVPElement();
		strokeElement->svp = strokeSVP;
		strokeElement->element = element;

		m_drawFillItems.append(fillElement);
		m_drawStrokeItems.append(strokeElement);

		if(!m_fillPainters.find(element) && element->isFilled())
			m_fillPainters.insert(element, new LibartFillPainter(element));

		// Spec: A zero value causes no stroke to be painted.
		if(!m_strokePainters.find(element) && element->isStroked() && element->getStrokeWidth()->baseVal()->value() > 0)
			m_strokePainters.insert(element, new LibartStrokePainter(element));
	}
}

void LibartText::init(const SVGMatrixImpl *screenCTM)
{
	int curx = 0, cury = 0, endx = 0, endy = 0;
	KSVGTextChunk *textChunk = CanvasText::createTextChunk(m_canvas, screenCTM, curx, cury, endx, endy);

	if(textChunk->count() > 0)
		CanvasText::createGlyphs(textChunk, m_canvas, screenCTM, curx, cury, endx, endy);

	delete textChunk;
}

void LibartText::clearSVPs()
{
	m_drawFillItems.clear();
	m_drawStrokeItems.clear();
	m_fillPainters.clear();
	m_strokePainters.clear();
}

void LibartText::addTextDecoration(SVGTextContentElementImpl *element, double x, double y, double width, double height) const
{
	if(m_text->isFilled() || m_text->isStroked())
	{
		// compute rect svp
		ArtVpath *vec = allocVPath(6);

		vec[0].code = ART_MOVETO;
		vec[0].x = x;
		vec[0].y = y;

		vec[1].code = ART_LINETO;
		vec[1].x = x;
		vec[1].y = y + height;

		vec[2].code = ART_LINETO;
		vec[2].x = x + width;
		vec[2].y = y + height;

		vec[3].code = ART_LINETO;
		vec[3].x = x + width;
		vec[3].y = y;

		vec[4].code = ART_LINETO;
		vec[4].x = x;
		vec[4].y = y;

		vec[5].code = ART_END;
		double affine[6];
		KSVGHelper::matrixToAffine(m_text->screenCTM(), affine);

		ArtVpath *temp = art_vpath_affine_transform(vec, affine);
		art_free(vec);
		vec = temp;

		if(m_text->isFilled())
		{
			ArtSvpWriter *swr;
			ArtSVP *temp = art_svp_from_vpath(vec);

			swr = art_svp_writer_rewind_new(ART_WIND_RULE_ODDEVEN);

			art_svp_intersector(temp, swr);
			ArtSVP *fillSVP = art_svp_writer_rewind_reap(swr);

			SVPElement *fillElement = new SVPElement();
			fillElement->svp = fillSVP;
			fillElement->element = element;

			m_drawFillItems.append(fillElement);

			if(!m_fillPainters.find(element) && element->isFilled())
				m_fillPainters.insert(element, new LibartFillPainter(element));

			art_svp_free(temp);
		}
		// Stroking
		if(m_text->isStroked() || m_text->getStrokeColor()->paintType() == SVG_PAINTTYPE_URI)
		{
			double ratio = art_affine_expansion(affine);
			ArtSVP *strokeSVP = art_svp_vpath_stroke(vec, (ArtPathStrokeJoinType)m_text->getJoinStyle(), (ArtPathStrokeCapType)m_text->getCapStyle(), m_text->getStrokeWidth()->baseVal()->value() * ratio, m_text->getStrokeMiterlimit(), 0.25);

			SVPElement *strokeElement = new SVPElement();
			strokeElement->svp = strokeSVP;
			strokeElement->element = element;

			m_drawStrokeItems.append(strokeElement);

			// Spec: A zero value causes no stroke to be painted.
			if(!m_strokePainters.find(element) && element->isStroked() && element->getStrokeWidth()->baseVal()->value() > 0)
				m_strokePainters.insert(element, new LibartStrokePainter(element));
		}
		art_free(vec);
	}
}

void LibartText::initClipItem()
{
	init();
}

ArtSVP *LibartText::clipSVP()
{
	ArtSVP *svp = 0;
	QPtrListIterator<SVPElement> it(m_drawFillItems);

	SVPElement *fill = it.current();
	while(fill && fill->svp)
	{
		if(svp == 0)
			svp = LibartCanvas::copy_svp(fill->svp);
		else
		{
			ArtSVP *svp_union = art_svp_union(svp, fill->svp);
			art_svp_free(svp);
			svp = svp_union;
		}

		fill = ++it;
	}

	return svp;
}

ArtRender *LibartPaintServer::createRenderer(QRect bbox, KSVGCanvas *c)
{
	int x0 = bbox.x();
	int y0 = bbox.y();
	int x1 = bbox.right();
	int y1 = bbox.bottom();

	c->clipToBuffer(x0, y0, x1, y1);

	// Note: We always pass 3 for the number of channels since the ART_ALPHA parameter
	// adds the alpha channel when present.
	ArtRender *render = 0;
	render = art_render_new(QMIN(x0, x1),
							QMIN(y0, y1),
							QMAX(x0, x1) + 1,
							QMAX(y0, y1) + 1,
							c->renderingBuffer() + x0 * c->nrChannels() + y0 * c->rowStride(),
							c->rowStride(), 3, 8,
							c->nrChannels() == 3 ? ART_ALPHA_NONE : ART_ALPHA_PREMUL, 0);

	return render;
}

void LibartGradient::parseGradientStops(SVGGradientElementImpl *gradient)
{
	const double epsilon = DBL_EPSILON;

	for(DOM::Node node = gradient->firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGStopElementImpl *elem = dynamic_cast<SVGStopElementImpl *>(m_gradient->ownerDoc()->getElementFromHandle(node.handle()));
		if(elem)
		{
			m_stops.resize(m_stops.size() + 1);

			ArtGradientStop *stop = &(m_stops[m_stops.size() - 1]);

			stop->offset = elem->offset()->baseVal();

			// Spec: clamp range to 0 to 1
			if(stop->offset < epsilon)
				stop->offset = 0;
			else if(stop->offset > 1 - epsilon)
				stop->offset = 1;

			// Spec: if offset is less than previous offset, set it to the previous offset
			if(m_stops.size() > 1 && stop->offset < (stop - 1)->offset + epsilon)
				stop->offset = (stop - 1)->offset;

			// Get color
			QColor qStopColor;

			if(elem->getStopColor()->colorType() == SVG_COLORTYPE_CURRENTCOLOR)
				qStopColor = elem->getColor()->rgbColor().color();
			else
				qStopColor = elem->getStopColor()->rgbColor().color();

			// Convert in a libart suitable form
			QString tempName = qStopColor.name();
			const char *str = tempName.latin1();

			int stopColor = 0;

			for(int i = 1; str[i]; i++)
			{
				int hexval;
				if(str[i] >= '0' && str[i] <= '9')
					hexval = str[i] - '0';
				else if (str[i] >= 'A' && str[i] <= 'F')
					hexval = str[i] - 'A' + 10;
				else if (str[i] >= 'a' && str[i] <= 'f')
					hexval = str[i] - 'a' + 10;
				else
					break;

				stopColor = (stopColor << 4) + hexval;
			}

			// Apply stop-opacity
			float opacity = elem->stopOpacity();

			// Get rgba color including stop-opacity
			Q_UINT32 rgba = (stopColor << 8) | int(opacity * 255.0 + 0.5);
			Q_UINT32 r, g, b, a;

			a = rgba & 0xff;
			r = (rgba >> 24) & 0xff;
			g = (rgba >> 16) & 0xff;
			b = (rgba >> 8) & 0xff;

			stop->color[0] = ART_PIX_MAX_FROM_8(r);
			stop->color[1] = ART_PIX_MAX_FROM_8(g);
			stop->color[2] = ART_PIX_MAX_FROM_8(b);
			stop->color[3] = ART_PIX_MAX_FROM_8(a);
		}
	}
}

void LibartGradient::finalizePaintServer()
{
	parseGradientStops(m_gradient->stopsSource());

	QString _href = SVGURIReferenceImpl::getTarget(m_gradient->href()->baseVal().string());
	if(!_href.isEmpty())
		reference(_href);

	setFinalized();
}

void LibartGradient::reference(const QString &)
{
}

void LibartLinearGradient::render(KSVGCanvas *c, ArtSVP *svp, float opacity, QByteArray mask, QRect screenBBox)
{
	if(!m_stops.isEmpty())
	{
		m_linear->converter()->finalize(getBBoxTarget(), m_linear->ownerSVGElement(), m_linear->gradientUnits()->baseVal());

		ArtKSVGGradientLinear *linear = art_new(ArtKSVGGradientLinear, 1);

		if(m_linear->spreadMethod()->baseVal() == SVG_SPREADMETHOD_REPEAT)
			linear->spread = ART_GRADIENT_REPEAT;
		else if(m_linear->spreadMethod()->baseVal() == SVG_SPREADMETHOD_REFLECT)
			linear->spread = ART_GRADIENT_REFLECT;
		else
			linear->spread = ART_GRADIENT_PAD;

		linear->interpolation = m_linear->getColorInterpolation() == CI_SRGB ? ART_KSVG_SRGB_INTERPOLATION : ART_KSVG_LINEARRGB_INTERPOLATION;

		ArtRender *render = createRenderer(screenBBox, c);

		double _x1 = m_linear->x1()->baseVal()->value();
		double _y1 = m_linear->y1()->baseVal()->value();
		double _x2 = m_linear->x2()->baseVal()->value();
		double _y2 = m_linear->y2()->baseVal()->value();

		// Respect current transformation matrix (so gradients zoom with...)
		SVGTransformableImpl *transformable = dynamic_cast<SVGTransformableImpl *>(getBBoxTarget());
		SVGMatrixImpl *matrix = 0;
		if(transformable)
			matrix = transformable->getScreenCTM();
		else
			matrix = SVGSVGElementImpl::createSVGMatrix();

		const double epsilon = DBL_EPSILON;

		if(m_linear->gradientUnits()->baseVal() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX)
		{
			// Here we're undoing the unit-converter's work because putting the
			// bounding box transform into the matrix here lets the gradient transform
			// sit at the right point in the chain to work with bounding box coordinates. It
			// also removes the need for code to generate the 'not perpendicular to gradient vector' effect.
			SVGRectImpl *userBbox = getBBoxTarget()->getBBox();

			double width = userBbox->width();
			double height = userBbox->height();

			// Catch case of width or height of zero, which can be the case for lines.
			if(width < epsilon)
				width = 1;
			if(height < epsilon)
				height = 1;

			_x1 /= width;
			_y1 /= height;
			_x2 /= width;
			_y2 /= height;

			matrix->translate(userBbox->x(), userBbox->y());
			matrix->scaleNonUniform(width, height);

			userBbox->deref();
		}

		// Adjust to gradient transform
		SVGMatrixImpl *gradTrans = m_linear->gradientTransform()->baseVal()->concatenate();
		if(gradTrans)
		{
			matrix->multiply(gradTrans);
			gradTrans->deref();
		}

		double dx = _x2 - _x1;
		double dy = _y2 - _y1;

		if(fabs(dx) < epsilon && fabs(dy) < epsilon)
		{
			// Lines can generate (0, 0) with bbox coords.
			dx = 1;
			dy = 0;
		}

		double angle = atan2(dy, dx);
		double length = sqrt(dx * dx + dy * dy);

		const double pi = 3.14159265358979323846;

		matrix->translate(_x1, _y1);
		matrix->scale(length);
		matrix->rotate(angle * 180.0 / pi);

		double affine[6];

		KSVGHelper::matrixToAffine(matrix, affine);
		art_affine_invert(linear->affine, affine);

		matrix->deref();

		QMemArray<ArtGradientStop> stops = m_stops;
		stops.detach();

		for(unsigned int i = 0; i < stops.size(); i++)
			stops[i].color[3] = ArtPixMaxDepth(stops[i].color[3] * opacity + 0.5);

		if(m_linear->x1()->baseVal()->valueInSpecifiedUnits() == m_linear->x2()->baseVal()->valueInSpecifiedUnits()
			 && m_linear->y1()->baseVal()->valueInSpecifiedUnits() == m_linear->y2()->baseVal()->valueInSpecifiedUnits())
		{
			// Spec: If x1 == x2 and y1 == y2, paint the area in a single colour, using the colour
			// of the last stop.
			//
			// Using valueInSpecifiedUnits() so that we are comparing the values before possible
			// conversion to bounding box units by the converter.
			if(stops.size() > 1)
			{
				stops[0] = stops[stops.size() - 1];
				stops.resize(1);
			}
		}

		linear->stops = &(stops[0]);
		linear->n_stops = stops.size();

		art_render_svp(render, svp);
		art_ksvg_render_gradient_linear(render, linear, ART_FILTER_HYPER);

		if(mask.data())
			art_render_mask(render, screenBBox.left(), screenBBox.top(), screenBBox.right() + 1, screenBBox.bottom() + 1,
				(const art_u8 *)mask.data(), screenBBox.width());

		art_render_invoke(render);

		art_free(linear);
	}
}

void LibartRadialGradient::render(KSVGCanvas *c, ArtSVP *svp, float opacity, QByteArray mask, QRect screenBBox)
{
	if(!m_stops.isEmpty())
	{
		m_radial->converter()->finalize(getBBoxTarget(), m_radial->ownerSVGElement(), m_radial->gradientUnits()->baseVal());

		ArtKSVGGradientRadial *radial = art_new(ArtKSVGGradientRadial, 1);

		if(m_radial->spreadMethod()->baseVal() == SVG_SPREADMETHOD_REPEAT)
			radial->spread = ART_GRADIENT_REPEAT;
		else if(m_radial->spreadMethod()->baseVal() == SVG_SPREADMETHOD_REFLECT)
			radial->spread = ART_GRADIENT_REFLECT;
		else
			radial->spread = ART_GRADIENT_PAD;

		radial->interpolation = m_radial->getColorInterpolation() == CI_SRGB ? ART_KSVG_SRGB_INTERPOLATION : ART_KSVG_LINEARRGB_INTERPOLATION;

		ArtRender *render = createRenderer(screenBBox, c);

		// Respect current transformation matrix (so gradients zoom with...)
		SVGTransformableImpl *transformable = dynamic_cast<SVGTransformableImpl *>(getBBoxTarget());
		SVGMatrixImpl *matrix = 0;
		if(transformable)
			matrix = transformable->getScreenCTM();
		else
			matrix = SVGSVGElementImpl::createSVGMatrix();

		double _cx = m_radial->cx()->baseVal()->value();
		double _cy = m_radial->cy()->baseVal()->value();
		double _r = m_radial->r()->baseVal()->value();
		
		double _fx;
		double _fy;

		// Spec: If attribute fx is not specified, fx will coincide with cx.
		if(m_radial->getAttribute("fx").isEmpty())
			_fx = _cx;
		else
			_fx = m_radial->fx()->baseVal()->value();

		// Spec: If attribute fy is not specified, fy will coincide with cy.
		if(m_radial->getAttribute("fy").isEmpty())
			_fy = _cy;
		else
			_fy = m_radial->fy()->baseVal()->value();

		const double epsilon = DBL_EPSILON;

		if(m_radial->gradientUnits()->baseVal() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX)
		{
			// Here we're undoing the unit-converter's work because putting the
			// bounding box transform into the matrix here lets the gradient transform
			// sit at the right point in the chain to work with bounding box coordinates.
			// It also produces the elliptical shape due to the non-uniform scaling.
			SVGRectImpl *userBBox = getBBoxTarget()->getBBox();

			double width = userBBox->width();
			double height = userBBox->height();

			// Catch case of width or height of zero, which can be the case for lines.
			if(width < epsilon)
				width = 1;
			if(height < epsilon)
				height = 1;

			_cx /= width;
			_cy /= height;
			_fx /= width;
			_fy /= height;
			_r /= (sqrt(width * width + height * height) / 1.4142135623731);

			matrix->translate(userBBox->x(), userBBox->y());
			matrix->scaleNonUniform(width, height);

			userBBox->deref();
		}

		// Adjust to gradient transforms
		SVGMatrixImpl *transform = m_radial->gradientTransform()->baseVal()->concatenate();

		if(transform)
		{
			matrix->multiply(transform);
			transform->deref();
		}

		double fx = (_fx - _cx) / _r;
		double fy = (_fy - _cy) / _r;

		if(fx * fx + fy * fy > 0.99)
		{
			// Spec: If (fx, fy) lies outside the circle defined by (cx, cy) and r, set (fx, fy)
			// to the point of intersection of the line through (fx, fy) and the circle.
			//
			// Note: We need to keep (fx, fy) inside the unit circle in order for
			// libart to render the gradient correctly.
			double angle = atan2(fy, fx);
			fx = cos(angle) * 0.99;
			fy = sin(angle) * 0.99;
		}

		radial->fx = fx;
		radial->fy = fy;

		matrix->translate(_cx, _cy);
		matrix->scale(_r);

		double affine[6];

		KSVGHelper::matrixToAffine(matrix, affine);
		art_affine_invert(radial->affine, affine);

		matrix->deref();

		QMemArray<ArtGradientStop> stops = m_stops;
		stops.detach();

		for(unsigned int i = 0; i < stops.size(); i++)
			stops[i].color[3] = ArtPixMaxDepth(stops[i].color[3] * opacity + 0.5);

		radial->stops = &(stops[0]);
		radial->n_stops = stops.size();

		art_render_svp(render, svp);
		art_ksvg_render_gradient_radial(render, radial, ART_FILTER_HYPER);

		if(mask.data())
			art_render_mask(render, screenBBox.left(), screenBBox.top(), screenBBox.right() + 1, screenBBox.bottom() + 1,
				(const art_u8 *)mask.data(), screenBBox.width());

		art_render_invoke(render);

		art_free(radial);
	}
}

LibartPattern::LibartPattern(SVGPatternElementImpl *pattern)
	: m_pattern(pattern)
{
}

void LibartPattern::finalizePaintServer()
{
	m_pattern->finalizePaintServer();
	setFinalized();
}

void LibartPattern::reference(const QString &href)
{
	m_pattern->reference(href);
}

void LibartPattern::render(KSVGCanvas *c, ArtSVP *svp, float opacity, QByteArray mask, QRect screenBBox)
{
	SVGPatternElementImpl::Tile tile = m_pattern->createTile(getBBoxTarget());

	if(!tile.image().isNull())
	{
		QWMatrix m = tile.screenToTile();
		double affine[6];
		 
		affine[0] = m.m11();
		affine[1] = m.m12();
		affine[2] = m.m21();
		affine[3] = m.m22();
		affine[4] = m.dx();
		affine[5] = m.dy();

		int alpha = int(opacity * 255 + 0.5);

		ksvg_art_rgb_texture(svp, c->renderingBuffer() + screenBBox.x() * c->nrChannels() + screenBBox.y() * c->rowStride(), screenBBox.left(), screenBBox.top(), screenBBox.right() + 1, screenBBox.bottom() + 1, c->rowStride(), c->nrChannels(), tile.image().bits(), tile.image().width(), tile.image().height(), tile.image().width() * 4, affine, ART_FILTER_NEAREST, 0L, alpha, (art_u8 *)mask.data());
	}
}

// vim:ts=4:noet
