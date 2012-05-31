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

#include <kdebug.h>

#include "SVGPaint.h"
#include "SVGColor.h"

#include "SVGPaintImpl.h"

using namespace KSVG;

#include "SVGPaintImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_cacheimpl.h"

SVGPaintImpl::SVGPaintImpl(SVGElementImpl *object) : SVGColorImpl(object)
{
	m_paintType = SVG_PAINTTYPE_UNKNOWN;
}

SVGPaintImpl::SVGPaintImpl(const SVGPaintImpl &other) : SVGColorImpl(other)
{
	(*this) = other;
}

SVGPaintImpl::~SVGPaintImpl()
{
}

SVGPaintImpl &SVGPaintImpl::operator=(const SVGPaintImpl &other)
{
	m_uri = other.m_uri;
	m_paintType = other.m_paintType;
	
	*(static_cast<SVGColorImpl *>(this)) = other;
	
	return *this;
}

unsigned short SVGPaintImpl::paintType() const
{
	return m_paintType;
}

DOM::DOMString SVGPaintImpl::uri() const
{
	return m_uri;
}

void SVGPaintImpl::setUri(const DOM::DOMString &uri)
{
	m_uri = uri;
	m_paintType = SVG_PAINTTYPE_URI;
}

void SVGPaintImpl::setPaint(unsigned short paintType, const DOM::DOMString &/*uri*/, const DOM::DOMString &/*rgbColor*/, const DOM::DOMString &/*iccColor*/)
{
	m_paintType = paintType;
}

void SVGPaintImpl::setRGBColor(QColor color)
{
	m_paintType = SVG_PAINTTYPE_RGBCOLOR;
	SVGColorImpl::setRGBColor(color);
}

void SVGPaintImpl::setRGBColor(int r, int g, int b)
{
	m_paintType = SVG_PAINTTYPE_RGBCOLOR;
	SVGColorImpl::setRGBColor(r, g, b);
}

void SVGPaintImpl::setRGBColor(const DOM::DOMString &rgbColor)
{
	m_paintType = SVG_PAINTTYPE_RGBCOLOR;
	SVGColorImpl::setRGBColor(rgbColor);
}

void SVGPaintImpl::setRGBColorICCColor(const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor)
{
	m_paintType = SVG_PAINTTYPE_RGBCOLOR_ICCCOLOR;
	SVGColorImpl::setRGBColorICCColor(rgbColor, iccColor);
}

void SVGPaintImpl::setColor(unsigned short colorType, const DOM::DOMString &rgbColor, const DOM::DOMString &iccColor)
{
	SVGColorImpl::setColor(colorType, rgbColor, iccColor);
	
	switch(colorType)
	{
		case SVG_COLORTYPE_CURRENTCOLOR:
			m_paintType = SVG_PAINTTYPE_CURRENTCOLOR;
			break;
		case SVG_COLORTYPE_RGBCOLOR:
			m_paintType = SVG_PAINTTYPE_CURRENTCOLOR;
			break;
		case SVG_COLORTYPE_RGBCOLOR_ICCCOLOR:
			m_paintType = SVG_PAINTTYPE_CURRENTCOLOR;
			break;
		case SVG_COLORTYPE_UNKNOWN:
			m_paintType = SVG_PAINTTYPE_UNKNOWN;
			break;
	}
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPaintImpl::s_hashTable 3
 paintType	SVGPaintImpl::PaintType	DontDelete
 uri		SVGPaintImpl::URI		DontDelete
@end
*/

Value SVGPaintImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case PaintType:
			return Number(paintType());
		case URI:
			return String(uri().string());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

/*
@namespace KSVG
@begin SVGPaintImplConstructor::s_hashTable 11
SVG_PAINTTYPE_UNKNOWN				KSVG::SVG_PAINTTYPE_UNKNOWN					DontDelete|ReadOnly
SVG_PAINTTYPE_CURRENTCOLOR			KSVG::SVG_PAINTTYPE_CURRENTCOLOR			DontDelete|ReadOnly
SVG_PAINTTYPE_RGBCOLOR				KSVG::SVG_PAINTTYPE_RGBCOLOR				DontDelete|ReadOnly
SVG_PAINTTYPE_RGBCOLOR				KSVG::SVG_PAINTTYPE_RGBCOLOR_ICCCOLOR		DontDelete|ReadOnly
SVG_PAINTTYPE_NONE					KSVG::SVG_PAINTTYPE_NONE					DontDelete|ReadOnly
SVG_PAINTTYPE_URI_NONE				KSVG::SVG_PAINTTYPE_URI_NONE				DontDelete|ReadOnly
SVG_PAINTTYPE_URI_RGBCOLOR			KSVG::SVG_PAINTTYPE_URI_RGBCOLOR			DontDelete|ReadOnly
SVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR	KSVG::SVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR	DontDelete|ReadOnly
SVG_PAINTTYPE_URI_CURRENTCOLOR		KSVG::SVG_PAINTTYPE_URI_CURRENTCOLOR		DontDelete|ReadOnly
SVG_PAINTTYPE_URI					KSVG::SVG_PAINTTYPE_URI						DontDelete|ReadOnly
@end
*/

Value SVGPaintImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token);
}

Value KSVG::getSVGPaintImplConstructor(ExecState *exec)
{
	return cacheGlobalBridge<SVGPaintImplConstructor>(exec, "[[svgpaint.constructor]]");
}

// vim:ts=4:noet
