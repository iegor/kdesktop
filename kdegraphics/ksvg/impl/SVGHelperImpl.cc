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

#include <kdebug.h>

#include <qregexp.h>

#include "SVGRectImpl.h"
#include "SVGMatrixImpl.h"
#include "SVGHelperImpl.h"
#include "SVGElementImpl.h"
#include "SVGDocumentImpl.h"
#include "SVGStylableImpl.h"
#include "SVGTransformImpl.h"
#include "SVGStringListImpl.h"
#include "SVGSVGElementImpl.h"
#include "SVGTransformListImpl.h"
#include "SVGTransformableImpl.h"
#include "SVGAnimatedLengthImpl.h"
#include "SVGAnimatedLengthListImpl.h"

#include "KSVGCanvas.h"
#include "CanvasItem.h"

using namespace KSVG;

#include "ksvg_ecma.h"
#include "ksvg_window.h"

SVGHelperImpl::SVGHelperImpl()
{
}

SVGHelperImpl::~SVGHelperImpl()
{
}

void SVGHelperImpl::updateItem(KJS::ExecState *exec, const DOM::Node node)
{
	// Get document
	SVGDocumentImpl *doc = Window::retrieveActive(exec)->doc();

	// Update canvas, recursively if needed
	SVGShapeImpl *shape = dynamic_cast<SVGShapeImpl *>(doc->getElementFromHandle(node.handle()));

	if(shape && shape->item())
	{
		shape->item()->update(UPDATE_TRANSFORM);
		shape->item()->draw();
		shape->blit(doc->canvas());
	}

	if(const_cast<DOM::Node &>(node).hasChildNodes())
	{
		DOM::Node iterate = node.firstChild();
		for(; !iterate.isNull(); iterate = iterate.nextSibling())
			updateItem(exec, iterate);
	}
}

void SVGHelperImpl::copyAttributes(SVGElementImpl *src, SVGElementImpl *dst)
{
	QDictIterator<DOM::DOMString> it(src->attributes());
	for(; it.current(); ++it)
	{
		DOM::DOMString name = it.currentKey();
		DOM::DOMString value = it.current()->string();

		if(name != "id" && !dst->hasAttribute(name))
		{
			dst->setAttribute(name, value);
			dst->setAttributeInternal(name, value);
		}
	}
}

void SVGHelperImpl::parseList(SVGStringListImpl *list, char seperator, const QString &data)
{
	// TODO : more error checking/reporting
	list->clear();

	QStringList substrings = QStringList::split(seperator, data);
	QStringList::ConstIterator it = substrings.begin();
	QStringList::ConstIterator end = substrings.end();
	for(; it != end; ++it)
	{
		SharedString *string = new SharedString(*it);
		string->ref();
		
		list->appendItem(string);
	}
}

void SVGHelperImpl::parseLengthList(SVGAnimatedLengthListImpl *list, const QString &lengths, LengthMode mode, SVGElementImpl *object)
{
	// get either comma or space delimited lists
	// TODO : more error checking/reporting
	QStringList sublengths = QStringList::split(QRegExp("[, ]"), lengths);
	QStringList::ConstIterator it = sublengths.begin();
	QStringList::ConstIterator end = sublengths.end();
	
	SVGLengthImpl *lengthImpl = 0;
	for(; it != end; ++it)
	{
		lengthImpl = new SVGLengthImpl(mode, object);
		lengthImpl->ref();
		
		lengthImpl->setValueAsString(*it);		
		list->baseVal()->appendItem(lengthImpl);
	}
}

void SVGHelperImpl::parseCommaSeperatedList(SVGStringListImpl *list, const QString &data)
{
	parseList(list, ',', data);
}

void SVGHelperImpl::parseSemicolonSeperatedList(SVGStringListImpl *list, const QString &data)
{
	parseList(list, ';', data);
}

void SVGHelperImpl::parseTransformAttribute(SVGTransformListImpl *list, const QString &transform)
{
	// Split string for handling 1 transform statement at a time
	QStringList subtransforms = QStringList::split(')', transform);
	QStringList::ConstIterator it = subtransforms.begin();
	QStringList::ConstIterator end = subtransforms.end();
	for(; it != end; ++it)
	{
		QStringList subtransform = QStringList::split('(', (*it));

		subtransform[0] = subtransform[0].stripWhiteSpace().lower();
		subtransform[1] = subtransform[1].simplifyWhiteSpace();
		QRegExp reg("([-]?\\d*\\.?\\d+(?:e[-]?\\d+)?)");

		int pos = 0;
		QStringList params;
		
		while(pos >= 0)
		{
			pos = reg.search(subtransform[1], pos);
			if(pos != -1)
			{
				params += reg.cap(1);
				pos += reg.matchedLength();
			}
		}

		if(subtransform[0].startsWith(";") || subtransform[0].startsWith(","))
			subtransform[0] = subtransform[0].right(subtransform[0].length() - 1);

		SVGTransformImpl *t = SVGSVGElementImpl::createSVGTransform();

		if(subtransform[0] == "rotate")
		{
			if(params.count() == 3)
				t->setRotate(params[0].toDouble(),
							 params[1].toDouble(),
	 						 params[2].toDouble());
			else
				t->setRotate(params[0].toDouble(), 0, 0);
		}
		else if(subtransform[0] == "translate")
		{
			if(params.count() == 2)
				t->setTranslate(params[0].toDouble(), params[1].toDouble());
			else    // Spec : if only one param given, assume 2nd param to be 0
				t->setTranslate(params[0].toDouble(), 0);
		}
		else if(subtransform[0] == "scale")
		{
			if(params.count() == 2)
				t->setScale(params[0].toDouble(), params[1].toDouble());
			else    // Spec : if only one param given, assume uniform scaling
				t->setScale(params[0].toDouble(), params[0].toDouble());
		}
		else if(subtransform[0] == "skewx")
			t->setSkewX(params[0].toDouble());
		else if(subtransform[0] == "skewy")
			t->setSkewY(params[0].toDouble());
		else if(subtransform[0] == "matrix")
		{
			if(params.count() >= 6)
			{
				SVGMatrixImpl *ret = new SVGMatrixImpl(params[0].toDouble(),
													   params[1].toDouble(),
													   params[2].toDouble(),
													   params[3].toDouble(),
													   params[4].toDouble(),
													   params[5].toDouble());
				t->setMatrix(ret);
			}
		}

		list->appendItem(t);
	}
}

/// convert from user space to "real" pixels on rendering area
QRect SVGHelperImpl::fromUserspace(SVGElementImpl *obj, const QRect &r)
{
	QRect sr;

	SVGLocatableImpl *locate = dynamic_cast<SVGLocatableImpl *>(obj);
	
	if(locate)
		sr = locate->screenCTM()->qmatrix().mapRect(r);

	return sr;
}

// vim:ts=4:noet
