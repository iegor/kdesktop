/* This file is part of the KDE project
   Copyright (C) 2001,2002 Carsten Pfeiffer <pfeiffer@kde.org>

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

#include "config.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kinstance.h>
#include <klocale.h>

#include <mrml_utils.h>

#include "mrml.h"

extern "C" {
    KDE_EXPORT int kdemain( int argc, char **argv )
    {
        KLocale::setMainCatalogue("kdelibs");
        KInstance instance( "kio_mrml" );
        KGlobal::locale()->insertCatalogue( "kmrml" );

        kdDebug() << "Starting MRML " << getpid() << endl;

        if (argc != 4)
        {
            fprintf(stderr, "Usage: kio_mrml protocol domain-socket1 domain-socket2\n");
            exit(-1);
        }

        Mrml slave(argv[2], argv[3]);
        slave.dispatchLoop();

        kdDebug() << "Done" << endl;
        return 0;
    }
}

const int Mrml::bufsize = 8192;

Mrml::Mrml( const QCString& pool_socket, const QCString& app_socket )
    : TCPSlaveBase( 12789, "mrml", pool_socket, app_socket ),
      m_config( KGlobal::config() )
{
    MrmlShared::ref();
}

Mrml::~Mrml()
{
    KMrml::Util::self()->unrequireLocalServer();

    closeDescriptor();
    MrmlShared::deref();
}

bool Mrml::checkLocalServer( const KURL& url )
{
    if ( KMrml::Util::self()->requiresLocalServerFor( url ) )
    {
        if ( !KMrml::Util::self()->startLocalServer( m_config ) )
            return false;
    }

    return true;
}

void Mrml::get( const KURL& url )
{
//      qDebug("******* getting: %s (user: %s)", url.url().latin1(), url.user().latin1());

    if ( !checkLocalServer( url ) )
    {
        error( KIO::ERR_SLAVE_DEFINED, i18n("Unable to start the Indexing Server. "
                                            "Aborting the query.") );
        return;
    }

    int retriesLeft = 5;
tryConnect:

    QCString utf8;
    bool sendError = (retriesLeft <= 0);

    if ( connectToHost( url.host(), port(url), sendError ) )
    {
//         qDebug(" connected!");

        QString task = metaData( MrmlShared::kio_task() );

        if ( task == MrmlShared::kio_initialize() ) {
            startSession( url );
        }

        else if ( task == MrmlShared::kio_startQuery() ) {
            QString meta = metaData( MrmlShared::mrml_data() );
            if ( meta.isEmpty() ) {
                closeDescriptor();
                error( KIO::ERR_SLAVE_DEFINED, i18n("No MRML data is available.") );
                return;
            }

            utf8 = meta.utf8();
            write( utf8, utf8.length() );

            emitData( readAll() );
        }

        // no task metadata available, we're called from KonqRun or something
        // like that. Emitting the mimetype seems to suffice for now. After
        // that, MrmlPart is going to start and start the get() again.
        else
        {
            mimeType( "text/mrml" );
            finished();
        }

    }
    else
    {
        if ( retriesLeft-- >= 0 )
        {
#ifdef HAVE_USLEEP
            usleep( 500 ); // wait a while for gift to start up
#endif
            goto tryConnect;
            return;
        }

        error( KIO::ERR_COULD_NOT_CONNECT,
               i18n("Could not connect to GIFT server.") );
        return;
    }

    closeDescriptor();
    //data( QByteArray() ); // send an empty QByteArray to signal end of data.
    finished();
}

// make sure we're connected when you call this!
QCString Mrml::readAll()
{
    QCString data;

    char buf[bufsize];
    ssize_t bytes = 0;

    while ( (bytes = read( buf, bufsize-1 )) > 0 ) {
        buf[bytes] = '\0';
        data.append( buf );
    }

//     qDebug("*** readAll()::: %i, %s", data.length(), data.data());
    return data;
}

QCString Mrml::loginString()
{
    return "<mrml><get-server-properties/></mrml>";
}

QCString Mrml::getConfigurationString()
{
    return "<mrml><get-configuration/></mrml>";
}

// ### needed?
QCString Mrml::getSessionsString( const QString& username,
                                  const QString& password )
{
    QCString data = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><mrml><get-sessions ";
    if ( !username.isEmpty() ) { // ### pop up auth-dialog???
        data.append( "user-name=\"");
        data.append( username.utf8() );
        data.append( "\"");

        if ( !password.isEmpty() ) {
            data.append( " password=\"");
            data.append( password.utf8() );
            data.append( "\"" );
        }

    }
    data.append( "/></mrml>" );

    return data;
}


bool Mrml::startSession( const KURL& url )
{
    // might first ask for collections, and then for algorithms for the
    // desired collection-id

    // Wolfgang says, we shouldn't create an own session-id here, as gcc 2.95
    // apparently makes problems in exception handling somehow. So we simply
    // accept the server's session-id.
    QString msg = mrmlString( QString::null ).arg(
        "<open-session user-name=\"%1\" session-name=\"kio_mrml session\" /> \
         <get-algorithms />                                                  \
         <get-collections />                                                 \
         </mrml>" ).arg( user( url ));

    QCString utf8 = msg.utf8();
//     qDebug(":::Writing: %s", utf8.data());
    write( utf8, utf8.length() );

    emitData( readAll() );

    return true;
}

QString Mrml::mrmlString( const QString& sessionId, const QString& transactionId )
{
    QString msg =
        "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>            \
         <!DOCTYPE mrml SYSTEM \"http://isrpc85.epfl.ch/Charmer/code/mrml.dtd\"> \
         %1                                                                      \
         </mrml>";

    if ( sessionId.isEmpty() ) // when we don't have one yet
        return msg.arg( "<mrml>%1" );

    if ( transactionId.isNull() )
        return msg.arg( "<mrml session-id=\"%1\">%1" ).arg( sessionId );
    else
        return msg.arg( "<mrml session-id=\"%1\" transaction-id=\"%1\">%1")
                  .arg( sessionId ).arg( transactionId );
}

void Mrml::emitData( const QCString& msg )
{
    mimeType( "text/mrml" );
    data( msg );
    processedSize( msg.count() );
}

void Mrml::mimetype( const KURL& url )
{
    if ( url.protocol() == "mrml" ) {
        mimeType( "text/mrml" );
        finished();
    }
    else
        KIO::TCPSlaveBase::mimetype( url );
}
