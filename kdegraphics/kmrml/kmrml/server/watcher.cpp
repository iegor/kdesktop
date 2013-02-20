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
#include <kdebug.h>
#include <kdeversion.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "watcher.h"

using namespace KMrml;

Watcher::Watcher( const QCString& name )
    : KDEDModule( name )
{
    m_daemons.setAutoDelete( true );

    // safety, for clients that die without unregistering
    KApplication::dcopClient()->setNotifications( true );
    connect( KApplication::dcopClient(),
             SIGNAL( applicationRemoved( const QCString& )),
             SLOT( slotAppUnregistered( const QCString& )));
}

Watcher::~Watcher()
{
    KApplication::dcopClient()->setNotifications( false );
}

bool Watcher::requireDaemon( const QCString& clientAppId,
                             const QString& daemonKey,
                             const QString& commandline,
                             uint timeout /* seconds */,
                             int restartOnFailure )
{
    if ( !KApplication::dcopClient()->isApplicationRegistered( clientAppId ) )
        kdWarning() << "Watcher::requireDaemon: " << daemonKey
                    << ": Client AppID is not registered with DCOP: "
                    << clientAppId << endl;

    DaemonData *daemon = m_daemons.find( daemonKey );

    if ( daemon )
    {
        if ( !daemon->apps.find( clientAppId ) )
            daemon->apps.append( clientAppId );

        // timeout, commandline and restart values are: first come, first serve
        return true; // process already running, all fine
    }

    else // start daemon
    {
        daemon = new DaemonData( daemonKey, commandline,
                                 timeout, restartOnFailure );
        m_daemons.insert( daemonKey, daemon );
        daemon->apps.append( clientAppId );

#if KDE_VERSION >= 306
        daemon->process = new KProcess();
        daemon->process->setUseShell( true );
#else
        daemon->process = new KShellProcess();
#endif
        daemon->process->setEnvironment( "LC_ALL", "C" );
        daemon->process->setEnvironment( "LANG", "C" );
        daemon->process->setEnvironment( "LANGUAGE", "C" );
        *daemon->process << commandline;
        connect( daemon->process, SIGNAL( processExited( KProcess * ) ),
                 SLOT( slotProcExited( KProcess * )));
        return startDaemon( daemon );
    }
}

void Watcher::unrequireDaemon( const QCString& clientAppId,
                               const QString& daemonKey )
{
    unrequireDaemon( m_daemons.find( daemonKey ), clientAppId );
}

void Watcher::unrequireDaemon( DaemonData *daemon,
                               const QCString& clientAppId )
{
    if ( daemon )
    {
        daemon->apps.remove( clientAppId );
        if ( daemon->apps.isEmpty() )
        {
            if ( !daemon->timer )
            {
                daemon->timer = new QTimer();
                connect( daemon->timer, SIGNAL( timeout() ),
                         SLOT( slotTimeout() ));
            }
            daemon->timer->start( daemon->timeout * 1000, true );
        }
    }
    else
        kdWarning() << "Watcher::unrequireDaemon: daemon unknown. client: "
                    << clientAppId << endl;
}

QStringList Watcher::runningDaemons() const
{
    QStringList result;
    QDictIterator<DaemonData> it( m_daemons );
    for ( ; it.current(); ++it )
        result.append( it.current()->commandline );

    return result;
}

void Watcher::slotProcExited( KProcess *proc )
{
    DaemonData *daemon = findDaemonFromProcess( proc );

    if ( proc->normalExit() )
    {
        emitExited( daemon );
        return;
    }

    if ( daemon )
    {
        if ( --daemon->restartOnFailure <= 0 )
        {
            if ( KMessageBox::questionYesNo( 0L,
                             i18n("<qt>The server with the command line"
                                  "<br>%1<br>"
                                  "is not available anymore. Do you want to "
                                  "restart it?" ).arg( daemon->commandline ),
                                            i18n("Service Failure"), i18n("Restart Server"), i18n("Do Not Restart") )
                 == KMessageBox::Yes )
            {
                daemon->restartOnFailure = 1;
            }
        }

        if ( daemon->restartOnFailure > 0 )
        {
            startDaemon( daemon );
            return;
        }
    }

    emitFailure( daemon );
}

bool Watcher::startDaemon( DaemonData *daemon )
{
    if ( daemon->process->start( KProcess::NotifyOnExit ) )
        return true;

    else
    {
        if ( KMessageBox::questionYesNo( 0L,
                      i18n("Unable to start the server with the "
                           "command line"
                           "<br>%1<br>"
                          "Try again?").arg( daemon->commandline ),
                                         i18n("Service Failure"), i18n("Try Again"), i18n("Do Not Try") )
             == KMessageBox::Yes )
        {
            return startDaemon( daemon );
        }
    }

    return false;
}

void Watcher::slotTimeout()
{
    QTimer *timer = static_cast<QTimer*>( const_cast<QObject *>( sender() ) );
    DaemonData *daemon = findDaemonFromTimer( timer );
    if ( daemon )
    {
        if ( daemon->apps.isEmpty() )
        {
            // the daemon and KProcess might get deleted by killing the
            // KProcess (through slotProcExited()), so don't dereference
            // daemon after proc->kill()
            QString key = daemon->daemonKey;

            // noone registered during the timeout, so kill the daemon
            if ( !daemon->process->kill() )
                daemon->process->kill( SIGKILL );

            m_daemons.remove( key );
        }
    }
}

DaemonData * Watcher::findDaemonFromProcess( KProcess *proc )
{
    DaemonData *daemon;
    QDictIterator<DaemonData> it( m_daemons );
    for ( ; (daemon = it.current()); ++it )
    {
        if ( daemon->process == proc )
            return daemon;
    }

    return 0L;
}

DaemonData * Watcher::findDaemonFromTimer( QTimer *timer )
{
    DaemonData *daemon;
    QDictIterator<DaemonData> it( m_daemons );
    for ( ; (daemon = it.current()); ++it )
    {
        if ( daemon->timer == timer )
            return daemon;
    }

    return 0L;
}

void Watcher::slotAppUnregistered( const QCString& appId )
{
    if ( m_daemons.isEmpty() )
        return;

    DaemonData *daemon;
    QDictIterator<DaemonData> it( m_daemons );
    for ( ; (daemon = it.current()); ++it )
    {
        if ( daemon->apps.find( appId ) != -1 )
            unrequireDaemon( daemon, appId );
    }
}

void Watcher::emitExited( DaemonData *daemon )
{
    if ( daemon )
    {
        daemonExited( daemon->daemonKey,
                      daemon->process->pid(),
                      daemon->process->exitStatus() );

        m_daemons.remove( daemon->daemonKey );
    }
}

void Watcher::emitFailure( DaemonData *daemon )
{
    if ( daemon )
    {
        daemonDied( daemon->daemonKey, daemon->process->pid() );
        m_daemons.remove( daemon->daemonKey ); // deletes daemon + KProcess
    }
}

extern "C" {
    KDE_EXPORT KDEDModule *create_daemonwatcher(const QCString & obj )
    {
        return new Watcher( obj );
    }
}


#include "watcher.moc"
