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

#ifndef KSVG_LOOKUP_H
#define KSVG_LOOKUP_H

#include <kjs/object.h>
#include <kjs/lookup.h>
#include <kjs/interpreter.h> // for ExecState

#include "ksvg_bridge.h"
#include "ksvg_scriptinterpreter.h"

#define KSVG_GET_COMMON \
public: \
    \
    /* The standard hasProperty call, auto-generated. Looks in hashtable, forwards to parents. */ \
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier &propertyName) const; \
    \
    /* get() method, called by KSVGBridge::get */ \
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier &propertyName, const KJS::ObjectImp* bridge) const; \
    \
    /* Called by lookupGet(). Auto-generated. Forwards to the parent which has the given property. */ \
    KJS::Value getInParents(KJS::ExecState *exec, const KJS::Identifier &propertyName, const KJS::ObjectImp* bridge) const; \
    \
    KJS::Object prototype(KJS::ExecState *exec) const;\
    \
    static const KJS::ClassInfo s_classInfo; \
    \
    static const struct KJS::HashTable s_hashTable; \
	\
	int m_attrFlags;

// For classes with properties to read, and a hashtable.
#define KSVG_GET \
    KSVG_GET_COMMON \
    KJS::Value cache(KJS::ExecState *exec) const;

// Same thing, for base classes (kalyptus helps finding them)
// The difference is that cache() is virtual
#define KSVG_BASECLASS_GET \
    KSVG_GET_COMMON \
    virtual KJS::Value cache(KJS::ExecState *exec) const;

// For classes without properties, but with a parent class to forward to
#define KSVG_FORWARDGET \
public: \
    \
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier &propertyName) const; \
    \
    /* will have the code for getInParents */ \
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier &propertyName, const KJS::ObjectImp* bridge) const; \
    \
    KJS::Object prototype(KJS::ExecState *exec) const;\
    \
    static const KJS::ClassInfo s_classInfo; \
    \
    KJS::Value cache(KJS::ExecState *exec) const;

// For read-write classes only, i.e. those which support put()
#define KSVG_PUT \
    \
    /* put() method, called by KSVGBridge::put */ \
    bool put(KJS::ExecState *exec, const KJS::Identifier &propertyName, const KJS::Value &value, int attr); \
	\
    /* Called by lookupPut. Auto-generated. Looks in hashtable, forwards to parents. */ \
    bool putInParents(KJS::ExecState *exec, const KJS::Identifier &propertyName, const KJS::Value &value, int attr);

// For classes which inherit a read-write class, but have no readwrite property themselves
#define KSVG_FORWARDPUT \
    \
    /* put() method, called by KSVGBridge::put */ \
    bool put(KJS::ExecState *exec, const KJS::Identifier &propertyName, const KJS::Value &value, int attr);

// For classes which need to be accessable with getElementById -> elements
#define KSVG_NO_TAG_BRIDGE \
public: \
	KJS::ObjectImp *bridge(KJS::ExecState *) const { return 0; }

#define KSVG_BRIDGE \
public: \
	KJS::ObjectImp *bridge(KJS::ExecState *) const; \
	virtual DOM::DOMString tagName() const { return s_tagName; } \
	static const DOM::DOMString s_tagName;

// Fast setting of default values, if the token is known
// Note: this is actually misnamed it should be KSVG_SET_DEFAULT_ATTRIBUTE
#define KSVG_SET_ALT_ATTRIBUTE(Token, Name) putValueProperty(ownerDoc()->ecmaEngine()->globalExec(), Token, String(Name), Internal);

// Check if attribute has not been parsed, if the token is known
#define KSVG_TOKEN_NOT_PARSED_ELEMENT(Token, Element) (~Element->m_attrFlags & (1 << Token))
#define KSVG_TOKEN_NOT_PARSED(Token) KSVG_TOKEN_NOT_PARSED_ELEMENT(Token, this)

// Checks if the interpreter is in attribute "getting" mode
#define KSVG_CHECK_ATTRIBUTE bool attributeMode = static_cast<KSVGScriptInterpreter *>(exec->interpreter())->attributeGetMode();

// Sets the class specific flags to a ZERO value
#define KSVG_EMPTY_FLAGS m_attrFlags = 0;

// to be used in generatedata.cpp
// GET p1=exec, p2=propertyName, p3=bridge
// PUT p1=exec, p2=propertyName, p3=value, p4=attr
#define GET_METHOD_ARGS KJS::ExecState *p1, const KJS::Identifier &p2, const KJS::ObjectImp *p3
#define PUT_METHOD_ARGS KJS::ExecState *p1, const KJS::Identifier &p2, const KJS::Value &p3, int p4

