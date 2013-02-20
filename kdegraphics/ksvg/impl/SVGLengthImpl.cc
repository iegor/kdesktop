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

#include <qfont.h>
#include <qregexp.h>
#include <qwidget.h>
#include <qpaintdevicemetrics.h>

#include "SVGLength.h"

#include "SVGRectImpl.h"
#include "SVGLengthImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGHelperImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGStringListImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedRectImpl.h"
#include "svgpathparser.h"

#include "KSVGCanvas.h"

using namespace KSVG;

#include "SVGLengthImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_cacheimpl.h"

// keep track of textual description of the unit type
QString UnitText[] =
{
	"", "", "%", "em", "ex", "px", "cm", "mm", "in", "pt", "pc"
};

SVGLengthImpl::SVGLengthImpl(LengthMode mode, SVGElementImpl *context) : DOM::DomShared(), m_mode(mode), m_context(context)
{
	KSVG_EMPTY_FLAGS

	m_unitType = SVG_LENGTHTYPE_UNKNOWN;
	m_value = 0;
	m_valueInSpecifiedUnits = 0;
	m_bboxContext = 0;
}

SVGLengthImpl::SVGLengthImpl(const SVGLengthImpl &other) : DOM::DomShared()
{
	(*this) = other;
}

SVGLengthImpl::~SVGLengthImpl()
{
}

double SVGLengthImpl::dpi()
{
	if(m_context && m_context->ownerDoc())
	{
		if(m_mode == LENGTHMODE_WIDTH)
			return 25.4 * m_context->ownerDoc()->screenPixelsPerMillimeterX();
		else if(m_mode == LENGTHMODE_HEIGHT)
			return 25.4 * m_context->ownerDoc()->screenPixelsPerMillimeterY();
		else if(m_mode == LENGTHMODE_OTHER)
			return 25.4 * m_context->ownerDoc()->screenPixelsPerMillimeterX();
	}
	return 90.0;
}

SVGLengthImpl &SVGLengthImpl::operator=(const SVGLengthImpl &other)
{
	m_unitType = other.m_unitType;
	m_value	= other.m_value;
	m_valueInSpecifiedUnits	= other.m_valueInSpecifiedUnits;
	m_bboxContext = other.m_bboxContext;
	m_mode = other.m_mode;
	m_context = other.m_context;
	
	return *this;
}

unsigned short SVGLengthImpl::unitType() const
{
	return m_unitType;
}

void SVGLengthImpl::setValue(float value)
{
	m_value = value;
	getValFromPx();
}

float SVGLengthImpl::value()
{
	if(m_unitType == SVG_LENGTHTYPE_PERCENTAGE)
	{
		float value = m_valueInSpecifiedUnits / 100.0;
		SVGRectImpl *bbox = 0;
		if(m_bboxContext && (bbox = m_bboxContext->getBBox()))
		{
			float result = 0;
			if(m_mode == LENGTHMODE_WIDTH)
				result = value * bbox->width();
			else if(m_mode == LENGTHMODE_HEIGHT)
				result = value * bbox->height();
			else if(m_mode == LENGTHMODE_OTHER)
				result = value * sqrt(pow(bbox->width(), 2) + pow(bbox->height(), 2)) / sqrt(2.0);

			bbox->deref();
			return result;
		}
		else
			return percentageOfViewport();
	}
	else
		return m_value;
}

void SVGLengthImpl::setValueInSpecifiedUnits(float valueInSpecifiedUnits)
{
	m_valueInSpecifiedUnits = valueInSpecifiedUnits;
	convertNumToPx();
}

float SVGLengthImpl::valueInSpecifiedUnits() const
{
	return m_valueInSpecifiedUnits;
}

void SVGLengthImpl::setValueAsString(const DOM::DOMString &valueAsString)
{
	convertStringToPx(valueAsString.string());
}

DOM::DOMString SVGLengthImpl::valueAsString() const
{
	DOM::DOMString valueAsString = QString::number(m_valueInSpecifiedUnits);
	valueAsString += UnitText[m_unitType];
	return valueAsString;
}

void SVGLengthImpl::newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits)
{
	m_valueInSpecifiedUnits = valueInSpecifiedUnits;
	m_unitType = unitType;
	convertNumToPx();
}

