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

#ifndef LIBARTCANVASITEMS_H
#define LIBARTCANVASITEMS_H

#include <qptrlist.h>

#include "CanvasItems.h"
#include "LibartCanvas.h"
#include "BezierPathLibart.h"

#include "SVGPathElementImpl.h"
#include "SVGPolyElementImpl.h"
#include "SVGLineElementImpl.h"
#include "SVGRectElementImpl.h"
#include "SVGTextElementImpl.h"
#include "SVGCircleElementImpl.h"
#include "SVGEllipseElementImpl.h"

// Helpers
#define allocVPath(n) art_new(ArtVpath, n)
#define allocBPath(n) art_new(ArtBpath, n)

#define LIBART_CLASS(Class, Type, Member) \
class Libart##Class : public LibartShape \
{ \
public: \
    Libart##Class(LibartCanvas *c, Type *Member); \
    virtual ~Libart##Class() { } \
	virtual void draw(); \
	virtual bool isVisible(); \
	virtual void init(); \
	virtual void init(const SVGMatrixImpl *screenCTM); \
	virtual SVGElementImpl *element() const { return m_##Member; } \
protected: \
    Type *m_##Member; \
};

struct _ArtSVP;
struct _ArtBpath;
struct _ArtRender;
struct _ArtGradientStop;

namespace KSVG
{
	class SVGImageElementImpl;
	class SVGGradientElementImpl;
	class SVGRadialGradientElementImpl;
	class SVGLinearGradientElementImpl;
	class SVGPatternElementImpl;

	class LibartPaintServer : public CanvasPaintServer
	{
	public:
		LibartPaintServer() : CanvasPaintServer() {}
		virtual ~LibartPaintServer() {}

		virtual void render(KSVGCanvas *c, _ArtSVP *svp, float opacity, QByteArray mask, QRect screenBBox) = 0;

		_ArtRender *createRenderer(QRect rect, KSVGCanvas *c);
	};

	class LibartGradient : public LibartPaintServer
	{
	public:
		LibartGradient(SVGGradientElementImpl *gradient) : m_gradient(gradient) {}
		virtual ~LibartGradient() {}

		void parseGradientStops(SVGGradientElementImpl *gradient);

		virtual void finalizePaintServer();
		virtual void reference(const QString &href);

	protected:
		SVGGradientElementImpl *m_gradient;
		QMemArray<_ArtGradientStop> m_stops;
	};

	class LibartLinearGradient : public LibartGradient
	{
	public:
		LibartLinearGradient(SVGLinearGradientElementImpl *linear) : LibartGradient(linear), m_linear(linear) {}

		virtual void render(KSVGCanvas *c, _ArtSVP *svp, float opacity, QByteArray mask, QRect screenBBox);

	protected:
		SVGLinearGradientElementImpl *m_linear;
	};

	class LibartRadialGradient : public LibartGradient
	{
	public:
		LibartRadialGradient(SVGRadialGradientElementImpl *radial) : LibartGradient(radial), m_radial(radial) {}

		virtual void render(KSVGCanvas *c, _ArtSVP *svp, float opacity, QByteArray mask, QRect screenBBox);

	protected:
		SVGRadialGradientElementImpl *m_radial;
	};

	class LibartPattern : public LibartPaintServer
	{
	public:
		LibartPattern(SVGPatternElementImpl *pattern);
		virtual ~LibartPattern() {}

		virtual void finalizePaintServer();
		virtual void reference(const QString &href);

		virtual void render(KSVGCanvas *c, _ArtSVP *svp, float opacity, QByteArray mask, QRect screenBBox);

	protected:
		SVGPatternElementImpl *m_pattern;
	};

	class LibartPainter
	{
	public:
		LibartPainter() { m_color = 0; }
		virtual ~LibartPainter() {}
		
		void update(SVGStylableImpl *style);
		void draw(LibartCanvas *canvas, _ArtSVP *svp, SVGStylableImpl *style, SVGShapeImpl *shape);

		virtual float opacity(SVGStylableImpl *style) const = 0;
		virtual unsigned short paintType(SVGStylableImpl *style) const = 0;
		virtual QString paintUri(SVGStylableImpl *style) const = 0;
		virtual QRgb color(SVGStylableImpl *style) const = 0;

		protected:
			art_u32 m_color;
	};

	class LibartFillPainter : public LibartPainter
	{
	public:
		LibartFillPainter(SVGStylableImpl *style);

