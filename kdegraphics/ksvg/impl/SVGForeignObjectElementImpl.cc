/*
    Copyright (C) 2001-20032 KSVG Team
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

#include "SVGRectImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGForeignObjectElementImpl.h"

using namespace KSVG;

#include "SVGForeignObjectElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGForeignObjectElementImpl::SVGForeignObjectElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl()
{
	 m_x = new SVGAnimatedLengthImpl();
	 m_x->ref();
	 
	 m_y = new SVGAnimatedLengthImpl();
	 m_y->ref();
	 
	 m_width = new SVGAnimatedLengthImpl();
	 m_width->ref();
	 
	 m_height = new SVGAnimatedLengthImpl();
	 m_height->ref();
}

SVGForeignObjectElementImpl::~SVGForeignObjectElementImpl()
{
	if(m_x)
		m_x->deref();
	if(m_y)
		m_y->deref();
	if(m_width)
		m_width->deref();
	if(m_height)
		m_height->deref();
}

SVGRectImpl *SVGForeignObjectElementImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();
	ret->setX(m_x->baseVal()->value());
	ret->setY(m_y->baseVal()->value());
	ret->setWidth(m_width->baseVal()->value());
	ret->setHeight(m_height->baseVal()->value());
	return ret;
}

SVGAnimatedLengthImpl *SVGForeignObjectElementImpl::x() const
{
	return m_x;
}

SVGAnimatedLengthImpl *SVGForeignObjectElementImpl::y() const
{
	return m_y;
}

SVGAnimatedLengthImpl *SVGForeignObjectElementImpl::width() const
{
	return m_width;
}

SVGAnimatedLengthImpl *SVGForeignObjectElementImpl::height() const
{
	return m_height;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGForeignObjectElementImpl::s_hashTable 5
 x		SVGForeignObjectElementImpl::X		DontDelete|ReadOnly
 y 		SVGForeignObjectElementImpl::Y		DontDelete|ReadOnly
 width	SVGForeignObjectElementImpl::Width	DontDelete|ReadOnly
 height	SVGForeignObjectElementImpl::Height	DontDelete|ReadOnly
@end
*/

Value SVGForeignObjectElementImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case X:
			return m_x->cache(exec);
		case Y:
			return m_y->cache(exec);
		case Width:
			return m_width->cache(exec);
		case Height:
			return m_height->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

// vim:ts=4:noet
