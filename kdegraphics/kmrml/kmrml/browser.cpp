/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "browser.h"
#include "mrml_part.h"

#include <qscrollview.h>

using namespace KMrml;

Browser::Browser( MrmlPart *parent, const char *name )
    : KParts::BrowserExtension( parent, name ),
      m_part( parent )
{

}

Browser::~Browser()
{

}

void Browser::saveState( QDataStream& stream )
{
//     BrowserExtension::saveState( stream );

    m_part->saveState( stream );
}

void Browser::restoreState( QDataStream& stream )
{
//     BrowserExtension::restoreState( stream );
    // ### BrowserExtension::restoreState() calls openURL() at the end (arghh).

    m_part->restoreState( stream );
}

int Browser::xOffset()
{
    return static_cast<QScrollView*>( m_part->widget())->contentsX();
}

int Browser::yOffset()
{
    return static_cast<QScrollView*>( m_part->widget())->contentsY();
}

#include "browser.moc"
