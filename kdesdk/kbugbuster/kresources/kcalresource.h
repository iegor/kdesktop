/*
    This file is part of KBugBuster.

    Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/
#ifndef KCALRESOURCE_H
#define KCALRESOURCE_H

#include <qptrlist.h>
#include <qstring.h>
#include <qdatetime.h>

#include <kurl.h>
#include <kconfig.h>
#include <kdirwatch.h>

#include <libkcal/incidence.h>
#include <libkcal/calendarlocal.h>
#include <libkcal/icalformat.h>
#include <libkcal/resourcecached.h>

#include <bugsystem.h>

namespace KIO {
class FileCopyJob;
class Job;
}

namespace KBB {
class ResourcePrefs;
}

/**
  This class provides a calendar stored as a remote file.
*/
class KCalResource : public KCal::ResourceCached
{
    Q_OBJECT

    friend class KCalResourceConfig;

  public:
    /**
      Reload policy.
      
      @see setReloadPolicy(), reloadPolicy()
    */
    enum { ReloadNever, ReloadOnStartup, ReloadOnceADay, ReloadAlways };
  
    /**
      Create resource from configuration information stored in KConfig object.
    */
    KCalResource( const KConfig * );
    ~KCalResource();

    void readConfig( const KConfig *config );
    void writeConfig( KConfig *config );

    KBB::ResourcePrefs *prefs();

    /**
      Return name of file used as cache for remote file.
    */
    QString cacheFile();

    KABC::Lock *lock();

    bool isSaving();

    void dump() const;

  protected slots:
    void slotBugListAvailable( const Package &, const QString &,
                               const Bug::List &bugs );

    void slotLoadJobResult( KIO::Job * );
    void slotSaveJobResult( KIO::Job * );

  protected:
    bool doOpen();
    void doClose();
    bool doLoad();
    bool doSave();
 
  private:
    void init();

    KBB::ResourcePrefs *mPrefs;

    KURL mDownloadUrl;
    KURL mUploadUrl;

    int mReloadPolicy;

    KCal::ICalFormat mFormat;

    bool mOpen;

    KIO::FileCopyJob *mDownloadJob;
    KIO::FileCopyJob *mUploadJob;
    
    KABC::Lock *mLock;

    class Private;
    Private *d;
};

#endif
