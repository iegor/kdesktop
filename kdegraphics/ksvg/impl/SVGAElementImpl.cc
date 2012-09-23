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

#include "SVGAElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGAnimatedStringImpl.h"

using namespace KSVG;

#include "SVGAElementImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGAElementImpl::SVGAElementImpl(DOM::ElementImpl *impl) : SVGContainerImpl(impl), SVGURIReferenceImpl(), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl()
{
	KSVG_EMPTY_FLAGS

	m_target = new SVGAnimatedStringImpl();
	m_target->ref();
}

SVGAElementImpl::~SVGAElementImpl()
{
	if(m_target)
		m_target->deref();
}

/*
@namespace KSVG
@begin SVGAElementImpl::s_hashTable 2
 target	SVGAElementImpl::Target	DontDelete|ReadOnly
@end
*/

Value SVGAElementImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case Target:
			return m_target->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGAElementImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case Target:
		{
			if(m_target)
				m_target->deref();

			SVGAnimatedStringImpl *temp = new SVGAnimatedStringImpl();
			temp->ref();
			temp->setBaseVal(value.toString(exec).string());
			setTarget(temp);
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

void SVGAElementImpl::setTarget(SVGAnimatedStringImpl *target)
{
	m_target = target;
}

SVGAnimatedStringImpl *SVGAElementImpl::target() const
{
	return m_target;
}

SVGAElementImpl *SVGAElementImpl::getLink(SVGElementImpl *sourceElem)
{
	for(DOM::Node node = *sourceElem; !node.isNull(); node = node.parentNode())
	{
		SVGElementImpl *elem = sourceElem->ownerDoc()->getElementFromHandle(node.handle());
		if(elem)
		{
			SVGAElementImpl *link = dynamic_cast<SVGAElementImpl *>(elem);
			if(link)
				return link;
		}
	}

	return false;
}

// vim:ts=4:noet
