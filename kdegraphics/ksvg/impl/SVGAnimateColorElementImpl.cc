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

#include <math.h>

#include <kdebug.h>

#include "SVGColorImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGStylableImpl.h"
#include "SVGAnimateColorElementImpl.h"

using namespace KSVG;

SVGAnimateColorElementImpl::SVGAnimateColorElementImpl(DOM::ElementImpl *impl) : SVGAnimationElementImpl(impl)
{
	m_fromColor = new SVGColorImpl(this);
	m_fromColor->ref();

	m_toColor = new SVGColorImpl(this);
	m_toColor->ref();
}

SVGAnimateColorElementImpl::~SVGAnimateColorElementImpl()
{
	m_fromColor->deref();
	m_toColor->deref();
}

/*
 * Outstanding issues
 * - 'values' support
 * - 'by' support
 */

void SVGAnimateColorElementImpl::setAttributes()
{
	SVGAnimationElementImpl::setAttributes();
	
	SVGStylableImpl::setColor(getFrom(), m_fromColor);
	SVGStylableImpl::setColor(getTo(), m_toColor);

	ownerDoc()->timeScheduler()->addTimer(this, int(getStartTime() * 1000.0));
}

void SVGAnimateColorElementImpl::handleTimerEvent()
{
	if(!m_connected)
	{
		double duration = getSimpleDuration() * 1000.0;
		double dinterval = SVGTimeScheduler::staticTimerInterval;

		m_step = 0;
		m_steps = (int) rint(duration / dinterval);

		m_connected = true;
		ownerDoc()->timeScheduler()->connectIntervalTimer(this);
	}
	else
	{
		QColor fromColor(m_fromColor->rgbColor().color());
		QColor toColor(m_toColor->rgbColor().color());
	
		int red = (int) rint(((toColor.red() - fromColor.red()) / static_cast<double>(m_steps)) * m_step + fromColor.red());
		int green = (int) rint(((toColor.green() - fromColor.green()) / static_cast<double>(m_steps)) * m_step + fromColor.green());
		int blue = (int) rint(((toColor.blue() - fromColor.blue()) / static_cast<double>(m_steps)) * m_step + fromColor.blue());

		QString color = "rgb(" + QString::number(red) + "," + QString::number(green) + "," + QString::number(blue) + ")";
		applyAttribute(getAttributeName(), color);
	}

	if(m_step < m_steps)
		m_step++;
	else
	{
		ownerDoc()->timeScheduler()->disconnectIntervalTimer(this);
		m_connected = false;
	}
}

// vim:ts=4:noet
