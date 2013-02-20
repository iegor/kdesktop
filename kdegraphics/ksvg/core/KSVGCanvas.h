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

#ifndef KSVGCANVAS_H
#define KSVGCANVAS_H

#include <qmap.h>
#include <qdict.h>
#include <qcolor.h>
#include <qobject.h>
#include <qptrlist.h>
#include <qdict.h>

#include <Converter.h>

namespace KSVG
{

class SVGShapeImpl;
class SVGMatrixImpl;
class SVGStylableImpl;
class SVGElementImpl;
class SVGTextElementImpl;
class SVGRectElementImpl;
class SVGLineElementImpl;
class SVGPathElementImpl;
class SVGImageElementImpl;
class SVGCircleElementImpl;
class SVGMarkerElementImpl;
class SVGEllipseElementImpl;
class SVGPolygonElementImpl;
class SVGPolylineElementImpl;
class SVGClipPathElementImpl;

class CanvasItem;
class CanvasChunk;
class CanvasItemList;
class CanvasClipPath;
class CanvasPaintServer;

// Must be a QObject to be able to be loaded by KLibLoader...
class KSVGCanvas : public QObject
{
Q_OBJECT
public:
	KSVGCanvas(unsigned int width, unsigned int height);
	virtual ~KSVGCanvas();

	void setViewportDimension(unsigned int w, unsigned int h);

	void setup(QPaintDevice *drawWidget, QPaintDevice *directWindow);
	void setup(unsigned char *buffer, unsigned int width = 0, unsigned int height = 0);

	void reset();
	void update();
	void update(float zoomFactor);
	void update(const QPoint &panPoint, bool erase = true);
	void resize(unsigned int w, unsigned int h);
	void retune(unsigned int csh, unsigned int csv);
	void invalidate(CanvasItem *item, bool recalc = true);
	CanvasItemList collisions(const QPoint &p, bool exact = false) const;

	void setBackgroundColor(const QColor &c) { m_backgroundColor = c; }
	void blit();
	void blit(const QRect &rect, bool direct);

	float zoom() const { return m_zoom; }
	QPoint pan() const { return m_pan; }
	void setPan(const QPoint &pan) { m_pan = pan; }

	int width() const { return m_width; }
	int height() const { return m_height; }

	virtual void setRenderBufferSize(int w, int h);
	void clipToBuffer(int &x0, int &y0, int &x1, int &y1) const;

	// creating canvas items
	virtual CanvasItem *createRectangle(SVGRectElementImpl *rect) = 0;
	virtual CanvasItem *createEllipse(SVGEllipseElementImpl *ellipse) = 0;
	virtual CanvasItem *createCircle(SVGCircleElementImpl *circle) = 0;
	virtual CanvasItem *createLine(SVGLineElementImpl *line) = 0;
	virtual CanvasItem *createPolyline(SVGPolylineElementImpl *poly) = 0;
	virtual CanvasItem *createPolygon(SVGPolygonElementImpl *poly) = 0;
	virtual CanvasItem *createPath(SVGPathElementImpl *path) = 0;
	virtual CanvasItem *createClipPath(SVGClipPathElementImpl *clippath) = 0;
	virtual CanvasItem *createImage(SVGImageElementImpl *image) = 0;
	virtual CanvasItem *createCanvasMarker(SVGMarkerElementImpl *marker) = 0;
	virtual CanvasItem *createText(SVGTextElementImpl *text) = 0;
	virtual CanvasPaintServer *createPaintServer(SVGElementImpl *pserver) = 0;

	void insert(CanvasItem *item, int z = -1);
	void removeItem(CanvasItem *);

	// Enable to have the canvas updated and blitted on item insertion.
	void setImmediateUpdate(bool immediateUpdate) { m_immediateUpdate = immediateUpdate; }
	bool immediateUpdate() const { return m_immediateUpdate; }

	QPtrList<CanvasItem> allItems() const { return m_items; }

	unsigned char *renderingBuffer() const { return m_buffer; }
	unsigned int nrChannels() const { return m_nrChannels; }
	unsigned int rowStride() const { return m_nrChannels * m_width; }

	T2P::Converter *fontContext() { return m_fontContext; }
	QPaintDevice *drawWindow() { return m_drawWindow; }
	QPaintDevice *directWindow() { return m_directWindow; }

	T2P::FontVisualParams *fontVisualParams(SVGStylableImpl *style) const;
	virtual T2P::BezierPath *toBezierPath(CanvasItem *item) const { Q_UNUSED(item); return 0; }

	// Assign z indices to the element and its children, starting with z, and 
	// return the next z value to be used.
	unsigned int setElementItemZIndexRecursive(SVGElementImpl *element, unsigned int z);

protected:
	void addToChunks(CanvasItem *item);
	void removeFromChunks(CanvasItem *item);

	void initVars();

	void fill();
	void clear(const QRect &r);

	virtual void setBuffer(unsigned char *buffer);

protected:
	class ChunkManager
	{
	public:
		ChunkManager() { m_chunks.setAutoDelete(true); }
		void addChunk(CanvasChunk *chunk);
		CanvasChunk *getChunk(short x, short y) const;

		void clear();

	private:
		QDict<CanvasChunk> m_chunks;
	} m_chunkManager;

	QValueList<CanvasChunk *> m_dirtyChunks;

	QMap<CanvasItem *, QPtrList<CanvasChunk> > m_chunksByItem;
	QPtrList<CanvasItem> m_items;

	QDict<CanvasClipPath> m_clipPaths;

	unsigned int m_viewportWidth, m_viewportHeight;
	int m_width, m_height;
	int m_chunkSizeHor, m_chunkSizeVer;

	QPaintDevice *m_drawWindow;
	QPaintDevice *m_directWindow;

	float m_zoom;
	QPoint m_pan;

	GC m_gc;

	T2P::Converter *m_fontContext;

	unsigned char *m_buffer;
	unsigned int m_nrChannels;

	QColor m_backgroundColor;

	bool m_immediateUpdate;
};

}

#endif

// vim:ts=4:noet
