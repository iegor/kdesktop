//////////////////////////////////////////////////////////////////////////
// tagListItem.cpp                                                      //
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

#include "tagListItem.h"

TagListItem::TagListItem( QListView * parent, const QString & tagName, int count )
    : QCheckListItem( parent, tagName, QCheckListItem::CheckBox ), m_name( tagName ), m_count( count )
{
}

// virtual
void TagListItem::stateChange( bool state )
{
    QCheckListItem::stateChange( state );
    emit signalItemChecked( this );
}

// virtual
int TagListItem::compare( QListViewItem * i, int col, bool ascending ) const
{
    if ( col == 1 )
    {
        int them = static_cast<TagListItem *>( i )->count();
        if ( m_count < them )
            return -1;
        else if ( m_count > them )
            return 1;
        else
            return 0;
    }
    return QCheckListItem::compare( i, col, ascending );
}

int TagListItem::count() const
{
    return m_count;
}

// virtual
QString TagListItem::text( int column ) const
{
    if ( column == 0 )
        return m_name;
    else if ( column == 1 )
        return QString::number( m_count );
    else
        return QString::null;
}

QString TagListItem::name() const
{
    return m_name;
}

void TagListItem::setName( const QString & name )
{
    m_name = name;
}

#include "tagListItem.moc"
