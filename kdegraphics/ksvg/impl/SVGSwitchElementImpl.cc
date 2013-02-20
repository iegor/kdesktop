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

#include "SVGDocumentImpl.h"
#include "SVGSwitchElementImpl.h"
#include "KSVGCanvas.h"

using namespace KSVG;

SVGSwitchElementImpl::SVGSwitchElementImpl(DOM::ElementImpl *impl) : SVGContainerImpl(impl), SVGTestsImpl(), SVGLangSpaceImpl(), SVGExternalResourcesRequiredImpl(), SVGStylableImpl(this), SVGTransformableImpl()
{
}

SVGSwitchElementImpl::~SVGSwitchElementImpl()
{
}

void SVGSwitchElementImpl::createItem(KSVGCanvas *c)
{
	if(!c)
		c = ownerDoc()->canvas();

	DOM::Node node = firstChild();
	for(; !node.isNull(); node = node.nextSibling())
	{
		SVGElementImpl *element = ownerDoc()->getElementFromHandle(node.handle());
		SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(element);
		SVGTestsImpl *tests = dynamic_cast<SVGTestsImpl *>(element);
		SVGStylableImpl *style = dynamic_cast<SVGStylableImpl *>(element);

		bool ok = tests ? tests->ok() : true;

		if(element && shape && ok && style->getVisible() && style->getDisplay() && (shape->directRender() || !directRender()))
		{
			element->createItem(c);
			break;
		}
	}
}

// vim:ts=4:noet
