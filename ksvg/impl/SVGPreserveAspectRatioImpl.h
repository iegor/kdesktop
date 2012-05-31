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

#ifndef SVGPreserveAspectRatioImpl_H
#define SVGPreserveAspectRatioImpl_H

#include <dom/dom_misc.h>

#include "ksvg_lookup.h"

class QString;

namespace KSVG
{

class SVGMatrixImpl;
class SVGPreserveAspectRatioImpl : public DOM::DomShared
{ 
public:
	SVGPreserveAspectRatioImpl();
	virtual ~SVGPreserveAspectRatioImpl();

	void setAlign(unsigned short);
	unsigned short align() const;

	void setMeetOrSlice(unsigned short);
	unsigned short meetOrSlice() const;

	void parsePreserveAspectRatio(const QString &);
	SVGMatrixImpl *getCTM(float logicX, float logicY, float logicWidth, float logicHeight,
						  float physX, float physY, float physWidth, float physHeight);

protected:
	unsigned short m_align;
	unsigned short m_meetOrSlice;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		Align, MeetOrSlice
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGPreserveAspectRatioImplConstructor : public KJS::ObjectImp
{
public:
	SVGPreserveAspectRatioImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;

	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGPreserveAspectRatioImplConstructor(KJS::ExecState *exec);

}

#endif

// vim:ts=4:noet
