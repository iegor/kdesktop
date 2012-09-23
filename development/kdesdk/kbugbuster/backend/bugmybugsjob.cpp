/*
    This file is part of KBugBuster.

    Copyright (c) 2002-2003 Cornelius Schumacher <schumacher@kde.org>
    Copyright (c) 2004      Martijn Klingens     <klingens@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#include "bugmybugsjob.h"
#include "bug.h"
#include "bugimpl.h"
#include "packageimpl.h"
#include "bugserver.h"
#include "domprocessor.h"
#include "htmlparser.h"

#include <kdebug.h>

#include <qbuffer.h>
#include <qregexp.h>

BugMyBugsJob::BugMyBugsJob( BugServer *server )
: BugJob( server )
{
}

BugMyBugsJob::~BugMyBugsJob()
{
}

void BugMyBugsJob::start()
{
    KURL url = server()->serverConfig().baseUrl();
    url.setFileName( "buglist.cgi" );
    url.setQuery( "bug_status=NEW&bug_status=ASSIGNED&bug_status=UNCONFIRMED&bug_status=REOPENED" );
    url.addQueryItem( "email1", server()->serverConfig().user() );
    url.addQueryItem( "emailtype1", "exact" );
    url.addQueryItem( "emailassigned_to1", "1" );
    url.addQueryItem( "emailreporter1", "1" );
    url.addQueryItem( "format", "rdf" );
    BugJob::start( url );
}

void BugMyBugsJob::process( const QByteArray &data )
{
    Bug::List bugs;

    Processor *processor = new RdfProcessor( server() );
    KBB::Error err = processor->parseBugList( data, bugs );
    delete processor;

    if ( err )
        emit error( i18n( "My Bugs: %2" ).arg( err.message() ) );
    else
        emit bugListAvailable( i18n( "My Bugs" ), bugs );
}

#include "bugmybugsjob.moc"

// vim: set sw=4 ts=4 et:

