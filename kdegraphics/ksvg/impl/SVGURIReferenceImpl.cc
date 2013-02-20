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

#include "SVGURIReferenceImpl.h"
#include "SVGAnimatedStringImpl.h"

using namespace KSVG;

#include "SVGURIReferenceImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGURIReferenceImpl::SVGURIReferenceImpl()
{
	KSVG_EMPTY_FLAGS

	m_href = new SVGAnimatedStringImpl();
	m_href->ref();
}

SVGURIReferenceImpl::~SVGURIReferenceImpl()
{
	if(m_href)
		m_href->deref();
}

SVGAnimatedStringImpl *SVGURIReferenceImpl::href() const
{
	return m_href;
}

bool SVGURIReferenceImpl::parseURIReference(const QString &urireference, QString &uri, QString &elementreference)
{
	int seperator = urireference.find("#");

	if(seperator == -1)
		return false;

	uri = urireference.left(seperator);
	elementreference = urireference.mid(seperator + 1);

	return true;
}

bool SVGURIReferenceImpl::isUrl(const QString &url)
{
	QString temp = url.stripWhiteSpace();
	return temp.startsWith("url(#") && temp.endsWith(")");
}

QString SVGURIReferenceImpl::getTarget(const QString &url)
{
	if(url.startsWith("url(")) // URI References, ie. fill:url(#target)
	{
		unsigned int start = url.find("#") + 1;
		unsigned int end = url.findRev(")");

		return url.mid(start, end - start);
	}
	else if(url.find("#") > -1)	// format is #target
	{
		unsigned int start = url.find("#") + 1;

		return url.mid(start, url.length() - start);
	}
	else // Normal Reference, ie. style="color-profile:changeColor"
		return url;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGURIReferenceImpl::s_hashTable 2
 href  SVGURIReferenceImpl::Href DontDelete|ReadOnly
@end
*/

Value SVGURIReferenceImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case Href:
			return m_href->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGURIReferenceImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;

	switch(token)
	{
		case Href:
		{
			if(m_href)
				m_href->deref();

			SVGAnimatedStringImpl *temp = new SVGAnimatedStringImpl();
			temp->ref();
			temp->setBaseVal(value.toString(exec).string());
			m_href = temp;
			break;
		}
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
