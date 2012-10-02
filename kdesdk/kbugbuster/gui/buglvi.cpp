/*
    buglvi.cpp  -  Custom QListViewItem that holds a Bug object

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

#include <qfont.h>
#include <qpainter.h>

#include <kstringhandler.h>
#include <kdebug.h>

#include "bugsystem.h"
#include "bugserver.h"

#include "buglvi.h"

using namespace KBugBusterMainWindow;

BugLVI::BugLVI( KListView *parent , const Bug &bug )
: KListViewItem( parent, bug.number() + "  ",
                 i18n( "1 day", "%n days", bug.age() ),
                 bug.title(), //KStringHandler::csqueeze( bug.title(), 70 ),
                 Bug::statusLabel( bug.status() ),
                 Bug::severityLabel( bug.severity() ),
                 bug.submitter().fullName() )
{
    m_bug = bug;

    bool hasCommands = BugSystem::self()->server()->hasCommandsFor( bug );
    mCommandState = hasCommands ? BugCommand::Queued : BugCommand::None;

    if ( bug.age() == 0xFFFFFFFF )
        setText( 1, i18n( "Unknown" ) );

    Person developer = bug.developerTODO();
    if ( !developer.name.isEmpty() )
        setText( 3, i18n( "%1 (%2)" ).arg( Bug::statusLabel( bug.status() ), developer.name ) );
}

BugLVI::~BugLVI()
{
}

QString BugLVI::key( int column, bool /* ascending */ ) const
{
    QString key;

    if ( column == 0 )
    {
        key = text( 0 ).rightJustify( 10, '0' );
    }
    else if ( column == 1 )
    {
        if ( m_bug.age() == 0xFFFFFFFF )
            key = "0";
        else
            key = QString::number( m_bug.age() ).rightJustify( 10, '0' );
    }
    else if ( column == 4 )
    {
        key = QString::number( 10 - m_bug.severity() );
        key += m_bug.number().rightJustify( 10, '0' );
    }
    else
    {
        key = text( column );
    }

    return key;
}

void BugLVI::paintCell(QPainter* p, const QColorGroup& cg,
                       int column, int width, int align)
{
    QColorGroup newCg = cg;
    if ( mCommandState == BugCommand::Queued ) {
        QFont font = p->font();
        font.setBold( true );
        p->setFont( font );
    } else if ( mCommandState == BugCommand::Sent ) {
        QFont font = p->font();
        font.setItalic( true );
        p->setFont( font );
    } else if ( m_bug.status() == Bug::Closed ) {
        // Different color for closed bugs
        newCg.setColor( QColorGroup::Text, cg.color( QColorGroup::Dark ) );
    }

    KListViewItem::paintCell( p, newCg, column, width, align );
}

void BugLVI::setCommandState( BugCommand::State state)
{
    mCommandState = state;
}

// vim: set et ts=4 sw=4 sts=4:

