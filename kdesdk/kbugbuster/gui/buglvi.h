/*
    buglvi.h  -  Custom QListViewItem that holds a Bug object
    
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

#ifndef KBBMAINWINDOW_BUGLVI_H
#define KBBMAINWINDOW_BUGLVI_H

#include <klistview.h>

#include "bug.h"
#include "bugcommand.h"

namespace KBugBusterMainWindow
{

/**
 * @author Martijn Klingens
 */
class BugLVI : public KListViewItem
{
public:
    BugLVI( KListView *parent , const Bug &bug );
    ~BugLVI();

    Bug& bug() { return m_bug; }
    void setBug( Bug &bug ) { m_bug = bug; }

    QString key ( int column, bool ascending ) const;

    void setCommandState( BugCommand::State state );

    void paintCell(QPainter* p, const QColorGroup& cg,
                   int column, int width, int align);

private:
    Bug m_bug;
    BugCommand::State mCommandState;
};
 
}   // namespace

#endif // KBBMAINWINDOW_BUGLVI_H

/* vim: set et ts=4 softtabstop=4 sw=4: */

