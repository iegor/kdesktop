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

#include "SVGAnimatedBooleanImpl.h"
#include "SVGExternalResourcesRequiredImpl.h"

using namespace KSVG;

#include "SVGExternalResourcesRequiredImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

SVGExternalResourcesRequiredImpl::SVGExternalResourcesRequiredImpl()
{
	KSVG_EMPTY_FLAGS

	m_externalResourcesRequired = new SVGAnimatedBooleanImpl();
	m_externalResourcesRequired->ref();
}

SVGExternalResourcesRequiredImpl::~SVGExternalResourcesRequiredImpl()
{
	if(m_externalResourcesRequired)
		m_externalResourcesRequired->deref();
}

SVGAnimatedBooleanImpl *SVGExternalResourcesRequiredImpl::externalResourcesRequired() const
{
	return m_externalResourcesRequired;
}

// Ecma stuff

/*
@namespace KSVG
@begin SVGExternalResourcesRequiredImpl::s_hashTable 2
 externalResourcesRequired  SVGExternalResourcesRequiredImpl::ExternalResourcesRequired DontDelete|ReadOnly
@end
*/

Value SVGExternalResourcesRequiredImpl::getValueProperty(ExecState *exec, int token) const
{
	switch(token)
	{
		case ExternalResourcesRequired:
			return m_externalResourcesRequired->cache(exec);
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
			return Undefined();
	}
}

void SVGExternalResourcesRequiredImpl::putValueProperty(ExecState *exec, int token, const KJS::Value &value, int attr)
{
	// This class has just ReadOnly properties, only with the Internal flag set
	// it's allowed to modify those.
	if(!(attr & KJS::Internal))
		return;
	switch(token)
	{
		case ExternalResourcesRequired:
			m_externalResourcesRequired->setBaseVal(value.toBoolean(exec));
			break;
		default:
			kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
	}
}

// vim:ts=4:noet
