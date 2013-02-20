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


#ifndef PKG_IFACE_H
#define PKG_IFACE_H

#include "../config.h"
#include <qptrlist.h>
#include <qstring.h>
#include <qstringlist.h>

#include <kglobal.h>
#include <kstandarddirs.h>

#include "packageInfo.h"
#include "managementWidget.h"

class packageInfo;
class pkgOptions;
class Locations;
class LcacheObj;
class cacheObj;
class KAccel;
class KActionCollection;
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//  flags to install and uninstall
class param
{
public:
  param(const QString &nameP, bool initP,  bool invertP, const QString &flagP);
  param(const QString &nameP, bool initP,  bool invertP, const QString &flagP, const QString &flagAP);
  ~param();

  QString name;  // Name of flag
  bool init;         // Initial value
  bool invert;       // Whether it needs to be inverted
  QString flag; // text flag on command
  QString flagA; // text flag on command
};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
class pkgInterface: public QObject
{
  Q_OBJECT

public:
  pkgInterface();
  virtual ~pkgInterface();

  virtual bool isType(char *buf, const QString &fname) =  0;
  // looks at start of file to check that package is correct type

  virtual void makeMenu(KActionCollection* act);
  virtual void setMenu(KActionCollection* act, bool enable);

  bool ifExe(QString exe);
  // Check if this executable exists
  
  virtual packageInfo *getPackageInfo(char mode, const QString &name,
				      const QString &version) =  0;
  // get info on installed or uninstalled package. version is only set if
  // mode is 'i' (ie, if the package is already installed).

  virtual QStringList getFileList(packageInfo *p) =  0;
  // get list of files in the package

  virtual QStringList depends(const QString &name, int src);
  // check dependencies for package

  virtual QStringList verify(packageInfo *p, const QStringList &files);
  // check the installed files in a package

  virtual QStringList FindFile(const QString &name, bool seachAll=false) = 0;
  // search for packages containg a file

  virtual QStringList getChangeLog(packageInfo *p) = 0;
  // Get change log

  virtual bool filesTab(packageInfo *p) = 0;
  // If files tab is to be enabled

  virtual bool changeTab(packageInfo *p) = 0;
  // If change log tab is to be enabled

  virtual bool parseName(const QString &name, QString *n, QString *v) = 0;
  // breakup file name into package name and version

  virtual void listPackages(QPtrList<packageInfo> *pki);
  // scan various locations for list of packages

  virtual void listInstalledPackages(QPtrList<packageInfo> *pki) = 0;
  // produce list of currently installed packages

  virtual QStringList  listInstalls(const QStringList &packs, bool install, bool &cancel);
  // Convert list of packages requested to install to list of all packages to install

  virtual void smerge(packageInfo *p);
  // merge in package info entry

  QString getDir(cacheObj *cp);
  // list directory local or remote

  void listDir(QPtrList<packageInfo> *pki, const QString &fname, const QString &dir, bool subdirs = FALSE);
  // list the packages in a directory

  packageInfo *collectDir(const QString &name, const QString &size, const QString &dir);
  // build packageInfo object from directory entry

  QString getPackList(cacheObj *cp);
  // get packages information file

  virtual QString  provMap(const QString &p);
  // convert from package depends to package

  QString setOptions(int flags, QPtrList<param> &params);
  // convert un/install flags to text

  virtual QString doUninstall(int uninstallFlags, const QString &packs, bool &test);
  virtual QString doInstall(int installFlags, const QString &packs, bool &test);
  virtual QString uninstall(int uninstallFlags, QPtrList<packageInfo> *p,
				  bool &test);
  virtual QString uninstall(int uninstallFlags, packageInfo *p,
				  bool &test);
  virtual QString install(int installFlags, QPtrList<packageInfo> *p,
				  bool &test);
  virtual QString install(int installFlags, packageInfo *p,
				  bool &test);

  virtual QStringList readApt();
  virtual void writeApt(const QStringList &list);

  ///////////// DATA ///////////////////////
  pkgOptions *uninstallation, *installation;

  QString icon;
  // name icon file
  QString head;
  // capitalized name of package type
  QString name;
  // More descriptive name
  QPixmap pict, bad_pict, new_pict, updated_pict;
  // icons for package states
  QPixmap folder;
  // icon for package group
  QPixmap markInst;
  QPixmap markUnInst;
  // icon indicating mark for install/uninstall

  Locations *locatedialog;
  // dialog for setting the locations of  uninstalled packages
  LcacheObj *packageLoc;
  // List of locations of uninstalled pacckages

  bool dirOK;
  // variables related to reading packages from directories

  QString packagePattern;
  QString queryMsg;
  QString typeID;
  // Parameters for reading packages from directories

  QPtrList<param> paramsInst;
  QPtrList<param> paramsUninst;

  bool noFetch;
  // kpackage doesn't fetch this type of package itself

  bool defaultHandle;
  // This package type defaults to on

  QString errExe;
  // The name of an executable that wasn't found

  QString procMsg;
  // for running processes

  QString DELMSG;

  bool hasRemote;
  // can access on remote host

  bool hasSearchAll;
  // can search uninstalled packages for files

  bool hasProgram;
  // the program needed to handle this package type is available

public slots:
  virtual void setLocation() = 0;
  virtual void setAvail(LcacheObj *) = 0;
};

#endif
