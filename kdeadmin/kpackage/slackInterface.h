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

#ifndef SLACK_IFACE_H
#define SLACK_IFACE_H

#include "../config.h"

#include <qptrlist.h>
#include <kprocess.h>

#include "pkgInterface.h"

class packageInfo;
class updateLoc;
class cacheObj;

class SLACK: public pkgInterface
{
  Q_OBJECT

public:
  SLACK();
  ~SLACK();

  bool isType(char *buf, const QString &fname);

  packageInfo *getPackageInfo(char mode, const QString &name,
			      const QString &version);
  QStringList getFileList(packageInfo *p);
  QStringList getChangeLog(packageInfo *p);

  bool filesTab(packageInfo *p);
  // If files tab is to be enabled

  bool changeTab(packageInfo *p);
  // If change log tab is to be enabled

  QStringList FindFile(const QString &name, bool seachAll=false);
  bool parseName(const QString& name, QString *n, QString *v);

  QString install(int installFlags, QPtrList<packageInfo> *plist, bool &test);

public slots:
  void setLocation();
  void setAvail(LcacheObj *);

private:
  packageInfo* collectInfo(const char *inp, int insState);
   void listInstalledPackages(QPtrList<packageInfo> *pki);

  QString doUninstall(int installFlags, const QString &packs, bool &test);
  QString doInstall(int installFlags, const QString &packs, bool &test);

  void listPackages(QPtrList<packageInfo> *pki);
  void listPackList(QPtrList<packageInfo> *pki, const QString &s,
		    cacheObj *cp, int insState);

  void initTranslate();

  void smerge(packageInfo *p);

  QDict<QString> *trl;
};

#endif



