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

#include "CanvasItem.h"
#include "CanvasItems.h"
#include "KSVGCanvas.moc"

#include "SVGRectImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGStringListImpl.h"
#include "SVGClipPathElementImpl.h"
#include "SVGImageElementImpl.h"
#include "SVGDocumentImpl.h"

#include <kdebug.h>
#include <kglobal.h>

#include <qstring.h>
#include <qdatetime.h>
#include <qpaintdevicemetrics.h>
#include <qwmatrix.h>

#include <X11/Xlib.h>

#include <math.h>

#include <libs/xrgbrender/gdk-pixbuf-xlibrgb.h>

#include <Font.h>
#include <fontconfig/fontconfig.h>

#define USE_TIMER

using namespace KSVG;

KSVGCanvas::KSVGCanvas(unsigned int width, unsigned int height) : m_viewportWidth(width), m_viewportHeight(height), m_width(width), m_height(height)
{
	m_fontContext = 0;

	m_items.setAutoDelete(true);

	m_chunkSizeVer = CHUNK_SIZE_VERTICAL;
	m_chunkSizeHor = CHUNK_SIZE_HORIZONTAL;

	m_zoom = 1;

	m_buffer = 0;

	m_backgroundColor = QColor(250, 250, 250);

	m_immediateUpdate = false;
}

void KSVGCanvas::setup(QPaintDevice *drawWindow, QPaintDevice *directWindow)
{
	m_drawWindow = drawWindow;
	m_directWindow = directWindow;

	m_buffer = 0;
	m_nrChannels = 3;

	setRenderBufferSize(m_width, m_height);

	xlib_rgb_init_with_depth(m_drawWindow->x11Display(), XScreenOfDisplay(m_drawWindow->x11Display(), m_drawWindow->x11Screen()), m_drawWindow->x11Depth());
	m_gc = XCreateGC(m_drawWindow->x11Display(), m_drawWindow->handle(), 0, 0);
}

void KSVGCanvas::setViewportDimension(unsigned int w, unsigned int h)
{
	m_viewportWidth  = w;
	m_viewportHeight = h;
	setRenderBufferSize(w, h);
}

void KSVGCanvas::setup(unsigned char *buffer, unsigned int width, unsigned int height)
{
	setBuffer(buffer);
	m_drawWindow = 0;
	m_directWindow = 0;

	m_nrChannels = 4;

	if(height > 0)
	{
		m_width = width;
		m_height = height;
	}

	setRenderBufferSize(m_width, m_height);

	m_gc = 0;
}

void KSVGCanvas::setBuffer(unsigned char *buffer)
{
	m_buffer = buffer;
}

KSVGCanvas::~KSVGCanvas()
{
	if(m_fontContext)
		delete m_fontContext;

	if(m_buffer && m_gc)
		delete []m_buffer;

	if(m_gc)
		XFreeGC(m_drawWindow->x11Display(), m_gc);

	reset();
}

void KSVGCanvas::retune(unsigned int csh, unsigned int csv)
{
	m_chunkSizeHor = csh;
	m_chunkSizeVer = csv;
}

