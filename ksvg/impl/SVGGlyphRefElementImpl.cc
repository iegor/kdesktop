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

#include "SVGSVGElementImpl.h"
#include "SVGGlyphRefElementImpl.h"

using namespace KSVG;

#include "SVGGlyphRefElementImpl.lut.h"
#include "ksvg_lookup.h"

SVGGlyphRefElementImpl::SVGGlyphRefElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGURIReferenceImpl(), SVGStylableImpl(this)
{
}

SVGGlyphRefElementImpl::~SVGGlyphRefElementImpl()
{
}

void SVGGlyphRefElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();
}

DOM::DOMString SVGGlyphRefElementImpl::format()
{
	return "";
}

DOM::DOMString SVGGlyphRefElementImpl::glyphRef()
{
	return "";
}

float SVGGlyphRefElementImpl::x() const
{
	return m_x;
}

float SVGGlyphRefElementImpl::y() const
{
	return m_y;
}

float SVGGlyphRefElementImpl::dx() const
{
	return m_dx;
}

float SVGGlyphRefElementImpl::dy() const
{
	return m_dy;
}

/*
@namespace KSVG
@begin SVGGlyphRefElementImpl::s_hashTable 7
 glyphRef	SVGGlyphRefElementImpl::GlyphRef	DontDelete|ReadOnly
 format		SVGGlyphRefElementImpl::Format		DontDelete|ReadOnly
 x			SVGGlyphRefElementImpl::X			DontDelete|ReadOnly
 y			SVGGlyphRefElementImpl::Y			DontDelete|ReadOnly
 dx			SVGGlyphRefElementImpl::Dx			DontDelete|ReadOnly
 dy			SVGGlyphRefElementImpl::Dy			DontDelete|ReadOnly
@end
*/

Value SVGGlyphRefElementImpl::getValueProperty(ExecState *, int token) const
{
	//KSVG_CHECK_ATTRIBUTE

	switch(token)
	{
		case GlyphRef: return String(m_glyphRef);
		case Format: return String(m_format);
		case X: return Number(m_x);
		case Y: return Number(m_y);
		case Dx: return Number(m_dx);
		case Dy: return Number(m_dy);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGGlyphRefElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;
	
	switch(token)
	{
		case GlyphRef:
			m_glyphRef = value.toString(exec).string();
			break;
		case Format:
			m_format = value.toString(exec).string();
			break;
		case X:
			m_x = value.toNumber(exec);
			break;
		case Y:
			m_y = value.toNumber(exec);
			break;
		case Dx:
			m_dx = value.toNumber(exec);
			break;
		case Dy:
			m_dy = value.toNumber(exec);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