void SVGLengthImpl::convertToSpecifiedUnits(unsigned short unitType)
{
	m_unitType = unitType;
	getValFromPx();
}

SVGLengthImpl::operator float()
{
	return valueInSpecifiedUnits();
}

bool SVGLengthImpl::getValFromPx()
{
	if(m_unitType == SVG_LENGTHTYPE_UNKNOWN)
		return false;

	switch(m_unitType)
	{
	//	case SVG_LENGTHTYPE_PERCENTAGE: TODO
	//	case SVG_LENGTHTYPE_EMS: TODO
	//	case SVG_LENGTHTYPE_EXS: TODO
			//break;
		case SVG_LENGTHTYPE_PX:
			m_valueInSpecifiedUnits = m_value;
			break;
		case SVG_LENGTHTYPE_CM:
			m_valueInSpecifiedUnits = m_value / dpi() * 2.54;
			break;
		case SVG_LENGTHTYPE_MM:
			m_valueInSpecifiedUnits = m_value / dpi() * 25.4;
			break;
		case SVG_LENGTHTYPE_IN:
			m_valueInSpecifiedUnits = m_value / dpi();
			break;
		case SVG_LENGTHTYPE_PT:
			m_valueInSpecifiedUnits = m_value / dpi() * 72.0;
			break;
		case SVG_LENGTHTYPE_PC:
			m_valueInSpecifiedUnits = m_value / dpi() * 6.0;
			break;
	};
	return true;
}

void SVGLengthImpl::convertStringToPx(QString s)
{
	if(s.isEmpty())
		return;

	double convNum = 0;
	const char *start = s.latin1();
	const char *end = getNumber(start, convNum);
	m_valueInSpecifiedUnits = convNum;

        if(uint(end - start) < s.length())
	{
		if(s.endsWith(UnitText[SVG_LENGTHTYPE_PX]))
			m_unitType = SVG_LENGTHTYPE_PX;
		else if(s.endsWith(UnitText[SVG_LENGTHTYPE_CM]))
			m_unitType = SVG_LENGTHTYPE_CM;
		else if(s.endsWith(UnitText[SVG_LENGTHTYPE_PC]))
			m_unitType = SVG_LENGTHTYPE_PC;
		else if(s.endsWith(UnitText[SVG_LENGTHTYPE_MM]))
			m_unitType = SVG_LENGTHTYPE_MM;
		else if(s.endsWith(UnitText[SVG_LENGTHTYPE_IN]))
			m_unitType = SVG_LENGTHTYPE_IN;
		else if(s.endsWith(UnitText[SVG_LENGTHTYPE_PT]))
			m_unitType = SVG_LENGTHTYPE_PT;
 		else if(s.endsWith(UnitText[SVG_LENGTHTYPE_PERCENTAGE]))
			m_unitType = SVG_LENGTHTYPE_PERCENTAGE;
		else if(s.endsWith(UnitText[SVG_LENGTHTYPE_EMS]))
			m_unitType = SVG_LENGTHTYPE_EMS;
		else if(s.endsWith(UnitText[SVG_LENGTHTYPE_EXS]))
			m_unitType = SVG_LENGTHTYPE_EXS;
		else if(s.isEmpty())
			m_unitType = SVG_LENGTHTYPE_NUMBER;
		else
			m_unitType = SVG_LENGTHTYPE_UNKNOWN;
	}
	else
		m_unitType = SVG_LENGTHTYPE_PX;
	convertNumToPx();
}

