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

#ifndef SVGAnimateTransformElementImpl_H
#define SVGAnimateTransformElementImpl_H

#include "ksvg_lookup.h"

#include "SVGAnimationElementImpl.h"

class QTimer;

namespace KSVG
{

class SVGTransformImpl;
class SVGTransformListImpl;
class SVGAnimateTransformElementImpl : public SVGAnimationElementImpl
{
public:
	SVGAnimateTransformElementImpl(DOM::ElementImpl *);
	virtual ~SVGAnimateTransformElementImpl();

	virtual void handleTimerEvent(bool needCombine = false);
	virtual void setAttributes();

private:
	QString m_type, m_lastTransform;

	int m_times;

	int m_rotateX, m_rotateY;

	int m_steps, m_step;

	double m_from, m_to, m_newFrom, m_newTo;
	double m_addStep;

	QTimer *m_transformTimer;
	SVGTransformListImpl *m_transformList;

	bool m_firstEvent;
	bool m_setAttributes;
	
public:
	KSVG_BRIDGE
	KSVG_FORWARDGET
	KSVG_FORWARDPUT
};

KSVG_REGISTER_ELEMENT(SVGAnimateTransformElementImpl, "animateTransform")

}

#endif

// vim:ts=4:noet
