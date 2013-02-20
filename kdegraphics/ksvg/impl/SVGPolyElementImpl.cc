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
#include <cfloat>

#include <kdebug.h>

#include "SVGRectImpl.h"
#include "SVGPointListImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGPolyElementImpl.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

SVGPolyElementImpl::SVGPolyElementImpl(DOM::ElementImpl *impl) : SVGShapeImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl(), SVGAnimatedPointsImpl()
{
}

SVGPolyElementImpl::~SVGPolyElementImpl()
{
}

SVGRectImpl *SVGPolyElementImpl::getBBox()
{
	SVGRectImpl *ret = SVGSVGElementImpl::createSVGRect();

	unsigned int nrPoints = points()->numberOfItems();
	float minx, miny, maxx, maxy, tempx, tempy;
	minx = points()->getItem(0)->x();
	miny = points()->getItem(0)->y();
	maxx = points()->getItem(0)->x();
	maxy = points()->getItem(0)->y();

	for(unsigned int i = 1; i < nrPoints; ++i)
	{
		tempx = points()->getItem(i)->x();
		tempy = points()->getItem(i)->y();

		if(tempx < minx)
			minx = tempx;
		if(tempx > maxx)
			maxx = tempx;
		if(tempy < miny)
			miny = tempy;
		if(tempy > maxy)
			maxy = tempy;
	}

	ret->setX(minx);
	ret->setY(miny);
	ret->setWidth(maxx - minx);
	ret->setHeight(maxy - miny);
	return ret;
}

bool SVGPolyElementImpl::findOutSlope(unsigned int point, double *outSlope) const
{
	unsigned int nextPoint;

	if(point == points()->numberOfItems() - 1)
	{
		if(m_isOpenPath)
			return false;
		else
			nextPoint = 0;
	}
	else
		nextPoint = point + 1;

	if(point == nextPoint)
		return false;

	double x = points()->getItem(point)->x();
	double y = points()->getItem(point)->y();
	double nextX = points()->getItem(nextPoint)->x();
	double nextY = points()->getItem(nextPoint)->y();
	const double epsilon = DBL_EPSILON;

	if(fabs(x - nextX) < epsilon && fabs(y - nextY) < epsilon)
		return findOutSlope(nextPoint, outSlope);
	else
	{
		double slope = SVGAngleImpl::todeg(atan2(nextY - y, nextX - x));
		*outSlope = slope;
		return true;
	}
}

bool SVGPolyElementImpl::findInSlope(unsigned int point, double *inSlope) const
{
	unsigned int prevPoint;

	if(point == 0)
	{
		if(m_isOpenPath)
			return false;
		else
			prevPoint = points()->numberOfItems() - 1;
	}
	else
		prevPoint = point - 1;

	if(point == prevPoint)
		return false;
	
	double x = points()->getItem(point)->x();
	double y = points()->getItem(point)->y();
	double prevX = points()->getItem(prevPoint)->x();
	double prevY = points()->getItem(prevPoint)->y();
	const double epsilon = DBL_EPSILON;

	if(fabs(x - prevX) < epsilon && fabs(y - prevY) < epsilon)
		return findInSlope(prevPoint, inSlope);
	else
	{
		double slope = SVGAngleImpl::todeg(atan2(y - prevY, x - prevX));
		*inSlope = slope;
		return true;
	}
}

// vim:ts=4:noet
