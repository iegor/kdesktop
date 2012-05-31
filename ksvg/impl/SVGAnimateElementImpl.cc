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

#include "SVGDocumentImpl.h"
#include "SVGAnimateElementImpl.h"

using namespace KSVG;

SVGAnimateElementImpl::SVGAnimateElementImpl(DOM::ElementImpl *impl) : SVGAnimationElementImpl(impl)
{
	m_animVal = 0.0;
	m_addStep = 0.0;
}

SVGAnimateElementImpl::~SVGAnimateElementImpl()
{
}

void SVGAnimateElementImpl::setAttributes()
{
	SVGAnimationElementImpl::setAttributes();
	
	ownerDoc()->timeScheduler()->addTimer(this, int(getStartTime() * 1000.0));
}

void SVGAnimateElementImpl::handleTimerEvent()
{
	if(!m_connected)
	{
		double duration = getSimpleDuration() * 1000.0;
		double dinterval = SVGTimeScheduler::staticTimerInterval;

		m_step = 0;
		m_steps = (int) rint(duration / dinterval);

		double to = 0, from = 0;
		if(getTo().isEmpty())
			to = targetElement()->getAttribute(getAttributeName()).string().toDouble();
		else
			to = getTo().toDouble();

		if(getFrom().isEmpty())
			from = targetElement()->getAttribute(getAttributeName()).string().toDouble();
		else
			from = getFrom().toDouble();
			
		// 'by' support
		if(!getBy().isEmpty())
		{
			m_animVal = from;
			m_addStep = getBy().toDouble() / m_steps;
		}
		else
		{
			m_animVal = from;
			m_addStep = (to - m_animVal) / m_steps;
		}

		m_connected = true;
		ownerDoc()->timeScheduler()->connectIntervalTimer(this);
	}
	else
	{
		m_animVal += m_addStep;
		applyAttribute(getAttributeName(), QString::number(m_animVal));
	}

	if(m_step < m_steps)
		m_step++;
	else
	{
		ownerDoc()->timeScheduler()->disconnectIntervalTimer(this);
		m_connected = false;
		if(m_fill == REMOVE)
			applyAttribute(getAttributeName(), targetElement()->getAttribute(getAttributeName()).string());
	}

#if 0
		m_attributeTimer = addTimer(interval() * 1000, false);
		

		/*
		m_steps = (getSimpleDuration() * 1000) / SVGAnimationElementImpl::timerTime;
		m_step = 0;

		if(m_additive == "sum" && m_times == 1 && !needCombine)
		{
			float add; 
			
			SVGLengthImpl *temp = new SVGLengthImpl();
			temp->ref();
			temp->setValueAsString(targetElement()->getAttribute(m_attributeName).string());
			add = temp->value();
			temp->deref();

			m_from += add;
			m_newFrom += add;
			m_to += add;
			m_newTo += add;

			m_additiveAdded = add;
		}

		m_addStep = (m_to - m_from) / m_steps;
		m_attributeTimer = addTimer(SVGAnimationElementImpl::timerTime, false);*/
	}
	else
	{
/*		m_from += m_addStep;

		if(m_additive == "replace" && needCombine)
			needCombine = false;
		
		applyAttribute(m_attributeName, QString::number(m_from), needCombine);

		m_step++;

		if(m_step == m_steps)
		{
			removeTimer(m_attributeTimer);
            
			if(getRepeatDuration() == "indefinite" || getRepeatCount() == "indefinite")
			{
				m_times++;
				m_firstEvent = true;

				if(m_accumulate != "sum")
				{
					m_from = m_newFrom;
					m_to = m_newTo;
				}
				else
				{
					m_from += (m_newFrom - m_additiveAdded) / 2;
					m_to += (m_newTo - m_additiveAdded) / 2;
				}

				m_addStep = 0;
				m_attributeTimer = 0;

				addTimer(1);
			}
			else if(!getRepeatCount().isEmpty())
			{
				if(m_times <= getRepeatCount().toInt())
				{
					m_times++;
					m_firstEvent = true;

					if(m_accumulate != "sum")
					{
						m_from = m_newFrom;
						m_to = m_newTo;
					}
					else
					{
						m_from += (m_newFrom - m_additiveAdded) / 2;
						m_to += (m_newTo - m_additiveAdded) / 2;
					}
								
					m_addStep = 0;
					m_attributeTimer = 0;

					addTimer(1);
				}
			}
		}*/
	}
#endif // 0
}

// vim:ts=4:noet