namespace KSVG
{
	/**
	 * Helper method for property lookups
	 *
	 * This method does it all (looking in the hashtable, checking for function
	 * overrides, creating the function or retrieving from cache, calling
	 * getValueProperty in case of a non-function property, forwarding to parent[s] if
	 * unknown property).
	 *
	 * Template arguments:
	 * @param FuncImp the class which implements this object's functions
	 * @param ThisImp the class of "this". It must implement the getValueProperty(exec,token) method,
	 * for non-function properties, and the getInParents() method (auto-generated).
	 *
	 * Method arguments:
	 * @param exec execution state, as usual
	 * @param propertyName the property we're looking for
	 * @param table the static hashtable for this class
	 * @param thisObj "this"
	 */
	template<class FuncImp, class ThisImp>
	inline KJS::Value lookupGet(KJS::ExecState *exec,
								const KJS::Identifier &propertyName,
								const KJS::HashTable *table,
								const ThisImp *thisObj, // the 'impl' object
								const KJS::ObjectImp *bridge)
	{
		const KJS::HashEntry *entry = KJS::Lookup::findEntry(table, propertyName);

		if(!entry) // not found, forward to parents
			return thisObj->getInParents(exec, propertyName, bridge);

		if(entry->attr & KJS::Function)
			return KJS::lookupOrCreateFunction<FuncImp>(exec, propertyName,
														const_cast<KJS::ObjectImp *>(bridge),
														entry->value, entry->params, entry->attr);

		return thisObj->getValueProperty(exec, entry->value);
    }

	/**
	 * Simplified version of lookupGet in case there are no functions, only "values".
	 * Using this instead of lookupGet removes the need for a FuncImp class.
	 */
	template <class ThisImp>
    inline KJS::Value lookupGetValue(KJS::ExecState *exec,
									 const KJS::Identifier &propertyName,
									 const KJS::HashTable *table,
									 const ThisImp *thisObj, // the 'impl' object
                                     const KJS::ObjectImp *bridge)
	{
		const KJS::HashEntry *entry = KJS::Lookup::findEntry(table, propertyName);

		if(!entry) // not found, forward to parents
			return thisObj->getInParents(exec, propertyName, bridge);

		if(entry->attr & KJS::Function)
			kdError(26004) << "Function bit set! Shouldn't happen in lookupGetValue! propertyName was " << propertyName.qstring() << endl;

		return thisObj->getValueProperty(exec, entry->value);
	}

    /**
	 * This one is for "put".
	 * Lookup hash entry for property to be set, and set the value.
	 * The "this" class must implement putValueProperty.
	 * If it returns false, put() will return false, and KSVGRequest will set a dynamic property in ObjectImp
	 */
	template <class ThisImp>
	inline bool lookupPut(KJS::ExecState *exec,
						  const KJS::Identifier &propertyName,
						  const KJS::Value &value,
						  int attr,
						  const KJS::HashTable *table,
						  ThisImp *thisObj)
    {
		const KJS::HashEntry *entry = KJS::Lookup::findEntry(table, propertyName);

		if(!entry) // not found, forward to parents
			return thisObj->putInParents(exec, propertyName, value, attr);
		else if(entry->attr & KJS::Function) // Function: put as override property
			return false;
		else if(entry->attr & KJS::ReadOnly && !(attr & KJS::Internal)) // readonly! Can't put!
		{
#ifdef KJS_VERBOSE
			kdWarning(26004) <<" Attempt to change value of readonly property '" << propertyName.qstring() << "'" << endl;
#endif
			return true; // "we did it" -> don't put override property
		}
		else
		{
			if(static_cast<KSVGScriptInterpreter *>(exec->interpreter())->attributeSetMode())
				thisObj->m_attrFlags |= (1 << entry->value);

			thisObj->putValueProperty(exec, entry->value, value, attr);
			return true;
		}
	}
}

// Same as kjs' DEFINE_PROTOTYPE, but with a pointer to the hashtable too, and no ClassName here
// The ClassProto ctor(exec) must be public, so we can use KJS::cacheGlobalObject... (Niko)
#define KSVG_DEFINE_PROTOTYPE(ClassProto) \
  namespace KSVG { \
  class ClassProto : public KJS::ObjectImp { \
  public: \
    static KJS::Object self(KJS::ExecState *exec); \
    ClassProto( KJS::ExecState *exec ) \
      : KJS::ObjectImp( exec->interpreter()->builtinObjectPrototype() ) {} \
    virtual const KJS::ClassInfo *classInfo() const { return &info; } \
    static const KJS::ClassInfo info; \
    KJS::Value get(KJS::ExecState *exec, const KJS::Identifier &propertyName) const; \
    bool hasProperty(KJS::ExecState *exec, const KJS::Identifier &propertyName) const; \
    \
    static const struct KJS::HashTable s_hashTable; \
  }; \
  }

