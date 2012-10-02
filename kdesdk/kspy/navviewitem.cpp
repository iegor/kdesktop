/***************************************************************************
                          navviewitem.cpp  -  description
                             -------------------
    begin                : Tue May 1 2001
    copyright            : (C) 2001 by Richard Moore
    email                : rich@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qobject.h>

#include "navview.h"
#include "navviewitem.h"

NavViewItem::NavViewItem(NavView *parent, QObject *obj )
  : KListViewItem(parent, obj->name(), obj->className() )
{
  data = obj;
  setExpandable( true );
}

NavViewItem::NavViewItem(NavViewItem *parent, QObject *obj )
  : KListViewItem(parent, obj->name(), obj->className() )
{
  data = obj;
  setExpandable( true );
}

NavViewItem::~NavViewItem()
{
}

