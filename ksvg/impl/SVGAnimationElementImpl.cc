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

#include <kdebug.h>

#include <qtimer.h>

#include "CanvasItem.h"
#include "SVGHelperImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGStringListImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGAnimationElementImpl.h"

using namespace KSVG;

#include "SVGAnimationElementImpl.lut.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGAnimationElementImpl::SVGAnimationElementImpl(DOM::ElementImpl *impl) : SVGElementImpl(impl), SVGTestsImpl(), SVGExternalResourcesRequiredImpl()
{
	KSVG_EMPTY_FLAGS

	m_connected = false;
	m_targetElement = 0;

	m_values = new SVGStringListImpl();
	m_keyTimes= new SVGStringListImpl();
	m_keySplines = new SVGStringListImpl();

	m_fill = REMOVE;
	m_additive = REPLACE;
	m_accumulate = ACCUMULATE_NONE;
}

SVGAnimationElementImpl::~SVGAnimationElementImpl()
{
	if(m_targetElement)
		m_targetElement->deref();
}

SVGElementImpl *SVGAnimationElementImpl::targetElement() const
{
	if(!m_targetElement)
	{
		SVGAnimationElementImpl *modify = const_cast<SVGAnimationElementImpl *>(this);
		if(!m_href.isEmpty())
			modify->setTargetElement(ownerDoc()->getElementByIdRecursive(ownerSVGElement(), SVGURIReferenceImpl::getTarget(m_href)));
		else if(!parentNode().isNull())
			modify->setTargetElement(ownerDoc()->getElementFromHandle(parentNode().handle()));			
	}

	return m_targetElement;
}

double SVGAnimationElementImpl::parseClockValue(const QString &data) const
{
	QString parse = data.stripWhiteSpace();
	QString debugOutput = "parseClockValue(" + parse + ") -> ";
	
	if(parse == "indefinite") // Saves some time...
		return -1;

	double result;

	int doublePointOne = parse.find(':');
	int doublePointTwo = parse.find(':', doublePointOne + 1);

	if(doublePointOne != -1 && doublePointTwo != -1) // Spec: "Full clock values"
	{
		unsigned int hours = parse.mid(0, 2).toUInt();
		unsigned int minutes = parse.mid(3, 2).toUInt();
		unsigned int seconds = parse.mid(6, 2).toUInt();
		unsigned int milliseconds = 0;

		result = (3600 * hours) + (60 * minutes) + seconds;

		if(parse.find('.') != -1)
		{
			QString temp = parse.mid(9, 2);
			milliseconds = temp.toUInt();
			result += (milliseconds * (1 / pow(10.0, temp.length())));
		}
	}
	else if(doublePointOne != -1 && doublePointTwo == -1) // Spec: "Partial clock values"
	{
		unsigned int minutes = parse.mid(0, 2).toUInt();
		unsigned int seconds = parse.mid(3, 2).toUInt();
		unsigned int milliseconds = 0;

		result = (60 * minutes) + seconds;

		if(parse.find('.') != -1)
		{
			QString temp = parse.mid(6, 2);
			milliseconds = temp.toUInt();
			result += (milliseconds * (1 / pow(10.0, temp.length())));
		}
	}
	else // Spec: "Timecount values"
	{
		int dotPosition = parse.find('.');

		if(parse.endsWith("h"))
		{
			if(dotPosition == -1)
				result = parse.mid(0, parse.length() - 1).toUInt() * 3600;
			else
			{
				result = parse.mid(0, dotPosition).toUInt() * 3600;
				QString temp = parse.mid(dotPosition + 1, parse.length() - dotPosition - 2);
				result += (3600.0 * temp.toUInt()) * (1 / pow(10.0, temp.length()));
			}
		}
		else if(parse.endsWith("min"))
		{
			if(dotPosition == -1)
				result = parse.mid(0, parse.length() - 3).toUInt() * 60;
			else
			{
				result = parse.mid(0, dotPosition).toUInt() * 60;
				QString temp = parse.mid(dotPosition + 1, parse.length() - dotPosition - 4);
				result += (60.0 * temp.toUInt()) * (1 / pow(10.0, temp.length()));
			}
		}
		else if(parse.endsWith("ms"))
		{
			if(dotPosition == -1)
				result = parse.mid(0, parse.length() - 2).toUInt() / 1000.0;
			else
			{
				result = parse.mid(0, dotPosition).toUInt() / 1000.0;
				QString temp = parse.mid(dotPosition + 1, parse.length() - dotPosition - 3);
				result += (temp.toUInt() / 1000.0) * (1 / pow(10.0, temp.length()));
			}
		}
		else if(parse.endsWith("s"))
		{
			if(dotPosition == -1)
				result = parse.mid(0, parse.length() - 1).toUInt();
			else
			{
				result = parse.mid(0, dotPosition).toUInt();
				QString temp = parse.mid(dotPosition + 1, parse.length() - dotPosition - 2);
				result += temp.toUInt() * (1 / pow(10.0, temp.length()));
			}
		}
		else
			result = parse.toDouble();
	}

	kdDebug() << debugOutput << result << endl;
	return result;
}

