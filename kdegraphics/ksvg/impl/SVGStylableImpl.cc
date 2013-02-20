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

#include <qrect.h>

#include "CanvasItem.h"
#include "KSVGCanvas.h"

#include "SVGPaint.h"
#include "SVGColorImpl.h"
#include "SVGPaintImpl.h"
#include "SVGHelperImpl.h"
#include "SVGLengthImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGStylableImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGStringListImpl.h"
#include "SVGImageElementImpl.h"
#include "SVGURIReferenceImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGColorProfileElementImpl.h"
#include "SVGAnimatedLengthListImpl.h"

using namespace KSVG;

#include "SVGStylableImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGStylableImpl::SVGStylableImpl(SVGElementImpl *object) : m_object(object)
{
	KSVG_EMPTY_FLAGS

	// View propidx.html, if you want to verify those default values (Niko)
	m_flags = SVG_STYLE_FLAG_NONE;

	// Initialize all pointers to 0
	// Important!
	m_color = 0;
	m_fillColor = 0;
	m_stopColor = 0;
	m_dashArray = 0;
	m_dashOffset = 0; 
	m_strokeWidth = 0;
	m_strokeColor = 0;
	m_fontFamily = 0;

	m_fillOpacity = 1;
	m_strokeOpacity = 1;
	m_opacity = 1;

	// Special case, getFontSize() could be accessed
	// _before_ processStyle() is called -> no default
	// value for font-size yet -> crash
	// SVGLengthImpl access it when parsing em/ex values (Niko)
	m_fontSize = -1;
}

SVGStylableImpl::~SVGStylableImpl()
{
	if(m_strokeWidth)
		m_strokeWidth->deref();
	if(m_fontFamily)
		m_fontFamily->deref();
	if(m_strokeColor)
		m_strokeColor->deref();
	if(m_fillColor)
		m_fillColor->deref();
	if(m_color)
		m_color->deref();
	if(m_stopColor)
		m_stopColor->deref();
	if(m_dashOffset)
		m_dashOffset->deref();
	if(m_dashArray)
		m_dashArray->deref();
}

