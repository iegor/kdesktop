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

#include "SVGCursorElementImpl.h"
#include "SVGAnimatedLengthImpl.h"

using namespace KSVG;

#include "SVGCursorElementImpl.lut.h"
#include "ksvg_bridge.h"

SVGCursorElementImpl::SVGCursorElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGURIReferenceImpl(), SVGTestsImpl(), SVGExternalResourcesRequiredImpl()
{
	KSVG_EMPTY_FLAGS

	m_x = new SVGAnimatedLengthImpl();
	m_x->ref();

	m_y = new SVGAnimatedLengthImpl();
	m_y->ref();
}

SVGCursorElementImpl::~SVGCursorElementImpl()
{
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
}

SVGAnimatedLengthImpl *SVGCursorElementImpl::x() const
{
	return m_x;
}

SVGAnimatedLengthImpl *SVGCursorElementImpl::y() const
{
	return m_y;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGCursorElementImpl::s_hashTable 3
 x	SVGCursorElementImpl::X	DontDelete|ReadOnly
 y	SVGCursorElementImpl::Y	DontDelete|ReadOnly
@end
*/

Value SVGCursorElementImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case X:
			return m_x->cache(exec);
		case Y:
			return m_y->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGCursorElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case X:
			x()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Y:
			y()->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
