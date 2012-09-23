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

#ifndef KSVGCacheImpl_H
#define KSVGCacheImpl_H

#include "ksvg_scriptinterpreter.h"

// Lookup or create JS object around an existing "DOM Object"
template<class DOMObj, class KJSDOMObj>
inline KJS::Value cacheDOMObject(KJS::ExecState *exec, DOMObj *domObj)
{
   KJS::ObjectImp *ret;
   if(!domObj)
      return KJS::Null();

   KSVGScriptInterpreter *interp = static_cast<KSVGScriptInterpreter *>(exec->interpreter());
   if((ret = interp->getDOMObject(domObj)))
      return KJS::Value(ret);
   else
   {
      ret = new KJSDOMObj(exec, domObj);
      interp->putDOMObject(domObj, ret);
      return KJS::Value(ret);
   }
}

// Lookup or create singleton Impl object, and return a unique bridge object for it.
// (Very much like KJS::cacheGlobalObject, which is for a singleton ObjectImp)
// This one is mostly used for Constructor objects.
template <class ClassCtor>
inline KJS::Object cacheGlobalBridge(KJS::ExecState *exec, const KJS::Identifier &propertyName)
{
   KJS::ValueImp *obj = static_cast<KJS::ObjectImp*>(exec->interpreter()->globalObject().imp())->getDirect(propertyName);
   if(obj)
      return KJS::Object::dynamicCast(KJS::Value(obj));
   else
   {
      ClassCtor* ctor = new ClassCtor(exec); // create the ClassCtor instance
      KJS::Object newObject(new KSVGBridge<ClassCtor>(exec, ctor)); // create the bridge around it
      exec->interpreter()->globalObject().put(exec, propertyName, newObject, KJS::Internal);
      return newObject;
   }
}

#endif

// vim:ts=4:noet
