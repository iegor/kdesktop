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

#include "SVGDocumentImpl.h"
#include "SVGSetElementImpl.h"

using namespace KSVG;

SVGSetElementImpl::SVGSetElementImpl(DOM::ElementImpl *impl) : SVGAnimationElementImpl(impl)
{
}

SVGSetElementImpl::~SVGSetElementImpl()
{
}

void SVGSetElementImpl::setAttributes()
{
	SVGAnimationElementImpl::setAttributes();

	// Always create singleShot timers when used by <set> (Niko)
	// Those are automatically deleted by the scheduler after timeout.
	ownerDoc()->timeScheduler()->addTimer(this, int(getStartTime() * 1000.0));
}

void SVGSetElementImpl::handleTimerEvent()
{
	applyAttribute(getAttributeName(), getTo());
}

// vim:ts=4:noet
