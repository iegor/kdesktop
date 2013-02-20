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

#include "SVGElementImpl.h"
#include "SVGUseElementImpl.h"
#include "SVGElementInstanceImpl.h"
#include "SVGElementInstanceListImpl.h"

using namespace KSVG;

SVGElementInstanceImpl::SVGElementInstanceImpl()//FIXME : events::EventTarget()
{
	m_correspondingElement = 0;
	m_correspondingUseElement = 0;
	m_parentNode = 0;
	m_childNodes = 0;
	m_firstChild = 0;
	m_lastChild = 0;
	m_previousSibling = 0;
	m_nextSibling = 0;
}

SVGElementInstanceImpl::~SVGElementInstanceImpl()
{
	if(m_correspondingElement)
		m_correspondingElement->deref();
	if(m_correspondingUseElement)
		m_correspondingUseElement->deref();
	if(m_parentNode)
		m_parentNode->deref();
	if(m_childNodes)
		m_childNodes->deref();
	if(m_firstChild)
		m_firstChild->deref();
	if(m_lastChild)
		m_lastChild->deref();
	if(m_previousSibling)
		m_previousSibling->deref();
	if(m_nextSibling)
		m_nextSibling->deref();
}

void SVGElementInstanceImpl::setCorrespondingElement(SVGElementImpl *corresponding)
{
	m_correspondingElement = corresponding;

	if(m_correspondingElement)
		m_correspondingElement->ref();
}

SVGElementImpl *SVGElementInstanceImpl::correspondingElement() const
{
	return m_correspondingElement;
}

SVGUseElementImpl *SVGElementInstanceImpl::correspondingUseElement() const
{
	return m_correspondingUseElement;
}

SVGElementInstanceImpl *SVGElementInstanceImpl::parentNode() const
{
	return m_parentNode;
}

SVGElementInstanceListImpl *SVGElementInstanceImpl::childNodes() const
{
	return m_childNodes;
}

SVGElementInstanceImpl *SVGElementInstanceImpl::firstChild() const
{
	return m_firstChild;
}

SVGElementInstanceImpl *SVGElementInstanceImpl::lastChild() const
{
	return m_lastChild;
}

SVGElementInstanceImpl *SVGElementInstanceImpl::previousSibling() const
{
	return m_previousSibling;
}

SVGElementInstanceImpl *SVGElementInstanceImpl::nextSibling() const
{
	return m_nextSibling;
}

// vim:ts=4:noet
