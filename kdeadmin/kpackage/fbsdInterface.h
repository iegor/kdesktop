/*
** Copyright (C) 2000 by Alex Hayward
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

#ifndef FBSD_IFACE_H
#define FBSD_IFACE_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//#ifdef HAVE_FBSD_PKGTOOLS

#include <qptrlist.h>
#include <qmap.h>

#include "packageInfo.h"
#include "pkgInterface.h"

class KDir;
class cacheObj;
class bsdPortsIndexItem;

class fbsdInterface : public pkgInterface
{
 Q_OBJECT

public:
  fbsdInterface();
  ~fbsdInterface();

  void init();

  bool isType(char *buf, const QString &fname);

  packageInfo *getPackageInfo(char mode, const QString &name, const QString &version);
  QStringList getFileList(packageInfo *p);
  QStringList getChangeLog(packageInfo *p);

  bool filesTab(packageInfo *p);
  // If files tab is to be enabled

  bool changeTab(packageInfo *p);
  // If change log tab is to be enabled

  QString uninstall(int uninstallFlags, QPtrList<packageInfo> *p, bool &test);
  QString uninstall(int uninstallFlags, packageInfo *p, bool &test);
  QString doUninstall(int uninstallFlags, const QString &packs, bool &test);
  QString doInstall(int installFlags, const QString &packs, bool &test);

  QStringList FindFile(const QString &name, bool seachAll=false);
  void collectDepends(packageInfo *p, const QString &name, int src);
  bool parseName(const QString& name, QString *n, QString *v);

  void listInstalledPackages(QPtrList<packageInfo> *pki);
  void listPackages(QPtrList<packageInfo> *pki);

  QDict <bsdPortsIndexItem> ports;
public slots:
  void setLocation();
  void setAvail(LcacheObj *);

private:
  /**
   * @short Add the name and version identifiers to a QMap<QString, QString>.
   *
   * name is parsed in to name and version and these are added to
   * d. Errors are handled.
   */
  void addNV(QMap<QString, QString> &d, const QString &name);
  int parseItem(QStringList::Iterator &it, QString &name, QString &value, QString separator, QStringList list );
  int pathInfo(QMap<QString, QString> &a);

};

/**
 * @short Ports description linked list item
 *
 * Each item in the list describes one port from the ports collection.
 */
class bsdPortsIndexItem {
public:
  /**
   * desc is a line from the INDEX file (/usr/ports/INDEX under FreeBSD)
   * which has a particular format:
   *
   * name|port path|inst prefix|1 line commect|DESCR file|maintainer|categories|build-deps|run-deps
   *
   * Multiple space separated categories may be specified.
   *
   * desc must remain allocated (ie, its not copied) will be modified.
   *
   * binaries should be true if this is a binary package.
   *
   * dname is the name of the base directory of this ports/packages tree.
   */
  bsdPortsIndexItem(fbsdInterface *parent, char *desc, bool binaries, const QString &dname);

  /** @short true if this has a binary packages. */
  bool bin;

  /** @short true if this has a source port available. */
  bool port;

  /** @short true if this package is installed (set in listInstalledPackages) */
  bool installed;

  /** @short The next item in this linked list */
  bsdPortsIndexItem *next;

  QStringList fields;

  enum {NAME=0, PATH, PREFIX, COMMENT, DESC_PATH, MAINT, CATS, BDEPS, RDEPS};

  QString bin_filename;
  QString bin_filename_base;
  QString port_dirname;

   /**
   * @short Given the path to an INDEX file process each port in it.
   *
   * binaries should be true if the file is an index for packages, false for ports.
   * dname is the base directory.
   */
  static void processFile(fbsdInterface *parent, const QString &fname, bool binaries, const QString &dname);

private:
  unsigned int name_hash;
  static unsigned char calc_hash1(const char *name);
  static unsigned int calc_hash4(const char *name);
  static unsigned char hash1(unsigned int hash4);
};

#endif

