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

#include "SVGMatrixImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGPreserveAspectRatioImpl.h"

using namespace KSVG;

#include "SVGPreserveAspectRatioImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_cacheimpl.h"

SVGPreserveAspectRatioImpl::SVGPreserveAspectRatioImpl()
{
	KSVG_EMPTY_FLAGS

	m_meetOrSlice = SVG_MEETORSLICE_UNKNOWN;
	m_align = SVG_PRESERVEASPECTRATIO_UNKNOWN;
}

SVGPreserveAspectRatioImpl::~SVGPreserveAspectRatioImpl()
{
}

void SVGPreserveAspectRatioImpl::setAlign(unsigned short align)
{
	m_align = align;
}

unsigned short SVGPreserveAspectRatioImpl::align() const
{
	return m_align;
}

void SVGPreserveAspectRatioImpl::setMeetOrSlice(unsigned short meetOrSlice)
{
	m_meetOrSlice = meetOrSlice;
}

unsigned short SVGPreserveAspectRatioImpl::meetOrSlice() const
{
	return m_meetOrSlice;
}

void SVGPreserveAspectRatioImpl::parsePreserveAspectRatio(const QString &str)
{
	// Spec: set the defaults
	setAlign(SVG_PRESERVEASPECTRATIO_NONE);
	setMeetOrSlice(SVG_MEETORSLICE_MEET);

	QStringList params = QStringList::split(' ', str.simplifyWhiteSpace());

	if(params[0].compare("none") == 0)
		m_align = SVG_PRESERVEASPECTRATIO_NONE;
	else if(params[0].compare("xMinYMin") == 0)
		m_align = SVG_PRESERVEASPECTRATIO_XMINYMIN;
	else if(params[0].compare("xMidYMin") == 0)
		m_align = SVG_PRESERVEASPECTRATIO_XMIDYMIN;
	else if(params[0].compare("xMaxYMin") == 0)
		m_align = SVG_PRESERVEASPECTRATIO_XMAXYMIN;
	else if(params[0].compare("xMinYMid") == 0)
		m_align = SVG_PRESERVEASPECTRATIO_XMINYMID;
	else if(params[0].compare("xMidYMid") == 0)
		m_align = SVG_PRESERVEASPECTRATIO_XMIDYMID;
	else if(params[0].compare("xMaxYMid") == 0)
		m_align = SVG_PRESERVEASPECTRATIO_XMAXYMID;
	else if(params[0].compare("xMinYMax") == 0)
		m_align = SVG_PRESERVEASPECTRATIO_XMINYMAX;
	else if(params[0].compare("xMidYMax") == 0)
		m_align = SVG_PRESERVEASPECTRATIO_XMIDYMAX;
	else if(params[0].compare("xMaxYMax") == 0)
		m_align = SVG_PRESERVEASPECTRATIO_XMAXYMAX;

	if(params[1].compare("slice") == 0)
		m_meetOrSlice = SVG_MEETORSLICE_SLICE;
	else
		m_meetOrSlice = SVG_MEETORSLICE_MEET;
}

SVGMatrixImpl *SVGPreserveAspectRatioImpl::getCTM(float logicX, float logicY, float logicWidth, float logicHeight,
												  float /*physX*/, float /*physY*/, float physWidth, float physHeight)
{
	SVGMatrixImpl *temp = SVGSVGElementImpl::createSVGMatrix();

	if(align() == SVG_PRESERVEASPECTRATIO_UNKNOWN)
		return temp;

	float vpar = logicWidth / logicHeight;
	float svgar = physWidth / physHeight;

	if(align() == SVG_PRESERVEASPECTRATIO_NONE)
	{
		temp->scaleNonUniform(physWidth / logicWidth, physHeight / logicHeight);
		temp->translate(-logicX, -logicY);
	}
	else if(vpar < svgar && (meetOrSlice() == SVG_MEETORSLICE_MEET) || vpar >= svgar && (meetOrSlice() == SVG_MEETORSLICE_SLICE))
	{
		temp->scale(physHeight / logicHeight);

		if(align() == SVG_PRESERVEASPECTRATIO_XMINYMIN || align() == SVG_PRESERVEASPECTRATIO_XMINYMID || align() == SVG_PRESERVEASPECTRATIO_XMINYMAX)
			temp->translate(-logicX, -logicY);
		else if(align() == SVG_PRESERVEASPECTRATIO_XMIDYMIN || align() == SVG_PRESERVEASPECTRATIO_XMIDYMID || align() == SVG_PRESERVEASPECTRATIO_XMIDYMAX)
			temp->translate(-logicX - (logicWidth - physWidth * logicHeight / physHeight) / 2, -logicY);
		else
			temp->translate(-logicX - (logicWidth - physWidth * logicHeight / physHeight), -logicY);
	}
	else
	{
		temp->scale(physWidth / logicWidth);

		if(align() == SVG_PRESERVEASPECTRATIO_XMINYMIN || align() == SVG_PRESERVEASPECTRATIO_XMIDYMIN || align() == SVG_PRESERVEASPECTRATIO_XMAXYMIN)
			temp->translate(-logicX, -logicY);
		else if(align() == SVG_PRESERVEASPECTRATIO_XMINYMID || align() == SVG_PRESERVEASPECTRATIO_XMIDYMID || align() == SVG_PRESERVEASPECTRATIO_XMAXYMID)
			temp->translate(-logicX, -logicY - (logicHeight - physHeight * logicWidth / physWidth) / 2);
		else
			temp->translate(-logicX, -logicY - (logicHeight - physHeight * logicWidth / physWidth));
	}

	return temp;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGPreserveAspectRatioImpl::s_hashTable 3
 align			SVGPreserveAspectRatioImpl::Align   	 DontDelete
 meetOrSlice	SVGPreserveAspectRatioImpl::MeetOrSlice  DontDelete
@end
*/

Value SVGPreserveAspectRatioImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case Align:
			return Number(align());
		case MeetOrSlice:
			return Number(meetOrSlice());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return KJS::Undefined();
	}
}

