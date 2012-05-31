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
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef SVGRectImpl_H
#define SVGRectImpl_H

#include <dom/dom_misc.h>

#include "ksvg_lookup.h"

class QRect;

namespace KSVG
{

class SVGRectImpl : public DOM::DomShared
{
public:
	SVGRectImpl();
	SVGRectImpl(const QRect &);
	virtual ~SVGRectImpl();

	void setX(float x);
	float x() const;

	void setY(float y);
	float y() const;

	void setWidth(float width);
	float width() const;

	void setHeight(float height);
	float height() const;

	QRect qrect() const;

	SVGRectImpl &operator=(const QRect &);

private:
	float m_x;
	float m_y;
	float m_width;
	float m_height;

public:
	KSVG_GET
	KSVG_PUT
	
	enum
	{
		// Properties
		X, Y, Width, Height
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

#endif

// vim:ts=4:noet
