/*
    packagelvi.cpp  -  Custom QListViewItem that holds a Package object
    
    copyright   : (c) 2001 by Martijn Klingens
    email       : klingens@kde.org

    *************************************************************************
    *                                                                       *
    * This program is free software; you can redistribute it and/or modify  *
    * it under the terms of the GNU General Public License as published by  *
    * the Free Software Foundation; either version 2 of the License, or     *
    * (at your option) any later version.                                   *
    *                                                                       *
    *************************************************************************
*/

#include "packagelvi.h"

PackageLVI::PackageLVI( QListView *parent , const Package &pkg, const QString &component )
: QListViewItem( parent, pkg.name(), pkg.description() )
{
    m_package = pkg;
    m_component = component;
}

PackageLVI::PackageLVI( QListViewItem *parent , const Package &pkg, const QString &component )
: QListViewItem( parent, component )
{
    m_package = pkg;
    m_component = component;
}

PackageLVI::~PackageLVI()
{
}

/* vim: set et ts=4 sw=4 softtabstop=4: */

