/* This file is part of the KDE project
   Copyright (C) 2001 Carsten Pfeiffer <pfeiffer@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation, version 2.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MRML_H
#define MRML_H


#include <kio/tcpslavebase.h>
#include <kurl.h>

#include <kmrml_config.h>
#include "mrml_shared.h"

class Mrml : public KIO::TCPSlaveBase
{
public:
    Mrml( const QCString&, const QCString& );
    ~Mrml();

    virtual void get( const KURL& url );

    virtual void mimetype( const KURL& url );

private:
    QCString readAll();
    void emitData( const QCString& );

    bool startSession( const KURL& url );

    // helpers
    inline QString sessionId() {
        return metaData( MrmlShared::sessionId() );
    }

    // misc
    short int port( const KURL& url )
    {
        return (url.port() != 0) ?
                              url.port() :
                              m_config.settingsForHost( url.host() ).port();
    }

    static QString mrmlString( const QString& sessionId,
                               const QString& transactionId = QString::null );

    static QCString loginString();
    static QCString getConfigurationString();
    static QCString getSessionsString( const QString& username,
                                       const QString& password );
    QString user( const KURL& url ) {
        return url.hasUser() ?
            url.user() : m_config.defaultSettings().user;
    }
    QString pass( const KURL& url ) {
        return url.hasPass() ?
            url.pass() : m_config.defaultSettings().pass;
    }

    bool checkLocalServer( const KURL& url );

    static const int bufsize;
    QString defaultUser;
    QString defaultPass;

    KMrml::Config m_config;

};

#endif // MRML_H
