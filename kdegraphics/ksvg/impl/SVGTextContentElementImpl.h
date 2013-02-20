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

#ifndef SVGTextContentElementImpl_H
#define SVGTextContentElementImpl_H

#include "ksvg_lookup.h"

#include "SVGShapeImpl.h"
#include "SVGTestsImpl.h"
#include "SVGStylableImpl.h"
#include "SVGLangSpaceImpl.h"
#include "SVGTextContentElement.h"
#include "SVGExternalResourcesRequiredImpl.h"

namespace T2P
{
	class GlyphLayoutParams;
}

namespace KSVG
{

class SVGAnimatedLengthImpl;
class SVGAnimatedEnumerationImpl;
class SVGTextContentElementImpl : public SVGShapeImpl,
								  public SVGTestsImpl,
								  public SVGLangSpaceImpl,
								  public SVGExternalResourcesRequiredImpl,
								  public SVGStylableImpl
{
public:
	SVGTextContentElementImpl(DOM::ElementImpl *impl);
	virtual ~SVGTextContentElementImpl();

	QString textDirectionAwareText();

	virtual T2P::GlyphLayoutParams *layoutParams() const;

	SVGAnimatedLengthImpl *textLength() const;
	SVGAnimatedEnumerationImpl *lengthAdjust() const;

	virtual long getNumberOfChars();
	float getComputedTextLength();

//	float getSubStringLength(const unsigned long &charnum, const unsigned long &nchars);
//	SVGPoint getStartPositionOfChar(const unsigned long &charnum);
//	SVGPoint getEndPositionOfChar(const unsigned long &charnum);
//	SVGRect getExtentOfChar(const unsigned long &charnum);
//	float getRotationOfChar(const unsigned long &charnum);
//	long getCharNumAtPosition(const SVGPoint &point);
//	void selectSubString(const unsigned long &charnum, const unsigned long &nchars);

private:
	SVGAnimatedEnumerationImpl *m_lengthAdjust;
	SVGAnimatedLengthImpl *m_textLength;

public:
	KSVG_GET
	KSVG_PUT

	enum
	{
		// Properties
		TextLength, LengthAdjust,	
		// Functions
		GetTextLength, GetLengthAdjust,
		GetNumberOfChars, GetComputedTextLength,
		GetSubStringLength, GetStartPositionOfChar,
		GetEndPositionOfChar, GetExtentOfChar,
		GetRotationOfChar, GetCharNumAtPosition,
		SelectSubString
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGTextContentElementImplConstructor : public KJS::ObjectImp
{
public:
	SVGTextContentElementImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;
	
	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGTextContentElementImplConstructor(KJS::ExecState *exec);

}

KSVG_DEFINE_PROTOTYPE(SVGTextContentElementImplProto)
KSVG_IMPLEMENT_PROTOFUNC(SVGTextContentElementImplProtoFunc, SVGTextContentElementImpl)

#endif

// vim:ts=4:noet
