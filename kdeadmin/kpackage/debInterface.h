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


#ifndef DEB_IFACE_H
#define DEB_IFACE_H

#include "../config.h"

#include <qptrlist.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtextstream.h>
#include <qstringlist.h>

#include <kprocess.h>

#include "pkgInterface.h"

class packageInfo;
class updateLoc;
class cacheObj;

#define AVAIL          "/var/lib/dpkg/available"
#define STATUS          "/var/lib/dpkg/status"

#define INFODIR         "/var/lib/dpkg/info/"


class DEB: public pkgInterface
{
  Q_OBJECT

public:
  DEB();
  ~DEB();

  virtual bool isType(char *buf, const QString &fname);

  virtual packageInfo *getPackageInfo(char mode, const QString &name,
			      const QString &version);
  virtual QStringList getFileList(packageInfo *p);
  virtual QStringList getChangeLog(packageInfo *p);

  bool filesTab(packageInfo *p);
  // If files tab is to be enabled

  bool changeTab(packageInfo *p);
  // If change log tab is to be enabled

  virtual void listInstalledPackages(QPtrList<packageInfo> *pki);

  virtual QStringList FindFile(const QString &name, bool seachAll=false);
  virtual bool parseName(const QString &name, QString *n, QString *v);

  virtual packageInfo* collectInfo(QStringList &ln,  pkgInterface *pkgInt = 0);

  QString uninstall(int uninstallFlags, QPtrList<packageInfo> *p,
				  bool &test);
  QString install(int installFlags, QPtrList<packageInfo> *p,
				  bool &test);
  QString doInstall(int installFlags, const QString &packs, bool &test);

public slots:
  void setLocation();
  void setAvail(LcacheObj *);

protected:
  packageInfo *getIPackageInfo(const QString &name);
  packageInfo *getIRPackageInfo(const QString &name);
  packageInfo *getUPackageInfo(const QString &name);

  QStringList getIChangeLog(packageInfo *p);

  void listPackList(QPtrList<packageInfo> *pki,
		    const QString &fname, cacheObj *cp);

  virtual void distPackages(QPtrList<packageInfo> *pki, cacheObj *cp);
  void listUnIPackages(QPtrList<packageInfo> *pki, LcacheObj *pCache);

  QStringList getIFileList(packageInfo *p);
  QStringList getUFileList(const QString &fn);
};

#endif



