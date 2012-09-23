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

#include "SVGStringListImpl.h"
#include "SVGViewElementImpl.h"

using namespace KSVG;

#include "SVGViewElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGViewElementImpl::SVGViewElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGExternalResourcesRequiredImpl(), SVGFitToViewBoxImpl(), SVGZoomAndPanImpl()
{
	KSVG_EMPTY_FLAGS

	m_viewTarget = new SVGStringListImpl();
	m_viewTarget->ref();
}

SVGViewElementImpl::~SVGViewElementImpl()
{
	if(m_viewTarget)
		m_viewTarget->deref();
}

SVGStringListImpl *SVGViewElementImpl::viewTarget() const
{
	return m_viewTarget;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGViewElementImpl::s_hashTable 2
 viewTarget	SVGViewElementImpl::ViewTarget	DontDelete|ReadOnly
@end
*/

Value SVGViewElementImpl::getValueProperty(ExecState *, int token) const
{
	//KSVG_CHECK_ATTRIBUTE

	switch(token)
	{
		case ViewTarget:
			// TODO
			return Undefined();
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGViewElementImpl::putValueProperty(ExecState *, int token, const Value &, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case ViewTarget:
			// TODO
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