void SVGPreserveAspectRatioImpl::putValueProperty(ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case Align:
			m_align = static_cast<unsigned short>(value.toNumber(exec));
			break;
		case MeetOrSlice:
			m_meetOrSlice = static_cast<unsigned short>(value.toNumber(exec));
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

/*
@namespace KSVG
@begin SVGPreserveAspectRatioImplConstructor::s_hashTable 17
 SVG_PRESERVEASPECTRATIO_UNKNOWN	KSVG::SVG_PRESERVEASPECTRATIO_UNKNOWN		DontDelete|ReadOnly
 SVG_PRESERVEASPECTRATIO_NONE		KSVG::SVG_PRESERVEASPECTRATIO_NONE			DontDelete|ReadOnly
 SVG_PRESERVEASPECTRATIO_XMINYMIN	KSVG::SVG_PRESERVEASPECTRATIO_XMINYMIN		DontDelete|ReadOnly
 SVG_PRESERVEASPECTRATIO_XMIDYMIN	KSVG::SVG_PRESERVEASPECTRATIO_XMIDYMIN		DontDelete|ReadOnly
 SVG_PRESERVEASPECTRATIO_XMAXYMIN	KSVG::SVG_PRESERVEASPECTRATIO_XMAXYMIN		DontDelete|ReadOnly
 SVG_PRESERVEASPECTRATIO_XMINYMID	KSVG::SVG_PRESERVEASPECTRATIO_XMINYMID		DontDelete|ReadOnly
 SVG_PRESERVEASPECTRATIO_XMIDYMID	KSVG::SVG_PRESERVEASPECTRATIO_XMIDYMID		DontDelete|ReadOnly
 SVG_PRESERVEASPECTRATIO_XMAXYMID	KSVG::SVG_PRESERVEASPECTRATIO_XMAXYMID		DontDelete|ReadOnly
 SVG_PRESERVEASPECTRATIO_XMINYMAX	KSVG::SVG_PRESERVEASPECTRATIO_XMINYMAX		DontDelete|ReadOnly
 SVG_PRESERVEASPECTRATIO_XMIDYMAX	KSVG::SVG_PRESERVEASPECTRATIO_XMIDYMAX		DontDelete|ReadOnly
 SVG_PRESERVEASPECTRATIO_XMAXYMAX	KSVG::SVG_PRESERVEASPECTRATIO_XMAXYMAX		DontDelete|ReadOnly
 SVG_MEETORSLICE_UNKNOWN			KSVG::SVG_MEETORSLICE_UNKNOWN				DontDelete|ReadOnly
 SVG_MEETORSLICE_MEET				KSVG::SVG_MEETORSLICE_MEET					DontDelete|ReadOnly
 SVG_MEETORSLICE_SLICE				KSVG::SVG_MEETORSLICE_SLICE					DontDelete|ReadOnly
@end
*/

Value SVGPreserveAspectRatioImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token > SVG_PRESERVEASPECTRATIO_XMAXYMAX ? token - SVG_MEETORSLICE_UNKNOWN : token);
}

Value KSVG::getSVGPreserveAspectRatioImplConstructor(ExecState *exec)
{
    return cacheGlobalBridge<SVGPreserveAspectRatioImplConstructor>(exec, "[[svgpreserveaspectratio.constructor]]");
}

// vim:ts=4:noet