// same as IMPLEMENT_PROTOTYPE but in the KSVG namespace, and with ClassName here
// so that KSVG_DEFINE_PROTOTYPE can be put in a header file ('info' defined here)
#define KSVG_IMPLEMENT_PROTOTYPE(ClassName,ClassProto,ClassFunc) \
    KJS::Value KSVG::ClassProto::get(KJS::ExecState *exec, const KJS::Identifier &propertyName) const \
    { \
      return lookupGetFunction<ClassFunc,KJS::ObjectImp>(exec, propertyName, &s_hashTable, this ); \
    } \
    bool KSVG::ClassProto::hasProperty(KJS::ExecState *exec, const KJS::Identifier &propertyName) const \
    { /*stupid but we need this to have a common macro for the declaration*/ \
      return KJS::ObjectImp::hasProperty(exec, propertyName); \
    } \
    KJS::Object KSVG::ClassProto::self(KJS::ExecState *exec) \
    { \
      return KJS::cacheGlobalObject<ClassProto>( exec, "[[" ClassName ".prototype]]" ); \
    } \
    const KJS::ClassInfo ClassProto::info = { ClassName, 0, &s_hashTable, 0 }; \

// same as KSVG_IMPLEMENT_PROTOTYPE but with a parent class to forward calls to
// Not used within KSVG up to now - each class does a self proto lookup in generateddata.cpp
#define KSVG_IMPLEMENT_PROTOTYPE_WITH_PARENT(ClassName,ClassProto,ClassFunc,ParentProto) \
    KJS::Value KSVG::ClassProto::get(KJS::ExecState *exec, const KJS::Identifier &propertyName) const \
    { \
      KJS::Value val = lookupGetFunction<ClassFunc,KJS::ObjectImp>(exec, propertyName, &s_hashTable, this ); \
      if ( val.type() != UndefinedType ) return val; \
      /* Not found -> forward request to "parent" prototype */ \
      return ParentProto::self(exec).get( exec, propertyName ); \
    } \
    bool KSVG::ClassProto::hasProperty(KJS::ExecState *exec, const KJS::Identifier &propertyName) const \
    { \
      if (KJS::ObjectImp::hasProperty(exec, propertyName)) \
        return true; \
      return ParentProto::self(exec).hasProperty(exec, propertyName); \
    } \
    KJS::Object KSVG::ClassProto::self(KJS::ExecState *exec) \
    { \
      return KJS::cacheGlobalObject<ClassProto>( exec, "[[" ClassName ".prototype]]" ); \
    } \
    const KJS::ClassInfo ClassProto::info = { ClassName, 0, &s_hashTable, 0 }; \

#define KSVG_IMPLEMENT_PROTOFUNC(ClassFunc,Class) \
  namespace KSVG { \
  class ClassFunc : public KJS::ObjectImp { \
  public: \
    ClassFunc(KJS::ExecState *exec, int i, int len) \
       : KJS::ObjectImp( /*proto? */ ), id(i) { \
       KJS::Value protect(this); \
       put(exec,"length",KJS::Number(len),KJS::DontDelete|KJS::ReadOnly|KJS::DontEnum); \
    } \
    /** Used by call() to check the type of thisObj. Generated code */ \
    Class * cast(const KJS::ObjectImp* bridge) const; \
    \
    virtual bool implementsCall() const { return true; } \
    /** You need to implement that one */ \
    virtual KJS::Value call(KJS::ExecState *exec, KJS::Object &thisObj, const KJS::List &args); \
  private: \
    int id; \
  }; \
  }

// To be used when casting the type of an argument
#define KSVG_CHECK(ClassName, theObj) \
    ClassName* obj = cast(static_cast<KJS::ObjectImp*>(theObj.imp())); \
    if (!obj) { \
        kdDebug(26004) << k_funcinfo << " Wrong object type: expected " << ClassName::s_classInfo.className << " got " << thisObj.classInfo()->className << endl; \
        Object err = Error::create(exec,TypeError);   \
        exec->setException(err);                      \
        return err;                                   \
    }

// To be used in all call() implementations!
// Can't use if (!thisObj.inherits(&ClassName::s_classInfo) since we don't
// use the (single-parent) inheritance of ClassInfo...
#define KSVG_CHECK_THIS(ClassName) KSVG_CHECK(ClassName, thisObj)

#endif

// vim:ts=4:noet
