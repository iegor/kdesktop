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

#include "SVGGlyphElementImpl.h"

using namespace KSVG;

#include "SVGGlyphElementImpl.lut.h"

SVGGlyphElementImpl::SVGGlyphElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGStylableImpl(this)
{
	KSVG_EMPTY_FLAGS
}

SVGGlyphElementImpl::~SVGGlyphElementImpl()
{
}

QString SVGGlyphElementImpl::d() const
{
	return m_d;
}

/*
@namespace KSVG
@begin SVGGlyphElementImpl::s_hashTable 11
 unicode		SVGGlyphElementImpl::Unicode		DontDelete|ReadOnly
 glyph-name		SVGGlyphElementImpl::GlyphName		DontDelete|ReadOnly
 d				SVGGlyphElementImpl::D				DontDelete|ReadOnly
 orientation	SVGGlyphElementImpl::Orientation	DontDelete|ReadOnly
 arabic-form	SVGGlyphElementImpl::ArabicForm		DontDelete|ReadOnly
 lang			SVGGlyphElementImpl::Lang			DontDelete|ReadOnly
 horiz-adv-x	SVGGlyphElementImpl::HorizAdvX		DontDelete|ReadOnly
 vert-origin-x	SVGGlyphElementImpl::VertOriginX	DontDelete|ReadOnly
 vert-origin-y	SVGGlyphElementImpl::VertOriginY	DontDelete|ReadOnly
 vert-adv-y		SVGGlyphElementImpl::VertAdvY		DontDelete|ReadOnly
@end
*/

Value SVGGlyphElementImpl::getValueProperty(ExecState *, int token) const
{
	//KSVG_CHECK_ATTRIBUTE

	switch(token)
	{
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGGlyphElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
    // This class has just ReadOnly properties, only with the Internal flag set
    // it's allowed to modify those.
    if(!(attr & KJS::Internal))
        return;

    switch(token)
    {
        case D:
			m_d = value.toString(exec).qstring();
			break;
		case Unicode:
		case GlyphName:
		case Orientation:
		case ArabicForm:
		case Lang:
		case HorizAdvX:
		case VertOriginX:
		case VertOriginY:
		case VertAdvY:
			break;
        default:
            kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
    }
}

// vim:ts=4:noet
