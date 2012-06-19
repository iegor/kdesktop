/***************************************************************************
   loadallbugsdlg.cpp  -  progress dialog while loading all bugs for a package
                             -------------------
    copyright            : (C) 2002 by David Faure
    email                : david@mandrakesoft.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation version 2.                               *
 *                                                                         *
 ***************************************************************************/

#include "loadallbugsdlg.h"
#include "bugsystem.h"
#include "bugcache.h"
#include <kdebug.h>
#include <kio/defaultprogress.h>
#include <qtimer.h>

LoadAllBugsDlg::LoadAllBugsDlg(  const Package& pkg, const QString &component )
    : QDialog( 0L, "progressdlg", TRUE )
{
    m_bugLoadingProgress = new KIO::DefaultProgress( this );
    connect( m_bugLoadingProgress, SIGNAL( stopped() ),
             this, SLOT( slotStopped() ) );
    setCaption( i18n( "Loading All Bugs for Product %1" ).arg( pkg.name() ) );
    connect( BugSystem::self(),
             SIGNAL( bugDetailsAvailable( const Bug &, const BugDetails & ) ),
             SLOT( slotBugDetailsAvailable( const Bug &, const BugDetails & ) ) );
    connect( BugSystem::self(),
             SIGNAL( bugDetailsLoadingError() ),
             SLOT( slotBugDetailsLoadingError() ) );
    // The package (and its buglist) has to be in the cache already...
    m_bugs = BugSystem::self()->cache()->loadBugList( pkg, component, true );
    m_count = m_bugs.count();
    m_bugLoadingProgress->slotTotalSize( 0, m_count );
    kdDebug() << "LoadAllBugsDlg: " << m_count << " bugs to load" << endl;
    m_processed = 0;
    QTimer::singleShot( 0, this, SLOT( loadNextBug() ) );
}

void LoadAllBugsDlg::slotBugDetailsAvailable( const Bug &bug, const BugDetails & )
{
    kdDebug() << "LoadAllBugsDlg::slotBugDetailsAvailable " << bug.number() << endl;
    m_bugLoadingProgress->slotInfoMessage( 0L, i18n( "Bug %1 loaded" ).arg(bug.number()) );
    loadNextBug();
}

void LoadAllBugsDlg::slotBugDetailsLoadingError()
{
    // Abort at the first error. Otherwise we get spammed with "no host bugs.kde.org" msg boxes....
    reject();
}

void LoadAllBugsDlg::loadNextBug()
{
    kdDebug() << "LoadAllBugsDlg::loadNextBug" << endl;
    if ( m_bugs.isEmpty() )
    {
        kdDebug() << "LoadAllBugsDlg::loadNextBug DONE!" << endl;
        accept();
    } else {
        BugCache* cache = BugSystem::self()->cache();
        Bug bug;
        do {
            bug = m_bugs.front();
            m_bugs.pop_front();
            m_processed++;
            m_bugLoadingProgress->slotPercent( 0L, (100 * m_processed) / m_count );
            kdDebug() << "looking at bug " << bug.number() << " in cache:" << cache->hasBugDetails( bug ) << endl;
        } while ( cache->hasBugDetails( bug ) && !m_bugs.isEmpty() );
        if ( !cache->hasBugDetails( bug ) ) {
            kdDebug() << "LoadAllBugsDlg::loadNextBug loading bug " << bug.number() << endl;
            BugSystem::self()->retrieveBugDetails( bug );
        } else {
            kdDebug() << "LoadAllBugsDlg::loadNextBug DONE!" << endl;
            accept();
        }
    }
}

void LoadAllBugsDlg::slotStopped()
{
    kdDebug() << "LoadAllBugsDlg::slotStopped" << endl;
    // TODO abort job?
    reject();
}

#include "loadallbugsdlg.moc"
