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

#include <qimage.h>

#include "SVGPaint.h"
#include "SVGRectImpl.h"
#include "SVGAngleImpl.h"
#include "SVGPaintImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGUnitTypes.h"
#include "SVGHelperImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGPointListImpl.h"
#include "SVGMarkerElement.h"
#include "SVGMarkerElementImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGPathSegListImpl.h"
#include "SVGAnimatedRectImpl.h"
#include "SVGAnimatedAngleImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGPolygonElementImpl.h"
#include "SVGClipPathElementImpl.h"
#include "SVGPolylineElementImpl.h"
#include "SVGStopElementImpl.h"
#include "SVGGradientElement.h"
#include "SVGGradientElementImpl.h"
#include "SVGLinearGradientElementImpl.h"
#include "SVGRadialGradientElementImpl.h"
#include "SVGPatternElementImpl.h"
#include "SVGAnimatedNumberImpl.h"
#include "SVGAnimatedLengthListImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGAnimatedStringImpl.h"
#include "SVGPreserveAspectRatioImpl.h"
#include "SVGAnimatedPreserveAspectRatioImpl.h"
#include "SVGAnimatedTransformListImpl.h"
#include "SVGTransformListImpl.h"
#include "SVGUnitConverter.h"

#include "Glyph.h"
#include "Converter.h"
#include "KSVGTextChunk.h"

#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_u.h"
#include "agg_scanline_p.h"
#include "agg_bounding_rect.h"
#include "agg_ellipse.h"
#include "agg_span_image_filter_rgba32.h"
#include "agg_color_rgba8.h"
#include "agg_gray8.h"
#include "agg_span_gradient.h"
#include "agg_span_interpolator_linear.h"
#include "agg_span_pattern_rgba32.h"
#include "agg_renderer_scanline.h"

#include "AggCanvas.h"
#include "AggCanvasItems.h"

extern "C"
{
/* These are in KSVGHelper.cpp */
int linearRGBFromsRGB(int sRGB8bit);
int sRGBFromLinearRGB(int linearRGB8bit);
}

struct color_function_profile
{
    color_function_profile() {}
    color_function_profile(const agg::rgba8 *colors) :
        m_colors(colors) {}

    const agg::rgba8& operator [] (unsigned v) const
    {
        return m_colors[v];
    }

    const agg::rgba8 *m_colors;
};

using namespace KSVG;

// agg2 helpers

agg::vcgen_stroke::line_cap_e toAggLineCap(PathStrokeCapType cap)
{
	if(cap == PATH_STROKE_CAP_BUTT)
		return agg::vcgen_stroke::butt_cap;
	else if(cap == PATH_STROKE_CAP_ROUND)
		return agg::vcgen_stroke::round_cap;
	else
		return agg::vcgen_stroke::square_cap;
}

template<class Source>
stroke<Source>::stroke(Source& src, KSVG::SVGStylableImpl *style) : m_s(src)
{
	m_s.width(style->getStrokeWidth()->baseVal()->value());
	m_s.line_join((agg::vcgen_stroke::line_join_e)style->getJoinStyle());
	m_s.miter_limit(style->getStrokeMiterlimit());
	m_s.line_cap(toAggLineCap(style->getCapStyle()));
}

template<class Source>
dash_stroke<Source>::dash_stroke(Source& src, KSVG::SVGStylableImpl *style) : m_d(src), m_ds(m_d)
{
	unsigned int dashLength = style->getDashArray() ? style->getDashArray()->baseVal()->numberOfItems() : 0;
	// there are dashes to be rendered
	unsigned int count = (dashLength % 2) == 0 ? dashLength : dashLength * 2;
	for(unsigned int i = 0; i < count; i += 2)
		m_d.add_dash(style->getDashArray()->baseVal()->getItem(i % dashLength)->value(),
		 			 style->getDashArray()->baseVal()->getItem((i + 1) % dashLength)->value());
	m_d.dash_start(style->getDashOffset()->baseVal()->value());
	m_ds.width(style->getStrokeWidth()->baseVal()->value());
	m_ds.line_join((agg::vcgen_stroke::line_join_e)style->getJoinStyle());
	m_ds.miter_limit(style->getStrokeMiterlimit());
	m_ds.line_cap(toAggLineCap(style->getCapStyle()));
}

template<class Source>
dash_stroke_simple<Source>::dash_stroke_simple(Source& src, KSVG::SVGStylableImpl *style)
{
	//if(style->isStroked() && style->getStrokeWidth()->baseVal()->value() > 0)
	//{
		unsigned int dashLength = style->getDashArray() ? style->getDashArray()->baseVal()->numberOfItems() : 0;
		if(dashLength > 0)
			impl = new dash_stroke<Source>(src, style);
		else
			impl = new stroke<Source>(src, style);
	//}
	//else
		//impl = 0;
}

void renderPathSolid(AggCanvas *canvas, const agg::rgba8 &color)
{
	agg::scanline_p8 sl;
	if(canvas->nrChannels() == 3)
	{
		typedef agg::pixfmt_rgb24 pixfmt;
        	typedef agg::renderer_base<pixfmt> renderer_base;
        	typedef agg::renderer_scanline_p_solid<renderer_base> renderer_solid;

		pixfmt pixf(canvas->buf());
		renderer_base rb(pixf);
		renderer_solid ren(rb);

		ren.color(color);
		canvas->m_ras.render(sl, ren);
	}
	else
	{
		typedef agg::pixfmt_rgba32 pixfmt;
        	typedef agg::renderer_base<pixfmt> renderer_base;
        	typedef agg::renderer_scanline_p_solid<renderer_base> renderer_solid;

		pixfmt pixf(canvas->buf());
		renderer_base rb(pixf);
		renderer_solid ren(rb);

		ren.color(color);
		canvas->m_ras.render(sl, ren);
	}
}

// #####

BezierPathAggStroked::BezierPathAggStroked(SVGStylableImpl *style)
: T2P::BezierPathAgg(), m_curved_trans_clipped(m_curved_trans), m_curved_stroked(m_curved, style), m_curved_stroked_trans(m_curved_stroked, m_transform), m_curved_stroked_trans_clipped(m_curved_stroked_trans), m_style(style)
{
}

BezierPathAggStroked::BezierPathAggStroked(const T2P::BezierPathAgg &other, SVGStylableImpl *style)
: T2P::BezierPathAgg(other), m_curved_trans_clipped(m_curved_trans), m_curved_stroked(m_curved, style), m_curved_stroked_trans(m_curved_stroked, m_transform), m_curved_stroked_trans_clipped(m_curved_stroked_trans), m_style(style)
{
}

AggShape::AggShape(AggCanvas *c, SVGStylableImpl *style) : CanvasItem(), BezierPathAggStroked(style), m_canvas(c)
{
	m_context = NORMAL;
	m_fillPainter = 0;
	m_strokePainter = 0;
}

AggShape::~AggShape()
{
	freeSVPs();
	delete m_fillPainter;
	delete m_strokePainter;
}

