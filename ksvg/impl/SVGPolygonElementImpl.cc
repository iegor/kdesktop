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

#include "SVGPointListImpl.h"
#include "SVGPolygonElementImpl.h"
#include "SVGDocumentImpl.h"
#include "KSVGCanvas.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

SVGPolygonElementImpl::SVGPolygonElementImpl(DOM::ElementImpl *impl) : SVGPolyElementImpl(impl)
{
	m_isOpenPath = false;
}

void SVGPolygonElementImpl::createItem(KSVGCanvas *c)
{
    if(!c)
        c = ownerDoc()->canvas();

	if(!m_item)
	{
		m_item = c->createPolygon(this);
		c->insert(m_item);
	}
}

void SVGPolygonElementImpl::drawMarkers()
{
	SVGPointListImpl *pts = points();
	unsigned int nrPoints = pts->numberOfItems();

	if(nrPoints > 0 && hasMarkers())
	{
		for(unsigned int i = 0; i < nrPoints; ++i)
		{
			double inSlope;
			double outSlope;
			bool haveInSlope = findInSlope(i, &inSlope);
			bool haveOutSlope = findOutSlope(i, &outSlope);

			if(!haveInSlope && haveOutSlope)
				inSlope = outSlope;
			else if(haveInSlope && !haveOutSlope)
				outSlope = inSlope;
			else if(!haveInSlope && !haveOutSlope)
			{
				inSlope = 0;
				outSlope = 0;
			}

			double bisector = SVGAngleImpl::shortestArcBisector(inSlope, outSlope);

			if(i == 0)
			{
				if(hasStartMarker())
					doStartMarker(this, this, pts->getItem(i)->x(), pts->getItem(i)->y(), bisector);
				if(hasEndMarker())
					doEndMarker(this, this, pts->getItem(i)->x(), pts->getItem(i)->y(), bisector);
			}
			else
			{
				if(hasMidMarker())
					doMidMarker(this, this, pts->getItem(i)->x(), pts->getItem(i)->y(), bisector);
			}
		}
	}
}

// vim:ts=4:noet
