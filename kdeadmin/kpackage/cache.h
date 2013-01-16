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



#ifndef CACHE_H
#define CACHE_H

#include <qdir.h>
#include <qptrlist.h>

#include <kurl.h>

#include "../config.h"
#include "packageInfo.h"

class Locations;
class LcacheObj;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class cacheObj
{
public:
  QString base;
  QString location;
  QString cacheFile;
  QString option;
  bool    subdirs;

  cacheObj(const QString &Pbase, const QString &Plocation, const QString &PcacheFile, const QString &Poption = QString::null, bool Psubdirs = FALSE);
  ~cacheObj();

  static QString PDir();
  // return path of kpackage cache directory

  static QString CDir();
  // return path of kpackage directory cache

  static  int newDCache(const QString &url, const QString &fn, QString &fname);
  // Checks directory cache
  //  -1  Cann't get cache file name
  //   0  Cache file exists
  //   1  Cache file doesn't exit
  // fname return  file name

  static  void rmDCache(const QString &fn);

  static void clearDCache();
  static void clearPCache();
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class LcacheObj: public QPtrList<cacheObj>
{
public:
  LcacheObj();
  ~LcacheObj();
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/**
 * @short the list of cached files in a directory stored in the file "kpackage_cachelist"
 **/
class CacheList : public QStringList
{
public:
  /**
   * create (and read) a cachelist object for the directory dir
   **/
  CacheList (const QDir& dir) ;

  /**
   * write this cachelist to disk
   **/
  void write (void) ;

  QString getCLFileName (void) const ;

private:
  QFile CLFile ;

  void read (void) ;
} ;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#endif
