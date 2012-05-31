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

#include "SVGAngle.h"

#include "SVGAngleImpl.h"

using namespace KSVG;

#include "SVGAngleImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_cacheimpl.h"

const double deg2rad = 0.017453292519943295769; // pi/180
const double deg2grad = 400.0 / 360.0;
const double rad2grad = deg2grad / deg2rad;

SVGAngleImpl::SVGAngleImpl()
{
	KSVG_EMPTY_FLAGS

	m_unitType = SVG_ANGLETYPE_UNKNOWN;
	m_valueInSpecifiedUnits = 0;
	m_value = 0;
}

SVGAngleImpl::~SVGAngleImpl()
{
}

unsigned short SVGAngleImpl::unitType() const
{
	return m_unitType;
}

void SVGAngleImpl::setValue(float value)
{
	m_value = value;
}

float SVGAngleImpl::value() const
{
	return m_value;
}

// calc m_value
void SVGAngleImpl::calculate()
{
	if(m_unitType == SVG_ANGLETYPE_GRAD)
		m_value = m_valueInSpecifiedUnits / deg2grad;
	else if(m_unitType == SVG_ANGLETYPE_RAD)
		m_value = m_valueInSpecifiedUnits / deg2rad;
	else if(m_unitType == SVG_ANGLETYPE_UNSPECIFIED || m_unitType == SVG_ANGLETYPE_DEG)
		m_value = m_valueInSpecifiedUnits;
}

void SVGAngleImpl::setValueInSpecifiedUnits(float valueInSpecifiedUnits)
{
	m_valueInSpecifiedUnits = valueInSpecifiedUnits;
	calculate();
}

float SVGAngleImpl::valueInSpecifiedUnits() const
{
	return m_valueInSpecifiedUnits;
}

void SVGAngleImpl::setValueAsString(const DOM::DOMString &valueAsString)
{
	m_valueAsString = valueAsString;

	QString s = valueAsString.string();
	
	bool bOK;
	m_valueInSpecifiedUnits = s.toFloat(&bOK);
	m_unitType = SVG_ANGLETYPE_UNSPECIFIED;
	
	if(!bOK)
    {
		if(s.endsWith("deg"))
			m_unitType = SVG_ANGLETYPE_DEG;
		else if(s.endsWith("grad"))
			m_unitType = SVG_ANGLETYPE_GRAD;
		else if(s.endsWith("rad"))
			m_unitType = SVG_ANGLETYPE_RAD;
	}
	
	calculate();
}

DOM::DOMString SVGAngleImpl::valueAsString() const
{
	m_valueAsString.string().setNum(m_valueInSpecifiedUnits);
	
	switch(m_unitType)
	{
		case SVG_ANGLETYPE_UNSPECIFIED:
		case SVG_ANGLETYPE_DEG:
			m_valueAsString.string() += "deg";
			break;
		case SVG_ANGLETYPE_RAD:
			m_valueAsString.string() += "rad";
			break;
		case SVG_ANGLETYPE_GRAD:
			m_valueAsString.string() += "grad";
			break;
	}
	
	return m_valueAsString;
}

void SVGAngleImpl::newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits)
{
	m_unitType = unitType;
	m_valueInSpecifiedUnits = valueInSpecifiedUnits;
	calculate();
}

void SVGAngleImpl::convertToSpecifiedUnits(unsigned short unitType)
{
	if(m_unitType == unitType)
		return;

	if(m_unitType == SVG_ANGLETYPE_DEG && unitType == SVG_ANGLETYPE_RAD)
		m_valueInSpecifiedUnits *= deg2rad;
	else if(m_unitType == SVG_ANGLETYPE_GRAD && unitType == SVG_ANGLETYPE_RAD)
		m_valueInSpecifiedUnits /= rad2grad;
	else if(m_unitType == SVG_ANGLETYPE_DEG && unitType == SVG_ANGLETYPE_GRAD)
		m_valueInSpecifiedUnits *= deg2grad;
	else if(m_unitType == SVG_ANGLETYPE_RAD && unitType == SVG_ANGLETYPE_GRAD)
		m_valueInSpecifiedUnits *= rad2grad;
	else if(m_unitType == SVG_ANGLETYPE_RAD && unitType == SVG_ANGLETYPE_DEG)
		m_valueInSpecifiedUnits /= deg2rad;
	else if(m_unitType == SVG_ANGLETYPE_GRAD && unitType == SVG_ANGLETYPE_DEG)
		m_valueInSpecifiedUnits /= deg2grad;

	m_unitType = unitType;
}