void SVGLengthImpl::convertNumToPx()
{
	switch(m_unitType)
	{
		case SVG_LENGTHTYPE_PX:
			m_value = m_valueInSpecifiedUnits;
			break;
		case SVG_LENGTHTYPE_CM:
			m_value = (m_valueInSpecifiedUnits / 2.54) * dpi();
			break;
		case SVG_LENGTHTYPE_MM:
			m_value = (m_valueInSpecifiedUnits / 25.4) * dpi();
			break;
		case SVG_LENGTHTYPE_IN:
			m_value = m_valueInSpecifiedUnits * dpi();
			break;
		case SVG_LENGTHTYPE_PT:
			m_value = (m_valueInSpecifiedUnits / 72.0) * dpi();
			break;
		case SVG_LENGTHTYPE_PC:
			m_value = (m_valueInSpecifiedUnits /  6.0) * dpi();
			break;
		case SVG_LENGTHTYPE_EMS: // Be careful here, always recheck coords-units-BE-01.svg after touching (Niko)
		case SVG_LENGTHTYPE_EXS:
			if(m_context)
			{
				SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(m_context);
				if(!style)
					break;

				bool sizeLocal = (style->getFontSize() != -1);
				bool familyLocal = (style->getFontFamily() && style->getFontFamily()->getFirst());
				
				SVGStylableImpl *parentStyle = 0;
				if((!sizeLocal || !familyLocal) && m_context)
						parentStyle = dynamic_cast<SVGStylableImpl *>(m_context->ownerDoc()->getElementFromHandle(m_context->parentNode().handle()));
				
				// Look up font-size in a SAFE way, because at this place
				// processStyle() has NOT yet been called, so we need
				// a different solution (Niko)
				QString useFont = "Arial";
				double useSize = 12;
				
				if(sizeLocal)
					useSize = style->getFontSize();
				else if(parentStyle && parentStyle->getFontSize() != -1)
					useSize = parentStyle->getFontSize();
			
				if(familyLocal)
					useFont = style->getFontFamily()->getFirst()->string();
				else if(parentStyle && parentStyle->getFontFamily() && parentStyle->getFontFamily()->getFirst())
					useFont = parentStyle->getFontFamily()->getFirst()->string();
				
				if(m_unitType == SVG_LENGTHTYPE_EMS)
					m_value = m_valueInSpecifiedUnits * useSize;
				else
				{
					// Easiest way, use qfont (Niko)
					QFont font(useFont);
					font.setPixelSize(static_cast<int>(useSize));

					QFontMetrics fm(font);
					m_value = m_valueInSpecifiedUnits * fm.boundingRect('x').height();
				}
			}
			break;
	};
}

void SVGLengthImpl::convertPercentageToFloat(const QString &perc, float &result)
{
	// TODO : more error checking ?
	if(perc.endsWith("%"))
		result = perc.left(perc.length() - 1).toFloat() / 100.0;
	else
		result = perc.toFloat();
}

QString SVGLengthImpl::convertValToPercentage(const QString &val, float benchmark)
{
	if(val.endsWith("%"))
		return val;

	QString result;
	float temp = val.toFloat();

	temp = (temp / benchmark) * 100.0;
	result.setNum(temp);
	result.append("%");

	return result;
}

SVGElementImpl *SVGLengthImpl::context() const
{
	return m_context;
}

void SVGLengthImpl::setContext(SVGElementImpl *context)
{
	m_context = context;
}

void SVGLengthImpl::setBBoxContext(SVGShapeImpl *bbox)
{
	m_bboxContext = bbox;
	convertNumToPx();
}

float SVGLengthImpl::percentageOfViewport()
{
	float width = 0, height = 0;
	float value = m_valueInSpecifiedUnits / 100.0;
	if(m_context->viewportElement())
	{
		SVGSVGElementImpl *svg = dynamic_cast<SVGSVGElementImpl *>(m_context->viewportElement());
		if(svg)
		{
			// Calculate against viewBox, otherwise svg width/height
			width = svg->viewBox()->baseVal()->width();
			if(width == 0)
				width = svg->width()->baseVal()->value();
			height = svg->viewBox()->baseVal()->height();
			if(height == 0)
				height = svg->height()->baseVal()->value();
		}

		if(m_mode == LENGTHMODE_WIDTH)
			return value * width;
		else if(m_mode == LENGTHMODE_HEIGHT)
			return value * height;
		else if(m_mode == LENGTHMODE_OTHER)
			return value * sqrt(pow(width, 2) + pow(height, 2)) / sqrt(2.0);
	}
	else if(m_context == m_context->ownerDoc()->rootElement())
	{
		if(!m_context->ownerDoc()->canvas()) // Happens when parsing <svg width="100%"> with printnodetest
			return 0.0;
				
		QPaintDeviceMetrics metrics(m_context->ownerDoc()->canvas()->drawWindow());

		if(m_mode == LENGTHMODE_WIDTH)
			return value * metrics.width();
		else if(m_mode == LENGTHMODE_HEIGHT)
			return value * metrics.height();
		else if(m_mode == LENGTHMODE_OTHER)
			return value * sqrt(pow(metrics.width(), 2) + pow(metrics.height(), 2)) / sqrt(2.0);
	}

	return 0;
}

