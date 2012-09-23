/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/
// $Id: kmyfileitemlist.cpp 465369 2005-09-29 14:33:08Z mueller $

#include "kmyfileitemlist.h"
#include <kfileitem.h>

KMyFileItemList::KMyFileItemList() {}
KMyFileItemList::KMyFileItemList( const QPtrList<KFileItem> & l ) : QPtrList<KFileItem>( l ) {}

KMyFileItemList & KMyFileItemList::operator=( const QPtrList<KFileItem> & l )
{
	return (KMyFileItemList &)QPtrList<KFileItem>::operator=( l );
}

int KMyFileItemList::compareItems( QPtrCollection::Item item1, QPtrCollection::Item item2 )
{
	KFileItem * fileitem1 = static_cast<KFileItem *>( item1 );
	KFileItem * fileitem2 = static_cast<KFileItem *>( item2 );
	if( fileitem1->name() == fileitem2->name() )
		return 0;
	else if( fileitem1->name() > fileitem2->name() )
		return 1;
	else
		return -1;
}
