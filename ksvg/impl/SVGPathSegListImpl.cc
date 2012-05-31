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

#include "SVGPathSegListImpl.h"

using namespace KSVG;

#include "SVGPathSegListImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

// Ecma stuff

/*
@namespace KSVG
@begin SVGPathSegListImpl::s_hashTable 2
 numberOfItems		SVGListDefs::NumberOfItems		DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGPathSegListImplProto::s_hashTable 11
 getItem			SVGListDefs::GetItem			DontDelete|Function 1
 removeItem			SVGListDefs::RemoveItem			DontDelete|Function 1
 appendItem			SVGListDefs::AppendItem			DontDelete|Function 1
 initialize			SVGListDefs::Initialize			DontDelete|Function 1
 insertItemBefore	SVGListDefs::InsertItemBefore	DontDelete|Function 2
 replaceItem		SVGListDefs::ReplaceItem		DontDelete|Function 2
 clear				SVGListDefs::Clear				DontDelete|Function 0
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGPathSegList", SVGPathSegListImplProto, SVGPathSegListImplProtoFunc)

Value SVGPathSegListImpl::getValueProperty(ExecState *exec, int token) const
{
	return SVGList<SVGPathSegImpl>::getValueProperty(exec, token);
}

Value SVGPathSegListImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGPathSegListImpl)

	return obj->call(exec, static_cast<SVGList<SVGPathSegImpl> *>(obj), args, id);
}

// vim:ts=4:noet
