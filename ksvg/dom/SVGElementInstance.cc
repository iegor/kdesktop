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

#include "SVGElementInstance.h"
#include "SVGElementInstanceImpl.h"
#include "SVGElementInstanceList.h"
#include "SVGElement.h"
#include "SVGUseElement.h"

using namespace KSVG;

SVGElementInstance::SVGElementInstance()
{
	impl = new SVGElementInstanceImpl();
	impl->ref();
}

SVGElementInstance::SVGElementInstance(const SVGElementInstance &other) : impl(0)
{
	(*this) = other;
}

SVGElementInstance &SVGElementInstance::operator =(const SVGElementInstance &other)
{
	if(impl == other.impl)
		return *this;

	if(impl)
		impl->deref();

	impl = other.impl;

	if(impl)
		impl->ref();

	return *this;
}

SVGElementInstance::SVGElementInstance(SVGElementInstanceImpl *other)
{
	impl = other;
	if(impl)
		impl->ref();
}

SVGElementInstance::~SVGElementInstance()
{
	if(impl)
		impl->deref();
}

SVGElement SVGElementInstance::correspondingElement() const
{
	if(!impl) return SVGElement(0);
	return SVGElement(impl->correspondingElement());
}

SVGUseElement SVGElementInstance::correspondingUseElement() const
{
	if(!impl) return SVGUseElement(0);
	return SVGUseElement(impl->correspondingUseElement());
}

SVGElementInstance SVGElementInstance::parentNode() const
{
	if(!impl) return SVGElementInstance(0);
	return SVGElementInstance(impl->parentNode());
}

SVGElementInstanceList SVGElementInstance::childNodes() const
{
	if(!impl) return SVGElementInstanceList(0);
	return SVGElementInstanceList(impl->childNodes());
}

SVGElementInstance SVGElementInstance::firstChild() const
{
	if(!impl) return SVGElementInstance(0);
	return SVGElementInstance(impl->firstChild());
}

SVGElementInstance SVGElementInstance::lastChild() const
{
	if(!impl) return SVGElementInstance(0);
	return SVGElementInstance(impl->lastChild());
}

SVGElementInstance SVGElementInstance::previousSibling() const
{
	if(!impl) return SVGElementInstance(0);
	return SVGElementInstance(impl->previousSibling());
}

SVGElementInstance SVGElementInstance::nextSibling() const
{
	if(!impl) return SVGElementInstance(0);
	return SVGElementInstance(impl->nextSibling());
}

// vim:ts=4:noet
