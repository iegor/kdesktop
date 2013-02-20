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

#include "SVGPathSegListImpl.h"
#include "SVGAnimatedPathDataImpl.h"

using namespace KSVG;

#include "SVGAnimatedPathDataImpl.lut.h"
#include "ksvg_bridge.h"

SVGAnimatedPathDataImpl::SVGAnimatedPathDataImpl() : DOM::DomShared()
{
	m_pathSegList = new SVGPathSegListImpl();
	m_pathSegList->ref();
	
	m_normalizedPathSegList = new SVGPathSegListImpl();
	m_normalizedPathSegList->ref();
	
	m_animatedPathSegList = new SVGPathSegListImpl();
	m_animatedPathSegList->ref();

	m_animatedNormalizedPathSegList = new SVGPathSegListImpl();
	m_animatedNormalizedPathSegList->ref();
}

SVGAnimatedPathDataImpl::~SVGAnimatedPathDataImpl()
{
	if(m_pathSegList)
		m_pathSegList->deref();
	if(m_normalizedPathSegList)
		m_normalizedPathSegList->deref();
	if(m_animatedPathSegList)
		m_animatedPathSegList->deref();
	if(m_animatedNormalizedPathSegList)
		m_animatedNormalizedPathSegList->deref();
}

SVGPathSegListImpl *SVGAnimatedPathDataImpl::pathSegList() const
{
	return m_pathSegList;
}

SVGPathSegListImpl *SVGAnimatedPathDataImpl::normalizedPathSegList() const
{
	return m_normalizedPathSegList;
}

SVGPathSegListImpl *SVGAnimatedPathDataImpl::animatedPathSegList() const
{
	return m_animatedPathSegList;
}

SVGPathSegListImpl *SVGAnimatedPathDataImpl::animatedNormalizedPathSegList() const
{
	return m_animatedNormalizedPathSegList;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGAnimatedPathDataImpl::s_hashTable 5
 pathSegList  			 		 SVGAnimatedPathDataImpl::PathSegList 					DontDelete|ReadOnly
 normalizedPathSegList   		 SVGAnimatedPathDataImpl::NormalizedPathSegList			DontDelete|ReadOnly
 animatedPathSegList  			 SVGAnimatedPathDataImpl::AnimatedPathSegList 			DontDelete|ReadOnly
 animatedNormalizedPathSegList   SVGAnimatedPathDataImpl::AnimatedNormalizedPathSegList DontDelete|ReadOnly
@end
*/

Value SVGAnimatedPathDataImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case PathSegList:
			return pathSegList()->cache(exec);
		case NormalizedPathSegList:
			return normalizedPathSegList()->cache(exec);
		case AnimatedPathSegList:
			return animatedPathSegList()->cache(exec);
		case AnimatedNormalizedPathSegList:
			return animatedNormalizedPathSegList()->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
}
}

// vim:ts=4:noet
