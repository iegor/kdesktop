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

#ifndef SVGAnimationElementImpl_H
#define SVGAnimationElementImpl_H

#include "SVGElementImpl.h"
#include "SVGTestsImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

#include "ksvg_lookup.h"

namespace KSVG
{

enum EFillMode
{
	REMOVE,
	FREEZE
};

enum EAdditiveMode
{
	REPLACE,
	SUM
};

enum EAccumulateMode
{
	ACCUMULATE_NONE,
	ACCUMULATE_SUM
};

enum ECalcMode
{
	DISCRETE,
	LINEAR,
	PACED,
	SPLINE
};

enum ERestart
{
	ALWAYS,
	WHENNOTACTIVE,
	NEVER
};

enum EAttributeType
{
	CSS,
	XML,
	AUTO
};

class SVGAnimationElementImpl : public SVGElementImpl,
								public SVGTestsImpl,
								public SVGExternalResourcesRequiredImpl
{
public:
	SVGAnimationElementImpl(DOM::ElementImpl *);
	virtual ~SVGAnimationElementImpl();

	SVGElementImpl *targetElement() const;
	void setTargetElement(SVGElementImpl *target);

	void applyAttribute(const QString &name, const QString &value);

	virtual void handleTimerEvent() { } //= 0;
	virtual void setAttributes();

	double getStartTime() const;
	double getCurrentTime() const;
	double getSimpleDuration() const;

	QString getRepeatCount() const { return m_repeatCount; }
	QString getRepeatDuration() const { return m_repeatDur; }

	QString getAttributeName() const { return m_attributeName; }
	QString getFrom() const { return m_from; }
	QString getTo() const { return m_to; }
	QString getBy() const { return m_by; }

protected:
	double parseClockValue(const QString &data) const;

	bool m_connected;
	EFillMode m_fill : 1;
	EAdditiveMode m_additive : 1;
	EAccumulateMode m_accumulate : 1;
	ECalcMode m_calcMode : 2;
	ERestart m_restart : 2;
	EAttributeType m_attributeType : 2;
	SVGElementImpl *m_targetElement;

	QString m_href;
	QString m_attributeName;
	QString m_from, m_to, m_by;

	double m_begin, m_end, m_duration;

	QString m_repeatCount, m_repeatDur;

	SVGStringListImpl *m_values, *m_keyTimes, *m_keySplines;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		TargetElement,
		Href, Additive, Accumulate, AttributeName, AttributeType,
		CalcMode, Values, KeyTimes, KeySplines, From, To, By,
		Begin, Dur, End, Min, Max, Restart, RepeatCount, RepeatDur, Fill,
		// Functions
		GetStartTime, GetCurrentTime, GetSimpleDuration
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

}

KSVG_DEFINE_PROTOTYPE(SVGAnimationElementImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGAnimationElementImplProtoFunc, SVGAnimationElementImpl)

#endif

// vim:ts=4:noet
