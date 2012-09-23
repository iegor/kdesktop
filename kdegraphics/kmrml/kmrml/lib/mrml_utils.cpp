/* This file is part of the KDE project
   Copyright (C) 2002 Carsten Pfeiffer <pfeiffer@kde.org>

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

#include <dcopclient.h>
#include <kapplication.h>
#include <kprocess.h>
#include <kstaticdeleter.h>

#include "watcher_stub.h"

#include "mrml_utils.h"

// after 100 of no use, terminate the mrmld
#define TIMEOUT 100
// how often to restart the mrmld in case of failure
#define NUM_RESTARTS 5

using namespace KMrml;

KStaticDeleter<Util> utils_sd;

Util *Util::s_self = 0L;

Util::Util()
{
    // we need our own dcopclient, when used in kio_mrml
    if ( !DCOPClient::mainClient() )
    {
        DCOPClient::setMainClient( new DCOPClient() );
        if ( !DCOPClient::mainClient()->attach() )
            qWarning( "kio_mrml: Can't attach to DCOP Server.");
    }
}

Util::~Util()
{
    if ( this == s_self )
        s_self = 0L;
}

Util *Util::self()
{
    if ( !s_self )
        s_self = utils_sd.setObject( new Util() );
    return s_self;
}

bool Util::requiresLocalServerFor( const KURL& url )
{
    return url.host().isEmpty() || url.host() == "localhost";
}

bool Util::startLocalServer( const Config& config )
{
    if ( config.serverStartedIndividually() )
        return true;

    DCOPClient *client = DCOPClient::mainClient();

    // ### check if it's already running (add dcop method to Watcher)
    Watcher_stub watcher( client, "kded", "daemonwatcher");
    return ( watcher.requireDaemon( client->appId(),
                                    "mrmld", config.mrmldCommandline(),
                                    TIMEOUT, NUM_RESTARTS )
             && watcher.ok() );
}

void Util::unrequireLocalServer()
{
    DCOPClient *client = DCOPClient::mainClient();

    Watcher_stub watcher( client, "kded", "daemonwatcher");
    watcher.unrequireDaemon( client->appId(), "mrmld" );
}