void KSVGCanvas::resize(unsigned int w, unsigned int h)
{
	if(m_buffer && (m_width != int(w) || m_height != int(h)))
	{
		unsigned char *oldbuffer = m_buffer;

		m_buffer = new unsigned char[w * h * m_nrChannels];

		int minw = kMin(int(w), m_width);
		int minh = kMin(int(h), m_height);

		int origstride = m_width * m_nrChannels;
		int newstride = w * m_nrChannels;

		// Redraw new areas, if any
		int diffw = w - m_width;
		int diffh = h - m_height;

		QRect r(m_width, 0, diffw, m_height + diffh);
		QRect r3(0, m_height, m_width + diffw, diffh);

		QWMatrix mtx;
		mtx.translate(m_pan.x(), m_pan.y());
		mtx.scale(m_zoom, m_zoom);

		m_width = w;
		m_height = h;

		setBuffer(m_buffer);
		fill();

		if(diffw > 0 || diffh > 0)
		{
			CanvasItemList drawables;
			if(diffw > 0)
			{
				QRect r2 = mtx.invert().map(r);

				// Recalc items
				for(int j = r2.top() / int(m_chunkSizeVer); j <= r2.bottom() / int(m_chunkSizeVer); j++)
				{
					for(int i = r2.left() / int(m_chunkSizeHor); i <= r2.right() / int(m_chunkSizeHor); i++)
					{
						CanvasChunk *chunk = m_chunkManager.getChunk(i, j);
						if(chunk)
						{
							for(CanvasItemList::ConstIterator it = chunk->list().begin(); it != chunk->list().end(); ++it)
							{
								if(!drawables.contains(*it))
									drawables.append(*it);
							}
						}
					}
				}
			}

			if(diffh > 0)
			{
				QRect r4 = mtx.invert().map(r3);

				// Recalc items
				for(int j = r4.top() / int(m_chunkSizeVer); j <= r4.bottom() / int(m_chunkSizeVer); j++)
				{
					for(int i = r4.left() / int(m_chunkSizeHor); i <= r4.right() / int(m_chunkSizeHor); i++)
					{
						CanvasChunk *chunk = m_chunkManager.getChunk(i, j);
						if(chunk)
						{
							for(CanvasItemList::ConstIterator it = chunk->list().begin(); it != chunk->list().end(); ++it)
							{
								if(!drawables.contains(*it))
									drawables.append(*it);
							}
						}
					}
				}
			}

			drawables.sort();

			for(CanvasItemList::Iterator it = drawables.begin(); it != drawables.end(); ++it)
				(*it)->draw();
		}

		for(int y = 0; y < minh; y++)
			memcpy(m_buffer + y * newstride, oldbuffer + y * origstride, minw * m_nrChannels);

		delete []oldbuffer;
	}
}

void KSVGCanvas::setRenderBufferSize(int w, int h)
{
	kdDebug(26005) << k_funcinfo << endl;

	if(m_drawWindow)
	{
		bool needsRedraw = (!m_buffer) || (m_width != w || m_height != h);

		if(needsRedraw)
		{
			QPaintDeviceMetrics metrics(m_drawWindow);
			m_width = kMin(int(w), metrics.width());
			m_height = kMin(int(h), metrics.height());

			if(m_buffer) 
				delete []m_buffer;

			m_buffer = new unsigned char[m_width * m_height * m_nrChannels];
		}
	}

	fill();
}

void KSVGCanvas::clear(const QRect &r)
{
	QRect r2 = r & QRect(0, 0, m_width, m_height);
	if(!r2.isEmpty() && m_buffer)
	{
		for(int i = 0; i < r2.height(); i++)
			memset(m_buffer + int(r2.x() * m_nrChannels) + int((r2.y() + i) * (m_width * m_nrChannels)), qRgba(250, 250, 250, 250), r2.width() * m_nrChannels);
	}
}

void KSVGCanvas::fill()
{
	if(m_buffer)
	{
		unsigned char r = m_backgroundColor.red();
		unsigned char g = m_backgroundColor.green();
		unsigned char b = m_backgroundColor.blue();

		if(m_nrChannels == 3)
		{
			if(r == g && r == b)
				memset(m_buffer, r, m_width * m_height * m_nrChannels);
			else
			{
				unsigned char *p = m_buffer;

				for(int i = 0; i < m_width * m_height; i++)
				{
					*p++ = r;
					*p++ = g;
					*p++ = b;
				}
			}
		}
		else
		{
			Q_UINT32 *p = reinterpret_cast<Q_UINT32 *>(m_buffer);
			unsigned char a = qAlpha(m_backgroundColor.rgb());

#if X_BYTE_ORDER == X_LITTLE_ENDIAN
			Q_UINT32 rgba = (a << 24) | (b << 16) | (g << 8) | r;
#else
			Q_UINT32 rgba = (r << 24) | (g << 16) | (b << 8) | a;
#endif
			for(int i = 0; i < m_width * m_height; i++)
				*p++ = rgba;
		}
	}
}

