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

#include <klocale.h>
#include <kglobal.h>

#include "SVGTestsImpl.h"
#include "SVGStringListImpl.h"

using namespace KSVG;

#include "SVGTestsImpl.lut.h"

SVGTestsImpl::SVGTestsImpl()
{
	KSVG_EMPTY_FLAGS

	m_requiredFeatures = new SVGStringListImpl();
	m_requiredFeatures->ref();

	m_requiredExtensions = new SVGStringListImpl();
	m_requiredExtensions->ref();

	m_systemLanguage = new SVGStringListImpl();
	m_systemLanguage->ref();
}

SVGTestsImpl::~SVGTestsImpl()
{
	if(m_requiredFeatures)
		m_requiredFeatures->deref();
	if(m_requiredExtensions)
		m_requiredExtensions->deref();
	if(m_systemLanguage)
		m_systemLanguage->deref();
}

void SVGTestsImpl::parseRequiredFeatures(const QString &/*value*/)
{
	// FIXME
}

void SVGTestsImpl::parseRequiredExtensions(const QString &value)
{
	m_requiredExtensions->appendItem(new SharedString(value));
}

void SVGTestsImpl::parseSystemLanguage(const QString &value)
{
	m_systemLanguage->appendItem(new SharedString(value));
}

SVGStringListImpl *SVGTestsImpl::requiredFeatures() const
{
	return m_requiredFeatures;
}

SVGStringListImpl *SVGTestsImpl::requiredExtensions() const
{
	return m_requiredExtensions;
}

SVGStringListImpl *SVGTestsImpl::systemLanguage() const
{
	return m_systemLanguage;
}

bool SVGTestsImpl::ok()
{
	for(unsigned int i = 0;i < m_requiredExtensions->numberOfItems();i++)
	{
		// FIXME
		return false;
	}
	for(unsigned int i = 0;i < m_systemLanguage->numberOfItems();i++)
	{
		QString value = m_systemLanguage->getItem(i)->string();
		if(value.isEmpty() || value != (KGlobal::locale()->language()).left(2))
			return false;
	}
	return true;
}

bool SVGTestsImpl::hasExtension(const DOM::DOMString &/*extension*/)
{
	return false;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGTestsImpl::s_hashTable 5
 requiredFeatures	SVGTestsImpl::RequiredFeatures		DontDelete|ReadOnly
 requiredExtensions	SVGTestsImpl::RequiredExtensions	DontDelete|ReadOnly
 systemLanguage		SVGTestsImpl::SystemLanguage		DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGTestsImplProto::s_hashTable 2
 hasExtension		SVGTestsImpl::HasExtension			DontDelete|Function 1
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGTests", SVGTestsImplProto, SVGTestsImplProtoFunc)

Value SVGTestsImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case RequiredFeatures:
			return m_requiredFeatures->cache(exec);
		case RequiredExtensions:
			return m_requiredExtensions->cache(exec);
		case SystemLanguage:
			return m_systemLanguage->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGTestsImpl::putValueProperty(ExecState *exec, int token, const Value &value, int attr)
{
    // This class has just ReadOnly properties, only with the Internal flag set
    // it's allowed to modify those.
    if(!(attr & KJS::Internal))
        return;

    switch(token)
    {
        case RequiredFeatures:
			parseRequiredFeatures(value.toString(exec).qstring());
            break;
        case RequiredExtensions:
			parseRequiredExtensions(value.toString(exec).qstring());
            break;
        case SystemLanguage:
			parseSystemLanguage(value.toString(exec).qstring());
            break;
        default:
            kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
    }
}

Value SVGTestsImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGTestsImpl)

	switch(id)
	{
		case SVGTestsImpl::HasExtension:
			return Boolean(obj->hasExtension(args[0].toString(exec).string()));
		default:
			kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
			break;
	}

	return Undefined();
}

// vim:ts=4:noet
