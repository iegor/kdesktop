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

#ifndef SVGAngleImpl_H
#define SVGAngleImpl_H

#include <dom/dom_misc.h>
#include <dom/dom_string.h>

#include "ksvg_lookup.h"

namespace KSVG
{

class SVGAngleImpl : public DOM::DomShared
{
public:
	SVGAngleImpl();
	virtual ~SVGAngleImpl();

	unsigned short unitType() const;

	void setValue(float value);
	float value() const; 

	void setValueInSpecifiedUnits(float valueInSpecifiedUnits);
	float valueInSpecifiedUnits() const;

	void setValueAsString(const DOM::DOMString &valueAsString);
	DOM::DOMString valueAsString() const;

	void newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits);
	void convertToSpecifiedUnits(unsigned short unitType);

	// Helpers
	static double todeg(double rad);
	static double torad(double deg);

	// Returns the angle that divides the shortest arc between the two angles.
	static double shortestArcBisector(double angle1, double angle2);

private:
	unsigned short m_unitType;
	float m_value;
	float m_valueInSpecifiedUnits;
	DOM::DOMString m_valueAsString;

	void calculate();

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		Value, ValueInSpecifiedUnits, ValueAsString, UnitType,
		// Functions
		ConvertToSpecifiedUnits, NewValueSpecifiedUnits
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGAngleImplConstructor : public KJS::ObjectImp
{
public:
	SVGAngleImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;

	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGAngleImplConstructor(KJS::ExecState *exec);

}

KSVG_DEFINE_PROTOTYPE(SVGAngleImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGAngleImplProtoFunc, SVGAngleImpl)

#endif

// vim:ts=4:noet
