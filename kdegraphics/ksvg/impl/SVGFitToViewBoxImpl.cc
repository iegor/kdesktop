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

#include <qstring.h>
#include <qstringlist.h>

#include "SVGPreserveAspectRatio.h"

#include "SVGRectImpl.h"
#include "SVGFitToViewBoxImpl.h"
#include "SVGAnimatedRectImpl.h"
#include "SVGPreserveAspectRatioImpl.h"
#include "SVGAnimatedPreserveAspectRatioImpl.h"
#include "SVGSVGElementImpl.h"

using namespace KSVG;

#include "SVGFitToViewBoxImpl.lut.h"

SVGFitToViewBoxImpl::SVGFitToViewBoxImpl()
{
	KSVG_EMPTY_FLAGS

	m_viewBox = new SVGAnimatedRectImpl();
	m_viewBox->ref();

	m_preserveAspectRatio = new SVGAnimatedPreserveAspectRatioImpl();
	m_preserveAspectRatio->ref();
}

SVGFitToViewBoxImpl::~SVGFitToViewBoxImpl()
{
	if(m_viewBox)
		m_viewBox->deref();
	if(m_preserveAspectRatio)
		m_preserveAspectRatio->deref();
}

SVGAnimatedRectImpl *SVGFitToViewBoxImpl::viewBox() const
{
	return m_viewBox;
}

SVGAnimatedPreserveAspectRatioImpl *SVGFitToViewBoxImpl::preserveAspectRatio() const
{
	return m_preserveAspectRatio;
}

void SVGFitToViewBoxImpl::parseViewBox(const QString &s)
{
	// Set default if preserveAspectRatio wasnt parsed earlier (Rob)
	if(m_preserveAspectRatio->baseVal()->align() == SVG_PRESERVEASPECTRATIO_UNKNOWN)
		m_preserveAspectRatio->baseVal()->setAlign(SVG_PRESERVEASPECTRATIO_XMIDYMID);
	if(m_preserveAspectRatio->baseVal()->meetOrSlice() == SVG_MEETORSLICE_UNKNOWN)
		m_preserveAspectRatio->baseVal()->setMeetOrSlice(SVG_MEETORSLICE_MEET);

	// allow for viewbox def with ',' or whitespace
	QString viewbox(s);
	QStringList points = QStringList::split(' ', viewbox.replace(',', ' ').simplifyWhiteSpace());

	viewBox()->baseVal()->setX(points[0].toFloat());
	viewBox()->baseVal()->setY(points[1].toFloat());
	viewBox()->baseVal()->setWidth(points[2].toFloat());
	viewBox()->baseVal()->setHeight(points[3].toFloat());
}

SVGMatrixImpl *SVGFitToViewBoxImpl::viewBoxToViewTransform(float viewWidth, float viewHeight) const
{
	if(viewBox()->baseVal()->width() == 0 || viewBox()->baseVal()->height() == 0)
		return SVGSVGElementImpl::createSVGMatrix();
	else
		return preserveAspectRatio()->baseVal()->getCTM(viewBox()->baseVal()->x(),
			viewBox()->baseVal()->y(), viewBox()->baseVal()->width(), viewBox()->baseVal()->height(),
			0, 0, viewWidth, viewHeight);
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGFitToViewBoxImpl::s_hashTable 3
 viewBox				SVGFitToViewBoxImpl::ViewBox				DontDelete
 preserveAspectRatio	SVGFitToViewBoxImpl::PreserveAspectRatio	DontDelete
@end
*/

Value SVGFitToViewBoxImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case ViewBox:
			return m_viewBox->cache(exec);
		case PreserveAspectRatio:
			return m_preserveAspectRatio->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGFitToViewBoxImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

    switch(token)
    {
        case ViewBox:
			parseViewBox(value.toString(exec).qstring());			
			break;
		case PreserveAspectRatio:
			if(preserveAspectRatio())
				preserveAspectRatio()->baseVal()->parsePreserveAspectRatio(value.toString(exec).qstring());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
