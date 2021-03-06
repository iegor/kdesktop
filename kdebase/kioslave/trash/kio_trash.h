/* This file is part of the KDE project
   Copyright (C) 2004 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KIO_TRASH_H
#define KIO_TRASH_H

#include <kio/slavebase.h>
#include "trashimpl.h"
namespace KIO { class Job; }

typedef TrashImpl::TrashedFileInfo TrashedFileInfo;
typedef TrashImpl::TrashedFileInfoList TrashedFileInfoList;

class TrashProtocol : public QObject, public KIO::SlaveBase
{
    Q_OBJECT
public:
    TrashProtocol( const QCString& protocol, const QCString &pool, const QCString &app);
    virtual ~TrashProtocol();
    virtual void stat(const KURL& url);
    virtual void listDir(const KURL& url);
    virtual void get( const KURL& url );
    virtual void put( const KURL& url, int , bool overwrite, bool );
    virtual void rename( const KURL &, const KURL &, bool );
    virtual void copy( const KURL &src, const KURL &dest, int permissions, bool overwrite );
    // TODO (maybe) chmod( const KURL& url, int permissions );
    virtual void del( const KURL &url, bool isfile );
    /**
     * Special actions: (first int in the byte array)
     * 1 : empty trash
     * 2 : migrate old (pre-kde-3.4) trash contents
     * 3 : restore a file to its original location. Args: KURL trashURL.
     */
    virtual void special( const QByteArray & data );

private slots:
    void slotData( KIO::Job*, const QByteArray& );
    void slotMimetype( KIO::Job*, const QString& );
    void jobFinished( KIO::Job* job );

private:
    typedef enum CopyOrMove { Copy, Move };
    void copyOrMove( const KURL& src, const KURL& dest, bool overwrite, CopyOrMove action );
    void createTopLevelDirEntry(KIO::UDSEntry& entry);
    bool createUDSEntry( const QString& physicalPath, const QString& fileName, const QString& url,
                         KIO::UDSEntry& entry, const TrashedFileInfo& info );
    void listRoot();
    void restore( const KURL& trashURL );

    TrashImpl impl;
    QString m_userName;
    QString m_groupName;
};

#endif
