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

#ifndef SVGLengthImpl_H
#define SVGLengthImpl_H

#include <dom/dom_misc.h>

#include "ksvg_lookup.h"

namespace KSVG
{

enum LengthMode
{
	LENGTHMODE_UNKNOWN = 0,
	LENGTHMODE_WIDTH = 1,
	LENGTHMODE_HEIGHT = 2,
	LENGTHMODE_OTHER = 3
};

class SVGShapeImpl;
class SVGLocatableImpl;
class SVGElementImpl;
class SVGLengthImpl : public DOM::DomShared
{
public:
	SVGLengthImpl(LengthMode mode = LENGTHMODE_UNKNOWN, SVGElementImpl *context = 0);
	SVGLengthImpl(const SVGLengthImpl &);
	virtual ~SVGLengthImpl();

	SVGLengthImpl &operator=(const SVGLengthImpl &);

	unsigned short unitType() const;

	void setValue(float value);
	float value();

	void setValueInSpecifiedUnits(float valueInSpecifiedUnits);
	float valueInSpecifiedUnits() const;

	void setValueAsString(const DOM::DOMString &);
	DOM::DOMString valueAsString() const;

	void newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits);
	void convertToSpecifiedUnits(unsigned short unitType);

	operator float();

	static void convertPercentageToFloat(const QString &perc, float &result);

	// This method converts the value val to percentage notation ("xxx%").
	//  If the value string ends with the percentage sign it is returned,
	// else the value is calculated against benchmark, ie. "0.2" -> "20%"
	// for benchmark 1.0. This method should be useful in contexts that
	// know the values should be in percentages up front, like bbox
	// calculations.
	static QString convertValToPercentage(const QString &val, float benchmark = 1.0);

	SVGElementImpl *context() const;
	void setContext(SVGElementImpl *context);
	void setBBoxContext(SVGShapeImpl *bbox);

private:
	float m_value;
	float m_valueInSpecifiedUnits;

	float percentageOfViewport();

	void convertStringToPx(QString s);
	void convertNumToPx();
	bool getValFromPx();

	unsigned short m_unitType : 4;

	// KSVG extension
	LengthMode m_mode : 2;
	SVGElementImpl *m_context;
	SVGShapeImpl *m_bboxContext;

	double dpi();

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		UnitType, Value, ValueAsString, ValueInSpecifiedUnits,
		// Functions
		NewValueSpecifiedUnits, ConvertToSpecifiedUnits
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);

};

class SVGLengthImplConstructor : public KJS::ObjectImp
{
public:
	SVGLengthImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;

	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGLengthImplConstructor(KJS::ExecState *exec);
	
}

KSVG_DEFINE_PROTOTYPE(SVGLengthImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGLengthImplProtoFunc, SVGLengthImpl)

#endif

// vim:ts=4:noet
