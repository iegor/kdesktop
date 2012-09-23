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

#include "SVGDocumentImpl.h"
#include "SVGTextPathElement.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGTextPathElementImpl.h"
#include "SVGAnimatedEnumerationImpl.h"

using namespace KSVG;

#include "SVGTextPathElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_ecma.h"
#include "ksvg_cacheimpl.h"

SVGTextPathElementImpl::SVGTextPathElementImpl(DOM::ElementImpl *impl) : SVGTextContentElementImpl(impl), SVGURIReferenceImpl()
{
	KSVG_EMPTY_FLAGS

	m_startOffset = new SVGAnimatedLengthImpl();
	m_startOffset->ref();

	m_method = new SVGAnimatedEnumerationImpl();
	m_method->ref();

	m_spacing = new SVGAnimatedEnumerationImpl();
	m_spacing->ref();
}

SVGTextPathElementImpl::~SVGTextPathElementImpl()
{
	if(m_startOffset)
		m_startOffset->deref();
	if(m_method)
		m_method->deref();
	if(m_spacing)
		m_spacing->deref();
}

SVGAnimatedLengthImpl *SVGTextPathElementImpl::startOffset() const
{
	return m_startOffset;
}

SVGAnimatedEnumerationImpl *SVGTextPathElementImpl::method() const
{
	return m_method;
}

SVGAnimatedEnumerationImpl *SVGTextPathElementImpl::spacing() const
{
	return m_spacing;
}

QString SVGTextPathElementImpl::text()
{
	// Otherwhise some js scripts which require a child, don't work (Niko)
	if(!hasChildNodes())
	{
		DOM::Text impl = static_cast<DOM::Document *>(ownerDoc())->createTextNode(DOM::DOMString(""));
		appendChild(impl);
	}

	return textDirectionAwareText();
}

void SVGTextPathElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: if not specified, effect is as if a value of "0" were specified
	if(KSVG_TOKEN_NOT_PARSED(StartOffset))
		KSVG_SET_ALT_ATTRIBUTE(StartOffset, "0")

	// Spec: if not specified, effect is as if a value of "align" were specified
	if(KSVG_TOKEN_NOT_PARSED(Method))
		KSVG_SET_ALT_ATTRIBUTE(Method, "align")

	// Spec: if not specified, effect is as if a value of "exact" were specified
	if(KSVG_TOKEN_NOT_PARSED(Spacing))
		KSVG_SET_ALT_ATTRIBUTE(Spacing, "exact")
}

T2P::GlyphLayoutParams *SVGTextPathElementImpl::layoutParams() const
{
	T2P::GlyphLayoutParams *params = SVGTextContentElementImpl::layoutParams();
	params->setTextPathStartOffset(startOffset()->baseVal()->value());
	return params;
}
	

// Ecma stuff

/*
@namespace KSVG
@begin SVGTextPathElementImpl::s_hashTable 5
 startOffset	SVGTextPathElementImpl::StartOffset	DontDelete|ReadOnly
 method	SVGTextPathElementImpl::Method	DontDelete|ReadOnly
 spacing	SVGTextPathElementImpl::Spacing	DontDelete|ReadOnly
@end
*/

Value SVGTextPathElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE

	switch(token)
	{
		case StartOffset:
			if(!attributeMode)
				return m_startOffset->cache(exec);
			else
				return Number(m_startOffset->baseVal()->value());
		case Method:
			if(!attributeMode)
				return m_method->cache(exec);
			else
				return Number(m_method->baseVal());
		case Spacing:
			if(!attributeMode)
				return m_spacing->cache(exec);
			else
				return Number(m_spacing->baseVal());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGTextPathElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case StartOffset:
		{
			QString param = value.toString(exec).qstring();

			if(param.endsWith("%"))
			{
				QString value = param.left(param.length() - 1);
				bool ok = false;
				double dValue = value.toDouble(&ok);
				if(ok)
					startOffset()->baseVal()->setValue(dValue / 100.0);
				else
					kdDebug() << "Couldn't parse startOffset: " << value << endl;
			}
			else
				startOffset()->baseVal()->setValueAsString(value.toString(exec).qstring());
				
			if(startOffset()->baseVal()->value() < 0) // A negative value is an error
				gotError(i18n("Negative value for attribute startOffset of element <textPath> is illegal"));
			break;
		}
		case Method:
		{
			QString param = value.toString(exec).qstring();

			if(param == "align")
				method()->setBaseVal(TEXTPATH_METHODTYPE_ALIGN);
			else if(param == "stretch")
				method()->setBaseVal(TEXTPATH_METHODTYPE_STRETCH);
			else
				method()->setBaseVal(TEXTPATH_METHODTYPE_UNKNOWN);

			break;
		}
		case Spacing:
		{
			QString param = value.toString(exec).qstring();

			if(param == "auto")
				spacing()->setBaseVal(TEXTPATH_SPACINGTYPE_AUTO);
			else if(param == "exact")
				spacing()->setBaseVal(TEXTPATH_SPACINGTYPE_EXACT);
			else
				spacing()->setBaseVal(TEXTPATH_SPACINGTYPE_UNKNOWN);

			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// CONSTANTS

/*
@namespace KSVG
@begin SVGTextPathElementImplConstructor::s_hashTable 7
 TEXTPATH_METHODTYPE_UNKNOWN		KSVG::TEXTPATH_METHODTYPE_UNKNOWN	DontDelete|ReadOnly
 TEXTPATH_METHODTYPE_ALIGN	KSVG::TEXTPATH_METHODTYPE_ALIGN	DontDelete|ReadOnly
 TEXTPATH_METHODTYPE_STRETCH	KSVG::TEXTPATH_METHODTYPE_STRETCH	DontDelete|ReadOnly
 TEXTPATH_SPACINGTYPE_UNKNOWN	KSVG::TEXTPATH_SPACINGTYPE_UNKNOWN	DontDelete|ReadOnly
 TEXTPATH_SPACINGTYPE_AUTO	KSVG::TEXTPATH_SPACINGTYPE_AUTO	DontDelete|ReadOnly
 TEXTPATH_SPACINGTYPE_EXACT	KSVG::TEXTPATH_SPACINGTYPE_EXACT	DontDelete|ReadOnly
@end
*/

using namespace KJS;

Value SVGTextPathElementImplConstructor::getValueProperty(ExecState *, int token) const
{
    return Number(token);
}

Value KSVG::getSVGTextPathElementImplConstructor(ExecState *exec)
{
    return cacheGlobalBridge<SVGTextPathElementImplConstructor>(exec, "[[svgtextpathelement.constructor]]");
}

// vim:ts=4:noet