// Helpers
double SVGAngleImpl::todeg(double rad)
{
	return rad / deg2rad;
}

double SVGAngleImpl::torad(double deg)
{
	return deg * deg2rad;
}

double SVGAngleImpl::shortestArcBisector(double angle1, double angle2)
{
	double bisector = (angle1 + angle2) / 2;

	if(fabs(angle1 - angle2) > 180)
		bisector += 180;

	return bisector;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGAngleImpl::s_hashTable 5
 value						SVGAngleImpl::Value					DontDelete
 valueInSpecifiedUnits		SVGAngleImpl::ValueInSpecifiedUnits	DontDelete
 valueAsString				SVGAngleImpl::ValueAsString			DontDelete
 unitType					SVGAngleImpl::UnitType				DontDelete
@end
@namespace KSVG
@begin SVGAngleImplProto::s_hashTable 3
 convertToSpecifiedUnits	SVGAngleImpl::ConvertToSpecifiedUnits	DontDelete|Function 1
 newValueSpecifiedUnits		SVGAngleImpl::NewValueSpecifiedUnits	DontDelete|Function 2
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGAngle", SVGAngleImplProto, SVGAngleImplProtoFunc)

Value SVGAngleImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case Value:
			return Number(value());
		case ValueInSpecifiedUnits:
			return Number(valueInSpecifiedUnits());
		case ValueAsString:
			return String(valueAsString().string());
		case UnitType:
			return Number(unitType());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGAngleImpl::putValueProperty(ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case Value:
			setValue(value.toNumber(exec));
			break;
		case ValueInSpecifiedUnits:
			setValueInSpecifiedUnits(value.toNumber(exec));
			break;
		case ValueAsString:
			setValueAsString(value.toString(exec).string());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

Value SVGAngleImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGAngleImpl)

	switch(id)
	{
		case SVGAngleImpl::ConvertToSpecifiedUnits:
			obj->convertToSpecifiedUnits(static_cast<unsigned short>(args[0].toNumber(exec)));
			break;
		case SVGAngleImpl::NewValueSpecifiedUnits:
			obj->newValueSpecifiedUnits(static_cast<unsigned short>(args[0].toNumber(exec)), args[1].toNumber(exec));
			break;
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

/*
@namespace KSVG
@begin SVGAngleImplConstructor::s_hashTable 7
 SVG_ANGLETYPE_UNKNOWN 		KSVG::SVG_ANGLETYPE_UNKNOWN			DontDelete|ReadOnly
 SVG_ANGLETYPE_UNSPECIFIED 	KSVG::SVG_ANGLETYPE_UNSPECIFIED		DontDelete|ReadOnly
 SVG_ANGLETYPE_RAD 			KSVG::SVG_ANGLETYPE_RAD				DontDelete|ReadOnly
 SVG_ANGLETYPE_DEG 			KSVG::SVG_ANGLETYPE_DEG				DontDelete|ReadOnly
 SVG_ANGLETYPE_GRAD			KSVG::SVG_ANGLETYPE_GRAD			DontDelete|ReadOnly
@end
*/

Value SVGAngleImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token);
}

Value KSVG::getSVGAngleImplConstructor(ExecState *exec)
{
	return cacheGlobalBridge<SVGAngleImplConstructor>(exec, "[[svgangle.constructor]]");
}

// vim:ts=4:noet
