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

#include "SVGClipPathElement.h"
#include "SVGClipPathElementImpl.h"
#include "KSVGCanvas.h"
#include "SVGDocumentImpl.h"
#include "SVGAnimatedEnumerationImpl.h"

using namespace KSVG;

#include "SVGClipPathElementImpl.lut.h"
#include "ksvg_bridge.h"
#include "ksvg_ecma.h"

SVGClipPathElementImpl::SVGClipPathElementImpl(DOM::ElementImpl *impl) : SVGContainerImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl(), SVGBBoxTarget()
{
	KSVG_EMPTY_FLAGS

	m_clipPathUnits = new SVGAnimatedEnumerationImpl();
	m_clipPathUnits->ref();
	m_clipPathUnits->setBaseVal(SVGClipPathElement::SVG_UNIT_TYPE_UNKNOWN);
}

SVGClipPathElementImpl::~SVGClipPathElementImpl()
{
	if(m_clipPathUnits)
		m_clipPathUnits->deref();
}

SVGAnimatedEnumerationImpl *SVGClipPathElementImpl::clipPathUnits() const
{
	return m_clipPathUnits;
}

/*
@namespace KSVG
@begin SVGClipPathElementImpl::s_hashTable 2
 clipPathUnits	SVGClipPathElementImpl::ClipPathUnits	DontDelete|ReadOnly
@end
*/

Value SVGClipPathElementImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case ClipPathUnits:
			return m_clipPathUnits->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGClipPathElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case ClipPathUnits:
			if(value.toString(exec).qstring() == "objectBoundingBox")
				m_clipPathUnits->setBaseVal(SVGClipPathElement::SVG_UNIT_TYPE_OBJECTBOUNDINGBOX);
			else
				m_clipPathUnits->setBaseVal(SVGClipPathElement::SVG_UNIT_TYPE_USERSPACEONUSE);
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

void SVGClipPathElementImpl::setAttributes()
{
	SVGElementImpl::setAttributes();

	// Spec: if attribute not specified, use userSpaceOnUse
	if(KSVG_TOKEN_NOT_PARSED(ClipPathUnits))
		KSVG_SET_ALT_ATTRIBUTE(ClipPathUnits, "userSpaceOnUse")

	if(!m_item)
		m_item = ownerDoc()->canvas()->createClipPath(this);
}

// vim:ts=4:noet
