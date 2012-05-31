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

// TODO: CLEANUP

#include <dom/dom_doc.h>
#include "SVGDocumentImpl.h"
#include "SVGPathElementImpl.h"
#include "SVGAnimateMotionElementImpl.h"

using namespace KSVG;

SVGAnimateMotionElementImpl::SVGAnimateMotionElementImpl(DOM::ElementImpl *impl) : SVGAnimationElementImpl(impl)
{
}

SVGAnimateMotionElementImpl::~SVGAnimateMotionElementImpl()
{
}

void SVGAnimateMotionElementImpl::setAttributes()
{
	SVGAnimationElementImpl::setAttributes();

/*
	// TODO: rotate..

	DOM::DOMString _path = getAttribute("path");
	if(!_path.isNull())
	{
		if(m_path)
			m_path->deref();

		m_path = new SVGPathElementImpl(reinterpret_cast<DOM::ElementImpl *>(static_cast<DOM::Document *>(ownerDoc())->createElement("path").handle()));
		m_path->setOwnerDoc(ownerDoc());
		m_path->setAttribute("d", _path);
		m_path->setAttributes();
	}

	addTimer(int(getStartTime() * 1000));*/
}

//void SVGAnimateMotionElementImpl::prerender(KSVGPainter *p)
//{
//	SVGAnimationElementImpl::prerender(p);

/*	if(!m_pathArray)
	{
		bool temp;
		m_pathArray = m_path->preparePath(&temp, p->worldMatrix());
	}*/
//}

void SVGAnimateMotionElementImpl::handleTimerEvent(bool /*needCombine*/)
{
/*	if(m_firstEvent)
	{
		m_firstEvent = false;

		m_steps = (int) (getSimpleDuration() * 1000) / interval();
		m_step = 0;

		m_motionTimer = addTimer(interval() * 1000, false);
	}
	else
	{
		m_step++;

		if(m_step <= m_pathArray->count() - 1)
		{
			QPoint p = m_pathArray->point(m_step - 50);
			applyAttribute("x", QString::number(p.x()));
			applyAttribute("y", QString::number(p.y()));
			kdDebug() << " X " << p.x() << " Y " << p.y() << " (" << m_pathArray->count() << "; " << m_step << ")" <<endl;
		}
		
		if(m_step == m_steps)
		{
			// TODO: repeat
			removeTimer(m_motionTimer);
		}
	}*/
}

// vim:ts=4:noet