// Clipping
void KSVGCanvas::clipToBuffer(int &x0, int &y0, int &x1, int &y1) const
{
	// clamp to viewport
	x0 = QMAX(x0, 0);
	x0 = QMIN(x0, int(m_width - 1));

	y0 = QMAX(y0, 0);
	y0 = QMIN(y0, int(m_height - 1));

	x1 = QMAX(x1, 0);
	x1 = QMIN(x1, int(m_width - 1));

	y1 = QMAX(y1, 0);
	y1 = QMIN(y1, int(m_height - 1));
}

T2P::FontVisualParams *KSVGCanvas::fontVisualParams(SVGStylableImpl *style) const
{
	T2P::FontVisualParams *fontVisualParams = new T2P::FontVisualParams();

	// Calc weight & slant
	int weight = 0, slant = 0;
	EFontStyle fontStyle = style->getFontStyle();
	QString fontWeight = style->getFontWeight();

	if(fontWeight.contains("bold"))
		weight |= FC_WEIGHT_DEMIBOLD;
	if(fontWeight.contains("bolder"))
		weight |= FC_WEIGHT_BOLD;
	if(fontWeight.contains("lighter"))
		weight |= FC_WEIGHT_LIGHT;

	bool ok = true;
	int weightNumber = fontWeight.toInt(&ok);

	if(ok)
		weight = weightNumber;

	if(fontStyle == FSNORMAL)
		slant |= FC_SLANT_ROMAN;
	else if(fontStyle == ITALIC)
		slant |= FC_SLANT_ITALIC;
	else if(fontStyle == OBLIQUE)
		slant |= FC_SLANT_OBLIQUE;

	// Calc font names
	SVGStringListImpl *fontList = style->getFontFamily();

	for(unsigned int i = 0; i <= fontList->numberOfItems(); i++)
	{
		DOM::DOMString *string = fontList->getItem(i);

		if(string)
			fontVisualParams->fontList().push_back(string->string().latin1());
	}

	fontVisualParams->setWeight(weight);
	fontVisualParams->setSlant(slant);
	fontVisualParams->setSize(style->getFontSize());

	return fontVisualParams;
}

void KSVGCanvas::invalidate(CanvasItem *item, bool recalc)
{
	if(m_chunksByItem.find(item) != m_chunksByItem.end())
	{
		if(recalc)
		{
			removeFromChunks(item);
			addToChunks(item);
		}

		QPtrListIterator<CanvasChunk> it = m_chunksByItem[item];
		for(it.toFirst(); it.current(); ++it)
		{
			(*it)->setDirty();
			if(!m_dirtyChunks.contains(*it))
				m_dirtyChunks.append(*it);
		}
	}
	else
		addToChunks(item);
}

void KSVGCanvas::insert(CanvasItem *item, int z)
{
	if(z == -1)
	{
		item->setZIndex(m_chunksByItem.size());
		m_chunksByItem[item] = QPtrList<CanvasChunk>();
		addToChunks(item);
		m_items.append(item);

		bool visible = item->isVisible();
		if(visible)
			invalidate(item, false);

		if(m_immediateUpdate)
		{
			if(visible)
			{
				item->draw();
				QRect bbox = item->bbox();
				blit(bbox, true);
			}
		}
	}
	else
	{
		// make some space
		for(unsigned int i = z; i < m_items.count(); i++)
			m_items.at(i)->setZIndex(m_items.at(i)->zIndex() + 1);

		item->setZIndex(z);
	}
}