/*
@namespace KSVG
@begin SVGAnimationElementImpl::s_hashTable 23 
 targetElement	SVGAnimationElementImpl::TargetElement	DontDelete|ReadOnly
 href			SVGAnimationElementImpl::Href			DontDelete|ReadOnly
 additive		SVGAnimationElementImpl::Additive		DontDelete|ReadOnly
 accumulate		SVGAnimationElementImpl::Accumulate		DontDelete|ReadOnly
 attributeName	SVGAnimationElementImpl::AttributeName	DontDelete|ReadOnly
 attributeType	SVGAnimationElementImpl::AttributeType	DontDelete|ReadOnly
 calcMode		SVGAnimationElementImpl::CalcMode		DontDelete|ReadOnly
 values			SVGAnimationElementImpl::Values			DontDelete|ReadOnly
 keyTimes		SVGAnimationElementImpl::KeyTimes		DontDelete|ReadOnly
 keySplines		SVGAnimationElementImpl::KeySplines		DontDelete|ReadOnly
 from			SVGAnimationElementImpl::From			DontDelete|ReadOnly
 to				SVGAnimationElementImpl::To				DontDelete|ReadOnly
 by				SVGAnimationElementImpl::By				DontDelete|ReadOnly
 begin			SVGAnimationElementImpl::Begin			DontDelete|ReadOnly
 dur			SVGAnimationElementImpl::Dur			DontDelete|ReadOnly
 end			SVGAnimationElementImpl::End			DontDelete|ReadOnly
 min			SVGAnimationElementImpl::Min			DontDelete|ReadOnly
 max			SVGAnimationElementImpl::Max			DontDelete|ReadOnly
 restart		SVGAnimationElementImpl::Restart		DontDelete|ReadOnly
 repeatCount	SVGAnimationElementImpl::RepeatCount	DontDelete|ReadOnly
 repeatDur		SVGAnimationElementImpl::RepeatDur		DontDelete|ReadOnly
 fill			SVGAnimationElementImpl::Fill			DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGAnimationElementImplProto::s_hashTable 5
 getStartTime		SVGAnimationElementImpl::GetStartTime		DontDelete|Function 0
 getCurrentTime		SVGAnimationElementImpl::GetCurrentTime		DontDelete|Function 0
 getSimpleDuration	SVGAnimationElementImpl::GetSimpleDuration	DontDelete|Function 0
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGAnimationElement",SVGAnimationElementImplProto,SVGAnimationElementImplProtoFunc)

Value SVGAnimationElementImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case TargetElement:
			return m_targetElement->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGAnimationElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	QString val = value.toString(exec).qstring();
	switch(token)
	{
		case Href:
			m_href = val;
			break;
		case Additive:
			m_additive = (val == "sum") ? SUM : REPLACE;
			break;
		case Accumulate:
			m_accumulate = (val == "sum") ? ACCUMULATE_SUM : ACCUMULATE_NONE;
			break;
		case AttributeName:
			m_attributeName = val;
			break;
		case AttributeType:
			if(val == "css")
				m_attributeType = CSS;
			else if(val == "xml")
				m_attributeType = XML;
			else
				m_attributeType = AUTO;
			break;
		case CalcMode: // FIXME: See spec for default values!!!
			if(val == "discrete")
				m_calcMode = DISCRETE;
			else if(val == "linear")
				m_calcMode = LINEAR;
			else if(val == "spline")
				m_calcMode = SPLINE;
			else if(val == "paced")
				m_calcMode = PACED;
			break;
		case Values:
			SVGHelperImpl::parseSemicolonSeperatedList(m_values, val);
			break;
		case KeyTimes:
			SVGHelperImpl::parseSemicolonSeperatedList(m_keyTimes, val);
			break;
		case KeySplines:
			SVGHelperImpl::parseSemicolonSeperatedList(m_keySplines, val);
			break;
		case From:
			m_from = val;
			break;
		case To:
			m_to = val;
			break;
		case By:
			m_by = val;
			break;
		case Begin:
		case End:
		{
			// Create list
			SVGStringListImpl *temp = new SVGStringListImpl();
			temp->ref();
			
			// Feed data into list
			SVGHelperImpl::parseSemicolonSeperatedList(temp, val);

			// Parse data
			for(unsigned int i = 0; i < temp->numberOfItems(); i++)
			{
				QString current = temp->getItem(i)->string();
				
				if(current.startsWith("accessKey"))
				{
					// Register keyDownEventListener for the character
					QString character = current.mid(current.length() - 2, 1);

					kdDebug() << "ACCESSKEY CHARACTER " << character << endl;
				}
				else if(current.startsWith("wallclock"))
				{
					int firstBrace = current.find("(");
					int secondBrace = current.find(")");
					
					QString wallclockValue = current.mid(firstBrace + 1, secondBrace - firstBrace - 2);

					kdDebug() << "WALLCLOCK VALUE " << wallclockValue << endl;
				}	
				else if(current.contains("."))
				{
					int dotPosition = current.find(".");
					
					QString element = current.mid(0, dotPosition);
					QString clockValue;

					if(current.contains("begin"))
						clockValue = current.mid(dotPosition + 6);
					else if(current.contains("end"))
						clockValue = current.mid(dotPosition + 4);
					else if(current.contains("repeat"))
						clockValue = current.mid(dotPosition + 7);
					else // DOM2 Event Reference
					{
						int plusMinusPosition = -1;
						
						if(current.contains("+"))
							plusMinusPosition = current.find("+");
						else if(current.contains("-"))
							plusMinusPosition = current.find("-");

						QString event = current.mid(dotPosition + 1, plusMinusPosition - dotPosition - 1);

						clockValue = current.mid(dotPosition + event.length() + 1);
						kdDebug() << "EVENT " << event << endl;
					}

					kdDebug() << "ELEMENT " << element << " CLOCKVALUE " << clockValue << endl;
				}
				else
				{
					if(token == Begin)
						m_begin = parseClockValue(current);
					else
						m_end = parseClockValue(current);
				}
			}

			temp->deref();
			
			break;
		}
		case Dur:
			m_duration = parseClockValue(val);
			break;
//		case Min:
//		case Max:
		case Restart:
			if(val == "whenNotActive")
				m_restart = WHENNOTACTIVE;
			else if(val == "never")
				m_restart = NEVER;
			else
				m_restart = ALWAYS;
			break;
		case RepeatCount:
			m_repeatCount = val;
			break;
		case RepeatDur:
			m_repeatDur = val;
			break;
		case Fill:
			m_fill = (val == "freeze") ? FREEZE : REMOVE;
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

Value SVGAnimationElementImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &)
{
	KSVG_CHECK_THIS(SVGAnimationElementImpl)

	switch(id)
	{
		case SVGAnimationElementImpl::GetStartTime:
			return Number(obj->getStartTime());
		case SVGAnimationElementImpl::GetCurrentTime:
			return Number(obj->getCurrentTime());
		case SVGAnimationElementImpl::GetSimpleDuration:
			return Number(obj->getSimpleDuration());
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

void SVGAnimationElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: Default value is "replace"
	if(KSVG_TOKEN_NOT_PARSED(Additive))
		KSVG_SET_ALT_ATTRIBUTE(Additive, "replace")
	
	// Spec: Default value is "none"
	if(KSVG_TOKEN_NOT_PARSED(Accumulate))
		KSVG_SET_ALT_ATTRIBUTE(Accumulate, "none")

	// Spec: Default value is "always"
	if(KSVG_TOKEN_NOT_PARSED(Restart))
		KSVG_SET_ALT_ATTRIBUTE(Restart, "always")
}

void SVGAnimationElementImpl::setTargetElement(SVGElementImpl *target)
{
	if(m_targetElement)
		m_targetElement->deref();

	m_targetElement = target;
	m_targetElement->ref();
}

void SVGAnimationElementImpl::applyAttribute(const QString &name, const QString &value)
{
	SVGElementImpl *target = targetElement();
	if(!target)
	{
		kdDebug() << k_funcinfo << " name: " << name << " value: " << value << " NO TARGET ELEMENT!" << endl;
		return;
	}
	
	// The only two cases I can imagine, where combining is needed (Niko)
	bool combine = (name == "style" || name == "transform");

	if(!combine)
		target->setAttributeInternal(name, value);
	else
	{
		kdDebug() << "TODO COMBINE: " << value << " NAME " << name << endl;
	}
}

double SVGAnimationElementImpl::getStartTime() const
{
	return m_begin;
}

double SVGAnimationElementImpl::getCurrentTime() const
{
	return 0.0;
}

double SVGAnimationElementImpl::getSimpleDuration() const
{
	return m_duration;
}

// vim:ts=4:noet