void SVGStylableImpl::processStyle()
{
	SVGStylableImpl *parentStyle = 0;
	if(m_object && m_object->ownerDoc())
		parentStyle = dynamic_cast<SVGStylableImpl *>(m_object->ownerDoc()->getElementFromHandle((*m_object).parentNode().handle()));

	// Spec: default "none"
	if(~m_flags & SVG_STYLE_FLAG_STROKE)
	{
		m_strokeColor = new SVGPaintImpl(m_object);
		m_strokeColor->ref();

		SVGPaintImpl *strokeColor = 0L;
		if(parentStyle)
			strokeColor = parentStyle->getStrokeColor();

		if(strokeColor)
			*m_strokeColor = *strokeColor;
		else
			m_strokeColor->setPaint(SVG_PAINTTYPE_NONE);
	}

	// Spec: default "black"
	if(~m_flags & SVG_STYLE_FLAG_FILL)
	{
		m_fillColor = new SVGPaintImpl(m_object);
		m_fillColor->ref();

		SVGPaintImpl *fillColor = 0;
		if(parentStyle)
			fillColor = parentStyle->getFillColor();

		if(fillColor)
			*m_fillColor = *fillColor;
		else
			m_fillColor->setRGBColor(DOM::DOMString("black"));
	}

	// Spec: no real default
	if(~m_flags & SVG_STYLE_FLAG_COLOR)
	{
		m_color = new SVGColorImpl(m_object);
		m_color->ref();
		SVGColorImpl *color = 0;
		if(parentStyle)
			color = parentStyle->getColor();

		if(color)
			*m_color = *color;
	}

	// Spec: default sRGB
	if(~m_flags & SVG_STYLE_FLAG_COLOR_INTERPOLATION)
	{
		if(parentStyle)
			m_colorInterpolation = parentStyle->getColorInterpolation();
		else
			m_colorInterpolation = CI_SRGB;
	}

	// Spec: default "1"
	if(~m_flags & SVG_STYLE_FLAG_STROKE_WIDTH)
	{
		m_strokeWidth = new SVGAnimatedLengthImpl(LENGTHMODE_OTHER, m_object);
		m_strokeWidth->ref();

		SVGAnimatedLengthImpl *strokeWidth = 0;
		if(parentStyle)
			strokeWidth = parentStyle->getStrokeWidth();

		if(strokeWidth)
			*m_strokeWidth = *strokeWidth;
		else
			m_strokeWidth->baseVal()->setValue(1.0);
	}

	// Spec: default "4"
	if(~m_flags & SVG_STYLE_FLAG_STROKE_MITER_LIMIT)
	{
		if(parentStyle)
			m_strokeMiterlimit = parentStyle->getStrokeMiterlimit();
		else
			m_strokeMiterlimit = 4;
	}

	// Spec: default "butt"
	if(~m_flags & SVG_STYLE_FLAG_STROKE_LINE_CAP)
	{
		if(parentStyle)
			m_capStyle = parentStyle->getCapStyle();
		else
			m_capStyle = PATH_STROKE_CAP_BUTT;
	}

	// Spec: default "miter"
	if(~m_flags & SVG_STYLE_FLAG_STROKE_LINE_JOIN)
	{
		if(parentStyle)
			m_joinStyle = parentStyle->getJoinStyle();
		else
			m_joinStyle = PATH_STROKE_JOIN_MITER;
	}

	// Spec: default "auto"
	if(~m_flags & SVG_STYLE_FLAG_CURSOR)
	{
		if(parentStyle)
			m_cursor = parentStyle->getCursor();
		else
			m_cursor = CURSOR_AUTO;
	}

	// Spec: default "visiblePainted"
	if(~m_flags & SVG_STYLE_FLAG_POINTER_EVENTS)
	{
		if(parentStyle)
			m_pointerEvents = parentStyle->getPointerEvents();
		else
			m_pointerEvents = PE_VISIBLE_PAINTED;
	}

	// Spec: default "0"
	if(~m_flags & SVG_STYLE_FLAG_STROKE_DASH_OFFSET)
	{
		m_dashOffset = new SVGAnimatedLengthImpl(LENGTHMODE_OTHER, m_object);
		m_dashOffset->ref();

		SVGAnimatedLengthImpl *dashOffset = 0;
		if(parentStyle)
			dashOffset = parentStyle->getDashOffset();

		if(dashOffset)
			*m_dashOffset = *dashOffset;
		else
			m_dashOffset->baseVal()->setValue(0);
	}

	// Spec: default "none" -> 0 == empty dash array
	if(~m_flags & SVG_STYLE_FLAG_STROKE_DASH_ARRAY)
	{
		SVGAnimatedLengthListImpl *dashArray = 0;
		if(parentStyle)
			dashArray = parentStyle->getDashArray();

		if(dashArray)
		{
			if (!m_dashArray)
			{
				m_dashArray = new SVGAnimatedLengthListImpl();
				m_dashArray->ref();
			}
			*m_dashArray = *dashArray;
		}
		else
			m_dashArray = 0;
	}

	// Spec: default "1" -> 1 == Not opaque
	if(~m_flags & SVG_STYLE_FLAG_FILL_OPACITY)
	{
		if(parentStyle)
			m_fillOpacity = parentStyle->getFillOpacity();
		else
			m_fillOpacity = 1;
	}

	if(~m_flags & SVG_STYLE_FLAG_STROKE_OPACITY)
	{
		if(parentStyle)
			m_strokeOpacity = parentStyle->getStrokeOpacity();
		else
			m_strokeOpacity = 1;
	}

	// Fake group opacity by multiplying by our parent's group opacity
	if(~m_flags & SVG_STYLE_FLAG_OPACITY)
	{
		if(parentStyle)
			m_opacity = parentStyle->getOpacity();
		else
			m_opacity = 1;
	}
	else
		if(parentStyle)
			m_opacity *= parentStyle->getOpacity();

	if(~m_flags & SVG_STYLE_FLAG_CLIP_PATH)
		m_clipPath = "";

	if(~m_flags & SVG_STYLE_FLAG_MASK)
		m_mask = "";

	// Spec: default "nonzero"
	if(~m_flags & SVG_STYLE_FLAG_FILL_RULE)
	{
		if(parentStyle)
			m_fillRule = parentStyle->getFillRule();
		else
			m_fillRule = RULE_NONZERO;
	}
	
	if(~m_flags & SVG_STYLE_FLAG_CLIP_RULE)
	{
		if(parentStyle)
			m_clipRule = parentStyle->getClipRule();
		else
			m_clipRule = RULE_NONZERO;
	}

	// Spec: default "hidden"
	if(~m_flags & SVG_STYLE_FLAG_OVERFLOW)
	{
		if(parentStyle)
			m_overflow = parentStyle->getOverflow();
		else
			m_overflow = false;
	}

	// We are not really, spec compatible here, we just
	// define a bool, to indicate wheter an element should
	// be rendered or not.
	if(~m_flags & SVG_STYLE_FLAG_DISPLAY)
		m_display = true;

	if(~m_flags & SVG_STYLE_FLAG_VISIBILITY)
	{
		if(parentStyle)
			m_visible = parentStyle->getVisible();
		else
			m_visible = true;
	}

	// Spec: default "medium"
	if(~m_flags & SVG_STYLE_FLAG_FONT_SIZE)
	{
		if(parentStyle)
			m_fontSize = parentStyle->getFontSize();
		else
			m_fontSize = fontSizeForText("medium");
	}

	// Spec: default "depends on user agent" -> "Arial" for SVG
	if(~m_flags & SVG_STYLE_FLAG_FONT_FAMILY)
	{
		if(!m_fontFamily)
		{
			m_fontFamily = new SVGStringListImpl();
			m_fontFamily->ref();
		}

		SVGStringListImpl *fontFamily = 0;
		if(parentStyle)
			fontFamily = parentStyle->getFontFamily();

		if(fontFamily)
			*m_fontFamily = *fontFamily;
		else
		{
			SharedString *string = new SharedString("Arial");
			string->ref();
			
			m_fontFamily->appendItem(string);
		}
	}

	// Spec: default "normal"
	if(~m_flags & SVG_STYLE_FLAG_FONT_STYLE)
	{
		if(parentStyle)
			m_fontStyle = parentStyle->getFontStyle();
		else
			m_fontStyle = FSNORMAL;
	}

	// Spec: default "normal"
	if(~m_flags & SVG_STYLE_FLAG_FONT_WEIGHT)
	{
		if(parentStyle)
			m_fontWeight = parentStyle->getFontWeight();
		else
			m_fontWeight = "normal";
	}

	// Spec: default "start"
	if(~m_flags & SVG_STYLE_FLAG_TEXT_ANCHOR)
	{
		if(parentStyle)
			m_textAnchor = parentStyle->getTextAnchor();
		else
			m_textAnchor = TASTART;
	}

	// Spec: default "LTR"
	if(~m_flags & SVG_STYLE_FLAG_TEXT_DIRECTION)
	{
		if(parentStyle)
			m_textDirection = parentStyle->getTextDirection();
		else
			m_textDirection = LTR;
	}

	// Spec: default "none"
	if(~m_flags & SVG_STYLE_FLAG_TEXT_DECORATION)
	{
		if(parentStyle)
			m_textDecoration = parentStyle->getTextDecoration();
		else
			m_textDecoration = TDNONE;
	}

	// Spec: default "baseline"
	if(~m_flags & SVG_STYLE_FLAG_BASELINE_SHIFT)
	{
		if(parentStyle)
			m_baselineShift = parentStyle->getBaselineShift();
		else
			m_baselineShift = "baseline";
	}

	// Spec: default "lr-tb", FIXME
	if(~m_flags & SVG_STYLE_FLAG_TEXT_WRITING_MODE)
	{
		if(parentStyle)
			m_textWritingMode = parentStyle->getTextWritingMode();
		else
			m_textWritingMode = LR;
	}

	// Spec: default "normal"
	if(~m_flags & SVG_STYLE_FLAG_TEXT_UNICODE_BIDI)
	{
		if(parentStyle)
			m_textUnicodeBidi = parentStyle->getTextUnicodeBidi();
		else
			m_textUnicodeBidi = UBNORMAL;
	}

	// Spec: default "auto"
	if(~m_flags & SVG_STYLE_FLAG_GLYPH_ORIENTATION_VERTICAL)
	{
		if(parentStyle)
			m_glyphOrientationVertical = parentStyle->getGlyphOrientationVertical();
		else
			m_glyphOrientationVertical = "auto";
	}	

	// Spec: default "auto"
	if(~m_flags & SVG_STYLE_FLAG_GLYPH_ORIENTATION_HORIZONTAL)
	{
		if(parentStyle)
			m_glyphOrientationHorizontal = parentStyle->getGlyphOrientationHorizontal();
		else
			m_glyphOrientationHorizontal = "auto";
	}	

	// Spec: default "normal"
	if(~m_flags & SVG_STYLE_FLAG_LETTER_SPACING)
	{
		if(parentStyle)
			m_letterSpacing = parentStyle->getLetterSpacing();
		else
			m_letterSpacing = "normal";
	}

	// Spec: default "normal"
	if(~m_flags & SVG_STYLE_FLAG_WORD_SPACING)
	{
		if(parentStyle)
			m_wordSpacing = parentStyle->getWordSpacing();
		else
			m_wordSpacing = "normal";
	}

	// Spec: default "black"
	if(~m_flags & SVG_STYLE_FLAG_STOP)
	{
		m_stopColor = new SVGColorImpl(m_object);
		m_stopColor->ref();

		m_stopColor->setRGBColor(DOM::DOMString("black"));
	}

	// Spec: default "none"
	if(~m_flags & SVG_STYLE_FLAG_MARKER_START)
	{
		if(parentStyle)
			m_startMarker = parentStyle->getStartMarker();
		else
			m_startMarker = QString::null;
	}

	// Spec: default "none"
	if(~m_flags & SVG_STYLE_FLAG_MARKER_MID)
	{
		if(parentStyle)
			m_midMarker = parentStyle->getMidMarker();
		else
			m_midMarker = QString::null;
	}

	// Spec: default "none"
	if(~m_flags & SVG_STYLE_FLAG_MARKER_END)
	{
		if(parentStyle)
			m_endMarker = parentStyle->getEndMarker();
		else
			m_endMarker = QString::null;
	}
}

