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

#include <qstringlist.h>

#include "SVGElementImpl.h"
#include "SVGViewSpecImpl.h"
#include "SVGTransformListImpl.h"

using namespace KSVG;

SVGViewSpecImpl::SVGViewSpecImpl() : SVGZoomAndPanImpl(), SVGFitToViewBoxImpl()
{
	m_transform = new SVGTransformListImpl();
	m_transform->ref();

	m_viewTarget = new SVGElementImpl(0);
	m_viewTarget->ref();
}

SVGViewSpecImpl::~SVGViewSpecImpl()
{
	if(m_transform)
		m_transform->deref();
	if(m_viewTarget)
		m_viewTarget->deref();
}

SVGTransformListImpl *SVGViewSpecImpl::transform() const
{
	return m_transform;
}

SVGElementImpl *SVGViewSpecImpl::viewTarget() const
{
	return m_viewTarget;
}

DOM::DOMString SVGViewSpecImpl::viewBoxString() const
{
	return m_viewBoxString;
}

DOM::DOMString SVGViewSpecImpl::preserveAspectRatioString() const
{
	return m_preserveAspectRatioString;
}

DOM::DOMString SVGViewSpecImpl::transformString() const
{
	return m_transformString;
}

DOM::DOMString SVGViewSpecImpl::viewTargetString() const
{
	return m_viewTargetString;
}

bool SVGViewSpecImpl::parseViewSpec(const QString &s)
{
	if(!s.startsWith("svgView("))
		return false;

	// remove 'svgView(' and ')'
	QStringList subAttrs = QStringList::split(';', s.mid(8));

	for(QStringList::ConstIterator it = subAttrs.begin() ; it != subAttrs.end(); ++it)
	{
		if((*it).startsWith("viewBox("))
			m_viewBoxString = (*it).mid(8);
		else if((*it).startsWith("zoomAndPan("))
			parseZoomAndPan(DOM::DOMString((*it).mid(11)));
		else if((*it).startsWith("preserveAspectRatio("))
			m_preserveAspectRatioString = (*it).mid(20);
	}
	return true;
}

// vim:ts=4:noet