void KSVGCanvas::removeItem(CanvasItem *item)
{
	removeFromChunks(item);
	m_items.remove(item);
}

void KSVGCanvas::removeFromChunks(CanvasItem *item)
{
	QPtrListIterator<CanvasChunk> it = m_chunksByItem[item];
	for(it.toFirst(); it.current(); ++it)
	{
		(*it)->remove(item);
		if(!m_dirtyChunks.contains(*it))
			m_dirtyChunks.append(*it);
	}
	m_chunksByItem.remove(item);
}

void KSVGCanvas::addToChunks(CanvasItem *item)
{
	QRect bbox = item->bbox();
	QWMatrix mtx;
	mtx.translate(m_pan.x(), m_pan.y());
	mtx.scale(m_zoom, m_zoom);

	bbox = mtx.invert().map(bbox);
	for(int j = bbox.top() / m_chunkSizeVer; j <= (bbox.bottom() / m_chunkSizeVer); j++)
	{
		for(int i = bbox.left() / int(m_chunkSizeHor); i <= (bbox.right() / m_chunkSizeHor); i++)
		{
			CanvasChunk *chunk = m_chunkManager.getChunk(i, j);
			if(!chunk)
			{
				chunk = new CanvasChunk(i, j);
				m_chunkManager.addChunk(chunk);
			}

			chunk->add(item);
			m_chunksByItem[item].append(chunk);
		}
	}
}

unsigned int KSVGCanvas::setElementItemZIndexRecursive(SVGElementImpl *element, unsigned int z)
{
	SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(element);

	if(shape)
	{
		CanvasItem *item = shape->item();

		if(item)
		{
			SVGImageElementImpl *image = dynamic_cast<SVGImageElementImpl *>(shape);

			if(image && image->svgImageRootElement())
			{
				// Set the z for all items in the svg image, since they live in the
				// same canvas.
				z = setElementItemZIndexRecursive(image->svgImageRootElement(), z);
			}
			else
			{
				item->setZIndex(z);
				invalidate(item, false);
				z++;
			}
		}
	}

	for(DOM::Node node = element->firstChild(); !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *e = element->ownerDoc()->getElementFromHandle(node.handle());

		if(e)
			z = setElementItemZIndexRecursive(e, z);
	}

	return z;
}

void KSVGCanvas::update(const QPoint &panPoint, bool erase)
{
#ifdef USE_TIMER
	QTime t;
	t.start();
#endif

	int dx = panPoint.x() - m_pan.x();
	int dy = panPoint.y() - m_pan.y();
	m_pan = panPoint;

	if(erase)
		fill();

	// reset clip paths
	QDictIterator<CanvasClipPath> itr(m_clipPaths);
	for(; itr.current(); ++itr)
		(*itr)->update(UPDATE_TRANSFORM);

	QWMatrix mtx;
	mtx.translate(m_pan.x(), m_pan.y());
	mtx.scale(m_zoom, m_zoom);

	QRect r(0, 0, m_width, m_height);
	QRect r2 = mtx.invert().map(r);

	// pan all items
	for(unsigned int i = 0; i < m_items.count(); i++)
		m_items.at(i)->update(UPDATE_PAN, dx, dy);

	// recalc items
	CanvasItemList drawables;
	QPtrListIterator<CanvasItem> it = m_items;
	for(int j = r2.top() / m_chunkSizeVer; j <= (r2.bottom() / m_chunkSizeVer); j++)
	{
		for(int i = r2.left() / m_chunkSizeHor; i <= (r2.right() / m_chunkSizeHor); i++)
		{
			CanvasChunk *chunk = m_chunkManager.getChunk(i, j);
			if(chunk)
			{
				for(CanvasItemList::ConstIterator it = chunk->list().begin(); it != chunk->list().end(); ++it)
				{
					if(!drawables.contains(*it))
						drawables.append(*it);
				}
			}
		}
	}

	drawables.sort();
	for(CanvasItemList::Iterator it = drawables.begin(); it != drawables.end(); ++it)
		(*it)->draw();

	if(m_drawWindow)
		blit(QRect(0, 0, m_width, m_height), false);

	m_dirtyChunks.clear();

#ifdef USE_TIMER
	kdDebug(26000) << k_funcinfo << " Total time: " << t.elapsed() << endl;
#endif
}

