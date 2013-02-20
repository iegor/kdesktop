/*
    Copyright (C) 2003 KSVG Team
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

#include "KSVGCanvas.h"
#include "CanvasItem.h"
#include "SVGShapeImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGTimeScheduler.moc"

using namespace KSVG;

SVGTimer::SVGTimer(QObject *scheduler, unsigned int ms, bool singleShot)
{
	m_ms = ms;
	m_singleShot = singleShot;
	m_timer = new QTimer(scheduler);
}

SVGTimer::~SVGTimer()
{
	delete m_timer;
}

bool SVGTimer::operator==(const QTimer *timer)
{
	return (m_timer == timer);
}

const QTimer *SVGTimer::qtimer() const
{
	return m_timer;
}

void SVGTimer::start(QObject *receiver, const char *member)
{
	QObject::connect(m_timer, SIGNAL(timeout()), receiver, member);
	m_timer->start(m_ms, m_singleShot);
}

void SVGTimer::stop()
{
	m_timer->stop();
}

bool SVGTimer::isActive() const
{
	return m_timer->isActive();
}

unsigned int SVGTimer::ms() const
{
	return m_ms;
}

bool SVGTimer::singleShot() const
{
	return m_singleShot;
}

void SVGTimer::notifyAll()
{
	if(m_notifyList.isEmpty())
		return;

	QValueList<SVGElementImpl *> elements;
	for(unsigned int i = m_notifyList.count();i > 0; i--)
	{
		SVGElementImpl *element = m_notifyList[i - 1];
		if(!element)
			continue;

		SVGAnimationElementImpl *animation = dynamic_cast<SVGAnimationElementImpl *>(element);
		if(animation)
		{
			animation->handleTimerEvent();

			SVGElementImpl *target = animation->targetElement();
			if(!elements.contains(target))
				elements.append(target);
		}
	}

	// Optimized update logic (to avoid 4 updates, on the same element)
	QValueList<SVGElementImpl *>::iterator it2;
	for(it2 = elements.begin(); it2 != elements.end(); ++it2)
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(*it2);
		if(shape && shape->item())
			shape->item()->update(UPDATE_TRANSFORM);
	}
}

void SVGTimer::addNotify(SVGElementImpl *element)
{
	m_notifyList.append(element);
}

void SVGTimer::removeNotify(SVGElementImpl *element)
{
	m_notifyList.remove(element);
	
	if(m_notifyList.isEmpty())
		stop();
}

const unsigned int SVGTimeScheduler::staticTimerInterval = 15; // milliseconds

SVGTimeScheduler::SVGTimeScheduler(SVGDocumentImpl *doc) : QObject(), m_doc(doc)
{
	// Create static interval timers but don't start it yet!
	m_intervalTimer = new SVGTimer(this, staticTimerInterval, false);	
	m_creationTime.start();
}

SVGTimeScheduler::~SVGTimeScheduler()
{
	// Usually singleShot timers cleanup themselves, after usage
	SVGTimerList::iterator it;
	for(it = m_timerList.begin(); it != m_timerList.end(); ++it)
	{
		SVGTimer *svgTimer = *it;
		delete svgTimer;
	}
	delete m_intervalTimer;
}

void SVGTimeScheduler::addTimer(SVGElementImpl *element, unsigned int ms)
{
	SVGTimer *svgTimer = new SVGTimer(this, ms, true);
	svgTimer->addNotify(element);
	m_timerList.append(svgTimer);
}

void SVGTimeScheduler::connectIntervalTimer(SVGElementImpl *element)
{
	m_intervalTimer->addNotify(element);
}

void SVGTimeScheduler::disconnectIntervalTimer(SVGElementImpl *element)
{
	m_intervalTimer->removeNotify(element);
}

void SVGTimeScheduler::startAnimations()
{
	SVGTimerList::iterator it;
	for(it = m_timerList.begin(); it != m_timerList.end(); ++it)
	{
		SVGTimer *svgTimer = *it;
		if(svgTimer && !svgTimer->isActive())
			svgTimer->start(this, SLOT(slotTimerNotify()));
	}
}

void SVGTimeScheduler::toggleAnimations()
{
	if(m_intervalTimer->isActive())
		m_intervalTimer->stop();
	else
		m_intervalTimer->start(this, SLOT(slotTimerNotify()));
}

bool SVGTimeScheduler::animationsPaused() const
{
	return !m_intervalTimer->isActive();
}

void SVGTimeScheduler::slotTimerNotify()
{
	QTimer *senderTimer = const_cast<QTimer *>(static_cast<const QTimer *>(sender()));

	SVGTimer *svgTimer = 0;
	SVGTimerList::iterator it;
	for(it = m_timerList.begin(); it != m_timerList.end(); ++it)
	{
		SVGTimer *cur = *it;
		if(*cur == senderTimer)
		{
			svgTimer = cur;
			break;
		}
	}

	if(!svgTimer)
	{
		svgTimer = (*m_intervalTimer == senderTimer) ? m_intervalTimer : 0;
	
		if(!svgTimer)
			return;
	}

	svgTimer->notifyAll();

	// Animations need direct updates
	if(m_doc->canvas())
		m_doc->canvas()->update();
	emit m_doc->finishedRendering();

	if(svgTimer->singleShot())
	{
		m_timerList.remove(svgTimer);
		delete svgTimer;
	}

	// The singleShot timers of ie. <animate> with begin="3s" are notified
	// by the previous call, and now all connections to the interval timer
	// are created and now we just need to fire that timer (Niko)
	if(svgTimer != m_intervalTimer && !m_intervalTimer->isActive())
		m_intervalTimer->start(this, SLOT(slotTimerNotify()));
}

float SVGTimeScheduler::elapsed() const
{
	return float(m_creationTime.elapsed()) / 1000.0;
}

// vim:ts=4:noet