bool SVGStylableImpl::isStroked() const
{
	if(!m_strokeColor)
		return false;
	
	return m_strokeColor->paintType() != SVG_PAINTTYPE_UNKNOWN &&
		   m_strokeColor->paintType() != SVG_PAINTTYPE_NONE &&
		   m_strokeColor->paintType() != SVG_PAINTTYPE_URI_NONE;
}

bool SVGStylableImpl::isFilled() const
{
	if(!m_fillColor)
		return false;
	
	return m_fillColor->paintType() != SVG_PAINTTYPE_UNKNOWN &&
		   m_fillColor->paintType() != SVG_PAINTTYPE_NONE &&
		   m_fillColor->paintType() != SVG_PAINTTYPE_URI_NONE;
}

QString SVGStylableImpl::extractUrlId(const QString &url)
{
	QString id;

	if(url.startsWith("url(#"))
	{
		int idstart = url.find("#") + 1;
		id = url.mid(idstart, url.length() - idstart - 1);
	}
	else
		id = url;

	return id;
}

void SVGStylableImpl::setMarkers(const QString &marker)
{
	setStartMarker(marker);
	setMidMarker(marker);
	setEndMarker(marker);
}

void SVGStylableImpl::setStartMarker(const QString &startMarker)
{
	if(startMarker.startsWith("url(#"))
	{
		int idstart = startMarker.find("#") + 1;
		m_startMarker = startMarker.mid(idstart, startMarker.length() - idstart - 1);
	}
	else if(startMarker == "none")
		m_startMarker = QString::null;
}

