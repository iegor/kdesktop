//////////////////////////////////////////////////////////////////////////
// tagListItem.h                                                        //
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

#ifndef _TAGLISTITEM_H_
#define _TAGLISTITEM_H_

#include <qobject.h>
#include <qlistview.h>

class TagListItem: public QObject, public QCheckListItem
{
    Q_OBJECT
public:
    TagListItem( QListView * parent, const QString & tagName, int count = 1 );
    int count() const;
    QString name() const;
    void setName( const QString & name );

protected:
    virtual void stateChange( bool state );
    virtual int compare( QListViewItem * i, int col, bool ascending ) const;
    virtual QString text( int column ) const;

signals:
    void signalItemChecked( TagListItem * );

private:
    QString m_name;
    int m_count;
};

#endif
