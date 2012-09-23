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

#include "SVGAnimatedLengthListImpl.h"

using namespace KSVG;

#include "SVGAnimatedLengthListImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGAnimatedLengthListImpl::SVGAnimatedLengthListImpl() : DOM::DomShared()
{
	m_baseVal = new SVGLengthListImpl();
	m_baseVal->ref();

	m_animVal = new SVGLengthListImpl();
	m_animVal->ref();
}

SVGAnimatedLengthListImpl::SVGAnimatedLengthListImpl(const SVGAnimatedLengthListImpl &other) : DOM::DomShared()
{
	(*this) = other;
}

SVGAnimatedLengthListImpl::~SVGAnimatedLengthListImpl()
{
	if(m_baseVal)
		m_baseVal->deref();
	if(m_animVal)
		m_animVal->deref();
}

SVGAnimatedLengthListImpl &SVGAnimatedLengthListImpl::operator=(const SVGAnimatedLengthListImpl &other)
{
	*m_baseVal = *(other.m_baseVal);
	*m_animVal = *(other.m_animVal);

	return *this;
}

SVGLengthListImpl *SVGAnimatedLengthListImpl::baseVal() const
{
	return m_baseVal;
}

SVGLengthListImpl *SVGAnimatedLengthListImpl::animVal() const
{
	return m_animVal;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGAnimatedLengthListImpl::s_hashTable 3
 baseVal	SVGAnimatedLengthListImpl::BaseVal	DontDelete|ReadOnly
 animVal	SVGAnimatedLengthListImpl::AnimVal	DontDelete|ReadOnly
@end
*/

Value SVGAnimatedLengthListImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case BaseVal:
			return m_baseVal->cache(exec);
		case AnimVal:
			return m_animVal->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

// vim:ts=4:noet