void KSVGCanvas::update(float zoomFactor)
{
#ifdef USE_TIMER
	QTime t;
	t.start();
#endif

	if(zoomFactor >= 1)
	{
		int newWidth  = static_cast<int>(m_viewportWidth * zoomFactor);
		int newHeight = static_cast<int>(m_viewportHeight * zoomFactor);
		setRenderBufferSize(newWidth, newHeight);
	}
	else
		fill();

	// reset clip paths
	QDictIterator<CanvasClipPath> itr(m_clipPaths);
	for(; itr.current(); ++itr)
		(*itr)->update(UPDATE_TRANSFORM);

	m_zoom = zoomFactor;

	QWMatrix mtx;
	mtx.translate(m_pan.x(), m_pan.y());
	mtx.scale(m_zoom, m_zoom);

	QRect r(0, 0, m_width, m_height);
	QRect r2 = mtx.invert().map(r);

	// zoom all items
	for(unsigned int i = 0; i < m_items.count(); i++)
		m_items.at(i)->update(UPDATE_ZOOM);

	// recalc items
	CanvasItemList drawables;
	QPtrListIterator<CanvasItem> it = m_items;
	for(int j = r2.top() / m_chunkSizeVer; j <= (r2.bottom() / m_chunkSizeVer); j++)
	{
		for(int i = r2.left() / m_chunkSizeHor; i <= (r2.right() / m_chunkSizeHor); i++)
		{
			CanvasChunk *chunk = m_chunkManager.getChunk(i, j);
			if(chunk)
			{
				for(CanvasItemList::ConstIterator it = chunk->list().begin(); it != chunk->list().end(); ++it)
				{
					if(!drawables.contains(*it))
						drawables.append(*it);
				}
			}
		}
	}

	drawables.sort();
	for(CanvasItemList::Iterator it = drawables.begin(); it != drawables.end(); ++it)
		(*it)->draw();

	if(m_drawWindow)
		blit(QRect(0, 0, m_width, m_height), false);

	m_dirtyChunks.clear();

#ifdef USE_TIMER
	kdDebug(26000) << k_funcinfo << " Total time: " << t.elapsed() << endl;
#endif
}

void KSVGCanvas::reset()
{
	m_items.clear();
	m_chunkManager.clear();
	m_chunksByItem.clear();
	m_dirtyChunks.clear();
	m_pan.setX(0);
	m_pan.setY(0);
	m_zoom = 1;
}

void KSVGCanvas::update()
{
#ifdef USE_TIMER
	QTime t;
	t.start();
#endif

	QWMatrix mtx;
	mtx.translate(m_pan.x(), m_pan.y());
	mtx.scale(m_zoom, m_zoom);

	// Process dirty chunks
	QPtrList<CanvasChunk> chunkList;
	CanvasItemList drawables;
	for(unsigned int i = 0; i < m_dirtyChunks.count(); i++)
	{
		CanvasChunk *chunk = m_dirtyChunks[i];
		Q_ASSERT(chunk->isDirty());

		QRect r = chunk->bbox();
		QRect chunkbox(mtx.map(r.topLeft()), mtx.map(r.bottomRight()));
		clear(chunkbox);
		chunkList.append(chunk);

		for(CanvasItemList::ConstIterator it = chunk->list().begin(); it != chunk->list().end(); ++it)
		{
//			kdDebug(26005) << k_funcinfo << " Checking: " << *it << endl;
			if(!drawables.contains(*it))
			{
//				kdDebug(26005) << k_funcinfo << " Yes, appending to update list!" << endl;
				drawables.append(*it);
			}
		}

		chunk->unsetDirty();
	}

	drawables.sort();

	// Draw dirty chunks
	for(CanvasItemList::Iterator it = drawables.begin(); it != drawables.end(); ++it)
	{
//		kdDebug(26005) << " Need to redraw dirty : " << (*it) << " with z : " << (*it)->zIndex() << endl;
		(*it)->draw();
	}

	// Blit dirty chunks
	QPtrListIterator<CanvasChunk> it = chunkList;
	for(it.toFirst(); it.current(); ++it)
	{
		QRect r = (*it)->bbox();
		QRect chunkbox(mtx.map(r.topLeft()), mtx.map(r.bottomRight()));
		blit(chunkbox, false);
	}

	m_dirtyChunks.clear();

#ifdef USE_TIMER
	kdDebug(26005) << k_funcinfo << " Total time: " << t.elapsed() << endl;
#endif
}

