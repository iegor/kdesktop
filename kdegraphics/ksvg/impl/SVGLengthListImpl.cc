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

#include "SVGLengthListImpl.h"

using namespace KSVG;

#include "SVGLengthListImpl.lut.h"
#include "ksvg_scriptinterpreter.h"
#include "ksvg_bridge.h"

// Ecma stuff

/*
@namespace KSVG
@begin SVGLengthListImpl::s_hashTable 2
 numberOfItems		SVGListDefs::NumberOfItems		DontDelete|ReadOnly
@end
@namespace KSVG
@begin SVGLengthListImplProto::s_hashTable 11
 getItem			SVGListDefs::GetItem			DontDelete|Function 1
 removeItem			SVGListDefs::RemoveItem			DontDelete|Function 1
 appendItem			SVGListDefs::AppendItem			DontDelete|Function 1
 initialize			SVGListDefs::Initialize			DontDelete|Function 1
 insertItemBefore	SVGListDefs::InsertItemBefore	DontDelete|Function 2
 replaceItem		SVGListDefs::ReplaceItem		DontDelete|Function 2
 clear				SVGListDefs::Clear				DontDelete|Function 0
@end
*/

KSVG_IMPLEMENT_PROTOTYPE("SVGLengthList", SVGLengthListImplProto, SVGLengthListImplProtoFunc)

Value SVGLengthListImpl::getValueProperty(ExecState *exec, int token) const
{
	return SVGList<SVGLengthImpl>::getValueProperty(exec, token);
}

Value SVGLengthListImplProtoFunc::call(ExecState *exec, Object &thisObj, const List &args)
{
	KSVG_CHECK_THIS(SVGLengthListImpl)

	return obj->call(exec, static_cast<SVGList<SVGLengthImpl> *>(obj), args, id);
}

// vim:ts=4:noet
