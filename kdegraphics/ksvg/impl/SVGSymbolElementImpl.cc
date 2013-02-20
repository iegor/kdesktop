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

#include "SVGRectImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGAnimatedRectImpl.h"
#include "SVGSymbolElementImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGPreserveAspectRatioImpl.h"
#include "SVGAnimatedPreserveAspectRatioImpl.h"
#include "KSVGCanvas.h"

using namespace KSVG;

#include "SVGSymbolElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"

SVGSymbolElementImpl::SVGSymbolElementImpl(DOM::ElementImpl *impl) : SVGShapeImpl(impl), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGFitToViewBoxImpl()
{
	KSVG_EMPTY_FLAGS

	m_width = new SVGAnimatedLengthImpl(LENGTHMODE_WIDTH, this);
	m_width->ref();

	m_height = new SVGAnimatedLengthImpl(LENGTHMODE_HEIGHT, this);
	m_height->ref();
}

SVGSymbolElementImpl::~SVGSymbolElementImpl()
{
	if(m_width)
		m_width->deref();
	if(m_height)
		m_height->deref();
}
/*
@namespace KSVG
@begin SVGSymbolElementImpl::s_hashTable 3
 width	SVGSymbolElementImpl::Width		DontDelete|ReadOnly
 height	SVGSymbolElementImpl::Height	DontDelete|ReadOnly
@end
*/

Value SVGSymbolElementImpl::getValueProperty(ExecState *exec, int token) const
{
	KSVG_CHECK_ATTRIBUTE

	switch(token)
	{
		case Width:
			if(!attributeMode)
				return m_width->cache(exec);
			else
				return Number(m_width->baseVal()->value());
		case Height:
			if(!attributeMode)
				return m_height->cache(exec);
			else
				return Number(m_height->baseVal()->value());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGSymbolElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case Width:
			m_width->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		case Height:
			m_height->baseVal()->setValueAsString(value.toString(exec).qstring());
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
    }
}

// vim:ts=4:noet
