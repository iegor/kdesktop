/*
    Copyright (C) 2003 KSVG Team
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

#ifndef CANVASITEM_H
#define CANVASITEM_H

#include <qrect.h>
#include <qpoint.h>
#include <qvaluelist.h>

#define CHUNK_SIZE_HORIZONTAL 32
#define CHUNK_SIZE_VERTICAL   32

namespace KSVG
{

class SVGElementImpl;

enum RenderContext
{
	/* NONE = for initializing */
	NONE = 0,
	/* NORMAL = use baseVal()'s for calculation, and fillRule */
	NORMAL = 1,
	/* CLIPPING = use baseVal()'s for calculation, and clipRule */
	CLIPPING = 2,
	/* ANIMATION = use animVal()'s for calculation, and fillRule */
	ANIMATION = 4
};

enum CanvasItemUpdate
{
	UPDATE_STYLE,
	UPDATE_LINEWIDTH,
	UPDATE_TRANSFORM,
	UPDATE_ZOOM,
	UPDATE_PAN
};

class CanvasItem
{
public:
	CanvasItem() { m_zIndex = 0; m_referenced = false; }
	virtual ~CanvasItem() { }

	virtual QRect bbox() const = 0;
	virtual bool fillContains(const QPoint &) = 0;
	virtual bool strokeContains(const QPoint &) = 0;
	virtual void update(CanvasItemUpdate reason, int param1 = 0, int param2 = 0) = 0;
	virtual void draw() = 0;
	virtual bool isVisible() = 0;

	void setZIndex(unsigned int zIndex) { m_zIndex = zIndex; }
	unsigned int zIndex() const { return m_zIndex; }

	void setReferenced(bool referenced) { m_referenced = referenced; }
	bool referenced() const { return m_referenced; }

	virtual SVGElementImpl *element() const { return 0; }

protected:
	unsigned int m_zIndex;
	bool m_referenced;
};

class CanvasItemPtr
{
public:
	CanvasItemPtr() : ptr(0) { }
	CanvasItemPtr(CanvasItem *p) : ptr(p) { }

	bool operator<=(const CanvasItemPtr& that) const
	{
		// Order same-z objects by identity.
		if(that.ptr->zIndex() == ptr->zIndex())
			return that.ptr >= ptr;
		return that.ptr->zIndex() >= ptr->zIndex();
	}
	bool operator<(const CanvasItemPtr& that) const
	{
		// Order same-z objects by identity.
		if(that.ptr->zIndex() == ptr->zIndex())
			return that.ptr > ptr;
		return that.ptr->zIndex() > ptr->zIndex();
	}
	bool operator>(const CanvasItemPtr& that) const
	{
		// Order same-z objects by identity.
		if(that.ptr->zIndex() == ptr->zIndex())
			return that.ptr < ptr;
		return that.ptr->zIndex() < ptr->zIndex();
	}
	bool operator==(const CanvasItemPtr& that) const { return that.ptr == ptr; }
	operator CanvasItem *() const { return ptr; }

private:
	CanvasItem *ptr;
};

class CanvasItemList : public QValueList<CanvasItem *>
{
public:
	void sort() { qHeapSort(*((QValueList<CanvasItemPtr> *) this)); }
};

class CanvasChunk
{
public:
	CanvasChunk(short x, short y) : m_x(x), m_y(y), m_dirty(false) { }

	void sort() { m_list.sort(); }
	const CanvasItemList &list() const { return m_list; }

	void add(CanvasItem *item) { m_list.prepend(item); m_dirty = true; }
	void remove(CanvasItem *item) { m_list.remove(item); m_dirty = true; }

	void setDirty() { m_dirty = true; }
	bool unsetDirty() { bool y = m_dirty; m_dirty = false; return y; }
	bool isDirty() const { return m_dirty; }

	short x() const { return m_x; }
	short y() const { return m_y; }

	QRect bbox() const { return QRect(m_x * CHUNK_SIZE_HORIZONTAL, m_y * CHUNK_SIZE_VERTICAL, CHUNK_SIZE_HORIZONTAL, CHUNK_SIZE_VERTICAL); }

private:
	CanvasItemList m_list;
	short m_x;
	short m_y;
	bool m_dirty : 1;
};

}

#endif

// vim:ts=4:noet
