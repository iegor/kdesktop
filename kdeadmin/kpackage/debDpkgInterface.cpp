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

#include "../config.h"

#include <unistd.h>
#include <stdlib.h>		// for getenv
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>		// for O_RDONLY
#include <setjmp.h>
#include <iostream>

#include <kurl.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "packageInfo.h"
#include "debDpkgInterface.h"
#include "updateLoc.h"
#include "kpackage.h"
#include "managementWidget.h"
#include "utils.h"
#include "options.h"
#include "cache.h"
#include <klocale.h>


extern KApplication *app;
extern Opts *params;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DEBDPKG::DEBDPKG():DEB()
{
  head = "DEBDPKG";
  name = i18n("DPKG: Debian");
  defaultHandle = 0;

  queryMsg = i18n("Querying DEB package list: ");
  procMsg = i18n("Kpackage: Waiting on DPKG");

  locatedialog = new Locations(i18n("Location of Debian Package Archives"));
  locatedialog->cLocations(3, 2,  this, i18n("Location", "L"),
			   "Deb",
			   i18n("Version\nArchitecture"),
			   i18n("Location of Base Folder of Debian Distribution"),
			   "stable frozen unstable\ni386 alpha sparc powerpc arm m68k");
  locatedialog->pLocations(3, 6, this, i18n("Packages", "P"),
			   "Deb", "*.deb Packages Packages.gz status available",
			   i18n("Location of 'Packages' Files for Sections of Debian Distributions"),
			   i18n("Location of Base Folder of Debian Distribution"));
  locatedialog->dLocations(2, 6, this, i18n("Folders", "F"),
			   "Deb", "*.deb",
			   i18n("Location of Folders Containing Debian Packages"));
  connect(locatedialog,SIGNAL(returnVal(LcacheObj *)),
	  this,SLOT(setAvail(LcacheObj *)));
  locatedialog->apply_slot();

  paramsInst.append(new param(i18n("Allow Downgrade"),TRUE,TRUE,"--refuse-downgrade"));
  paramsInst.append(new param(i18n("Check Conflicts"),TRUE,TRUE,"--force-conflicts"));
  paramsInst.append(new param(i18n("Check Dependencies"),TRUE,TRUE,"--force-depends"));
  paramsInst.append(new  param(i18n("Test (do not install)"),FALSE,FALSE,"--no-act"));

  paramsUninst.append(new param(i18n("Purge Config Files"),TRUE,FALSE,
			    "--purge","--remove"));
  paramsUninst.append(new param(i18n("Check Dependencies"),TRUE,TRUE,"--force-depends"));
  paramsUninst.append(new param(i18n("Test (do not uninstall)"),FALSE,FALSE,"--no-act"));

  hasProgram = ifExe("dpkg");
  
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DEBDPKG::~DEBDPKG()
{
}

//////////////////////////////////////////////////////////////////////////////
void DEBDPKG::distPackages(QPtrList<packageInfo> *pki, cacheObj *cp)
{
  LcacheObj *cList = new LcacheObj();
  QString loc = cp->base;

  QStringList opt = QStringList::split('\n',cp->option);
  QStringList::Iterator ocIt = opt.begin();
  QString rel = *ocIt;
  QString arch = *(++ocIt);

  QString parts[3] = {"main", "contrib", "non-free"};
  for (int i = 0; i < 3; i++) {
    QString file = loc + "/dists/";
    file += rel;
    file += "/";
    file += parts[i];
    file += "/binary-";
    file += arch;
    file += "/Packages";
    QString s;
    QString tmp = cp->cacheFile;
    tmp += s.setNum(i);
    cacheObj *cp = new cacheObj(loc,file,tmp);
    cList->append(cp);
  }

  listUnIPackages(pki, cList);
}

void DEBDPKG::listPackages(QPtrList<packageInfo> *pki)
{
  listInstalledPackages(pki);
  if (packageLoc) {
    listUnIPackages(pki, packageLoc);
  }
}




//////////////////////////////////////////////////////////////////////////////
// Call the script to uninstall packages setting parameters
// to dpkg dependent on flags, returning whether everyting worked
//////////////////////////////////////////////////////////////////////////////
QString DEBDPKG::doUninstall(int uninstallFlags, const QString &packs, bool &test)
{
  QString s = "dpkg ";
  // The -r or -p flag is set by setOptions
  s += setOptions(uninstallFlags, paramsUninst);
  s +=  packs;

  if (uninstallFlags>>3 & 1)
    test = 1;

  kdDebug() << "uCMD=" << s << "\n";

  return s;
}

#include "debDpkgInterface.moc"