QRect AggShape::bbox() const
{
	return m_bbox;
}

bool AggShape::fillContains(const QPoint &p)
{
	agg::rasterizer_scanline_aa<> ras;
	ras.filling_rule(m_style->getFillRule() == RULE_EVENODD ? agg::fill_even_odd : agg::fill_non_zero);
	ras.add_path(m_curved_trans);
	bool b = ras.hit_test(p.x(), p.y());
	return b;
}

bool AggShape::strokeContains(const QPoint &p)
{
	agg::rasterizer_scanline_aa<> ras;
	ras.add_path(m_curved_stroked_trans);
	bool b = ras.hit_test(p.x(), p.y());
	return b;
}

void AggShape::update(CanvasItemUpdate reason, int param1, int param2)
{
	if(reason == UPDATE_STYLE)
	{
		if(!m_fillPainter || !m_strokePainter)
			AggShape::init();
		if(m_fillPainter)
			m_fillPainter->update(m_style);
		if(m_strokePainter)
			m_strokePainter->update(m_style);
		m_canvas->invalidate(this, false);
	}
	else if(reason == UPDATE_TRANSFORM)
	{
		freeSVPs();
		init();
		m_canvas->invalidate(this, true);
	}
	else if(reason == UPDATE_ZOOM)
		init();
	else if(reason == UPDATE_PAN)
	{
		agg::trans_affine mtx(1, 0, 0, 1, param1, param2);
		m_transform *= mtx;
	}
	else if(reason == UPDATE_LINEWIDTH)
	{
		init();
		m_canvas->invalidate(this, true);
	}
}

void AggShape::draw(SVGShapeImpl *shape)
{
	if(!m_referenced && (!m_style->getVisible() || !m_style->getDisplay() || !shape->directRender()))
		return;

	//if(!m_strokeSVP && (!m_fillSVP || !m_style->isFilled()))
	//	init();
	agg::rect cb;
	if(m_canvas->nrChannels() == 3)
	{
		agg::pixfmt_rgb24 pixf(m_canvas->buf());
		agg::renderer_base<agg::pixfmt_rgb24> rb(pixf);
		cb = rb.clip_box();
	}
	else
	{
		agg::pixfmt_rgba32 pixf(m_canvas->buf());
		agg::renderer_base<agg::pixfmt_rgba32> rb(pixf);
		cb = rb.clip_box();
	}

	m_curved_trans_clipped.clip_box(cb.x1, cb.y1, cb.x2 + 1, cb.y2 + 1);
	m_curved_stroked_trans_clipped.clip_box(cb.x1, cb.y1, cb.x2 + 1, cb.y2 + 1);

	double x1, y1, x2, y2;
	agg::bounding_rect(m_curved_trans, *this, 0, 1, &x1, &y1, &x2, &y2);
	m_bbox = QRect(int(x1), int(y1), int(x2 - x1), int(y2 - y1));

	m_curved.approximation_scale(pow(m_transform.scale(), 0.75));

	if(m_fillPainter)
		m_fillPainter->draw(m_canvas, m_curved_trans, m_style, shape);
	if(m_strokePainter)
		m_strokePainter->draw(m_canvas, m_curved_stroked_trans, m_style, shape);
}

bool AggShape::isVisible(SVGShapeImpl *shape)
{
	return m_referenced || (m_style->getVisible() && m_style->getDisplay() && shape->directRender());
}

void AggShape::calcSVPs(const SVGMatrixImpl *matrix)
{
	// transform
	m_transform = agg::trans_affine(matrix->a(), matrix->b(), matrix->c(), matrix->d(), matrix->e(), matrix->f());

	double x1, y1, x2, y2;
	agg::bounding_rect(m_curved_trans, *this, 0, 1, &x1, &y1, &x2, &y2);
	m_bbox = QRect(int(x1), int(y1), int(x2 - x1), int(y2 - y1));
}

void AggShape::init(const SVGMatrixImpl *)
{
}

void AggShape::init()
{
	if(m_style->isFilled())
	{
		if(m_fillPainter == 0)
			m_fillPainter = new AggFillPaintServer(m_style);
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
			m_strokePainter = new AggStrokePaintServer(m_style);
	}
	else
	{
		delete m_strokePainter;
		m_strokePainter = 0;
	}
}

void AggShape::freeSVPs()
{
	m_storage.remove_all();
}

// #####

AggStrokePaintServer::AggStrokePaintServer(SVGStylableImpl *style)
{
	update(style);
}

void AggStrokePaintServer::update(SVGStylableImpl *style)
{
	if(style->getStrokeColor()->paintType() != SVG_PAINTTYPE_URI)
	{
		QColor qcolor;
		if(style->getStrokeColor()->paintType() == SVG_PAINTTYPE_CURRENTCOLOR)
			qcolor = style->getColor()->rgbColor().color();
		else
			qcolor = style->getStrokeColor()->rgbColor().color();

		short opacity = static_cast<short>(style->getStrokeOpacity() * style->getOpacity() * 255);

		// Spec: clamping
		opacity = opacity < 0 ? 0 : opacity;
		opacity = opacity > 255 ? 255 : opacity;

		m_color = agg::rgba8(qcolor.red(), qcolor.green(), qcolor.blue());
		m_color.opacity(style->getStrokeOpacity() * style->getOpacity());
	}
}

template<class VertexSource>
void AggStrokePaintServer::draw(AggCanvas *canvas, VertexSource &vs, SVGStylableImpl *style, SVGShapeImpl *shape)
{
	canvas->m_ras.reset();
	if(style->getStrokeColor()->paintType() == SVG_PAINTTYPE_URI)
	{
		AggPaintServer *pserver = static_cast<AggPaintServer *>(SVGPaintServerImpl::paintServer(shape->ownerDoc(), style->getStrokeColor()->uri().string()));
		if(!pserver) return;
		pserver->setBBoxTarget(shape);

		// TODO : Clipping
		if(!pserver->finalized())
			pserver->finalizePaintServer();
		canvas->m_ras.add_path(vs);
		pserver->render(canvas);
	}
	else
	{
		canvas->m_ras.add_path(vs);
		renderPathSolid(canvas, m_color);
	}
}

AggFillPaintServer::AggFillPaintServer(SVGStylableImpl *style)
{
	update(style);
}

void AggFillPaintServer::update(SVGStylableImpl *style)
{
	if(style->getFillColor()->paintType() != SVG_PAINTTYPE_URI)
	{
		QColor qcolor;
		if(style->getFillColor()->paintType() == SVG_PAINTTYPE_CURRENTCOLOR)
			qcolor = style->getColor()->rgbColor().color();
		else
			qcolor = style->getFillColor()->rgbColor().color();

		short opacity = static_cast<short>(style->getFillOpacity() * style->getOpacity() * 255);

		// Spec: clamping
		opacity = opacity < 0 ? 0 : opacity;
		opacity = opacity > 255 ? 255 : opacity;

		m_color = agg::rgba8(qcolor.red(), qcolor.green(), qcolor.blue());
		m_color.opacity(style->getFillOpacity() * style->getOpacity());
	}
}

