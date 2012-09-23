/*
    This file is part of KBugBuster.
    Copyright (c) 2002 Cornelius Schumacher <schumacher@kde.org>

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
#ifndef DOMPROCESSOR_H
#define DOMPROCESSOR_H

#include "bug.h"
#include "bugdetails.h"
#include "package.h"
#include "error.h"
#include "processor.h"

#include <kurl.h>

#include <qdom.h>

class BugServer;

class DomProcessor : public Processor
{
  public:
    DomProcessor( BugServer * );
    virtual ~DomProcessor();

    KBB::Error parsePackageList( const QByteArray &data,
                                 Package::List &packages );
    KBB::Error parseBugList( const QByteArray &data, Bug::List &bugs );
    KBB::Error parseBugDetails( const QByteArray &, BugDetails & );

    void setPackageListQuery( KURL & );
    void setBugListQuery( KURL &, const Package &, const QString &component );
    void setBugDetailsQuery( KURL &, const Bug & );

  protected:
    virtual KBB::Error parseDomPackageList( const QDomElement &,
                                            Package::List & );
    virtual KBB::Error parseDomBugList( const QDomElement &, Bug::List & );
    virtual KBB::Error parseDomBugDetails( const QDomElement &, BugDetails & );

    QString wrapLines( const QString & );
    bool parseAttributeLine( const QString &line, const QString &key,
                             QString &result );
    QDateTime parseDate( const QString & );
};

#endif

/*
 * vim:sw=4:ts=4:et
 */
