/*
    This file is part of KBugBuster.

    Copyright (c) 2002,2003 Cornelius Schumacher <schumacher@kde.org>

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

#include "buglistjob.h"
#include "bug.h"
#include "bugimpl.h"
#include "packageimpl.h"
#include "bugserver.h"
#include "domprocessor.h"
#include "htmlparser.h"

#include <kdebug.h>

#include <qbuffer.h>
#include <qregexp.h>

BugListJob::BugListJob( BugServer *server )
  : BugJob( server )
{
}

BugListJob::~BugListJob()
{
}

void BugListJob::start( const Package &pkg, const QString &component )
{
    m_package = pkg;
    m_component = component;

    BugJob::start( server()->bugListUrl( pkg, component ) );
}


void BugListJob::process( const QByteArray &data )
{
    Bug::List bugs;

    KBB::Error err = server()->processor()->parseBugList( data, bugs );

    if ( err ) {
      emit error( i18n("Package %1: %2").arg( m_package.name() )
                                        .arg( err.message() ) );
    } else {
      emit bugListAvailable( m_package, m_component, bugs );
    }
}


#include "buglistjob.moc"

/*
 * vim:sw=4:ts=4:et
 */