template<class VertexSource>
void AggFillPaintServer::draw(AggCanvas *canvas, VertexSource &vs, SVGStylableImpl *style, SVGShapeImpl *shape)
{
	canvas->m_ras.reset();
	if(style->getFillColor()->paintType() == SVG_PAINTTYPE_URI)
	{
		AggPaintServer *pserver = static_cast<AggPaintServer *>(SVGPaintServerImpl::paintServer(shape->ownerDoc(), style->getFillColor()->uri().string()));
		if(!pserver) return;
		pserver->setBBoxTarget(shape);

		// TODO : Clipping
		if(!pserver->finalized())
			pserver->finalizePaintServer();
		canvas->m_ras.add_path(vs);
		pserver->render(canvas);
	}
	else
	{
		canvas->m_ras.filling_rule(style->getFillRule() == RULE_EVENODD ? agg::fill_even_odd : agg::fill_non_zero);
		canvas->m_ras.add_path(vs);
		renderPathSolid(canvas, m_color);
	}
}

// #####

AggRectangle::AggRectangle(AggCanvas *c, SVGRectElementImpl *rect)
: AggShape(c, rect), m_rect(rect)
{
	init();
}

void AggRectangle::draw()
{
	if(isVisible())
		AggShape::draw(m_rect);
}

bool AggRectangle::isVisible()
{
	// Spec: a value of zero disables rendering
	return AggShape::isVisible(m_rect) && m_rect->width()->baseVal()->value() > 0 && m_rect->height()->baseVal()->value() > 0;
}

void AggRectangle::init()
{
	init(m_rect->screenCTM());
}

void AggRectangle::init(const SVGMatrixImpl *screenCTM)
{
	AggShape::init();
	if(m_storage.total_vertices() == 0)
	{
	double x = m_rect->x()->baseVal()->value();
	double y = m_rect->y()->baseVal()->value();
	double width = m_rect->width()->baseVal()->value();
	double height = m_rect->height()->baseVal()->value();
	double rx = m_rect->rx()->baseVal()->value();
	double ry = m_rect->ry()->baseVal()->value();

	// Spec: If there is no rx or ry specified, draw a normal rect
	if(rx == -1 && ry == -1)
	{
		m_storage.move_to(x, y);
		m_storage.line_to(x + width, y);
		m_storage.line_to(x + width, y + height);
		m_storage.line_to(x, y + height);
	}
	else
	{
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

		m_storage.move_to(x + rx, y);

		i++;
		m_storage.curve4(x + rx * (1 - 0.552), y, x, y + ry * (1 - 0.552), x, y + ry);
		i++;
		if(ry < height / 2)
		{
			m_storage.line_to(x, y + height - ry);
			i++;
		}
		m_storage.curve4(x, y + height - ry * (1 - 0.552), x + rx * (1 - 0.552), y + height, x + rx, y + height);
		i++;
		if(rx < width / 2)
		{
			m_storage.line_to(x + width - rx, y + height);
			i++;
		}
		m_storage.curve4(x + width - rx * (1 - 0.552), y + height, x + width, y + height - ry * (1 - 0.552), x + width, y + height - ry);
		i++;
		if(ry < height / 2)
		{
			m_storage.line_to(x + width, y + ry);
			i++;
		}
		m_storage.curve4(x + width, y + ry * (1 - 0.552), x + width - rx * (1 - 0.552), y, x + width - rx, y);
		i++;
		if(rx < width / 2)
		{
			m_storage.line_to(x + rx, y);
			i++;
		}
	}
	m_storage.close_polygon();
	}
	//if(m_context == NORMAL)
		calcSVPs(screenCTM);
	//else
	//	calcClipSVP(vec, m_rect, screenCTM, &m_fillSVP);
}

// #####

AggEllipse::AggEllipse(AggCanvas *c, SVGEllipseElementImpl *ellipse)
: AggShape(c, ellipse), m_ellipse(ellipse)
{
	init();
}
	
void AggEllipse::draw()
{
	if(isVisible())
		AggShape::draw(m_ellipse);
}

bool AggEllipse::isVisible()
{
	// Spec: dont render when rx and/or ry is zero
	return AggShape::isVisible(m_ellipse) && m_ellipse->rx()->baseVal()->value() > 0 && m_ellipse->ry()->baseVal()->value() > 0;
}

void AggEllipse::init()
{
	init(m_ellipse->screenCTM());
}

void AggEllipse::init(const SVGMatrixImpl *screenCTM)
{
	AggShape::init();
	if(m_storage.total_vertices() == 0)
	{
		double rx = m_ellipse->rx()->baseVal()->value();
		double ry = m_ellipse->ry()->baseVal()->value();
		double cx = m_ellipse->cx()->baseVal()->value();
		double cy = m_ellipse->cy()->baseVal()->value();

		agg::ellipse ell(cx, cy, rx, ry, 100);
		ell.rewind(0);
		double x, y;
		unsigned int cmd;
		while((cmd = ell.vertex(&x, &y)) != agg::path_cmd_stop)
			m_storage.add_vertex(x, y, cmd);
		m_storage.close_polygon();
	}
	//if(m_context == NORMAL)
		calcSVPs(screenCTM);
	//else
	//	calcClipSVP(vec2, m_ellipse, screenCTM, &m_fillSVP);
}

// #####

AggCircle::AggCircle(AggCanvas *c, SVGCircleElementImpl *circle)
: AggShape(c, circle), m_circle(circle)
{
	init();
}
	
void AggCircle::draw()
{
	if(isVisible())
		AggShape::draw(m_circle);
}

bool AggCircle::isVisible()
{
	// Spec: a value of zero disables rendering
	return AggShape::isVisible(m_circle) && m_circle->r()->baseVal()->value() > 0;
}

void AggCircle::init()
{
	init(m_circle->screenCTM());
}

void AggCircle::init(const SVGMatrixImpl *screenCTM)
{
	AggShape::init();
	if(m_storage.total_vertices() == 0)
	{
		double r = m_circle->r()->baseVal()->value();
		double cx = m_circle->cx()->baseVal()->value();
		double cy = m_circle->cy()->baseVal()->value();

		agg::ellipse ell(cx, cy, r, r, 100);
		ell.rewind(0);
		double x, y;
		unsigned int cmd;
		while((cmd = ell.vertex(&x, &y)) != agg::path_cmd_stop)
			m_storage.add_vertex(x, y, cmd);
		m_storage.close_polygon();
	}
	//if(m_context == NORMAL)
		calcSVPs(screenCTM);
	//else
	//	calcClipSVP(vec2, m_ellipse, screenCTM, &m_fillSVP);
}

// #####

AggLine::AggLine(AggCanvas *c, SVGLineElementImpl *line)
: AggShape(c, line), MarkerHelper(), m_line(line)
{
	init();
}

