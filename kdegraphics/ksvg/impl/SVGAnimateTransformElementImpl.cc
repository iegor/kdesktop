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

#include <qstringlist.h>

#include "SVGLengthImpl.h"
#include "SVGHelperImpl.h"
#include "SVGTransformImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGTransformListImpl.h"
#include "SVGAnimatedTransformListImpl.h"
#include "SVGAnimateTransformElementImpl.h"

using namespace KSVG;

SVGAnimateTransformElementImpl::SVGAnimateTransformElementImpl(DOM::ElementImpl *impl) : SVGAnimationElementImpl(impl)
{
	m_firstEvent = true;
	m_setAttributes = false;

	m_rotateX = -1;
	m_rotateY = -1;
	m_times = 1;
	m_from = 0;
	m_to = 0;

	m_addStep = 0;
}

SVGAnimateTransformElementImpl::~SVGAnimateTransformElementImpl()
{
}

void SVGAnimateTransformElementImpl::setAttributes()
{
	if(m_setAttributes)
		return;

	m_setAttributes = true;

	SVGAnimationElementImpl::setAttributes();

	/*
	// TODO: much :)
	
	DOM::DOMString _type = getAttribute("type");
	if(!_type.isNull())
		m_type = _type.string().lower();

	DOM::DOMString _from = getAttribute("from");
	if(!_from.isNull())
	{
		if(m_type != "rotate")
		{
			SVGLengthImpl *temp = SVGSVGElementImpl::createSVGLength();
			temp->setValueAsString(_from.string());

			m_from = temp->value();

			temp->deref();
		}
		else
		{
			SVGTransformListImpl *list = new SVGTransformListImpl();
			list->ref();

			SVGHelperImpl::parseTransformAttribute(list, m_type + "(" + _from.string() + ")");

			m_from = list->getFirst()->angle();
			
			QStringList stringList = QStringList::split(' ', list->getFirst()->toString());
			m_rotateX = stringList[1].toInt();
			m_rotateY = stringList[2].mid(0, stringList[2].length() - 1).toInt();
			
			list->deref();
		}

		m_newFrom = m_from;
		m_addStep = m_from;
	}
	
	DOM::DOMString _to = getAttribute("to");
	if(!_to.isNull())
	{
		if(m_type != "rotate")
		{
			SVGLengthImpl *temp = SVGSVGElementImpl::createSVGLength();
			temp->setValueAsString(_to.string());

			m_to = temp->value();

			temp->deref();
		}
		else
		{
			SVGTransformListImpl *list = new SVGTransformListImpl();
			list->ref();

			SVGHelperImpl::parseTransformAttribute(list, m_type + "(" + _to.string() + ")");

			m_to = list->getFirst()->angle();

			list->deref();
		}

		m_newTo = m_to;
	}

	// TODO: values + rotate including cx + cy
	DOM::DOMString _values = getAttribute("values");
	if(!_values.isNull())
	{
		QString test = _values.string();

		if(test.contains(";"))
		{
			SVGLengthImpl *temp = SVGSVGElementImpl::createSVGLength();
			
			QStringList list = QStringList::split(';', test);
			temp->setValueAsString(list[0]);
			m_from = temp->value();
			temp->setValueAsString(list[1]);
			m_to = temp->value();

			m_newFrom = m_from;
			m_newTo = m_to;
			m_addStep = m_from;

			temp->deref();
		}
	}
	
	if(getStartTime() != -1)
		addTimer(int(getStartTime() * 1000));
	else
		addTimer(1);
		*/
}

void SVGAnimateTransformElementImpl::handleTimerEvent(bool)
{ /*
	if(m_firstEvent)
	{
		m_firstEvent = false;

		m_steps = (int) (getSimpleDuration() * 1000) / interval();
		m_step = 0;

		m_addStep = (m_to - m_from) / m_steps;

		m_transformTimer = addTimer(interval() * 1000, false);
	}
	else
	{
		m_from += m_addStep;

		SVGTransformImpl *transform = SVGSVGElementImpl::createSVGTransform();
		
		if(m_type == "rotate")
		{
			int x = 0, y = 0;

			if(m_rotateX != -1)
				x = m_rotateX;

			if(m_rotateY != -1)
				y = m_rotateY;
			
			transform->setRotate(m_from, x, y);
		}
		else if(m_type == "scale")
			transform->setScale(m_from, m_from);
		else if(m_type == "skewx")
			transform->setSkewX(m_from);
		else if(m_type == "skewy")
			transform->setSkewY(m_from);

		QString trans = transform->toString();
		QString last = trans;
		
		if(targetElement()->hasAttribute("transform"))
		{
			trans += " " + targetElement()->getAttribute("transform").string();

			if(!m_lastTransform.isEmpty())
			{
				int pos = trans.find(m_lastTransform);
				
				QString extract;
				extract += trans.mid(0, pos);
				extract += trans.mid(pos + m_lastTransform.length() + 1, trans.length());

				trans = extract;
			}
		}

		m_lastTransform = last;
		
		transform->deref();
		
		applyAttribute("transform", trans);
		
		m_step++;

		if(m_step == m_steps)
		{
			removeTimer(m_transformTimer);

			if(getRepeatDuration() == "indefinite" || getRepeatCount() == "indefinite")
			{
				m_firstEvent = true;
				m_from = m_newFrom;
				m_to = m_newTo;

				m_addStep = 0;
				m_transformTimer = 0;

				addTimer(1);
			}
			else if(!getRepeatCount().isEmpty())
			{
				if(m_times <= getRepeatCount().toInt())
				{
					m_times++;
					m_firstEvent = true;
					m_from = m_newFrom;
					m_to = m_newTo;

					m_addStep = 0;
					m_transformTimer = 0;

					addTimer(1);
				}
			}
		}
	}
	*/
}

// vim:ts=4:noet
