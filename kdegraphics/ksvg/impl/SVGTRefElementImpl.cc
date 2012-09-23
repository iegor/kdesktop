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
#include "SVGSVGElementImpl.h"
#include "SVGTRefElementImpl.h"
#include "SVGTextElementImpl.h"
#include "SVGAnimatedStringImpl.h"

#include "KSVGLoader.h"

using namespace KSVG;

SVGTRefElementImpl::SVGTRefElementImpl(DOM::ElementImpl *impl) : SVGTSpanElementImpl(impl), SVGURIReferenceImpl()
{
}

SVGTRefElementImpl::~SVGTRefElementImpl()
{
}

void SVGTRefElementImpl::setAttributes()
{
	SVGTSpanElementImpl::setAttributes();

	DOM::DOMString _href = href()->baseVal();

	if(!_href.isNull())
		href()->setBaseVal(DOM::DOMString(SVGURIReferenceImpl::getTarget(_href.string())));

	// get the text of the referenced element
	QString text;

	QString url = _href.string().stripWhiteSpace(), filename, id;
	if(!SVGURIReferenceImpl::parseURIReference(url, filename, id))
		return;

	if(!filename.isEmpty()) // a reference into another svg
		text = KSVGLoader::getCharacterData(KURL(ownerDoc()->baseUrl().path(), filename), id);
	else
	{
		// a reference to an element in this svg
		SVGElementImpl *target = ownerSVGElement()->getElementById(id);
		SVGTextElementImpl *textTarget = dynamic_cast<SVGTextElementImpl *>(target);
		
		if(textTarget)
			text = textTarget->text();
	}

	text = handleText(text);

	if(!text.isEmpty())
	{
		DOM::Text impl = static_cast<DOM::Document *>(ownerDoc())->createTextNode(text);
		appendChild(impl);
	}
}

// vim:ts=4:noet
