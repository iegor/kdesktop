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

#ifndef AGGCANVASITEMS_H
#define AGGCANVASITEMS_H

#include "CanvasItem.h"
#include "CanvasItems.h"
#include "BezierPathAgg.h"

#include "SVGPathElementImpl.h"
#include "SVGPolyElementImpl.h"
#include "SVGLineElementImpl.h"
#include "SVGRectElementImpl.h"
#include "SVGTextElementImpl.h"
#include "SVGImageElementImpl.h"
#include "SVGCircleElementImpl.h"
#include "SVGEllipseElementImpl.h"

#include "agg_basics.h"
#include "agg_conv_transform.h"
#include "agg_conv_curve.h"
#include "agg_conv_dash.h"
#include "agg_conv_stroke.h"
#include "agg_conv_clip_polygon.h"
#include "agg_span_gradient.h"

#include "SVGBBoxTarget.h"

// gradient helpers

//------------------------------------------------------------------------
class gradient_polymorphic_wrapper_base
{
public:
    virtual int calculate(int x, int y, int) const = 0;
};

template<class GradientF>
class gradient_polymorphic_wrapper : public gradient_polymorphic_wrapper_base
{
public:
    virtual int calculate(int x, int y, int d) const
    {
        return m_gradient.calculate(x, y, d);
    }
private:
    GradientF m_gradient;
};

class gradient_radial_repeat
{
public:
    int calculate(int x, int y, int d) const
    {
	return int(sqrt(pow(x, 2) + pow(y, 2))) % d;
    }
};

class gradient_radial_reflect
{
public:
    int calculate(int x, int y, int d) const
    {
	int dist = int(sqrt(pow(x, 2) + pow(y, 2)));
	if((dist / d) % 2 == 0)
		return dist % d;
	else
		return d - (dist % d);
    }
};

class gradient_linear_repeat
{
public:
    int calculate(int x, int y, int d) const
    {
        return (x < 0) ? (d - (-x % d)) : (x % d);
    }
};

class gradient_linear_reflect
{
public:
    int calculate(int x, int y, int d) const
    {
	if((abs(x) / d) % 2 == 0)
	        return (x < 0) ? (-x % d) : (x % d);
	else
	        return (x > 0) ? (d - (x % d)) : (d - (-x % d));
    }
};


#define AGG_CLASS(Class, Type, Member) \
class Agg##Class : public AggShape \
{ \
public: \
    Agg##Class(AggCanvas *c, Type *Member); \
    virtual ~Agg##Class() { } \
	virtual void draw(); \
	virtual bool isVisible(); \
	virtual void init(); \
	virtual void init(const SVGMatrixImpl *screenCTM); \
	virtual SVGElementImpl *element() const { return m_##Member; } \
protected: \
    Type *m_##Member; \
};

class stroke_dash_base
{
public:
	virtual ~stroke_dash_base() {}

	virtual void     rewind(unsigned) = 0;
        virtual unsigned vertex(double*, double*) = 0;
};

template<class Source>
class stroke : public stroke_dash_base
{
public:
	typedef agg::conv_stroke<Source> stroke_type;

	stroke(Source& src, KSVG::SVGStylableImpl *style);

	void rewind(unsigned id) { m_s.rewind(id); }
	unsigned vertex(double* x, double* y) { return m_s.vertex(x, y); }

private:
	stroke_type m_s;
};

template<class Source>
class dash_stroke : public stroke_dash_base
{
public:
	typedef agg::conv_dash<Source> dash_type;
	typedef agg::conv_stroke<dash_type> dash_stroke_type;

	dash_stroke(Source& src, KSVG::SVGStylableImpl *style);

	void rewind(unsigned id) { m_ds.rewind(id); }
	unsigned vertex(double* x, double* y) { return m_ds.vertex(x, y); }

private:
	dash_type m_d;
	dash_stroke_type m_ds;
};

template<class Source>
class dash_stroke_simple
{
public:
	dash_stroke_simple(Source& src, KSVG::SVGStylableImpl *style);
	~dash_stroke_simple() { delete impl; }

	void rewind(unsigned id) { impl->rewind(id); }
	unsigned vertex(double* x, double* y) { return impl->vertex(x, y); }

private:
	stroke_dash_base *impl;
};

typedef dash_stroke_simple<curved>                     curved_stroked;
typedef agg::conv_transform<curved_stroked>          curved_stroked_trans;
typedef agg::conv_clip_polygon<curved_stroked_trans> curved_stroked_trans_clipped;
typedef agg::conv_clip_polygon<curved_trans>         curved_trans_clipped;
//typedef agg::conv_contour<curved_trans>              curved_trans_contour;
//typedef agg::conv_clip_polygon<curved_trans_contour> curved_trans_contour_clipped;

namespace KSVG
{
	class SVGGradientElementImpl;
	class SVGRadialGradientElementImpl;
	class SVGLinearGradientElementImpl;
	class SVGPatternElementImpl;

