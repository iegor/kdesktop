/*
    Copyright (C) 2002-2003 KSVG Team
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

#ifndef SVGHelperImpl_H
#define SVGHelperImpl_H

#include <dom/dom_element.h>

#include "SVGElementImpl.h"
#include "SVGLengthImpl.h"
#include "SVGDocumentImpl.h"
#include "ksvg_ecma.h"

#include "ksvg_lookup.h"

class QRect;
class QString;

namespace KSVG
{
class SVGStringListImpl;
class SVGTransformListImpl;
class SVGAnimatedLengthListImpl;

class SVGHelperImpl
{
public:
	SVGHelperImpl();
	~SVGHelperImpl();

	// Dynamic attribute updating
	template<class T>
	static void applyContainer(T *obj, int token, const QString &value)
	{
		SVGElementImpl *element = dynamic_cast<SVGElementImpl *>(obj);
		if(!element || !element->hasChildNodes())
			return;

		// Very fast propagation of attributes, when the token is known! (Niko)
		for(DOM::Node node = element->firstChild(); !node.isNull(); node = node.nextSibling())
		{
			T *cast = dynamic_cast<T *>(element->ownerDoc()->getElementFromHandle(node.handle()));
			if(cast)
				cast->putValueProperty(element->ownerDoc()->ecmaEngine()->globalExec(), token, KJS::String(value), KJS::Internal);
		}
	}

	// Update item on canvas	
	static void updateItem(KJS::ExecState *exec, const DOM::Node node);
	static void copyAttributes(SVGElementImpl *src, SVGElementImpl *dst);

	// SVGAnimatedLengthList
	static void parseLengthList(SVGAnimatedLengthListImpl *list, const QString &lengths, LengthMode mode = LENGTHMODE_UNKNOWN, SVGElementImpl *object = 0);

	// SVGStringList
	static void parseList(SVGStringListImpl *list, char seperator, const QString &data);
	static void parseCommaSeperatedList(SVGStringListImpl *list, const QString &data);
	static void parseSemicolonSeperatedList(SVGStringListImpl *list, const QString &data);

	// SVGTransformList
	static void parseTransformAttribute(SVGTransformListImpl *list, const QString &transform);

	// Tools
	static QRect fromUserspace(SVGElementImpl *, const QRect &);
};

}

#endif

// vim:ts=4:noet