// Ecma stuff
//
/*
@namespace KSVG
@begin SVGLengthImpl::s_hashTable 5
 unitType					SVGLengthImpl::UnitType					DontDelete|ReadOnly
 value						SVGLengthImpl::Value					DontDelete
 valueAsString				SVGLengthImpl::ValueAsString			DontDelete
 valueInSpecifiedUnits		SVGLengthImpl::ValueInSpecifiedUnits	DontDelete
@end
@namespace KSVG
@begin SVGLengthImplProto::s_hashTable 3
 newValueSpecifiedUnits		SVGLengthImpl::NewValueSpecifiedUnits	DontDelete|Function 2
 convertToSpecifiedUnits	SVGLengthImpl::ConvertToSpecifiedUnits	DontDelete|Function 1
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGLength", SVGLengthImplProto, SVGLengthImplProtoFunc)

Value SVGLengthImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case UnitType:
			return Number(unitType());
		case Value:
			return Number(m_value);
		case ValueAsString:
			return String(valueAsString().string());
		case ValueInSpecifiedUnits:
			return Number(valueInSpecifiedUnits());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return KJS::Undefined();
	}
}

void SVGLengthImpl::putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int)
{
	switch(token)
	{
		case Value:
			setValue(value.toNumber(exec));
			SVGHelperImpl::updateItem(exec, *m_context);
			break;
		case ValueAsString:
			setValueAsString(value.toString(exec).string());
			SVGHelperImpl::updateItem(exec, *m_context);
			break;
		case ValueInSpecifiedUnits:
			setValueInSpecifiedUnits(value.toNumber(exec));
			SVGHelperImpl::updateItem(exec, *m_context);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

Value SVGLengthImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGLengthImpl)

	switch(id)
	{
		case SVGLengthImpl::NewValueSpecifiedUnits:
			obj->newValueSpecifiedUnits(static_cast<unsigned short>(args[0].toNumber(exec)), args[1].toNumber(exec));
			SVGHelperImpl::updateItem(exec, *obj->context());
			return Undefined();
		case SVGLengthImpl::ConvertToSpecifiedUnits:
			obj->convertToSpecifiedUnits(static_cast<unsigned short>(args[0].toNumber(exec)));
			SVGHelperImpl::updateItem(exec, *obj->context());
			return Undefined();
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

/*
@namespace KSVG
@begin SVGLengthImplConstructor::s_hashTable 11
 SVG_LENGTHTYPE_UNKNOWN      KSVG::SVG_LENGTHTYPE_UNKNOWN     DontDelete|ReadOnly
 SVG_LENGTHTYPE_NUMBER       KSVG::SVG_LENGTHTYPE_NUMBER      DontDelete|ReadOnly
 SVG_LENGTHTYPE_PERCENTAGE   KSVG::SVG_LENGTHTYPE_PERCENTAGE  DontDelete|ReadOnly
 SVG_LENGTHTYPE_EMS          KSVG::SVG_LENGTHTYPE_EMS         DontDelete|ReadOnly
 SVG_LENGTHTYPE_EXS          KSVG::SVG_LENGTHTYPE_EXS         DontDelete|ReadOnly
 SVG_LENGTHTYPE_PX           KSVG::SVG_LENGTHTYPE_PX          DontDelete|ReadOnly
 SVG_LENGTHTYPE_CM           KSVG::SVG_LENGTHTYPE_CM          DontDelete|ReadOnly
 SVG_LENGTHTYPE_MM           KSVG::SVG_LENGTHTYPE_MM          DontDelete|ReadOnly
 SVG_LENGTHTYPE_PT           KSVG::SVG_LENGTHTYPE_PT          DontDelete|ReadOnly
 SVG_LENGTHTYPE_PC           KSVG::SVG_LENGTHTYPE_PC          DontDelete|ReadOnly
@end
*/

Value SVGLengthImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token);
}

Value KSVG::getSVGLengthImplConstructor(ExecState *exec)
{
	return cacheGlobalBridge<SVGLengthImplConstructor>(exec, "[[svglength.constructor]]");
}

// vim:ts=4:noet