CanvasItemList KSVGCanvas::collisions(const QPoint &p, bool exact) const
{
	QWMatrix mtx;
	mtx.translate(m_pan.x(), m_pan.y());
	mtx.scale(m_zoom, m_zoom);

	QPoint p2 = mtx.invert().map(p);
	if(p2.x() < 0 || p2.y() < 0)
		return CanvasItemList();

	unsigned int x = p2.x() / int(m_chunkSizeHor);
	unsigned int y = p2.y() / int(m_chunkSizeVer);

	CanvasItemList result;
	CanvasChunk *chunk = m_chunkManager.getChunk(x, y);
	if(!chunk)
		return result;

	CanvasItemList list = chunk->list();
	if(exact)
	{
		for(CanvasItemList::Iterator it = list.begin(); it != list.end(); ++it)
		{
			if((*it)->fillContains(p) || (*it)->strokeContains(p) || (*it)->bbox().contains(p))
				result.append(*it);
		}

		return result;
	}
	else
		return list;
}

void KSVGCanvas::blit(const QRect &rect, bool direct)
{
	if(m_drawWindow && m_width && m_height)
	{
		// clamp to viewport
		int x0 = rect.x();
		x0 = QMAX(x0, 0);
		x0 = QMIN(x0, int(m_width - 1));

		int y0 = rect.y();
		y0 = QMAX(y0, 0);
		y0 = QMIN(y0, int(m_height - 1));

		int x1 = rect.x() + rect.width() + 1;
		x1 = QMAX(x1, 0);
		x1 = QMIN(x1, int(m_width));

		int y1 = rect.y() + rect.height() + 1;
		y1 = QMAX(y1, 0);
		y1 = QMIN(y1, int(m_height));

		xlib_draw_rgb_image(direct ? m_directWindow->handle() : m_drawWindow->handle(), m_gc, x0, y0, x1 - x0, y1 - y0, XLIB_RGB_DITHER_NONE, m_buffer + (m_width * y0 + x0) * m_nrChannels, m_width * m_nrChannels);
	}
}

void KSVGCanvas::blit()
{
	return blit(QRect(0, 0, m_width, m_height), false);
}

void KSVGCanvas::ChunkManager::addChunk(CanvasChunk *chunk)
{
	QString key = QString("%1 %2").arg(chunk->x()).arg(chunk->y());
//	kdDebug(26005) << k_funcinfo << "Adding chunk : " << chunk << endl;
	m_chunks.insert(key, chunk);
}

CanvasChunk *KSVGCanvas::ChunkManager::getChunk(short x, short y) const
{
//	kdDebug(26005) << k_funcinfo << "getting chunk from : " << x << ", " << y << endl;
	QString key = QString("%1 %2").arg(x).arg(y);
	return m_chunks[key];
}

void KSVGCanvas::ChunkManager::clear()
{
	m_chunks.clear();
}

// vim:ts=4:noet
