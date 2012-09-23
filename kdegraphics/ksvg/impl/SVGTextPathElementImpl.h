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

#ifndef SVGTextPathElementImpl_H
#define SVGTextPathElementImpl_H

#include "SVGURIReferenceImpl.h"
#include "SVGTextContentElementImpl.h"

namespace KSVG
{

class SVGTextPathElementImpl : public SVGTextContentElementImpl,
							   public SVGURIReferenceImpl
{
public:
	SVGTextPathElementImpl(DOM::ElementImpl *);
	virtual ~SVGTextPathElementImpl();

	QString text();

	SVGAnimatedLengthImpl *startOffset() const;
	SVGAnimatedEnumerationImpl *method() const;
	SVGAnimatedEnumerationImpl *spacing() const;

	virtual void setAttributes();
	virtual T2P::GlyphLayoutParams *layoutParams() const;

private:
	SVGAnimatedLengthImpl *m_startOffset;
	SVGAnimatedEnumerationImpl *m_method;
	SVGAnimatedEnumerationImpl *m_spacing;

public:
	KSVG_GET
	KSVG_PUT
	KSVG_BRIDGE

	enum
	{
		// Properties
		StartOffset, Method, Spacing
	};

	KJS::Value getValueProperty(KJS::ExecState *exec, int token) const;
	void putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int attr);
};

class SVGTextPathElementImplConstructor : public KJS::ObjectImp
{
public:
	SVGTextPathElementImplConstructor(KJS::ExecState *) { }
	KJS::Value getValueProperty(KJS::ExecState *, int token) const;
	
	// no put - all read-only
	KSVG_GET
};

KJS::Value getSVGTextPathElementImplConstructor(KJS::ExecState *exec);

KSVG_REGISTER_ELEMENT(SVGTextPathElementImpl, "textPath")

}

#endif

// vim:ts=4:noet
