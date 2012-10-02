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

#include <typeinfo>
#include <stdlib.h>

#include <qdatetime.h>
#include <qstring.h>
#include <qptrlist.h>

#include <kdebug.h>
#include <kurl.h>
#include <kio/job.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <libkcal/vcaldrag.h>
#include <libkcal/vcalformat.h>
#include <libkcal/icalformat.h>
#include <libkcal/exceptions.h>
#include <libkcal/incidence.h>
#include <libkcal/event.h>
#include <libkcal/todo.h>
#include <libkcal/journal.h>
#include <libkcal/filestorage.h>

#include <kabc/locknull.h>

#include <kresources/configwidget.h>

#include "bugsystem.h"
#include "bugserver.h"

#include "kcalresourceconfig.h"
#include "resourceprefs.h"

#include "kcalresource.h"

KCalResource::KCalResource( const KConfig* config )
  : ResourceCached( config ), mLock( 0 )
{
  mPrefs = new KBB::ResourcePrefs;

  KConfigSkeletonItem::List items = mPrefs->items();
  KConfigSkeletonItem::List::Iterator it;
  for( it = items.begin(); it != items.end(); ++it ) {
    (*it)->setGroup( identifier() );
  }

  if ( config ) {
    readConfig( config );
  }

  init();
}

KCalResource::~KCalResource()
{
  close();

  if ( mDownloadJob ) mDownloadJob->kill();
  if ( mUploadJob ) mUploadJob->kill();

  delete mLock;
}

void KCalResource::init()
{
  mDownloadJob = 0;
  mUploadJob = 0;

  setType( "remote" );

  mOpen = false;

  mLock = new KABC::LockNull( true );

  KConfig config( "kbugbusterrc" );

  BugSystem::self()->readConfig( &config );
}

KBB::ResourcePrefs *KCalResource::prefs()
{
  return mPrefs;
}

void KCalResource::readConfig( const KConfig * )
{
  mPrefs->readConfig();
}

void KCalResource::writeConfig( KConfig *config )
{
  kdDebug() << "KCalResource::writeConfig()" << endl;

  ResourceCalendar::writeConfig( config );

  mPrefs->writeConfig();
}

QString KCalResource::cacheFile()
{
  QString file = locateLocal( "cache", "kcal/kresources/" + identifier() );
  kdDebug() << "KCalResource::cacheFile(): " << file << endl;
  return file;
}

bool KCalResource::doOpen()
{
  kdDebug(5800) << "KCalResource::doOpen()" << endl;

  mOpen = true;

  return true;
}

bool KCalResource::doLoad()
{
  kdDebug() << "KCalResource::doLoad()" << endl;

  if ( !mOpen ) return true;

  if ( mDownloadJob ) {
    kdWarning() << "KCalResource::doLoad(): download still in progress."
                << endl;
    return false;
  }
  if ( mUploadJob ) {
    kdWarning() << "KCalResource::doLoad(): upload still in progress."
                << endl;
    return false;
  }

  mCalendar.close();

  mCalendar.load( cacheFile() );

  BugSystem *kbb = BugSystem::self();

  kdDebug() << "KNOWN SERVERS:" << endl;
  QValueList<BugServer *> servers = kbb->serverList();
  QValueList<BugServer *>::ConstIterator it;
  for( it = servers.begin(); it != servers.end(); ++it ) {
    kdDebug() << "  " << (*it)->identifier() << endl;
  }

  kbb->setCurrentServer( mPrefs->server() );
  if ( !kbb->server() ) {
    kdError() << "Server not found." << endl;
    return false;
  } else {
    kdDebug() << "CURRENT SERVER: " << kbb->server()->identifier() << endl;
  }

  kbb->retrievePackageList();

  Package package = kbb->package( mPrefs->product() );

  connect( kbb, SIGNAL( bugListAvailable( const Package &, const QString &,
                                          const Bug::List & ) ),
           SLOT( slotBugListAvailable( const Package &, const QString &,
                                       const Bug::List & ) ) );

  kbb->retrieveBugList( package, mPrefs->component() );

  return true;
}

