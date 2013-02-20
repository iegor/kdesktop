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

#ifndef KSVGBridge_H
#define KSVGBridge_H

#include <kdebug.h>

#include <kjs/object.h>
#include <kjs/lookup.h>
#include <kjs/interpreter.h> // for ExecState

namespace KJS
{
	class Value;
	class UString;
	class ExecState;
}

// Base class for all bridges
// The T class must provide prototype(exec), get and hasProperty
// and have a static s_classInfo object
template<class T>
class KSVGBridge : public KJS::ObjectImp
{
public:
	KSVGBridge(KJS::ExecState *exec, T *impl) : KJS::ObjectImp(impl->prototype(exec)), m_impl(impl) { }

	T *impl() const { return m_impl; }

	virtual KJS::Value get(KJS::ExecState *exec, const KJS::Identifier &propertyName) const
	{
		kdDebug(26004) << "KSVGBridge::get(), " << propertyName.qstring() << " Name: " << classInfo()->className << " Object: " << m_impl << endl;

		// Look for standard properties (e.g. those in the hashtables)
		KJS::Value val = m_impl->get(exec, propertyName, this);

		if(val.type() != KJS::UndefinedType)
			return val;

		// Not found -> forward to ObjectImp.
		val = KJS::ObjectImp::get(exec, propertyName);
		if(val.type() == KJS::UndefinedType)
			kdDebug(26004) << "WARNING: " << propertyName.qstring() << " not found in... Name: " << classInfo()->className << " Object: " << m_impl << " on line : " << exec->context().curStmtFirstLine() << endl;
		return val;
	}

	virtual bool hasProperty(KJS::ExecState *exec, const KJS::Identifier &propertyName) const
	{
		kdDebug(26004) << "KSVGBridge::hasProperty(), " << propertyName.qstring() << " Name: " << classInfo()->className << " Object: " << m_impl << endl;

		if(m_impl->hasProperty(exec, propertyName))
			return true;

		return KJS::ObjectImp::hasProperty(exec, propertyName);
	}

	virtual const KJS::ClassInfo *classInfo() const { return &T::s_classInfo; }

protected:
	T *m_impl;
};

// Base class for readwrite bridges
// T must also implement put (use KSVG_PUT in the header file)
template<class T>
class KSVGRWBridge : public KSVGBridge<T>
{
public:
	KSVGRWBridge(KJS::ExecState *exec, T *impl) : KSVGBridge<T>(exec, impl) { }

	virtual void put(KJS::ExecState *exec, const KJS::Identifier &propertyName, const KJS::Value &value, int attr)
	{
//		if(!(attr & KJS::Internal))
//			kdDebug(26004) << "KSVGBridge::put(), " << propertyName.qstring() << " Name: " << classInfo()->className << " Object: " << m_impl << endl;

		// Try to see if we know this property (and need to take special action)
      if(this->m_impl->put(exec, propertyName, value, attr))
			return;

		// We don't -> set property in ObjectImp.
		KJS::ObjectImp::put(exec, propertyName, value, attr);
	}
};

#endif
