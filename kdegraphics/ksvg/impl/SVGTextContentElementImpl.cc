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
#include <klocale.h>

#include "Glyph.h"

#include "SVGStringListImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGTextContentElementImpl.h"
#include "SVGTextElementImpl.h"
#include "SVGAnimatedEnumerationImpl.h"
#include "SVGDocumentImpl.h"

using namespace KSVG;

#include "SVGTextContentElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_cacheimpl.h"

SVGTextContentElementImpl::SVGTextContentElementImpl(DOM::ElementImpl *impl) : SVGShapeImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this)
{
	KSVG_EMPTY_FLAGS

	m_lengthAdjust = new SVGAnimatedEnumerationImpl();
	m_lengthAdjust->ref();

	// Spec: default value 'spacing'
	m_lengthAdjust->setBaseVal(LENGTHADJUST_SPACING);

	m_textLength = new SVGAnimatedLengthImpl();
	m_textLength->baseVal()->setValueAsString("-1");
	m_textLength->ref();
}

SVGTextContentElementImpl::~SVGTextContentElementImpl()
{
	if(m_lengthAdjust)
		m_lengthAdjust->deref();
	if(m_textLength)
		m_textLength->deref();
}

QString SVGTextContentElementImpl::textDirectionAwareText()
{
	QString text;

	if(hasChildNodes())
	{
		bool ltr = getTextDirection() == LTR;
		DOM::Node node = ltr ? firstChild() : lastChild();

		for(; !node.isNull(); node = ltr ? node.nextSibling() : node.previousSibling())
		{
			if(node.nodeType() == TEXT_NODE)
			{
				DOM::Text textNode = node;
				QString temp = textNode.data().string();

				if(!ltr)
				{
					QString convert = temp;

					for(int i = temp.length(); i > 0; i--)
						convert[temp.length() - i] = temp[i - 1];

					text += convert;
				}
				else
					text += temp;
			}
			else
				return text;
		}
	}

	return text;
}

T2P::GlyphLayoutParams *SVGTextContentElementImpl::layoutParams() const
{
	SVGStylableImpl *style = const_cast<SVGTextContentElementImpl *>(this);

	T2P::GlyphLayoutParams *params = new T2P::GlyphLayoutParams();
	params->setTb(style->getTextWritingMode() == TB);
	params->setUseBidi(style->getTextUnicodeBidi() == UBNORMAL);
	if(!dynamic_cast<SVGTextElementImpl *>(m_object)) // not allowed for <text>
		params->setBaselineShift(style->getBaselineShift().latin1());

	bool worked = true;
	int deg = style->getGlyphOrientationVertical().toInt(&worked);
	if(!worked)
		params->setGlyphOrientationVertical(-90);
	else
		params->setGlyphOrientationVertical(deg);

	worked = true;
	deg = style->getGlyphOrientationHorizontal().toInt(&worked);
	if(!worked)
		params->setGlyphOrientationHorizontal(-90);
	else
		params->setGlyphOrientationHorizontal(deg);

	SVGLengthImpl *length = new SVGLengthImpl(LENGTHMODE_OTHER, const_cast<SVGTextContentElementImpl *>(this));
	length->ref();

	if(style->getLetterSpacing() != "normal" && style->getLetterSpacing() != "inherit")
		length->setValueAsString(DOM::DOMString(style->getLetterSpacing()));
	params->setLetterSpacing(length->value());

	if(style->getWordSpacing() != "normal" && style->getWordSpacing() != "inherit")
		length->setValueAsString(DOM::DOMString(style->getWordSpacing()));
	params->setWordSpacing(length->value());

	length->deref();

	return params;
}

SVGAnimatedLengthImpl *SVGTextContentElementImpl::textLength() const
{
	return m_textLength;
}

SVGAnimatedEnumerationImpl *SVGTextContentElementImpl::lengthAdjust() const
{
	return m_lengthAdjust;
}

long SVGTextContentElementImpl::getNumberOfChars()
{
	return 0;
}

float SVGTextContentElementImpl::getComputedTextLength()
{
	return 0.0;
}

