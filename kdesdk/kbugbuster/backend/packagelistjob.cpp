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

#include "packagelistjob.h"
#include "package.h"
#include "packageimpl.h"
#include "bugserver.h"
#include "domprocessor.h"
#include "htmlparser.h"

#include <kdebug.h>
#include <assert.h>

#include <qdom.h>
#include <qbuffer.h>

PackageListJob::PackageListJob( BugServer *server )
  : BugJob( server )
{
}

PackageListJob::~PackageListJob()
{
}

void PackageListJob::start()
{
    BugJob::start( server()->packageListUrl() );
}

void PackageListJob::process( const QByteArray &data )
{
    Package::List packages;
    KBB::Error err = server()->processor()->parsePackageList( data, packages );
    if ( err ) {
      emit error( err.message() );
    } else {
      emit packageListAvailable( packages );
    }
}


#include "packagelistjob.moc"

/* 
 * vim:sw=4:ts=4:et
 */
