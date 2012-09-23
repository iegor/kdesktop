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

#include "SVGLangSpaceImpl.h"

using namespace KSVG;

#include "SVGLangSpaceImpl.lut.h"
#include "ksvg_bridge.h"

SVGLangSpaceImpl::SVGLangSpaceImpl()
{
	KSVG_EMPTY_FLAGS

	// Spec: default for xml:space is "default"
	setXmlspace("default");
}

SVGLangSpaceImpl::~SVGLangSpaceImpl()
{
}

void SVGLangSpaceImpl::setXmllang(const DOM::DOMString &xmllang)
{
	m_xmllang = xmllang;
}

DOM::DOMString SVGLangSpaceImpl::xmllang() const
{
	return m_xmllang;
}

void SVGLangSpaceImpl::setXmlspace(const DOM::DOMString &xmlspace)
{
	m_xmlspace = xmlspace;
}

DOM::DOMString SVGLangSpaceImpl::xmlspace() const
{
	return m_xmlspace;
}

QString SVGLangSpaceImpl::handleText(const QString &data) const
{
	QString result = data;

	if(xmlspace() == "preserve")
	{
		// Spec: What to do here?
		//		 It will convert all newline and tab characters into space characters
		result.replace("\n\r", QString(" "));
		result.replace("\r\n", QString(" "));
		result.replace('\t', ' ');
	}
	else if(xmlspace() == "default")
	{
		// Spec: What to do here?
		//		 First, it will remove all newline characters (replace)
		//       Then it will convert all tab characters into space characters  (simplifyWhiteSpace)
		//       Then, it will strip off all leading and trailing space characters (stripWhiteSpace)
		//       Then, all contiguous space characters will be consolidated. (simplifyWhiteSpace)
		result.replace('\n', QString::null);
		result.replace('\r', QString::null);
		result = result.stripWhiteSpace().simplifyWhiteSpace();
	}

	return result;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGLangSpaceImpl::s_hashTable 5
 xmllang		SVGLangSpaceImpl::XmlLang		DontDelete
 xmlspace		SVGLangSpaceImpl::XmlSpace		DontDelete
 lang			SVGLangSpaceImpl::XmlLang		DontDelete
 space			SVGLangSpaceImpl::XmlSpace		DontDelete
@end
*/

Value SVGLangSpaceImpl::getValueProperty(ExecState *, int token) const
{
	switch(token)
	{
		case XmlLang:
			return String(m_xmllang.string());
		case XmlSpace:
			return String(m_xmlspace.string());
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGLangSpaceImpl::putValueProperty(KJS::ExecState *exec, int token, const KJS::Value &value, int /*attr*/)
{
	switch(token)
	{
		case XmlLang:
			m_xmllang = value.toString(exec).string();
			break;
		case XmlSpace:
			m_xmlspace = value.toString(exec).string();
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