/*
float SVGTextContentElementImpl::getSubStringLength(const unsigned long &charnum,const unsigned long &nchars)
{
}

SVGPoint SVGTextContentElementImpl::getStartPositionOfChar(const unsigned long &charnum)
{
}

SVGPoint SVGTextContentElementImpl::getEndPositionOfChar(const unsigned long &charnum)
{
}

SVGRect SVGTextContentElementImpl::getExtentOfChar(const unsigned long &charnum)
{
}

float SVGTextContentElementImpl::getRotationOfChar(const unsigned long &charnum)
{
}

long SVGTextContentElementImpl::getCharNumAtPosition(const SVGPoint &point)
{
}

void SVGTextContentElementImpl::selectSubString(const unsigned long &charnum,const unsigned long &nchars)
{
}
*/

// Ecma stuff

/*
@namespace KSVG
@begin SVGTextContentElementImpl::s_hashTable 3
 textLength				SVGTextContentElementImpl::TextLength						DontDelete|ReadOnly
 lengthAdjust			SVGTextContentElementImpl::LengthAdjust						DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGTextContentElementImplProto::s_hashTable 11
 getNumberOfChars		SVGTextContentElementImpl::GetNumberOfChars					DontDelete|Function 0
 getComputedTextLength	SVGTextContentElementImpl::GetComputedTextLength			DontDelete|Function 0
 getSubStringLength		SVGTextContentElementImpl::GetSubStringLength				DontDelete|Function 2
 getStartPositionOfChar	SVGTextContentElementImpl::GetStartPositionOfChar			DontDelete|Function 1
 getEndPositionOfChar	SVGTextContentElementImpl::GetEndPositionOfChar				DontDelete|Function 1
 getExtentOfChar 		SVGTextContentElementImpl::GetExtentOfChar 					DontDelete|Function 1
 getRotationOfChar 		SVGTextContentElementImpl::GetRotationOfChar 				DontDelete|Function 1
 getCharNumAtPosition 	SVGTextContentElementImpl::GetCharNumAtPosition				DontDelete|Function 1
 selectSubString		SVGTextContentElementImpl::SelectSubString					DontDelete|Function 2
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGTextContentElement", SVGTextContentElementImplProto, SVGTextContentElementImplProtoFunc)

Value SVGTextContentElementImpl::getValueProperty(ExecState *, int token) const
{
	//KSVG_CHECK_ATTRIBUTE

	switch(token)
	{
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGTextContentElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case TextLength:
			m_textLength->baseVal()->setValueAsString(value.toString(exec).string());
			if(m_textLength->baseVal()->value() < 0) // A negative value is an error
                                gotError(i18n("Negative value for attribute textLength of element <text> is illegal"));
			break;
		case LengthAdjust:
		{
			QString temp = value.toString(exec).qstring();
			if(temp == "spacingAndGlyphs")
				m_lengthAdjust->setBaseVal(LENGTHADJUST_SPACINGANDGLYPHS);
			else if(temp == "spacing")
				m_lengthAdjust->setBaseVal(LENGTHADJUST_SPACING);
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

Value SVGTextContentElementImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &)
{
	KSVG_CHECK_THIS(SVGTextContentElementImpl)

	switch(id)
	{
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

/*
@namespace KSVG
@begin SVGTextContentElementImplConstructor::s_hashTable 5
 LENGTHADJUST_UNKNOWN      		KSVG::LENGTHADJUST_UNKNOWN				DontDelete|ReadOnly
 LENGTHADJUST_SPACING      		KSVG::LENGTHADJUST_SPACING				DontDelete|ReadOnly
 LENGTHADJUST_SPACINGANDGLYPHS	KSVG::LENGTHADJUST_SPACINGANDGLYPHS		DontDelete|ReadOnly
@end
*/

Value SVGTextContentElementImplConstructor::getValueProperty(ExecState *, int token) const
{
	return Number(token);
}

Value KSVG::getSVGTextContentElementImplConstructor(ExecState *exec)
{
	return cacheGlobalBridge<SVGTextContentElementImplConstructor>(exec, "[[svgtextcontentelement.constructor]]");
}

// vim:ts=4:noet
