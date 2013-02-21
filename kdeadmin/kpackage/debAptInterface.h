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

#ifndef DEBAPT_IFACE_H
#define DEBAPT_IFACE_H

#include "../config.h"

#include <kaction.h>

#include "debInterface.h"

#define APT_SOURCE "/etc/apt/sources.list"

class DEBAPT: public DEB
{
  Q_OBJECT

public:
  DEBAPT();
  ~DEBAPT();

  virtual bool isType(char *buf, const QString &fname);

  void listPackages(QPtrList<packageInfo> *pki);
  QStringList listInstalls(const QStringList &packs, bool install, bool &cancel);

  QString doUninstall(int uninstallFlags, const QString &packs, bool &test);
  QString doInstall(int installFlags, const QString &packs, bool &test);
  QString install(int installFlags, QPtrList<packageInfo> *p,
		  bool &test);

  virtual QStringList FindFile(const QString &name, bool seachAll=false);

  void listRemotePackages(QPtrList<packageInfo> *pki);
  QStringList getFileList(packageInfo *p);

  KAction *updateM, *upgradeM, *fixupM, *fileM;

  void makeMenu(KActionCollection* act);
  void setMenu(KActionCollection* act, bool enable);

  QStringList readApt();
  QStringList readAptS();
  QStringList readAptF();
  void writeApt(const QStringList &list);

private slots:
  void updateS();	
  void upgradeS();
  void fixupS();
  void fileS();

private:
  void listAvail(QPtrList<packageInfo> *pki);
  void listRPack(QPtrList<packageInfo> *pki);
  QStringList getRFileList(packageInfo *p);

  QString env;
};
#endif