void KCalResource::slotBugListAvailable( const Package &, const QString &,
                                         const Bug::List &bugs )
{
  kdDebug() << "KCalResource::slotBugListAvailable()" << endl;

  if ( bugs.isEmpty() ) return;

  QString masterUid = "kbb_" + BugSystem::self()->server()->identifier();
  KCal::Todo *masterTodo = mCalendar.todo( masterUid );
  if ( !masterTodo ) {
    masterTodo = new KCal::Todo;
    masterTodo->setUid( masterUid );
    masterTodo->setSummary( resourceName() );
    mCalendar.addTodo( masterTodo );
  }

  Bug::List::ConstIterator it;
  for( it = bugs.begin(); it != bugs.end(); ++it ) {
    Bug bug = *it;
    kdDebug() << "  Bug " << bug.number() << ": " << bug.title() << endl;
    QString uid = "KBugBuster_" + bug.number();
    KCal::Todo *newTodo = 0;
    KCal::Todo *todo = mCalendar.todo( uid );
    if ( !todo ) {
      newTodo = new KCal::Todo;
      newTodo->setUid( uid );
      QString uri = "http://bugs.kde.org/show_bug.cgi?id=%1";
      newTodo->addAttachment( new KCal::Attachment( uri.arg( bug.number() ) ) );
      todo = newTodo;
    }

    todo->setSummary( bug.number() + ": " + bug.title() );
    todo->setRelatedTo( masterTodo );

    if ( newTodo ) mCalendar.addTodo( newTodo );
  }

  emit resourceChanged( this );
}

void KCalResource::slotLoadJobResult( KIO::Job *job )
{
  if ( job->error() ) {
    job->showErrorDialog( 0 );
  } else {
    kdDebug() << "KCalResource::slotLoadJobResult() success" << endl;

    mCalendar.close();
    mCalendar.load( cacheFile() );

    emit resourceChanged( this );
  }

  mDownloadJob = 0;

  emit resourceLoaded( this );
}

bool KCalResource::doSave()
{
  kdDebug() << "KCalResource::doSave()" << endl;

  if ( !mOpen ) return true;

  if ( readOnly() ) {
    emit resourceSaved( this );
    return true;
  }

  if ( mDownloadJob ) {
    kdWarning() << "KCalResource::save(): download still in progress."
                << endl;
    return false;
  }
  if ( mUploadJob ) {
    kdWarning() << "KCalResource::save(): upload still in progress."
                << endl;
    return false;
  }

  mCalendar.save( cacheFile() );

  mUploadJob = KIO::file_copy( KURL( cacheFile() ), mUploadUrl, -1, true );
  connect( mUploadJob, SIGNAL( result( KIO::Job * ) ),
           SLOT( slotSaveJobResult( KIO::Job * ) ) );

  return true;
}

bool KCalResource::isSaving()
{
  return mUploadJob;
}

void KCalResource::slotSaveJobResult( KIO::Job *job )
{
  if ( job->error() ) {
    job->showErrorDialog( 0 );
  } else {
    kdDebug() << "KCalResource::slotSaveJobResult() success" << endl;
  }

  mUploadJob = 0;

  emit resourceSaved( this );
}

void KCalResource::doClose()
{
  if ( !mOpen ) return;

  mCalendar.close();
  mOpen = false;
}

KABC::Lock *KCalResource::lock()
{
  return mLock;
}

void KCalResource::dump() const
{
  ResourceCalendar::dump();
  kdDebug(5800) << "  DownloadUrl: " << mDownloadUrl.url() << endl;
  kdDebug(5800) << "  UploadUrl: " << mUploadUrl.url() << endl;
  kdDebug(5800) << "  ReloadPolicy: " << mReloadPolicy << endl;
}

#include "kcalresource.moc"
