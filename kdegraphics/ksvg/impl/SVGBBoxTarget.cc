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

#include <dom/dom_misc.h>

#include "SVGShapeImpl.h"
#include "SVGBBoxTarget.h"

using namespace KSVG;

SVGBBoxTarget::SVGBBoxTarget()
{
	m_target = 0;
}

SVGBBoxTarget::~SVGBBoxTarget()
{
	if(m_target)
		dynamic_cast<DOM::DomShared *>(m_target)->deref();
}

SVGShapeImpl *SVGBBoxTarget::getBBoxTarget() const
{
	return m_target;
}

void SVGBBoxTarget::setBBoxTarget(SVGShapeImpl *target)
{
	if(m_target)
        dynamic_cast<DOM::DomShared *>(m_target)->deref();

	m_target = target;

	if(m_target)
		dynamic_cast<DOM::DomShared *>(m_target)->ref();
}

// vim:ts=4:noet