AggLine::~AggLine()
{
}

void AggLine::draw()
{
	if(isVisible())
	{
		// transform ( zoom?)
		//agg::trans_affine transform = m_transform;
		//agg::trans_affine mtx(m_canvas->zoom(), 0, 0, m_canvas->zoom(), m_canvas->pan().x(), m_canvas->pan().y());
		//m_transform *= mtx;
		//m_curved.approximation_scale(pow(m_transform.scale(), 0.75));

		if(m_style->isStroked())
		{
			m_canvas->m_ras.reset();
			m_canvas->m_ras.add_path(m_curved_stroked_trans);
			QColor qcolor;
			if(m_style->getStrokeColor()->paintType() == SVG_PAINTTYPE_CURRENTCOLOR)
				qcolor = m_style->getColor()->rgbColor().color();
			else
				qcolor = m_style->getStrokeColor()->rgbColor().color();
			agg::rgba8 color(qcolor.red(), qcolor.green(), qcolor.blue());
			color.opacity(m_style->getStrokeOpacity() * m_style->getOpacity());
			renderPathSolid(m_canvas, color);
		}
		//m_transform = transform;

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
}

bool AggLine::isVisible()
{
	return AggShape::isVisible(m_line);
}

void AggLine::init()
{
	init(m_line->screenCTM());
}

void AggLine::init(const SVGMatrixImpl *screenCTM)
{
	AggShape::init();
	if(m_storage.total_vertices() == 0)
	{
		m_storage.move_to(m_line->x1()->baseVal()->value(), m_line->y1()->baseVal()->value());
		// A subpath consisting of a moveto and lineto to the same exact location or a subpath consisting of a moveto
		// and a closepath will be stroked only if the 'stroke-linecap' property is set to "round", producing a circle
		// centered at the given point.
		double x2 = m_line->x2()->baseVal()->value();
		double y2 = m_line->y2()->baseVal()->value();
		if(x2 == m_line->x1()->baseVal()->value() && y2 == m_line->y1()->baseVal()->value() && m_line->getCapStyle() == PATH_STROKE_CAP_ROUND)
				m_storage.line_to(x2 + .5, y2);
		else
				m_storage.line_to(x2, y2);
	}
	//if(m_context == NORMAL)
		calcSVPs(screenCTM);
	//else
	//	calcClipSVP(polyline, m_poly, screenCTM, &m_fillSVP);
}

// #####
AggPoly::AggPoly(AggCanvas *c, SVGPolyElementImpl *poly)
: AggShape(c, poly), MarkerHelper(), m_poly(poly)
{
}

AggPoly::~AggPoly()
{
}

void AggPoly::init()
{
	init(m_poly->screenCTM());
}

void AggPoly::draw()
{
	if(isVisible())
	{
		AggShape::draw(m_poly);

		if(m_poly->hasMarkers())
			m_poly->drawMarkers();
	}
}

bool AggPoly::isVisible()
{
	return AggShape::isVisible(m_poly);
}

// #####
AggPolyline::AggPolyline(AggCanvas *c, SVGPolylineElementImpl *poly)
: AggPoly(c, poly)
{
	AggPoly::init();
}

AggPolyline::~AggPolyline()
{
}

void AggPolyline::init(const SVGMatrixImpl *screenCTM)
{
	AggShape::init();
	if(m_storage.total_vertices() == 0)
	{
		unsigned int numberOfPoints = m_poly->points()->numberOfItems();

		if(numberOfPoints < 1)
			return;

		m_storage.move_to(m_poly->points()->getItem(0)->x(), m_poly->points()->getItem(0)->y());

		// A subpath consisting of a moveto and lineto to the same exact location or a subpath consisting of a moveto
		// and a closepath will be stroked only if the 'stroke-linecap' property is set to "round", producing a circle
		// centered at the given point.
		if(numberOfPoints == 2)
		{
			double x1 = m_poly->points()->getItem(1)->x();
			double y1 = m_poly->points()->getItem(1)->y();
			if(x1 == m_poly->points()->getItem(0)->x() && y1 == m_poly->points()->getItem(0)->y() && m_poly->getCapStyle() == PATH_STROKE_CAP_ROUND)
				m_storage.line_to(m_poly->points()->getItem(1)->x() + .5, m_poly->points()->getItem(1)->y());
			else
				m_storage.line_to(m_poly->points()->getItem(1)->x(), m_poly->points()->getItem(1)->y());
		}
		else
		{
			unsigned int index;
			for(index = 1; index < numberOfPoints; index++)
				m_storage.line_to(m_poly->points()->getItem(index)->x(), m_poly->points()->getItem(index)->y());
		}
	}
	//if(m_context == NORMAL)
		calcSVPs(screenCTM);
	//else
	//	calcClipSVP(polyline, m_poly, screenCTM, &m_fillSVP);
}

// #####

AggPolygon::AggPolygon(AggCanvas *c, SVGPolygonElementImpl *poly)
: AggPoly(c, poly)
{
	AggPoly::init();
}

AggPolygon::~AggPolygon()
{
}

void AggPolygon::init(const SVGMatrixImpl *screenCTM)
{
	AggShape::init();
	if(m_storage.total_vertices() == 0)
	{
		int numberOfPoints = m_poly->points()->numberOfItems();

		if(numberOfPoints < 1)
			return;

		m_storage.move_to(m_poly->points()->getItem(0)->x(), m_poly->points()->getItem(0)->y());

		int index;
		for(index = 1; index < numberOfPoints; index++)
			m_storage.line_to(m_poly->points()->getItem(index)->x(), m_poly->points()->getItem(index)->y());

		m_storage.line_to(m_poly->points()->getItem(0)->x(), m_poly->points()->getItem(0)->y());
		m_storage.close_polygon();
	}
	//if(m_context == NORMAL)
		calcSVPs(screenCTM);
	//else
	//	calcClipSVP(polygon, m_poly, screenCTM, &m_fillSVP);
}

// #####

AggPath::AggPath(AggCanvas *c, SVGPathElementImpl *path)
: AggShape(c, path), SVGPathParser(), MarkerHelper(), m_path(path)
{
	init();
}

AggPath::~AggPath()
{
}

void AggPath::draw()
{
	AggShape::draw(m_path);

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

bool AggPath::isVisible()
{
	return AggShape::isVisible(m_path);
}

void AggPath::init()
{
	init(m_path->screenCTM());
}

void AggPath::init(const SVGMatrixImpl *screenCTM)
{
	AggShape::init();
	if(m_storage.total_vertices() == 0)
	{
		if(!m_path->getAttribute("d").string().isEmpty())
		{
			m_storage.start_new_path();
			parseSVG(m_path->getAttribute("d").string(), true);

			// A subpath consisting of a moveto and lineto to the same exact location or a subpath consisting of a moveto
			// and a closepath will be stroked only if the 'stroke-linecap' property is set to "round", producing a circle
			// centered at the given point.
			if(m_storage.total_vertices() == 2 && agg::is_line_to(m_storage.command(1)))
			{
				double x1, y1;
				double x2, y2;
				m_storage.vertex(0, &x1, &y1);
				m_storage.vertex(1, &x2, &y2);
				if(x1 == x2 && y1 == y2 && m_path->getCapStyle() == PATH_STROKE_CAP_ROUND)
					m_storage.modify_vertex(1, x2 + .5, y2);
			}
			// TODO : handle filled paths that are not closed explicitly
		}
	}

	// There are pure-moveto paths which reference paint servers *bah*
	// Do NOT render them
	bool dontrender = m_storage.total_vertices() == 1 && agg::is_move_to((*m_storage.begin()).cmd);
	if(!dontrender && m_context == NORMAL)
		calcSVPs(screenCTM);
	//else
	//	calcClipSVP(ksvg_art_bez_path_to_vec(m_array.data(), 0.25), m_path, screenCTM, &m_fillSVP);
}

void AggPath::svgMoveTo(double x1, double y1, bool, bool)
{
	m_storage.move_to(x1, y1);
}

void AggPath::svgLineTo(double x1, double y1, bool)
{
	m_storage.line_to(x1, y1);
}

void AggPath::svgCurveToCubic(double x1, double y1, double x2, double y2, double x3, double y3, bool)
{
	m_storage.curve4(x1, y1, x2, y2, x3, y3);
}

void AggPath::svgClosePath()
{
	m_storage.close_polygon();
}

// #####

AggMarker::AggMarker(AggCanvas *c, SVGMarkerElementImpl *marker)
: CanvasMarker(marker), m_canvas(c)//, m_clippingRectangle(0)
{
}

AggMarker::~AggMarker()
{
	//if(m_clippingRectangle)
	//	art_svp_free(m_clippingRectangle);
}

void AggMarker::init()
{
}

void AggMarker::draw()
{
}

void AggMarker::draw(SVGShapeImpl * /*obj*/, int  /*x*/, int  /*y*/, float  /*lwidth*/, double  /*angle*/)
{
}

// #####

AggImage::AggImage(AggCanvas *c, SVGImageElementImpl *image)
: m_canvas(c), m_image(image)
{
}

AggImage::~AggImage()
{
}

void AggImage::draw()
{
	if(isVisible())
	{
		//KSVGPolygon clippingPolygon = m_image->clippingShape();

		QImage *img = m_image->image();
		if(!img) return;
		QImage image = m_image->scaledImage();
		agg::rendering_buffer source_buffer;
		source_buffer.attach(image.bits(), image.width(), image.height(), image.width() * 4);

		typedef agg::pixfmt_rgb24 pixfmt;
 	      	typedef agg::renderer_base<pixfmt> renderer_base;

		pixfmt pixf(m_canvas->buf());
		renderer_base rb(pixf);

		typedef agg::span_interpolator_linear<> interpolator_type;
		typedef agg::span_image_filter_rgba32_bilinear<agg::order_bgra32, interpolator_type> span_gen_type;
		typedef agg::renderer_scanline_u<renderer_base, span_gen_type> renderer_type;
		SVGMatrixImpl *ctm = m_image->scaledImageMatrix();
		agg::trans_affine img_mtx(ctm->a(), ctm->b(), ctm->c(), ctm->d(), ctm->e(), ctm->f());
		kdDebug() << "ctm->e() : " << ctm->e() << endl;
		kdDebug() << "ctm->f() : " << ctm->f() << endl;
		double x1 = 0;
		double y1 = 0;
		double x2 = image.width();
		double y2 = image.height();
		img_mtx.transform(&x1, &y1);
		img_mtx.transform(&x2, &y2);
		img_mtx.invert();

		interpolator_type interpolator(img_mtx);
		agg::span_allocator<agg::rgba8> sa;
		span_gen_type sg(sa, source_buffer, agg::rgba(1, 1, 1, 0), interpolator);
	       	renderer_type ri(rb, sg);

	        agg::scanline_u8 sl;

		//rb.reset_clipping(true);
		// Clip image against buffer
		agg::path_storage viewp;
		viewp.move_to(x1, y1);
		viewp.line_to(x1, y2);
		viewp.line_to(x2, y2);
		viewp.line_to(x2, y1);
		viewp.close_polygon();
		m_canvas->m_ras.add_path(viewp);
		m_canvas->m_ras.render(sl, ri);

		ctm->deref();
	}
}

bool AggImage::isVisible()
{
	return (m_referenced || (m_image->getVisible() && m_image->getDisplay() && m_image->directRender())) && m_image->image();
}

void AggImage::init()
{
}

QRect AggImage::bbox() const
{
	QRect bbox(static_cast<int>(m_image->x()->baseVal()->value()),
			   static_cast<int>(m_image->y()->baseVal()->value()),
			   static_cast<int>(m_image->width()->baseVal()->value()),
			   static_cast<int>(m_image->height()->baseVal()->value()));


	return SVGHelperImpl::fromUserspace(m_image, bbox);
}

// #####

AggText::AggText(AggCanvas *c, SVGTextElementImpl *text)
: CanvasText(text), m_canvas(c)
{
	init();
	m_drawItems.setAutoDelete(true);
}

AggText::~AggText()
{
}

bool AggText::fillContains(const QPoint &p)
{
	QPtrListIterator<SVPElement> it(m_drawItems);

	SVPElement *fill = it.current();
	while(fill)
	{
		if(fill->svp)
		{
			agg::rasterizer_scanline_aa<> ras;
			ras.filling_rule(fill->element->getFillRule() == RULE_EVENODD ? agg::fill_even_odd : agg::fill_non_zero);
			ras.add_path(fill->svp->m_curved_trans);
			if(ras.hit_test(p.x(), p.y()))
				return true;
		}

		fill = ++it;
	}

	return false;
}

bool AggText::strokeContains(const QPoint &p)
{
	QPtrListIterator<SVPElement> it(m_drawItems);

	SVPElement *stroke = it.current();
	while(stroke)
	{
		if(stroke->svp)
		{
			agg::rasterizer_scanline_aa<> ras;
			ras.filling_rule(stroke->element->getFillRule() == RULE_EVENODD ? agg::fill_even_odd : agg::fill_non_zero);
			ras.add_path(stroke->svp->m_curved_stroked_trans);
			if(ras.hit_test(p.x(), p.y()))
				return true;
		}

		stroke = ++it;
	}

	return false;
}

QRect AggText::bbox() const
{
	QRect result, rect;

	QPtrListIterator<SVPElement> it(m_drawItems);

	SVPElement *elem = it.current();
	while(elem)
	{
		double x1, y1, x2, y2;
		if(elem && elem->svp)
		{
			if(agg::bounding_rect(elem->svp->m_curved_trans, *this, 0, 1, &x1, &y1, &x2, &y2))
			{
				rect.setX(int(x1));
				rect.setY(int(y1));
				rect.setWidth(int(x2 - x1));
				rect.setHeight(int(y2 - y1));

				result = result.unite(rect);
			}
		}
		elem = ++it;
	}
	return result;
}

void AggText::update(CanvasItemUpdate reason, int param1, int param2)
{
	if(reason == UPDATE_STYLE)
	{
		QPtrListIterator<SVPElement> it(m_drawItems);

		SVPElement *svpelement = it.current();
		SVGTextContentElementImpl *text;
		while(svpelement)
		{
			text = svpelement->element;
			if(svpelement->fillPainter)
				svpelement->fillPainter->update(text);
			if(svpelement->strokePainter)
				svpelement->strokePainter->update(text);

			svpelement = ++it;
		}
		m_canvas->invalidate(this, false);
	}
	else if(reason == UPDATE_TRANSFORM)
	{
		clearCurved();
		init();
		m_canvas->invalidate(this, true);
	}
	else if(reason == UPDATE_ZOOM)
	{
		clearCurved();
		init();
	}
	else if(reason == UPDATE_PAN)
	{
		QPtrListIterator<SVPElement> it(m_drawItems);

		SVPElement *svpelement = it.current();
		T2P::BezierPathAgg *bpath;
		while(svpelement)
		{
			bpath = svpelement->svp;
			agg::trans_affine mtx(1, 0, 0, 1, param1, param2);
			bpath->m_transform *= mtx;

			svpelement = ++it;
		}
	}
	/*
	else if(reason == UPDATE_LINEWIDTH)
	{
	}*/
}

void AggText::draw()
{
	QPtrListIterator<SVPElement> it(m_drawItems);

	SVPElement *svpelement = it.current();
	BezierPathAggStroked *bpath;
	SVGTextContentElementImpl *text;

	while(svpelement)
	{
		bpath = svpelement->svp;
		text = svpelement->element;
		if(!text->getVisible() || !text->getDisplay() || !text->directRender())
			return;

		bpath->m_curved.approximation_scale(pow(bpath->m_transform.scale(), 1.25));

		if(svpelement->fillPainter)
			svpelement->fillPainter->draw(m_canvas, bpath->m_curved_trans, text, text);
		if(svpelement->strokePainter)
			svpelement->strokePainter->draw(m_canvas, bpath->m_curved_stroked_trans, text, text);

		svpelement = ++it;
	}
}

bool AggText::isVisible()
{
	bool foundVisible = false;
	QPtrListIterator<SVPElement> it(m_drawItems);

	SVPElement *svpelement = it.current();
	SVGTextContentElementImpl *text;

	while(svpelement)
	{
		text = svpelement->element;
		if(text->getVisible() && text->getDisplay() && text->directRender())
		{
			foundVisible = true;
			break;
		}

		svpelement = ++it;
	}

	return foundVisible;
}

void AggText::init()
{
	init(m_text->screenCTM());
}

void AggText::renderCallback(SVGTextContentElementImpl *element, const SVGMatrixImpl *screenCTM, T2P::GlyphSet *glyph, T2P::GlyphLayoutParams *params, double anchor) const
{
	for(unsigned int i = 0; i < glyph->glyphCount(); i++)
	{
		T2P::GlyphAffinePair *glyphAffine = glyph->set().at(i);
		T2P::BezierPathAgg *bpath = const_cast<T2P::BezierPathAgg *>(static_cast<const T2P::BezierPathAgg *>(glyphAffine->transformatedPath()));

		// text-anchor/baseline-shift support
		if(!params->tb())
			bpath->m_transform = agg::trans_affine(screenCTM->a(), screenCTM->b(), screenCTM->c(), screenCTM->d(), screenCTM->e() - anchor * screenCTM->a(), screenCTM->f());
		else
			bpath->m_transform = agg::trans_affine(screenCTM->a(), screenCTM->b(), screenCTM->c(), screenCTM->d(), screenCTM->e(), screenCTM->f() - anchor * screenCTM->d());

		SVPElement *svpelement = new SVPElement();
		svpelement->svp = new BezierPathAggStroked(*bpath, element);
		svpelement->element = element;

		if(element->isFilled())
			svpelement->fillPainter = new AggFillPaintServer(element);

		// Spec: A zero value causes no stroke to be painted.
		if(element->isStroked() && element->getStrokeWidth()->baseVal()->value() > 0)
			svpelement->strokePainter = new AggStrokePaintServer(element);
		m_drawItems.append(svpelement);
	}
}

void AggText::init(const SVGMatrixImpl *screenCTM)
{
	int curx = 0, cury = 0, endx = 0, endy = 0;
	KSVGTextChunk *textChunk = CanvasText::createTextChunk(m_canvas, screenCTM, curx, cury, endx, endy);

	if(textChunk->count() > 0)
		CanvasText::createGlyphs(textChunk, m_canvas, screenCTM, curx, cury, endx, endy);

	delete textChunk;
}

void AggText::clearCurved()
{
	m_drawItems.clear();
	// TODO: Huh - nobody does anything with the *trans* objects?:
}

void AggText::addTextDecoration(SVGTextContentElementImpl *element, double x, double y, double width, double height) const
{
	if(element->isFilled() || element->isStroked())
	{
		// compute rect 
		BezierPathAggStroked *bpath = new BezierPathAggStroked(element);
		bpath->m_storage.move_to(x, y);
		bpath->m_storage.line_to(x + width, y);
		bpath->m_storage.line_to(x + width, y + height);
		bpath->m_storage.line_to(x, y + height);

		const SVGMatrixImpl *mat = m_text->screenCTM();
		bpath->m_transform = agg::trans_affine(mat->a(), mat->b(), mat->c(), mat->d(), mat->e(), mat->f());

		SVPElement *svpelement = new SVPElement();
		svpelement->svp = bpath;
		svpelement->element = element;

		if(element->isFilled())
			svpelement->fillPainter = new AggFillPaintServer(element);

		// Spec: A zero value causes no stroke to be painted.
		if(element->isStroked() && element->getStrokeWidth()->baseVal()->value() > 0)
			svpelement->strokePainter = new AggStrokePaintServer(element);

		m_drawItems.append(svpelement);
	}
}

AggText::SVPElement::~SVPElement()
{
	delete svp;
	delete fillPainter;
	delete strokePainter;
}

// ###

AggGradient::AggGradient(SVGGradientElementImpl *gradient) : m_gradient(gradient)
{
}

void AggGradient::parseGradientStops(SVGGradientElementImpl *gradient)
{
	bool srgb = m_gradient->getColorInterpolation() == CI_SRGB;
	int r = 0, g = 0, b = 0, a = 255, r1 = 0, g1 = 0, b1 = 0, a1 = 255;
	unsigned int end = 255;
	float oldOffset = -1, newOffset = -1;
	for(DOM::Node node = gradient->firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGStopElementImpl *elem = dynamic_cast<SVGStopElementImpl *>(gradient->ownerDoc()->getElementFromHandle(node.handle()));
		if(node.nodeName() == "stop" && elem)
		{
			oldOffset = newOffset;
			newOffset = elem->offset()->baseVal();

			// Spec: skip double offset specifications
			if(oldOffset == newOffset)
				continue;

			//offsets++;

			// Get color
			QColor qStopColor;

			if(elem->getStopColor()->colorType() == SVG_COLORTYPE_CURRENTCOLOR)
				qStopColor = elem->getColor()->rgbColor().color();
			else
				qStopColor = elem->getStopColor()->rgbColor().color();

			// Convert in an agg suitable form
			QString tempName = qStopColor.name();
			const char *str = tempName.latin1();

			// We need to take into account fill/stroke opacity, if available (Rob)
			float opacity = 1.0;
			SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(getBBoxTarget());
			if(style)
				opacity = style->getFillOpacity() * style->getOpacity();
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
			opacity *= elem->stopOpacity();

			// Get rgba color including stop-opacity
			Q_UINT32 rgba = (stopColor << 8) | int(floor(int(opacity * 255.0) + 0.5));

			// Convert from separated to premultiplied alpha
			a = rgba & 0xff;
			r = !srgb ? linearRGBFromsRGB((rgba >> 24)) : (rgba >> 24);
			g = !srgb ? linearRGBFromsRGB(((rgba >> 16) & 0xff)) : (rgba >> 16) & 0xff;
			b = !srgb ? linearRGBFromsRGB(((rgba >> 8) & 0xff)) : (rgba >> 8) & 0xff;

			end = int(newOffset * 255);
			// interpolate
			unsigned int start = (oldOffset == -1) ? 0 : int(oldOffset * 255);
			if(oldOffset == -1)
			{
				r1 = r;
				g1 = g;
				b1 = b;
				a1 = a;
			}
			int diffr = r - r1;
			int diffg = g - g1;
			int diffb = b - b1;
			int diffa = a - a1;
			unsigned int nsteps = end - start;
			for(unsigned int i = 0;i <= nsteps;i++)
			{
				double diff = double(i) / double(nsteps);
				m_colorprofile[start + i].r = !srgb ? sRGBFromLinearRGB(int(r1 + diff * diffr)) : int(r1 + diff * diffr);
				m_colorprofile[start + i].g = !srgb ? sRGBFromLinearRGB(int(g1 + diff * diffg)) : int(g1 + diff * diffg);
				m_colorprofile[start + i].b = !srgb ? sRGBFromLinearRGB(int(b1 + diff * diffb)) : int(b1 + diff * diffb);
				m_colorprofile[start + i].a = !srgb ? sRGBFromLinearRGB(int(a1 + diff * diffa)) : int(a1 + diff * diffa);
			}
			r1 = r;
			g1 = g;
			b1 = b;
			a1 = a;
		}
	}
	// last section
	for(unsigned int i = end;i <= 255;i++)
	{
		m_colorprofile[i].r = r;
		m_colorprofile[i].g = g;
		m_colorprofile[i].b = b;
		m_colorprofile[i].a = a;
	}
}

void AggGradient::finalizePaintServer()
{
	parseGradientStops(m_gradient->stopsSource());

	QString _href = SVGURIReferenceImpl::getTarget(m_gradient->href()->baseVal().string());
	if(!_href.isEmpty())
		reference(_href);

	setFinalized();
}

void AggGradient::reference(const QString &/*href*/)
{
}

void AggLinearGradient::render(AggCanvas *c)
{
	SVGLinearGradientElementImpl *linear = dynamic_cast<SVGLinearGradientElementImpl *>(m_gradient);

	linear->converter()->finalize(getBBoxTarget(), linear->ownerSVGElement(), linear->gradientUnits()->baseVal());

	double _x1 = linear->x1()->baseVal()->value();
	double _y1 = linear->y1()->baseVal()->value();
	double _x2 = linear->x2()->baseVal()->value();
	double _y2 = linear->y2()->baseVal()->value();

	// Adjust to gradient transform
	SVGMatrixImpl *gradTrans = linear->gradientTransform()->baseVal()->concatenate();
	if(gradTrans)
	{
		QWMatrix m = gradTrans->qmatrix();
		m.map(_x1, _y1, &_x1, &_y1);
		m.map(_x2, _y2, &_x2, &_y2);
		gradTrans->deref();
	}

	// Get the basic bbox that will be the rendering area
	SVGRectImpl *userBBox = getBBoxTarget()->getBBox();

	// Compute x1, y1, x2 and y2
	bool objectbbox = (linear->gradientUnits()->baseVal() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);

	// Respect current transformation matrix (so gradients zoom with...)
	SVGTransformableImpl *transformable = dynamic_cast<SVGTransformableImpl *>(getBBoxTarget());
	const SVGMatrixImpl *matrix = 0;
	if(transformable)
		matrix = transformable->screenCTM();

	if(objectbbox)
	{
		_x1 += userBBox->x();
		_y1 += userBBox->y();
		_x2 += userBBox->x();
		_y2 += userBBox->y();
	}

	userBBox->deref();

	gradient_polymorphic_wrapper_base* gr_ptr = &m_linPad;
	if(linear->spreadMethod()->baseVal() == SVG_SPREADMETHOD_REPEAT)
		gr_ptr = &m_linRepeat;
	else if(linear->spreadMethod()->baseVal() == SVG_SPREADMETHOD_REFLECT)
		gr_ptr = &m_linReflect;

	agg::trans_affine mtx_g1;
	double dx = _x2 - _x1;
	double dy = _y2 - _y1;
	double angle = (atan2(dy, dx));
	mtx_g1 *= agg::trans_affine_rotation(angle);
	mtx_g1 *= agg::trans_affine_translation(_x1, _y1);
	mtx_g1 *= agg::trans_affine(matrix->a(), matrix->b(), matrix->c(), matrix->d(), matrix->e(), matrix->f());
	mtx_g1.invert();

	double len = sqrt(dx * dx + dy * dy);
	if(len > 0)
	{
		typedef agg::span_interpolator_linear<> interpolator_type;
		typedef agg::span_gradient<agg::rgba8,
									interpolator_type,
									gradient_polymorphic_wrapper_base,
									color_function_profile> gradient_span_gen;
		typedef agg::span_allocator<gradient_span_gen::color_type> gradient_span_alloc;
		color_function_profile colors(m_colorprofile);
		gradient_span_alloc span_alloc;
		interpolator_type inter(mtx_g1);
		gradient_span_gen span_gen(span_alloc, inter, *gr_ptr, colors, 0, len);

		agg::scanline_u8 sl;

		if(c->nrChannels() == 3)
		{
			typedef agg::pixfmt_rgb24 pixfmt;
			typedef agg::renderer_base<pixfmt> renderer_base;
			typedef agg::renderer_scanline_u<renderer_base, gradient_span_gen> renderer_gradient;

			pixfmt pixf(c->buf());
			renderer_base rb(pixf);

			renderer_gradient r1(rb, span_gen);
			c->m_ras.render(sl, r1);
		}
		else
		{
			typedef agg::pixfmt_rgba32 pixfmt;
			typedef agg::renderer_base<pixfmt> renderer_base;
			typedef agg::renderer_scanline_u<renderer_base, gradient_span_gen> renderer_gradient;

			pixfmt pixf(c->buf());
			renderer_base rb(pixf);

			renderer_gradient r1(rb, span_gen);
			c->m_ras.render(sl, r1);
		}
	}
}

void AggRadialGradient::render(AggCanvas *c)
{
	SVGRadialGradientElementImpl *radial = dynamic_cast<SVGRadialGradientElementImpl *>(m_gradient);

	radial->converter()->finalize(getBBoxTarget(), radial->ownerSVGElement(), radial->gradientUnits()->baseVal());

	double _cx = radial->cx()->baseVal()->value();
	double _cy = radial->cy()->baseVal()->value();
	double _fx = radial->fx()->baseVal()->value();
	double _fy = radial->fy()->baseVal()->value();
	double _r = radial->r()->baseVal()->value();

	// Get the basic bbox that will be the rendering area
	SVGRectImpl *screenBBox = getBBoxTarget()->getBBoxInternal();

	if(screenBBox->width() == 0 || screenBBox->height() == 0)
	{
		screenBBox->deref();
		return;
	}

	screenBBox->deref();

	// Compute x1, y1, x2 and y2
	bool objectbbox = (radial->gradientUnits()->baseVal() == SVGUnitTypes::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);

	SVGRectImpl *userBBox = getBBoxTarget()->getBBox();
	float width = userBBox->width();
	float height = userBBox->height();

	if(objectbbox)
	{
		_cx += userBBox->x();
		_cy += userBBox->y();
		_fx += userBBox->x();
		_fy += userBBox->y();
	}

	gradient_polymorphic_wrapper_base* gr_ptr = &m_radialPad;
	if(radial->spreadMethod()->baseVal() == SVG_SPREADMETHOD_REPEAT)
		gr_ptr = &m_radialRepeat;
	else if(radial->spreadMethod()->baseVal() == SVG_SPREADMETHOD_REFLECT)
		gr_ptr = &m_radialReflect;

	agg::trans_affine mtx_g1;


	// Adjust to gradient transform
	SVGMatrixImpl *gradTrans = radial->gradientTransform()->baseVal()->concatenate();
	if(gradTrans)
	{
		agg::trans_affine mtx;
		QWMatrix m = gradTrans->qmatrix();
		mtx = agg::trans_affine(m.m11(), m.m12(), m.m21(), m.m22(), m.dx(), m.dy());
		gradTrans->deref();
		mtx_g1 *= mtx;
	}

	userBBox->deref();

	int diff = int(width - height); // allow slight tolerance
	if(objectbbox && !(diff > -2 && diff < 2))
	{
		// make elliptical or circular depending on bbox aspect ratio
		float ratioX = (width / height) * sqrt(2);
		float ratioY = (height / width) * sqrt(2);
		mtx_g1 *= agg::trans_affine_scaling((width > height) ? sqrt(2) : ratioX, (width > height) ? ratioY :sqrt(2));
	}

	mtx_g1 *= agg::trans_affine_translation(_cx, _cy);

	// Respect current transformation matrix (so gradients zoom with...)
	SVGTransformableImpl *transformable = dynamic_cast<SVGTransformableImpl *>(getBBoxTarget());
	const SVGMatrixImpl *matrix = 0;
	if(transformable)
		matrix = transformable->screenCTM();

	if(!matrix)
		return;

	mtx_g1 *= agg::trans_affine(matrix->a(), matrix->b(), matrix->c(), matrix->d(), matrix->e(), matrix->f());

	mtx_g1.invert();

	typedef agg::span_interpolator_linear<> interpolator_type;
	typedef agg::span_gradient<agg::rgba8,
								interpolator_type,
								gradient_polymorphic_wrapper_base,
								color_function_profile> gradient_span_gen;
	typedef agg::span_allocator<gradient_span_gen::color_type> gradient_span_alloc;
	color_function_profile colors(m_colorprofile);
	gradient_span_alloc span_alloc;
	interpolator_type inter(mtx_g1);
	gradient_span_gen span_gen(span_alloc, inter, *gr_ptr, colors, 0, _r);

	agg::scanline_u8 sl;
	if(c->nrChannels() == 3)
	{
		typedef agg::pixfmt_rgb24 pixfmt;
		typedef agg::renderer_base<pixfmt> renderer_base;
		typedef agg::renderer_scanline_u<renderer_base, gradient_span_gen> renderer_gradient;

		pixfmt pixf(c->buf());
		renderer_base rb(pixf);

		renderer_gradient r1(rb, span_gen);
		c->m_ras.render(sl, r1);
	}
	else
	{
		typedef agg::pixfmt_rgba32 pixfmt;
		typedef agg::renderer_base<pixfmt> renderer_base;
		typedef agg::renderer_scanline_u<renderer_base, gradient_span_gen> renderer_gradient;

		pixfmt pixf(c->buf());
		renderer_base rb(pixf);

		renderer_gradient r1(rb, span_gen);
		c->m_ras.render(sl, r1);
	}
}

AggPattern::AggPattern(SVGPatternElementImpl *pattern) : AggPaintServer(), m_pattern(pattern)
{
}

void AggPattern::finalizePaintServer()
{
	m_pattern->finalizePaintServer();
	setFinalized();
}

void AggPattern::reference(const QString &href)
{
	m_pattern->reference(href);
}

void AggPattern::render(AggCanvas *c)
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

		typedef agg::pixfmt_rgb24 pixfmt;
		typedef agg::renderer_base<pixfmt> renderer_base;
		typedef agg::renderer_scanline_p_solid<renderer_base> renderer_solid;

		pixfmt pixf(c->buf());
		renderer_base rb(pixf);
		renderer_solid rs(rb);
		//double width = c->buf().width();
		//double height = c->buf().height();

		typedef agg::span_pattern_rgba32<agg::order_rgba32> span_gen_type;
		typedef agg::renderer_scanline_u<renderer_base, span_gen_type> renderer_type;

		SVGRectImpl *screenBBox = getBBoxTarget()->getBBoxInternal();
		double offset_x = affine[4];
		double offset_y = affine[5];
		screenBBox->deref();
		agg::rendering_buffer m_pattern_rbuf;
		m_pattern_rbuf.attach(tile.image().bits(), tile.image().width(), tile.image().height(), tile.image().width() * 4);
		agg::span_allocator<agg::rgba8> sa;
	        span_gen_type sg(sa, m_pattern_rbuf, unsigned(offset_x), unsigned(offset_y));

		renderer_type rp(rb, sg);

		agg::scanline_u8 sl;
		rs.color(agg::rgba(0,0,0));
		c->m_ras.render(sl, rp);
	}
}

// vim:ts=4:noet
