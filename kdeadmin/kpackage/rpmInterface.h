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

#ifndef RPM_IFACE_H
#define RPM_IFACE_H

#include "../config.h"


#include <qptrlist.h>

#include "packageInfo.h"
#include "pkgInterface.h"

class KDir;
class cacheObj;

class RPM : public pkgInterface
{
 Q_OBJECT

public:
  RPM();
  ~RPM();

  bool isType(char *buf, const QString &fname);

  packageInfo *getPackageInfo(char mode, const QString &name, const QString &version);
  QStringList getFileList(packageInfo *p);
  QStringList getChangeLog(packageInfo *p);

  bool filesTab(packageInfo *p);
  // If files tab is to be enabled

  bool changeTab(packageInfo *p);
  // If change log tab is to be enabled

  QStringList depends(const QString &name, int src);
  QStringList verify(packageInfo *p, const QStringList &files);

  QString uninstall(int uninstallFlags, QPtrList<packageInfo> *p, bool &test);
  QString uninstall(int uninstallFlags, packageInfo *p, bool &test);
  QString doUninst(int uninstallFlags, const QStringList &files, bool &test);
  QString doUninstP(int uninstallFlags, const QStringList &files, bool &test);

  QString install(int installFlags, QPtrList<packageInfo> *p, bool &test);
  QString install(int installFlags, packageInfo *p, bool &test);
  QString doinst(int installFlags, const QStringList &files, bool &test);
  QString doinstP(int installFlags, const QStringList &files, bool &test);

  QStringList FindFile(const QString &name, bool seachAll=false);
  void collectDepends(packageInfo *p, const QString &name, int src);
  bool parseName(const QString &name, QString *n, QString *v);

  QString provMap(const QString &p);

public slots:
  void setLocation();
  void setAvail(LcacheObj *);

private:
  packageInfo* collectInfo(QStringList &ln);

  void listInstalledPackages(QPtrList<packageInfo> *pki);

  QDict<QString> provides;
  bool rpmSetup;
  QStringList infoList;

  QString packageQuery();
  
  QStringList getIFileList( packageInfo *p );
  QStringList getUFileList( const QString &fn );

  QStringList getIChangeLog( packageInfo *p );
  QStringList getUChangeLog( const QString &fn );

  packageInfo *getIPackageInfo( const QString &name);
  packageInfo *getUPackageInfo( const QString &name);

  QString quotePath( const QString &path);

 };

#endif



