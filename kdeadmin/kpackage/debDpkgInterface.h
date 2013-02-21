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

#ifndef DEBDPKG_IFACE_H
#define DEBDPKG_IFACE_H

#include "../config.h"

#include "debInterface.h"

class packageInfo;
class updateLoc;
class cacheObj;

class DEBDPKG: public DEB
{
  Q_OBJECT

public:
  DEBDPKG();
  ~DEBDPKG();
  
  QString doUninstall(int installFlags, const QString &packs, bool &test);

  void listPackList(QPtrList<packageInfo> *pki, const QString &fname,
		    cacheObj *cp);

  void listPackages(QPtrList<packageInfo> *pki);
  void distPackages(QPtrList<packageInfo> *pki, cacheObj *cp);

public slots:

protected:
  packageInfo *getIPackageInfo(const QString &name);
  packageInfo *getUPackageInfo(const QString &name);

};

#endif



