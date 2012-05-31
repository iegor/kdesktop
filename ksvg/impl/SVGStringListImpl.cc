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

#include "SVGStringListImpl.h"

using namespace KSVG;

#include "SVGStringListImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

// Ecma stuff

/*
@namespace KSVG
@begin SharedString::s_hashTable 2
 dummy				SharedString::Dummy			DontDelete|ReadOnly
@end
*/

/*
@namespace KSVG
@begin SVGStringListImpl::s_hashTable 2
 numberOfItems		SVGListDefs::NumberOfItems		DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGStringListImplProto::s_hashTable 11
 getItem			SVGListDefs::GetItem			DontDelete|Function 1
 removeItem			SVGListDefs::RemoveItem			DontDelete|Function 1
 appendItem			SVGListDefs::AppendItem			DontDelete|Function 1
 initialize			SVGListDefs::Initialize			DontDelete|Function 1
 insertItemBefore	SVGListDefs::InsertItemBefore	DontDelete|Function 2
 replaceItem		SVGListDefs::ReplaceItem		DontDelete|Function 2
 clear				SVGListDefs::Clear				DontDelete|Function 0
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGStringList", SVGStringListImplProto, SVGStringListImplProtoFunc)

Value SVGStringListImpl::getValueProperty(ExecState *exec, int token) const
{
	return SVGList<SharedString>::getValueProperty(exec, token);
}

Value SVGStringListImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGStringListImpl)

	return obj->call(exec, static_cast<SVGList<SharedString> *>(obj), args, id);
}

QString SVGStringListImpl::join(const QString &seperator) const
{
	SVGStringListImpl *self = const_cast<SVGStringListImpl *>(this);

	QString result;
	
	if(!self->getItem(0))
		return result;
	else
		result += self->getItem(0)->string();
			
	for(unsigned int i = 1; i <= numberOfItems(); i++)
	{
		DOM::DOMString *string = self->getItem(i);

		if(string)
			result += seperator + string->string();
	}

	return result;
}

// vim:ts=4:noet
