/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "kpackage.h"
#include "options.h"
#include "cache.h"
#include <klocale.h>
#include <kdebug.h>

extern Opts *opts;
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
cacheObj::cacheObj(const QString &Pbase, const QString &Plocation, const QString &PcacheFile,
		   const QString &Poption, bool Psubdirs)
{
  base = Pbase;
  location = Plocation;
  cacheFile = PcacheFile;
  option = Poption;
  subdirs = Psubdirs;
}

cacheObj::~cacheObj()
{}

QString cacheObj::PDir()
{
  struct stat buf;
  stat(QFile::encodeName(QDir::homeDirPath()), &buf);

  QString tmpd = opts->CacheDir ;

  QDir d(tmpd);
  if (!d.exists()) {
    if (!d.mkdir(tmpd)) {
      KpMsgE(i18n("Cannot create folder %1").arg(tmpd),TRUE);
      tmpd = "";
    } else {
      chown(QFile::encodeName(tmpd),buf.st_uid,buf.st_gid);
    }
  }
  return tmpd;
}

QString cacheObj::CDir()
{
  QString tmpd = PDir();
  if (!tmpd.isEmpty()) {
    struct stat buf;
    stat(QFile::encodeName(tmpd),&buf);

    tmpd += "dir/";

    QDir d(tmpd);
    if (!d.exists()) {
      if (!d.mkdir(tmpd)) {
	KpMsgE(i18n("Cannot create folder %1").arg(tmpd),TRUE);
	tmpd = "";
      } else {
	chown(QFile::encodeName(tmpd),buf.st_uid,buf.st_gid);
      }
    }
  }
  return tmpd;
}

int cacheObj::newDCache(const QString &url, const QString &fn, QString  &fname) {

  KURL u(url);
  if ( !u.isValid() ) {
    KpMsgE(i18n("Malformed URL: %1").arg(url),TRUE);
    return -1;
  }

  QString tmpd = cacheObj::CDir();
  if (tmpd.isEmpty()) {
    return -1;
  } else {
    if (u.protocol() == "file") {
      fname =  u.path();
      return 0;
    }

    fname = tmpd + fn;

    if (opts->DCache == Opts::NEVER) {
      return 1;
    }

    QFileInfo f(fname);

    if (f.exists() && f.size() > 0) {
      return 0;;
    } else {
      if (f.size() == 0)
	rmDCache(fname);
      return 1;
    }
  }
}

void  cacheObj::rmDCache(const QString &fn) {
  QString tmpd = cacheObj::CDir();
  tmpd += fn;

  if (!tmpd.isEmpty()) {
    unlink(QFile::encodeName(tmpd));
  }
}

void  cacheObj::clearDCache() {
  QString tmpd = cacheObj::CDir();

  if (!tmpd.isEmpty()) {
    QDir d(tmpd);
    CacheList cl(d) ;
    for (CacheList::iterator it = cl.begin() ; it != cl.end() ; ++it) {
      QString s = tmpd;
      s += *it;
      unlink(QFile::encodeName(s));
    }
    unlink(QFile::encodeName(cl.getCLFileName())) ; // also delete the kpackage_cachelist file
  }
}

void  cacheObj::clearPCache() {
  QString tmpd = cacheObj::PDir();

  if (!tmpd.isEmpty()) {
    QDir d(tmpd);
    CacheList cl(d);
    for (CacheList::iterator it = cl.begin() ; it != cl.end() ; ++it) {
      QString s = tmpd ;
      s += *it;
      unlink(QFile::encodeName(s));
    }
    unlink(QFile::encodeName(cl.getCLFileName())) ; // also delete the kpackage_cachelist file
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
LcacheObj::LcacheObj()
{
  setAutoDelete(TRUE);
}

LcacheObj::~LcacheObj()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
CacheList::CacheList (const QDir& dir)
{
  CLFile.setName (dir.path() + "/kpackage_cachelist") ;
  read() ;
}

void CacheList::read ()
{
  kdDebug() << "reading cachelist: " << CLFile.name() << "\n" ;
  if (CLFile.open (IO_ReadOnly)) {
    QTextStream stream (&CLFile) ;
    QString line ;
    while (!stream.eof()) {
      line = stream.readLine() ;
      if (line[0] != '#') { // not a comment
        append (line) ; // to this QStringList
      }
    }
    CLFile.close() ;
  }
  else {
    //      kdDebug() << "could not open cachelist " << CLFile.name() << "!\n" ;
  }
}

void CacheList::write ()
{
  kdDebug() << "writing cachelist: " << CLFile.name() << "\n" ;
  if (CLFile.open (IO_WriteOnly)) {
    QTextStream stream (&CLFile) ;
    stream << "# This file contains a list of files that have been cached in this folder.\n" ;
    stream << "# Please only delete this if you want kpackage to forget what it has cached.\n" ;
    for (QStringList::iterator it = begin() ; it != end() ; ++it) {
      stream << *it << "\n" ;
    }
	CLFile.close() ;
  }
  else {
      kdDebug() << "could not open cachelist " << CLFile.name() << "!\n" ;
  }
}

QString CacheList::getCLFileName () const
{
    return CLFile.name() ;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
