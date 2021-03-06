/* This file is part of the KDE project
   Copyright (C) 2000 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "bookmark_item.h"
#include <kbookmarkdrag.h>
#include <konq_sidebartree.h>
#include <kdebug.h>
#include <kiconloader.h>

#include "bookmark_module.h"

#define MYMODULE static_cast<KonqSidebarBookmarkModule*>(module())

KonqSidebarBookmarkItem::KonqSidebarBookmarkItem( KonqSidebarTreeItem *parentItem, KonqSidebarTreeTopLevelItem *topLevelItem, const KBookmark & bk, int key )
    : KonqSidebarTreeItem( parentItem, topLevelItem ), m_bk(bk), m_key(key)
{
    setText( 0, bk.text() );
    setPixmap( 0, SmallIcon(bk.icon()) );
}

QDragObject * KonqSidebarBookmarkItem::dragObject( QWidget * parent, bool )
{
    KBookmarkDrag * drag = KBookmarkDrag::newDrag( m_bk, parent );
    // TODO honour bool move ?
    return drag;
}

void KonqSidebarBookmarkItem::middleButtonClicked()
{
    emit tree()->createNewWindow( externalURL() );
}

void KonqSidebarBookmarkItem::rightButtonPressed()
{
    MYMODULE->showPopupMenu();
}

void KonqSidebarBookmarkItem::del()
{
    //maybe todo
}

KURL KonqSidebarBookmarkItem::externalURL() const
{
    return m_bk.isGroup() ? KURL() : m_bk.url();
}

QString KonqSidebarBookmarkItem::toolTipText() const
{
    return m_bk.url().prettyURL();
}

void KonqSidebarBookmarkItem::itemSelected()
{
    tree()->enableActions( false, false, false, false, false, false );
}

QString KonqSidebarBookmarkItem::key( int /*column*/, bool /*ascending*/ ) const
{
    return QString::number(m_key).rightJustify( 5, '0' );
}

KBookmark &KonqSidebarBookmarkItem::bookmark()
{
    return m_bk;
}
