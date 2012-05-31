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

#ifndef LOADER_H
#define LOADER_H

#include <qbuffer.h>
#include <qcstring.h>
#include <qmap.h>
#include <qobject.h>

#include <kio/job.h>
#include <kurl.h>

class Download
{
public:
    ~Download() {
        if ( m_buffer.isOpen() )
            m_buffer.close();
    }
    QBuffer m_buffer;
    // add context of MrmlPart for progress?
};


class Loader : public QObject
{
    friend class gcc_sucks;
    Q_OBJECT

public:
    static Loader *self();
    ~Loader();

    void requestDownload( const KURL& url );

    void removeDownload( const KURL& url );

signals:
    void finished( const KURL& url, const QByteArray& );

private slots:
    void slotData( KIO::Job *, const QByteArray& );
    void slotResult( KIO::Job * );

private:
    Loader();

    QMap<KIO::TransferJob*,Download*> m_downloads;
    typedef QMapIterator<KIO::TransferJob*,Download*> DownloadIterator;

    static Loader *s_self;

};

#endif // LOADER_H