void SVGStylableImpl::setMidMarker(const QString &midMarker)
{
	if(midMarker.startsWith("url(#"))
	{
		int idstart = midMarker.find("#") + 1;
		m_midMarker = midMarker.mid(idstart, midMarker.length() - idstart - 1);
	}
	else if(midMarker == "none")
		m_midMarker = QString::null;
}

void SVGStylableImpl::setEndMarker(const QString &endMarker)
{
	if(endMarker.startsWith("url(#"))
	{
		int idstart = endMarker.find("#") + 1;
		m_endMarker = endMarker.mid(idstart, endMarker.length() - idstart - 1);
	}
	else if(endMarker == "none")
		m_endMarker = QString::null;
}

bool SVGStylableImpl::hasMarkers() const
{
	return !m_startMarker.isEmpty() || !m_midMarker.isEmpty() || !m_endMarker.isEmpty();
}

void SVGStylableImpl::setPaint(const QString &param, SVGPaintImpl *svgPaint)
{
	if(param.stripWhiteSpace() == "none")
		svgPaint->setPaint(SVG_PAINTTYPE_NONE, DOM::DOMString(""), DOM::DOMString(""));
	else if(SVGURIReferenceImpl::isUrl(param))
		svgPaint->setUri(SVGURIReferenceImpl::getTarget(param));
	else
		setColor(param, svgPaint);
}

void SVGStylableImpl::setColor(const QString &param, SVGColorImpl *svgColor)
{
	if(param.stripWhiteSpace().startsWith("#"))
	{
		if(param.contains("icc-color"))
		{
			QString first = param.left(7);
			QString last = param.right(param.length() - 8);

			svgColor->setRGBColorICCColor(first, last);
		}
		else
		{
			QColor color;
			color.setNamedColor(param.stripWhiteSpace());
			svgColor->setRGBColor(color);
		}
	}
	else if(param.stripWhiteSpace().startsWith("rgb("))
	{
		QString parse = param.stripWhiteSpace();
		QStringList colors = QStringList::split(',', parse);
		QString r = colors[0].right((colors[0].length() - 4));
		QString g = colors[1];
		QString b = colors[2].left((colors[2].length() - 1));

		if(r.contains("%"))
		{
			r = r.left(r.length() - 1);
			r = QString::number(int((double(255 * r.toDouble()) / 100.0)));
		}
		
		if(g.contains("%"))
		{
			g = g.left(g.length() - 1);
			g = QString::number(int((double(255 * g.toDouble()) / 100.0)));
		}

		if(b.contains("%"))
		{
			b = b.left(b.length() - 1);
			b = QString::number(int((double(255 * b.toDouble()) / 100.0)));
		}

		svgColor->setRGBColor(int(r.toFloat()), int(g.toFloat()), int(b.toFloat()));
	}
	else
	{
        if(param.stripWhiteSpace().lower() == "currentcolor")
			svgColor->setColor(SVG_COLORTYPE_CURRENTCOLOR, DOM::DOMString(""), DOM::DOMString(""));
		else
			svgColor->setRGBColor(DOM::DOMString(param.stripWhiteSpace().lower()));
	}
}

QRect SVGStylableImpl::clip()
{
	return QRect();
}

void SVGStylableImpl::setClip(const QString &)
{
}

