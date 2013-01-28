//////////////////////////////////////////////////////////////////////////
// bookmarkListItem.cpp                                                 //
//                                                                      //
// Copyright (C)  2005  Lukas Tinkl <lukas@kde.org>                     //
//                                                                      //
// This program is free software; you can redistribute it and/or        //
// modify it under the terms of the GNU General Public License          //
// as published by the Free Software Foundation; either version 2       //
// of the License, or (at your option) any later version.               //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program; if not, write to the Free Software          //
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA            //
// 02111-1307, USA.                                                     //
//////////////////////////////////////////////////////////////////////////

#include "bookmarkListItem.h"

#include <kglobal.h>
#include <klocale.h>

BookmarkListItem::BookmarkListItem( QListView *parent, const QString & url, const QString & desc, time_t time )
    : KListViewItem( parent ), m_desc( desc )
{
    m_url = KURL::fromPathOrURL( url );
    m_dateTime.setTime_t( time );
}

int BookmarkListItem::compare( QListViewItem * i, int col, bool ascending ) const
{
    if ( col == 1 )
    {
        QDateTime them = static_cast<BookmarkListItem *>( i )->date();
        if ( m_dateTime < them )
            return -1;
        else if ( m_dateTime > them )
            return 1;
        else
            return 0;
    }
    return QListViewItem::compare( i, col, ascending );
}

QString BookmarkListItem::text( int column ) const
{
    if ( column == 0 )
        return m_desc;
    else if ( column == 1 )
        return KGlobal::locale()->formatDateTime( m_dateTime );

    return QString::null;
}

KURL BookmarkListItem::url() const
{
    return m_url;
}

QDateTime BookmarkListItem::date() const
{
    return m_dateTime;
}

QString BookmarkListItem::desc() const
{
    return m_desc;
}
