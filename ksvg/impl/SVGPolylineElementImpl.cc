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
#include "SVGPolylineElementImpl.h"
#include "SVGDocumentImpl.h"
#include "KSVGCanvas.h"
#include "SVGAngleImpl.h"

using namespace KSVG;

SVGPolylineElementImpl::SVGPolylineElementImpl(DOM::ElementImpl *impl) : SVGPolyElementImpl(impl)
{
	m_isOpenPath = true;
}

void SVGPolylineElementImpl::createItem(KSVGCanvas *c)
{
	if(!c)
		c = ownerDoc()->canvas();

	if(!m_item)
	{
		m_item = c->createPolyline(this);
		c->insert(m_item);
	}
}

void SVGPolylineElementImpl::drawMarkers()
{
	SVGPointListImpl *pts = points();
	unsigned int nrPoints = pts->numberOfItems();

	if(nrPoints > 0 && hasMarkers())
	{
		if(hasStartMarker())
		{
			double outSlope;

			if(!findOutSlope(0, &outSlope))
				outSlope = 0;

			doStartMarker(this, this, pts->getItem(0)->x(), pts->getItem(0)->y(), outSlope);
		}

		if(hasMidMarker())
		{
			for(unsigned int i = 1; i < nrPoints - 1; ++i)
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

				doMidMarker(this, this, pts->getItem(i)->x(), pts->getItem(i)->y(), bisector);
			}
		}

		if(hasEndMarker())
		{
			double inSlope;

			if(!findInSlope(nrPoints - 1, &inSlope))
				inSlope = 0;

			doEndMarker(this, this, pts->getItem(nrPoints - 1)->x(), pts->getItem(nrPoints - 1)->y(), inSlope);
		}
	}
}

// vim:ts=4:noet
