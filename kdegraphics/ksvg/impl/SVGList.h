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
    aint with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef SVGList_H
#define SVGList_H

#include "ksvg_bridge.h"
#include "ksvg_lookup.h"
#include "ksvg_scriptinterpreter.h"
#include <qptrlist.h>
#include <dom/dom_misc.h>

namespace KSVG
{

class SVGListDefs
{
public:
	enum
	{
		// Properties
		NumberOfItems,
		// Functions
		GetItem, RemoveItem, AppendItem, Initialize,
		InsertItemBefore, ReplaceItem, Clear
	};
};
	
template<class T>
class SVGList : public DOM::DomShared
{
public:
	SVGList() { m_impl.setAutoDelete(false); }
	SVGList(const SVGList &other) { *this = other; }
	~SVGList() { clear(); }

	SVGList<T> &operator=(const SVGList<T> &other)
	{
		// Clear own list
		clear();

		// Clone other's elements and append them
		SVGList<T> &get = const_cast<SVGList<T> &>(other);
		for(unsigned int i = 0; i < other.numberOfItems(); i++)
		{
			T *obj = new T(*get.getItem(i));
			obj->ref();

			appendItem(obj);
		}

		return *this;
	}

	unsigned int numberOfItems() const { return m_impl.count(); }

	void clear()
	{
		for(unsigned int i = 0; i < numberOfItems(); i++)
			getItem(i)->deref();

		m_impl.clear();
	}

	T *initialize(T *newItem)
	{
		clear();
		return appendItem(newItem);
	}

	T *getFirst() const { return m_impl.getFirst(); }

	T *getLast() const { return m_impl.getLast(); }

	T *getItem(unsigned int index) { return m_impl.at(index); }
	const T *getItem(unsigned int index) const { return const_cast<SVGList<T> *>(this)->m_impl.at(index); }

	T *insertItemBefore(T *newItem, unsigned int index)
	{
		m_impl.insert(index, newItem);
		return newItem;
	}

	T *replaceItem(T *newItem, unsigned int index)
	{
		m_impl.take(index);
		m_impl.insert(index, newItem);
		return newItem;
	}

	T *removeItem(unsigned int index)
	{
		return m_impl.take(index);
	}

	void removeItem(const T *item)
	{
		m_impl.remove(item);
	}

	T *appendItem(T *newItem)
	{
		m_impl.append(newItem);
		return newItem;
	}

	// Ecma stuff
	KJS::Value getValueProperty(KJS::ExecState *, int token) const
	{
		switch(token)
		{
			case SVGListDefs::NumberOfItems:
				return KJS::Number(numberOfItems());
			default:
				kdWarning() << "Unhandled token in " << k_funcinfo << " : " << token << endl;
				return KJS::Undefined();
		}
	}

	KJS::Value call(KJS::ExecState *exec, SVGList<T> *obj, const KJS::List &args, int id)
	{
		switch(id)
		{
			case SVGListDefs::GetItem:
				return obj->getItem(static_cast<unsigned int>(args[0].toNumber(exec)))->cache(exec);
			case SVGListDefs::RemoveItem:
				return obj->removeItem(static_cast<unsigned int>(args[0].toNumber(exec)))->cache(exec);
			case SVGListDefs::AppendItem:
				return obj->appendItem(static_cast<KSVGBridge<T> *>(args[0].imp())->impl())->cache(exec);
			case SVGListDefs::InsertItemBefore:
				return obj->insertItemBefore(static_cast<KSVGBridge<T> *>(args[0].imp())->impl(), static_cast<unsigned int>(args[1].toNumber(exec)))->cache(exec);
			case SVGListDefs::Initialize:
				return obj->initialize(static_cast<KSVGBridge<T> *>(args[0].imp())->impl())->cache(exec);
			case SVGListDefs::Clear:
			{
				obj->clear();
				return KJS::Undefined();
			}
			default:
				kdWarning() << "Unhandled function id in " << k_funcinfo << " : " << id << endl;
				break;
		}

		return KJS::Undefined();
	}	

private:
	QPtrList<T> m_impl;
};

}

#endif
