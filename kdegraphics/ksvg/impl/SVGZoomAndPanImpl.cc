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

#include "SVGZoomAndPan.h"
#include "SVGZoomAndPanImpl.h"

using namespace KSVG;

#include "SVGZoomAndPanImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_cacheimpl.h"

SVGZoomAndPanImpl::SVGZoomAndPanImpl()
{
	KSVG_EMPTY_FLAGS

	m_zoomAndPan = SVG_ZOOMANDPAN_MAGNIFY;
}

SVGZoomAndPanImpl::~SVGZoomAndPanImpl()
{
}

void SVGZoomAndPanImpl::setZoomAndPan(unsigned short zoomAndPan)
{
	m_zoomAndPan = zoomAndPan;
}

unsigned short SVGZoomAndPanImpl::zoomAndPan() const
{
	return m_zoomAndPan;
}

void SVGZoomAndPanImpl::parseZoomAndPan(const DOM::DOMString &attr)
{
	if(attr == "disable")
		m_zoomAndPan = SVG_ZOOMANDPAN_DISABLE;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGZoomAndPanImpl::s_hashTable 2
 zoomAndPan		SVGZoomAndPanImpl::ZoomAndPan		DontDelete
@end
*/

Value SVGZoomAndPanImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case ZoomAndPan:
			return Number(zoomAndPan());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return KJS::Undefined();
	}
}

void SVGZoomAndPanImpl::putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case ZoomAndPan:
			parseZoomAndPan(value.toString(exec).string());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

/*
@namespace KSVG
@begin SVGZoomAndPanImplConstructor::s_hashTable 5
 SVG_ZOOMANDPAN_UNKNOWN      KSVG::SVG_ZOOMANDPAN_UNKNOWN     DontDelete|ReadOnly
 SVG_ZOOMANDPAN_DISABLE      KSVG::SVG_ZOOMANDPAN_DISABLE     DontDelete|ReadOnly
 SVG_ZOOMANDPAN_MAGNIFY      KSVG::SVG_ZOOMANDPAN_MAGNIFY     DontDelete|ReadOnly
@end
*/

Value SVGZoomAndPanImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token);
}

Value KSVG::getSVGZoomAndPanImplConstructor(ExecState *exec)
{
	return cacheGlobalBridge<SVGZoomAndPanImplConstructor>(exec, "[[svgzoomandpan.constructor]]");
}

// vim:ts=4:noet