		float opacity(SVGStylableImpl *style) const { return style->getFillOpacity() * style->getOpacity(); }
		unsigned short paintType(SVGStylableImpl *style) const { return style->getFillColor()->paintType(); }
		QString paintUri(SVGStylableImpl *style) const { return style->getFillColor()->uri().string(); }
		QRgb color(SVGStylableImpl *style) const { return style->getFillColor()->rgbColor().color(); }
	};

	class LibartStrokePainter : public LibartPainter
	{
	public:
		LibartStrokePainter(SVGStylableImpl *style);

		float opacity(SVGStylableImpl *style) const { return style->getStrokeOpacity() * style->getOpacity(); }
		unsigned short paintType(SVGStylableImpl *style) const { return style->getStrokeColor()->paintType(); }
		QString paintUri(SVGStylableImpl *style) const { return style->getStrokeColor()->uri().string(); }
		QRgb color(SVGStylableImpl *style) const { return style->getStrokeColor()->rgbColor().color(); }
	};

	class LibartClipItem
	{
	public:
		LibartClipItem() { m_context = NORMAL; }
		virtual ~LibartClipItem() {}

		virtual void initClipItem() = 0;
		void setRenderContext(RenderContext context) { m_context = context; }
		virtual ArtSVP *clipSVP() = 0;

	protected:
		RenderContext m_context;
	};

	class LibartShape : public CanvasItem, public LibartClipItem
	{
	public:
		LibartShape(LibartCanvas *c, SVGStylableImpl *style);
		virtual ~LibartShape();

		virtual QRect bbox() const;
		virtual bool fillContains(const QPoint &p);
		virtual bool strokeContains(const QPoint &p);
		virtual void update(CanvasItemUpdate reason, int param1 = 0, int param2 = 0);
		void draw(SVGShapeImpl *shape);
		bool isVisible(SVGShapeImpl *shape);

		virtual void init();
		virtual void init(const SVGMatrixImpl *);
		virtual void reset() { freeSVPs(); init(); }

		void initClipItem();
		ArtSVP *clipSVP();

		static void calcClipSVP(ArtVpath *vec, SVGStylableImpl *style, const SVGMatrixImpl *matrix, _ArtSVP **clipSVP);
		static void calcSVPs(ArtVpath *vec, SVGStylableImpl *style, const SVGMatrixImpl *matrix, _ArtSVP **strokeSVP, _ArtSVP **fillSVP);
		static void calcSVPs(_ArtBpath *bpath, SVGStylableImpl *style, const SVGMatrixImpl *matrix, _ArtSVP **strokeSVP, _ArtSVP **fillSVP);

	protected:
		void freeSVPs();
		static void calcSVPInternal(ArtVpath *vec, SVGStylableImpl *style, double *affine, ArtSVP **strokeSVP, ArtSVP **fillSVP);

		_ArtSVP *m_fillSVP;
		_ArtSVP *m_strokeSVP;
		LibartFillPainter *m_fillPainter;
		LibartStrokePainter *m_strokePainter;
		LibartCanvas *m_canvas;
		SVGStylableImpl *m_style;
	};

	LIBART_CLASS(Rectangle, SVGRectElementImpl, rect)
	LIBART_CLASS(Ellipse, SVGEllipseElementImpl, ellipse)
	LIBART_CLASS(Circle, SVGCircleElementImpl, circle)

	class LibartLine : public LibartShape, public MarkerHelper
	{
	public:
		LibartLine(LibartCanvas *c, SVGLineElementImpl *line);
		virtual ~LibartLine();

		virtual void draw();
		virtual bool isVisible();
		virtual void init();
		virtual void init(const SVGMatrixImpl *screenCTM);

		virtual SVGElementImpl *element() const { return m_line; }

	protected:
		SVGLineElementImpl *m_line;
	};

	class LibartPoly : public LibartShape, public MarkerHelper
	{
	public:
		LibartPoly(LibartCanvas *c, SVGPolyElementImpl *poly);
		virtual ~LibartPoly();

		virtual void draw();
		virtual bool isVisible();
		virtual void init();
		virtual void init(const SVGMatrixImpl *screenCTM) = 0;

		virtual SVGElementImpl *element() const { return m_poly; }

	protected:
		SVGPolyElementImpl *m_poly;

	};
	class LibartPolyline : public LibartPoly
	{
	public:
		LibartPolyline(LibartCanvas *c, SVGPolylineElementImpl *poly);
		virtual ~LibartPolyline();

		virtual void init(const SVGMatrixImpl *screenCTM);
	};

	class LibartPolygon : public LibartPoly
	{
	public:
		LibartPolygon(LibartCanvas *c, SVGPolygonElementImpl *poly);
		virtual ~LibartPolygon();