float SVGStylableImpl::fontSizeForText(const QString &value)
{
	float ret = -1;

	// Spec: "On a computer screen a scaling factor of 1.2 is suggested between adjacent indexes"
	const float factor = 1.2;

	// Spec: "If the 'medium' font is 12pt, the 'large' font could be 14.4pt."
	const float mediumFont = 12.0;

	if(value == "xx-small")
		ret = mediumFont - (3.0 * factor);
	else if(value == "x-small")
		ret = mediumFont - (2.0 * factor);
	else if(value == "small")
		ret = mediumFont - factor;
	else if(value == "medium")
		ret = mediumFont;		
	else if(value == "large")
		ret = mediumFont + factor;
	else if(value == "x-large")
		ret = mediumFont + (2.0 * factor);
	else if(value == "xx-large")
		ret = mediumFont + (3.0 * factor);
		
	return ret;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGStylableImpl::s_hashTable 47
 className			SVGStylableImpl::ClassName			DontDelete|ReadOnly
 style				SVGStylableImpl::Style				DontDelete|ReadOnly
 stroke-width		SVGStylableImpl::StrokeWidth		DontDelete|ReadOnly
 stroke-miterlimit	SVGStylableImpl::StrokeMiterlimit	DontDelete|ReadOnly
 stroke-linecap		SVGStylableImpl::StrokeLineCap		DontDelete|ReadOnly
 stroke-linejoin	SVGStylableImpl::StrokeLineJoin		DontDelete|ReadOnly
 stroke				SVGStylableImpl::Stroke				DontDelete|ReadOnly
 fill				SVGStylableImpl::Fill				DontDelete|ReadOnly
 color				SVGStylableImpl::Color				DontDelete|ReadOnly
 stop-color			SVGStylableImpl::StopColor			DontDelete|ReadOnly
 font-size			SVGStylableImpl::FontSize			DontDelete|ReadOnly
 font-family		SVGStylableImpl::FontFamily			DontDelete|ReadOnly
 font-weight		SVGStylableImpl::FontWeight			DontDelete|ReadOnly
 font-style			SVGStylableImpl::FontStyle			DontDelete|ReadOnly
 text-decoration	SVGStylableImpl::TextDecoration		DontDelete|ReadOnly
 text-anchor		SVGStylableImpl::TextAnchor			DontDelete|ReadOnly
 direction			SVGStylableImpl::Direction			DontDelete|ReadOnly
 writing-mode		SVGStylableImpl::WritingMode		DontDelete|ReadOnly
 unicode-bidi		SVGStylableImpl::UnicodeBidi		DontDelete|ReadOnly
 opacity			SVGStylableImpl::Opacity			DontDelete|ReadOnly
 fill-opacity		SVGStylableImpl::FillOpacity		DontDelete|ReadOnly
 stroke-opacity		SVGStylableImpl::StrokeOpacity		DontDelete|ReadOnly
 clip-path			SVGStylableImpl::ClipPath			DontDelete|ReadOnly
 marker-start		SVGStylableImpl::MarkerStart		DontDelete|ReadOnly
 marker-mid			SVGStylableImpl::MarkerMid			DontDelete|ReadOnly
 marker-end			SVGStylableImpl::MarkerEnd			DontDelete|ReadOnly
 marker				SVGStylableImpl::Marker				DontDelete|ReadOnly
 cursor				SVGStylableImpl::Cursor				DontDelete|ReadOnly
 display			SVGStylableImpl::Display			DontDelete|ReadOnly
 overflow			SVGStylableImpl::Overflow			DontDelete|ReadOnly
 clip				SVGStylableImpl::Clip				DontDelete|ReadOnly
 visibility			SVGStylableImpl::Visibility			DontDelete|ReadOnly
 fill-rule			SVGStylableImpl::FillRule			DontDelete|ReadOnly
 clip-rule			SVGStylableImpl::ClipRule			DontDelete|ReadOnly
 stroke-dashoffset	SVGStylableImpl::StrokeDashOffset	DontDelete|ReadOnly
 stroke-dasharray	SVGStylableImpl::StrokeDashArray	DontDelete|ReadOnly
 color-profile		SVGStylableImpl::ColorProfile		DontDelete|ReadOnly
 baseline-shift		SVGStylableImpl::BaselineShift		DontDelete|ReadOnly
 letter-spacing		SVGStylableImpl::LetterSpacing		DontDelete|ReadOnly
 word-spacing		SVGStylableImpl::WordSpacing		DontDelete|ReadOnly
 pointer-events		SVGStylableImpl::PointerEvents		DontDelete|ReadOnly
 glyph-orientation-vertical	SVGStylableImpl::GlyphOrientationVertical	DontDelete|ReadOnly
 glyph-orientation-horizontal	SVGStylableImpl::GlyphOrientationHorizontal	DontDelete|ReadOnly
 color-interpolation	SVGStylableImpl::ColorInterpolation	DontDelete|ReadOnly
 mask				SVGStylableImpl::Mask	DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGStylableImplProto::s_hashTable 2
 getStyle			SVGStylableImpl::GetStyle			DontDelete|Function 0
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGStylable", SVGStylableImplProto, SVGStylableImplProtoFunc)

Value SVGStylableImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		//case ClassName:
		//	return String(className().string());
		case Style:
			return String(m_object ? m_object->DOM::Element::getAttribute("style") : "");
		case Visibility:
			return String(m_visible ? "visible" : "hidden");
		case Display:
			return String(m_display ? "inline" : "none");
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGStylableImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	QString param = value.toString(exec).qstring();
	
	if (param.isEmpty())
		return;
	
	bool redraw = false;
	bool inherit = (param == "inherit");
	int update = -1;

	switch(token)
	{
		case Style:
		{
			if(!m_object)
				return;

			QStringList substyles = QStringList::split(';', param);
			for(QStringList::Iterator it = substyles.begin(); it != substyles.end(); ++it)
			{
				QStringList substyle = QStringList::split(':', (*it));
				m_object->setAttributeInternal(substyle[0].stripWhiteSpace(), substyle[1].stripWhiteSpace());
			}
			break;
		}
		case StrokeWidth:
			if(m_flags & SVG_STYLE_FLAG_STROKE_WIDTH)
			{
				redraw = true;
				update = UPDATE_LINEWIDTH;
			}
			if(inherit)
				m_flags &= ~SVG_STYLE_FLAG_STROKE_WIDTH;
			else
			{
				m_flags |= SVG_STYLE_FLAG_STROKE_WIDTH;

				if(!m_strokeWidth)
				{
					m_strokeWidth = new SVGAnimatedLengthImpl(LENGTHMODE_OTHER, m_object);
					m_strokeWidth->ref();
				}

				m_strokeWidth->baseVal()->setValueAsString(param);
			}
			break;
		case StrokeMiterlimit:
			m_flags |= SVG_STYLE_FLAG_STROKE_MITER_LIMIT;
			if(!inherit)
				m_strokeMiterlimit = param.toUInt();
			break;
		case StrokeLineCap:
			m_flags |= SVG_STYLE_FLAG_STROKE_LINE_CAP;
			if(param == "butt")
				m_capStyle = PATH_STROKE_CAP_BUTT;
			else if(param == "round")
				m_capStyle = PATH_STROKE_CAP_ROUND;
			else if(param == "square")
				m_capStyle = PATH_STROKE_CAP_SQUARE;
			break;
		case StrokeLineJoin:
			m_flags |= SVG_STYLE_FLAG_STROKE_LINE_JOIN;
			if(param == "miter")
				m_joinStyle = PATH_STROKE_JOIN_MITER;
			else if(param == "round")
				m_joinStyle = PATH_STROKE_JOIN_ROUND;
			else if(param == "bevel")
				m_joinStyle = PATH_STROKE_JOIN_BEVEL;
			break;
		case Stroke:
			if(m_flags & SVG_STYLE_FLAG_STROKE)
				redraw = true;
			if(inherit)
				m_flags &= ~SVG_STYLE_FLAG_STROKE;
			else
			{
				m_flags |= SVG_STYLE_FLAG_STROKE;

				if(!m_strokeColor)
				{
					m_strokeColor = new SVGPaintImpl(m_object);
					m_strokeColor->ref();
				}

				setPaint(param, m_strokeColor);
			}
			break;
		case Fill:
			if(m_flags & SVG_STYLE_FLAG_FILL)
			{
				redraw = true;
				update = UPDATE_STYLE;
			}
			if(inherit)
				m_flags &= ~SVG_STYLE_FLAG_FILL;
			else
			{
				m_flags |= SVG_STYLE_FLAG_FILL;

				if(!m_fillColor)
				{
					m_fillColor = new SVGPaintImpl(m_object);
					m_fillColor->ref();
				}

				setPaint(param, m_fillColor);
			}
			break;
		case Color:
			if(m_flags & SVG_STYLE_FLAG_COLOR)
				redraw = true;
			if(inherit)
				m_flags &= ~SVG_STYLE_FLAG_COLOR;
			else
			{
				m_flags |= SVG_STYLE_FLAG_COLOR;

				if(!m_color)
				{
					m_color = new SVGColorImpl(m_object);
					m_color->ref();
				}
				setColor(param, m_color);
			}
			break;
		case StopColor:
			m_flags |= SVG_STYLE_FLAG_STOP;

			if(!m_stopColor)
			{
				m_stopColor = new SVGColorImpl(m_object);
				m_stopColor->ref();
			}

			if(!inherit)
				setColor(param, m_stopColor);
			break;
		case ColorInterpolation:
			if(inherit)
				m_flags &= ~SVG_STYLE_FLAG_COLOR_INTERPOLATION;
			else
			{
				m_flags |= SVG_STYLE_FLAG_COLOR_INTERPOLATION;
				if(param == "auto" || param == "sRGB")
					m_colorInterpolation = CI_SRGB;
				else
				if(param == "linearRGB")
					m_colorInterpolation = CI_LINEARRGB;
			}
			break;
		case FontSize:
		{
			m_flags |= SVG_STYLE_FLAG_FONT_SIZE;				
			if(!inherit)
			{
				double temp = fontSizeForText(param);
				if(temp != -1) // Is "absolute-size"
				{
					m_fontSize = temp;
					break;
				}

				SVGLengthImpl *length = SVGSVGElementImpl::createSVGLength();
				length->setContext(m_object);
				length->setValueAsString(DOM::DOMString(param));
				m_fontSize = length->value();
				length->deref();
			}
			break;
		}
		case FontFamily:
			m_flags |= SVG_STYLE_FLAG_FONT_FAMILY;

			// Hacks
			// #1 Replace "'" characters by ""
			param = param.replace('\'', QString::null);
			// #2 Replace "MS-Gothic" by "MS Gothic"
			param = param.replace("MS-Gothic", "MS Gothic");
			// #3 Replace "Helvetica" by "Arial"
			param = param.replace("Helvetica", "Arial");
			param = param.replace("helvetica", "Arial");

			if(!m_fontFamily)
			{
				m_fontFamily = new SVGStringListImpl();
				m_fontFamily->ref();
			}

			if(!inherit)
				SVGHelperImpl::parseCommaSeperatedList(m_fontFamily, param);
			break;
		case FontWeight:
			m_flags |= SVG_STYLE_FLAG_FONT_WEIGHT;
			if(!inherit)
				m_fontWeight = param;
			break;
		case FontStyle:
			m_flags |= SVG_STYLE_FLAG_FONT_STYLE;
			if(param == "normal")
				m_fontStyle = FSNORMAL;
			else if(param == "italic")
				m_fontStyle = ITALIC;
			else if(param == "oblique")
				m_fontStyle = OBLIQUE;
			break;
		case TextDecoration:
			m_flags |= SVG_STYLE_FLAG_TEXT_DECORATION;
			if(param == "none")
				m_textDecoration = TDNONE;
			{
				// CSS2 allows multiple decorations
				m_textDecoration = TDNONE;
				QStringList decorations = QStringList::split(' ', param);
				for(QStringList::Iterator it = decorations.begin(); it != decorations.end(); ++it)
				{
					if(*it == "underline")
						m_textDecoration |= UNDERLINE;
					else if(*it == "overline")
						m_textDecoration |= OVERLINE;
					else if(*it == "line-through")
						m_textDecoration |= LINE_THROUGH;
				}
			}
			break;
		case TextAnchor:
			m_flags |= SVG_STYLE_FLAG_TEXT_ANCHOR;
			if(param == "start")
				m_textAnchor = TASTART;
			else if(param == "middle")
				m_textAnchor = TAMIDDLE;
			else if(param == "end")
				m_textAnchor = TAEND;
			break;
		case Direction:
			m_flags |= SVG_STYLE_FLAG_TEXT_DIRECTION;
			// Spec: direction is only processed when unicode-bidi
			//       is set to bidi-override or embedded
			if(m_textUnicodeBidi == OVERRIDE ||
				m_textUnicodeBidi == EMBED ||
				m_textUnicodeBidi == UBNORMAL)
			{
				if(param == "rtl")
					m_textDirection = RTL;
				else if(param == "ltr")
					m_textDirection = LTR;
			}
			break;
		case WritingMode:
			m_flags |= SVG_STYLE_FLAG_TEXT_WRITING_MODE;
			if(param == "lr-tb" || param == "lr")
				m_textWritingMode = LR;
			else if(param == "rl-tb" || param == "rl")
				m_textWritingMode = RL;
			else if(param == "tb-lr" || param == "tb")
				m_textWritingMode = TB;
			break;
		case UnicodeBidi:
			m_flags |= SVG_STYLE_FLAG_TEXT_UNICODE_BIDI;
			if(param == "normal")
				m_textUnicodeBidi = UBNORMAL;
			else if(param == "embed")
				m_textUnicodeBidi = EMBED;
			else if(param == "bidi-override")
				m_textUnicodeBidi = OVERRIDE;
			break;
		case GlyphOrientationVertical:
			m_flags |= SVG_STYLE_FLAG_GLYPH_ORIENTATION_VERTICAL;
			m_glyphOrientationVertical = param;
			break;
		case GlyphOrientationHorizontal:
			m_flags |= SVG_STYLE_FLAG_GLYPH_ORIENTATION_HORIZONTAL;
			m_glyphOrientationHorizontal = param;
			break;
		case Opacity:
			m_flags |= SVG_STYLE_FLAG_OPACITY;

			if(!inherit)
			{
				SVGLengthImpl::convertPercentageToFloat(value.toString(exec).qstring(), m_opacity);
			}
			break;
		case FillOpacity:
			m_flags |= SVG_STYLE_FLAG_FILL_OPACITY;

			if(!inherit)
			{
				SVGLengthImpl::convertPercentageToFloat(value.toString(exec).qstring(), m_fillOpacity);
			}
			break;
		case StrokeOpacity:
			m_flags |= SVG_STYLE_FLAG_STROKE_OPACITY;

			if(!inherit)
			{
				SVGLengthImpl::convertPercentageToFloat(value.toString(exec).qstring(), m_strokeOpacity);
			}
			break;
		case ClipPath:
			m_flags |= SVG_STYLE_FLAG_CLIP_PATH;
			if(!inherit)
				m_clipPath = extractUrlId(param);
			break;
		case Mask:
			m_flags |= SVG_STYLE_FLAG_MASK;
			if(!inherit)
				m_mask = extractUrlId(param);
			break;
		case MarkerStart:
			m_flags |= SVG_STYLE_FLAG_MARKER_START;
			if(!inherit)
				setStartMarker(param);
			break;
		case MarkerMid:
			m_flags |= SVG_STYLE_FLAG_MARKER_MID;
			if(!inherit)
				setMidMarker(param);
			break;
		case MarkerEnd:
			m_flags |= SVG_STYLE_FLAG_MARKER_END;
			if(!inherit)
				setEndMarker(param);
			break;
		case Marker:
			m_flags |= (SVG_STYLE_FLAG_MARKER_START | SVG_STYLE_FLAG_MARKER_MID | SVG_STYLE_FLAG_MARKER_END);
			if(!inherit)
				setMarkers(param);
			break;
		case PointerEvents:
			m_flags |= SVG_STYLE_FLAG_POINTER_EVENTS;
			if(param == "none")
				m_pointerEvents = PE_NONE;
			else if(param == "stroke")
				m_pointerEvents = PE_STROKE;
			else if(param == "fill")
				m_pointerEvents = PE_FILL;
			else if(param == "painted")
				m_pointerEvents = PE_PAINTED;
			else if(param == "visibleStroke")
				m_pointerEvents = PE_VISIBLE_STROKE;
			else if(param == "visibleFill")
				m_pointerEvents = PE_VISIBLE_FILL;
			else if(param == "visiblePainted")
				m_pointerEvents = PE_VISIBLE_PAINTED;
			else if(param == "visible")
				m_pointerEvents = PE_VISIBLE;
			else if(param == "all")
				m_pointerEvents = PE_ALL;
			break;
		case Cursor:
			m_flags |= SVG_STYLE_FLAG_CURSOR;
			if(param == "auto")
				m_cursor = CURSOR_AUTO;
			else if(param == "crosshair")
				m_cursor = CURSOR_CROSSHAIR;
			else if(param == "default")
				m_cursor = CURSOR_DEFAULT;
			else if(param == "pointer")
				m_cursor = CURSOR_POINTER;
			else if(param == "move")
				m_cursor = CURSOR_MOVE;
			else if(param == "e-resize")
				m_cursor = CURSOR_E_RESIZE;
			else if(param == "ne-resize")
				m_cursor = CURSOR_NE_RESIZE;
			else if(param == "nw-resize")
				m_cursor = CURSOR_NW_RESIZE;
			else if(param == "n-resize")
				m_cursor = CURSOR_N_RESIZE;
			else if(param == "se-resize")
				m_cursor = CURSOR_SE_RESIZE;
			else if(param == "sw-resize")
				m_cursor = CURSOR_SW_RESIZE;
			else if(param == "s-resize")
				m_cursor = CURSOR_S_RESIZE;
			else if(param == "w-resize")
				m_cursor = CURSOR_W_RESIZE;
			else if(param == "text")
				m_cursor = CURSOR_TEXT;
			else if(param == "wait")
				m_cursor = CURSOR_WAIT;
			else if(param == "help")
				m_cursor = CURSOR_HELP;
			break;
		case Display:
			m_flags |= SVG_STYLE_FLAG_DISPLAY;
	
			if(param == "none")
				m_display = false;
			else if(!inherit)
				m_display = true;
			break;
		case Overflow:
			m_flags |= SVG_STYLE_FLAG_OVERFLOW;
			if(param == "hidden" || param == "scroll")
				m_overflow = false;
			else if(!inherit)
				m_overflow = true;
			break;
		case Clip:
			m_flags |= SVG_STYLE_FLAG_CLIP_PATH;
			if(!inherit)
				setClip(param);
			break;
		case Visibility:
			if(m_flags & SVG_STYLE_FLAG_VISIBILITY)
				redraw = true;
			if(inherit)
				m_flags &= ~SVG_STYLE_FLAG_COLOR;
			else
			{
				m_flags |= SVG_STYLE_FLAG_VISIBILITY;

				if(param == "visible")
					m_visible = true;
				else if(!inherit)
					m_visible = false;

				// Just a quick fix for the script-* files (Niko)
				// Any better solution??
				update = UPDATE_TRANSFORM;
				redraw = true;
			}
			SVGHelperImpl::applyContainer<SVGStylableImpl>(this, Visibility, param);
			break;
		case FillRule:
			m_flags |= SVG_STYLE_FLAG_FILL_RULE;
			if(!inherit)
				m_fillRule = (param == "evenodd" ? RULE_EVENODD : RULE_NONZERO);
			break;
		case ClipRule:
			m_flags |= SVG_STYLE_FLAG_CLIP_RULE;
			if(!inherit)
				m_clipRule = (param == "evenodd" ? RULE_EVENODD : RULE_NONZERO);
			break;
		case StrokeDashOffset:
			m_flags |= SVG_STYLE_FLAG_STROKE_DASH_OFFSET;

			if(!m_dashOffset)
			{
				m_dashOffset = new SVGAnimatedLengthImpl(LENGTHMODE_OTHER, m_object);
				m_dashOffset->ref();
			}

			if(!inherit)
				m_dashOffset->baseVal()->setValueAsString(param);
			break;
		case StrokeDashArray:
		{
			m_flags |= SVG_STYLE_FLAG_STROKE_DASH_ARRAY;

			if(!m_dashArray)
			{
				m_dashArray = new SVGAnimatedLengthListImpl();
				m_dashArray->ref();
			}
			else
				m_dashArray->baseVal()->clear();

			if(param != "none" && !inherit)
				SVGHelperImpl::parseLengthList(m_dashArray, param);
			break;
		}
		case ColorProfile:
		{
			m_flags |= SVG_STYLE_FLAG_COLOR_PROFILE;
			if(!inherit)
			{
				if(!m_object)
					return;

				SVGColorProfileElementImpl *handle = static_cast<SVGColorProfileElementImpl *>(m_object->ownerSVGElement()->getElementById(SVGURIReferenceImpl::getTarget(param)));
				if(handle)
					SVGImageElementImpl::applyColorProfile(handle, static_cast<SVGImageElementImpl *>(this));
			}
			break;
		}
		case BaselineShift:
		{
			m_flags |= SVG_STYLE_FLAG_BASELINE_SHIFT;
			if(!inherit)
				m_baselineShift = param;
			break;
		}
		case LetterSpacing:
			m_flags |= SVG_STYLE_FLAG_LETTER_SPACING;
		case WordSpacing:
		{
			if(!inherit)
			{
				if(token == WordSpacing)
				{
					m_flags |= SVG_STYLE_FLAG_WORD_SPACING;
					m_wordSpacing = param;
				}
				else
					m_letterSpacing = param;
			}
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}

	if(redraw)
	{
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(m_object);
		if(inherit)
			processStyle();
			
		if(shape && shape->item())
		{
			if(update > -1)
				shape->item()->update(static_cast<CanvasItemUpdate>(update));
			else if(m_object)
				m_object->ownerDoc()->canvas()->invalidate(shape->item(), false);
		}
	}
}

Value SVGStylableImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &)
{
	KSVG_CHECK_THIS(SVGStylableImpl)

	switch(id)
	{
		case SVGStylableImpl::GetStyle:
			return Undefined();
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

// vim:ts=4:noet
