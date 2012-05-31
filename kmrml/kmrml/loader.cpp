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

#include <kstaticdeleter.h>
#include <kio/scheduler.h>

#include "loader.h"

Loader *Loader::s_self = 0L;

KStaticDeleter<Loader> sd;

Loader * Loader::self()
{
    if ( !s_self )
        s_self = sd.setObject( new Loader() );

    return s_self;
}

Loader::Loader() : QObject()
{
}

Loader::~Loader()
{
    disconnect( this, SIGNAL( finished( const KURL&, const QByteArray& )));

    DownloadIterator it = m_downloads.begin();
    for ( ; it != m_downloads.end(); ++it ) {
        it.key()->kill();
        delete it.data();
    }

    s_self = 0L;
}

void Loader::requestDownload( const KURL& url )
{
    DownloadIterator it = m_downloads.begin();
    for ( ; it != m_downloads.end(); ++it ) {
        if ( it.key()->url() == url )
            return;
    }

    KIO::TransferJob *job = KIO::get( url, false, false );
    KIO::Scheduler::scheduleJob(job);

    connect( job , SIGNAL( data( KIO::Job *, const QByteArray& )),
             SLOT( slotData( KIO::Job *, const QByteArray& )));
    connect( job , SIGNAL( result( KIO::Job * )),
             SLOT( slotResult( KIO::Job * )));

    Download *d = new Download();
    m_downloads.insert( job, d );
}

void Loader::slotData( KIO::Job *job, const QByteArray& data )
{
    DownloadIterator it = m_downloads.find( static_cast<KIO::TransferJob*>(job) );
    if ( it != m_downloads.end() ) {
        QBuffer& buffer = it.data()->m_buffer;
        if ( !buffer.isOpen() )
            buffer.open( IO_ReadWrite );
        if ( !buffer.isOpen() ) {
            qDebug("********* EEK, can't open buffer for thumbnail download!");
            return;
        }

        buffer.writeBlock( data.data(), data.size() );
    }
}

void Loader::slotResult( KIO::Job *job )
{
    KIO::TransferJob *tjob = static_cast<KIO::TransferJob*>( job );

    DownloadIterator it = m_downloads.find( tjob );
    if ( it != m_downloads.end() ) {
        Download *d = it.data();

        if ( job->error() != 0 )
            emit finished( tjob->url(), QByteArray() );
        else
            emit finished( tjob->url(), d->m_buffer.buffer() );

        delete d;
        m_downloads.remove( it );
    }
}


// ### simultaneous downloads with multiple views? reference count downloads!
void Loader::removeDownload( const KURL& url )
{
    DownloadIterator it = m_downloads.begin();
    for ( ; it != m_downloads.end(); ++it ) {
        if ( it.key()->url() == url ) {
            it.key()->kill();
            delete it.data();
            return;
        }
    }
}

#include "loader.moc"
