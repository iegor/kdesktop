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

#ifndef KISS_IFACE_H
#define KISS_IFACE_H

#include "../config.h"

#include <qptrlist.h>
#include <qstringlist.h>
#include <kprocess.h>

#include "procbuf.h"
#include "pkgInterface.h"

class packageInfo;
class updateLoc;
class cacheObj;

class KISS: public pkgInterface
{
  Q_OBJECT

public:
  KISS();
  ~KISS();

  bool isType(char *buf, const QString &fname);
  param *initinstallOptions();
  param *inituninstallOptions();
  packageInfo *getPackageInfo(char mode, const QString &name, const QString &version);
  QStringList getFileList(packageInfo *p);
  QStringList getChangeLog(packageInfo *p);

  bool filesTab(packageInfo *p);
  // If files tab is to be enabled

  bool changeTab(packageInfo *p);
  // If change log tab is to be enabled

  QStringList FindFile(const QString &name, bool seachAll=false);
  bool parseName(const QString& name, QString *n, QString *v);

public slots:
  void setLocation();
  void setAvail(LcacheObj *);

private:
  packageInfo* collectInfo(const char *inp);
  void listInstalledPackages(QPtrList<packageInfo> *pki);

  QString install(int installFlags, QPtrList<packageInfo> *plist, bool &test);
  QString uninstall(int uninstallFlags, QPtrList<packageInfo> *plist, bool &test);

  QString doUninstall(int installFlags, const QString &packs, bool &test);
  QString doInstall(int installFlags, const QString &packs, bool &test);

  procbuf reader;
};

#endif



