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

#include "CanvasItem.h"
#include "SVGPointListImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGAnimatedPointsImpl.h"

using namespace KSVG;

#include "SVGAnimatedPointsImpl.lut.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGAnimatedPointsImpl::SVGAnimatedPointsImpl() : DOM::DomShared()
{
	KSVG_EMPTY_FLAGS

	m_points = new SVGPointListImpl();
	m_points->ref();

	m_animatedPoints = new SVGPointListImpl();
	m_animatedPoints->ref();
}

SVGAnimatedPointsImpl::~SVGAnimatedPointsImpl()
{
	if(m_points)
		m_points->deref();
	if(m_animatedPoints)
		m_animatedPoints->deref();
}

SVGPointListImpl *SVGAnimatedPointsImpl::points() const
{
	return m_points;
}

SVGPointListImpl *SVGAnimatedPointsImpl::animatedPoints() const
{
	return m_animatedPoints;
}

void SVGAnimatedPointsImpl::parsePoints(QString _points, SVGPointListImpl *points)
{
	if(_points.isEmpty())
		return;

	_points = _points.simplifyWhiteSpace();

	if(_points.contains(",,") || _points.contains(", ,"))
		return;

	_points.replace(',', ' ');
	_points.replace('\r', QString::null);
	_points.replace('\n', QString::null);

	_points = _points.simplifyWhiteSpace();

	QStringList pointList = QStringList::split(' ', _points);
	for(QStringList::Iterator it = pointList.begin(); it != pointList.end(); it++)
	{
		SVGPointImpl *point = SVGSVGElementImpl::createSVGPoint();
		point->setX((*(it++)).toFloat());
		point->setY((*it).toFloat());

		points->appendItem(point);
	}
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGAnimatedPointsImpl::s_hashTable 3
 points			SVGAnimatedPointsImpl::Points			DontDelete|ReadOnly
 animatedPoints	SVGAnimatedPointsImpl::AnimatedPoints	DontDelete|ReadOnly
@end
*/

Value SVGAnimatedPointsImpl::getValueProperty(ExecState *exec, int token) const
{
	//KSVG_CHECK_ATTRIBUTE
	
	switch(token)
	{
		case Points: // TODO: We need a pointList->toText function, quite trivial
//			if(!attributeMode)		
				return points()->cache(exec);
		case AnimatedPoints:
			return animatedPoints()->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGAnimatedPointsImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case Points:
			parsePoints(value.toString(exec).qstring(), m_points);
			break;
		case AnimatedPoints:
			parsePoints(value.toString(exec).qstring(), m_animatedPoints);
			break;
		default:	
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