	class AggPaintServer : public CanvasPaintServer
	{
	public:
		AggPaintServer() : CanvasPaintServer() {}
		virtual ~AggPaintServer() {}

		virtual void finalizePaintServer() = 0;
		virtual void reference(const QString &href) = 0;

		virtual void render(AggCanvas *c) = 0;
	};

	class AggGradient : public AggPaintServer
	{
	public:
		AggGradient(SVGGradientElementImpl *gradient);
		virtual ~AggGradient() {}

		void parseGradientStops(SVGGradientElementImpl *gradient);

		virtual void finalizePaintServer();
		virtual void reference(const QString &href);

	protected:
		SVGGradientElementImpl *m_gradient;
		agg::rgba8 m_colorprofile[256];
	};

	class AggLinearGradient : public AggGradient
	{
	public:
		AggLinearGradient(SVGLinearGradientElementImpl *linear) : AggGradient(linear) {}

		virtual void render(AggCanvas *c);

	protected:
		gradient_polymorphic_wrapper<agg::gradient_x> m_linPad;
		gradient_polymorphic_wrapper<gradient_linear_repeat> m_linRepeat;
		gradient_polymorphic_wrapper<gradient_linear_reflect> m_linReflect;
	};

	class AggRadialGradient : public AggGradient
	{
	public:
		AggRadialGradient(SVGRadialGradientElementImpl *radial) : AggGradient(radial) {}

		virtual void render(AggCanvas *c);

	protected:
		gradient_polymorphic_wrapper<agg::gradient_circle> m_radialPad;
		gradient_polymorphic_wrapper<gradient_radial_repeat> m_radialRepeat;
		gradient_polymorphic_wrapper<gradient_radial_reflect> m_radialReflect;
	};

	class AggPattern : public AggPaintServer
	{
	public:
		AggPattern(SVGPatternElementImpl *pattern);
		virtual ~AggPattern() {}

		virtual void finalizePaintServer();
		virtual void reference(const QString &);

		virtual void render(AggCanvas *c);

	protected:
		SVGPatternElementImpl *m_pattern;
	};

	class AggCanvas;

	class AggFillPaintServer
	{
	public:
		AggFillPaintServer(SVGStylableImpl *style);
		void update(SVGStylableImpl *style);
		template<class VertexSource>
		void draw(AggCanvas *canvas, VertexSource &vs, SVGStylableImpl *style, SVGShapeImpl *shape);

	private:
		agg::rgba8 m_color;
	};

	class AggStrokePaintServer
	{
	public:
		AggStrokePaintServer(SVGStylableImpl *style);
		void update(SVGStylableImpl *style);
		template<class VertexSource>
		void draw(AggCanvas *canvas, VertexSource &vs, SVGStylableImpl *style, SVGShapeImpl *shape);

	private:
		agg::rgba8 m_color;
	};

	class BezierPathAggStroked : public T2P::BezierPathAgg
	{
	public:
		BezierPathAggStroked(SVGStylableImpl *style);
		BezierPathAggStroked(const T2P::BezierPathAgg &other, SVGStylableImpl *style);

		curved_trans_clipped			m_curved_trans_clipped;
		curved_stroked					m_curved_stroked;
		curved_stroked_trans			m_curved_stroked_trans;
		curved_stroked_trans_clipped	m_curved_stroked_trans_clipped;
		SVGStylableImpl					*m_style;
	};

	class AggShape : public CanvasItem, public BezierPathAggStroked
	{
	public:
		AggShape(AggCanvas *c, SVGStylableImpl *style);
		virtual ~AggShape();

		virtual QRect bbox() const;
		virtual bool fillContains(const QPoint &p);
		virtual bool strokeContains(const QPoint &p);
		virtual void update(CanvasItemUpdate reason, int param1 = 0, int param2 = 0);
		void draw(SVGShapeImpl *shape);
		void calcSVPs(const SVGMatrixImpl *matrix);
		virtual void init();
		virtual void init(const SVGMatrixImpl *);
		bool isVisible(SVGShapeImpl *shape);

		void setRenderContext(RenderContext context) { m_context = context; }

		agg::path_storage &storage() { return m_storage; }
		agg::trans_affine &transform() { return m_transform; }

	protected:
		void freeSVPs();

		RenderContext			m_context;
		AggCanvas			*m_canvas;
		QRect				m_bbox;
		AggFillPaintServer		*m_fillPainter;
		AggStrokePaintServer		*m_strokePainter;
	};

	AGG_CLASS(Rectangle, SVGRectElementImpl, rect)
	AGG_CLASS(Ellipse, SVGEllipseElementImpl, ellipse)
	AGG_CLASS(Circle, SVGCircleElementImpl, circle)

	class AggLine : public AggShape, public MarkerHelper
	{
	public:
		AggLine(AggCanvas *c, SVGLineElementImpl *line);
		virtual ~AggLine();