		virtual void init(const SVGMatrixImpl *screenCTM);
	};

	class LibartPath : public LibartShape, public MarkerHelper, public T2P::BezierPathLibart, public ::SVGPathParser
	{
	public:
		LibartPath(LibartCanvas *c, SVGPathElementImpl *path);
		virtual ~LibartPath();

		virtual void draw();
		virtual bool isVisible();
		virtual void reset();
		virtual void init();
		virtual void init(const SVGMatrixImpl *screenCTM);

		virtual SVGElementImpl *element() const { return m_path; }

	protected:
		friend class LibartCanvas;
		SVGPathElementImpl *m_path;

		virtual void svgMoveTo(double x1, double y1, bool closed, bool abs = true);
		virtual void svgLineTo(double x1, double y1, bool abs = true);
		virtual void svgCurveToCubic(double x1, double y1, double x2, double y2, double x, double y, bool abs = true);
		virtual void svgClosePath();
	};

	class LibartClipPath : public CanvasClipPath
	{
	public:
		LibartClipPath(LibartCanvas *c, SVGClipPathElementImpl *clipPath);
		virtual ~LibartClipPath();

		virtual QRect bbox() const { return QRect(); }
		virtual bool fillContains(const QPoint &) { return true; }
		virtual bool strokeContains(const QPoint &) { return true; }
		virtual void update(CanvasItemUpdate, int param1 = 0, int param2 = 0);
		virtual void init();
		virtual void draw();
		virtual bool isVisible() { return false; }

		_ArtSVP *clipSVP();

	protected:
		LibartCanvas *m_canvas;
		_ArtSVP *m_clipSVP;

		QPtrList<CanvasItem> m_clipItems;
	};

	class LibartImage : public CanvasItem
	{
	public:
		LibartImage(LibartCanvas *c, SVGImageElementImpl *image);
		virtual ~LibartImage();

		virtual QRect bbox() const;
		virtual bool fillContains(const QPoint &) { return true; }
		virtual bool strokeContains(const QPoint &) { return true; }
		virtual void update(CanvasItemUpdate, int = 0, int = 0) { }
		virtual void init();
		virtual void draw();
		virtual bool isVisible();

		// We can't include SVGImageElementImpl.h here
		// because of compiliation errors (X11 headers!)
		virtual SVGElementImpl *element() const { return reinterpret_cast<SVGElementImpl *>(m_image); }

	protected:
		LibartCanvas *m_canvas;
		SVGImageElementImpl *m_image;
	};

	class LibartMarker : public CanvasMarker
	{
	public:
		LibartMarker(LibartCanvas *c, SVGMarkerElementImpl *marker);
		virtual ~LibartMarker();

		virtual QRect bbox() const { return QRect(); }
		virtual bool fillContains(const QPoint &) { return true; }
		virtual bool strokeContains(const QPoint &) { return true; }
		virtual void update(CanvasItemUpdate, int = 0, int = 0) { }
		virtual void init();
		virtual void draw();
		virtual bool isVisible() { return false; }

	protected:
		LibartCanvas *m_canvas;
	};
	
	class LibartText : public CanvasText, public LibartClipItem
	{
	public:
		LibartText(LibartCanvas *c, SVGTextElementImpl *text);
		virtual ~LibartText();

		virtual QRect bbox() const;
		virtual bool fillContains(const QPoint &p);
		virtual bool strokeContains(const QPoint &p);
		virtual void update(CanvasItemUpdate reason, int param1 = 0, int param2 = 0);
		virtual void draw();
		virtual bool isVisible();
		virtual void init();
		virtual void init(const SVGMatrixImpl *screenCTM);

		virtual void renderCallback(SVGTextContentElementImpl *element, const SVGMatrixImpl *screenCTM, T2P::GlyphSet *glyph, T2P::GlyphLayoutParams *params, double anchor) const;
		virtual void addTextDecoration(SVGTextContentElementImpl *element, double x, double y, double w, double h) const;

		void initClipItem();
		ArtSVP *clipSVP();

	protected:
		LibartCanvas *m_canvas;

	private:
		void clearSVPs();

		class SVPElement
		{
		public:
			SVPElement() { svp = 0; element = 0; } 
			~SVPElement();

			_ArtSVP *svp;
			SVGTextContentElementImpl *element;
		};

		// renderCallback() is const.
		mutable QPtrList<SVPElement> m_drawFillItems;
		mutable QPtrList<SVPElement> m_drawStrokeItems;
		mutable QPtrDict<LibartFillPainter> m_fillPainters;
		mutable QPtrDict<LibartStrokePainter> m_strokePainters;
	};
}

#endif

