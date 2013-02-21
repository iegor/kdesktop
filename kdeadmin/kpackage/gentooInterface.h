/*
**
** Copyright (C) 2004 Richard Lärkäng <nouseforaname@home.se>
**
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

#ifndef GENTOOINTERFACE_H
#define GENTOOINTERFACE_H

#include "pkgInterface.h"

class Gentoo : public pkgInterface
{
  Q_OBJECT

public:
  Gentoo();
  ~Gentoo();

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
  packageInfo* collectInstalledInfo(const QString& name, const QString& category);
  packageInfo* collectUninstalledInfo(const QString& name, const QString& category, const QString& version);
  void listInstalledPackages(QPtrList<packageInfo> *pki);

  QString install(int installFlags, QPtrList<packageInfo> *plist, bool &test);
  QString uninstall(int uninstallFlags, QPtrList<packageInfo> *plist, bool &test);

  QStringList archesPossible;
  QString portageDir;
  QStringList packageMask;
};


#endif // GENTOOINTERFACE_H