		virtual void draw();
		virtual bool isVisible();
		virtual void init();
		virtual void init(const SVGMatrixImpl *screenCTM);

		virtual SVGElementImpl *element() const { return m_line; }

	protected:
		SVGLineElementImpl *m_line;
	};

	class AggPoly : public AggShape, public MarkerHelper
	{
	public:
		AggPoly(AggCanvas *c, SVGPolyElementImpl *poly);
		virtual ~AggPoly();

		virtual void draw();
		virtual bool isVisible();
		virtual void init();
		virtual void init(const SVGMatrixImpl *screenCTM) = 0;

		virtual SVGElementImpl *element() const { return m_poly; }

	protected:
		SVGPolyElementImpl *m_poly;

	};
	class AggPolyline : public AggPoly
	{
	public:
		AggPolyline(AggCanvas *c, SVGPolylineElementImpl *poly);
		virtual ~AggPolyline();

		virtual void init(const SVGMatrixImpl *screenCTM);
	};

	class AggPolygon : public AggPoly
	{
	public:
		AggPolygon(AggCanvas *c, SVGPolygonElementImpl *poly);
		virtual ~AggPolygon();

		virtual void init(const SVGMatrixImpl *screenCTM);
	};

	class AggPath : public AggShape, public ::SVGPathParser, public MarkerHelper
	{
	public:
		AggPath(AggCanvas *c, SVGPathElementImpl *path);
		virtual ~AggPath();

		virtual void draw();
		virtual bool isVisible();
		virtual void init();
		virtual void init(const SVGMatrixImpl *screenCTM);

		virtual SVGElementImpl *element() const { return m_path; }

	protected:
		SVGPathElementImpl *m_path;

		virtual void svgMoveTo(double x1, double y1, bool closed, bool abs = true);
		virtual void svgLineTo(double x1, double y1, bool abs = true);
		virtual void svgCurveToCubic(double x1, double y1, double x2, double y2, double x, double y, bool abs = true);
		virtual void svgClosePath();
	};

	class AggMarker : public CanvasMarker
	{
	public:
		AggMarker(AggCanvas *c, SVGMarkerElementImpl *marker);
		virtual ~AggMarker();

		virtual QRect bbox() const { return QRect(); }
		virtual bool fillContains(const QPoint &) { return true; }
		virtual bool strokeContains(const QPoint &) { return true; }
		virtual void update(CanvasItemUpdate, int = 0, int = 0) { }
		virtual void init();
		virtual void draw();
		virtual bool isVisible() { return false; }

		void draw(SVGShapeImpl *obj, int x, int y, float lwidth = 1.0, double angle = 0.0);
		//const ArtSVP *clippingRectangle() const { return m_clippingRectangle; }

	protected:
		AggCanvas *m_canvas;
		//ArtSVP *m_clippingRectangle;
	};

	class AggImage : public CanvasItem
	{
	public:
		AggImage(AggCanvas *c, SVGImageElementImpl *image);
		virtual ~AggImage();

		virtual QRect bbox() const;
		virtual bool fillContains(const QPoint &) { return true; }
		virtual bool strokeContains(const QPoint &) { return true; }
		virtual void update(CanvasItemUpdate, int = 0, int = 0) { }
		virtual void init();
		virtual void draw();
		virtual bool isVisible();

		virtual SVGElementImpl *element() const { return m_image; }

	protected:
		AggCanvas *m_canvas;
		SVGImageElementImpl *m_image;
	};

	class AggText : public CanvasText
	{
	public:
		AggText(AggCanvas *c, SVGTextElementImpl *text);
		virtual ~AggText();

		virtual QRect bbox() const;
		virtual bool fillContains(const QPoint &p);
		virtual bool strokeContains(const QPoint &p);
		virtual void update(CanvasItemUpdate, int param1 = 0, int param2 = 0);
		virtual void draw();
		virtual bool isVisible();
		virtual void init();
		virtual void init(const SVGMatrixImpl *screenCTM);

		virtual void renderCallback(SVGTextContentElementImpl *element, const SVGMatrixImpl *screenCTM, T2P::GlyphSet *glyph, T2P::GlyphLayoutParams *params, double anchor) const;

		virtual void addTextDecoration(SVGTextContentElementImpl *element, double x, double y, double w, double h) const;

		unsigned operator [](unsigned) const { return 0; }

	protected:
		AggCanvas *m_canvas;

	private:
		void clearCurved();
		class SVPElement
		{
		public:
			SVPElement() { svp = 0; element = 0; fillPainter = 0; strokePainter = 0; } 
			~SVPElement();

			BezierPathAggStroked		*svp;
			SVGTextContentElementImpl 	*element;
			AggFillPaintServer			*fillPainter;
			AggStrokePaintServer		*strokePainter;
		};

		mutable QPtrList<SVPElement> m_drawItems;
	};
};

#endif

// vim:ts=4:noet
