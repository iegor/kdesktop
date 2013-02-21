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
#include <stdlib.h>

#include "klocale.h"
#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>

#include "kpackage.h"
#include "updateLoc.h"
#include "debAptInterface.h"
#include "cache.h"
#include "pkgOptions.h"



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

DEBAPT::DEBAPT():DEB()
{
  head = "DEBAPT";
  name = i18n("APT: Debian");

  queryMsg = i18n("Querying DEB APT package list: ");
  procMsg = i18n("KPackage: Waiting on APT-GET");

  hasRemote = TRUE;

  locatedialog = new Locations(i18n("Location of Debian Packages"));

  locatedialog->aLocations(1, 60, this, i18n("APT sources", "A"),
			   i18n("APT Sources Entries"));
  locatedialog->dLocations(1, 8, this, i18n("Folders", "F"),
			   "Deb", "*.deb",
			   i18n("Location of Folders Containing Debian Packages"));
   connect(locatedialog,SIGNAL(returnVal(LcacheObj *)),
	  this,SLOT(setAvail(LcacheObj *)));
  locatedialog->apply_slot();

  paramsInst.append(new param(i18n("Download only"),FALSE,FALSE,"-d"));
  paramsInst.append(new param(i18n("No download"),FALSE,FALSE,"--no-download"));
  paramsInst.append(new param(i18n("Ignore missing"),FALSE,FALSE,"-m"));
  paramsInst.append(new param(i18n("Ignore hold"),FALSE,FALSE,"--ignore-hold"));
  paramsInst.append(new param(i18n("Allow Unauthenticated"),FALSE,FALSE,"--allow-unauthenticated"));
  paramsInst.append(new param(i18n("Assume yes"),TRUE,FALSE,"--yes"));
  paramsInst.append(new param(i18n("Test (do not uninstall)"),FALSE,FALSE,"-s"));

  paramsUninst.append(new param(i18n("Purge Config Files"),FALSE,FALSE,"--purge"));
  paramsUninst.append(new param(i18n("Assume yes"),TRUE,FALSE,"--yes"));
  paramsUninst.append(new param(i18n("Test (do not uninstall)"),FALSE,FALSE,"-s"));

  env = "DEBIAN_FRONTEND=readline; export DEBIAN_FRONTEND; ";

  noFetch = TRUE;
  hasSearchAll = TRUE;

  hasProgram = ifExe("apt-get");
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DEBAPT::~DEBAPT()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool DEBAPT::isType(char *, const QString &)
{
  return false;
}

void DEBAPT::makeMenu(KActionCollection* act)
{
  updateM = new KAction( i18n("&Update"), QString::null,
			 0, this,
			 SLOT(updateS()), act, "debapt_update");

  upgradeM = new KAction( i18n("U&pgrade"), QString::null,
			  0, this,
			  SLOT(upgradeS()), act, "debapt_upgrade");

  fixupM = new KAction( i18n("&Fixup"), QString::null,
			 0, this,
			 SLOT(fixupS()), act, "debapt_fixup");

  fileM = new KAction( i18n("&Apt-File Update"), QString::null,
			 0, this,
			 SLOT(fileS()), act, "debapt_file");
}

void DEBAPT::setMenu(KActionCollection*, bool enable)
{
  updateM->setEnabled(enable);
  upgradeM->setEnabled(enable);
  fixupM->setEnabled(enable);
  fileM->setEnabled(enable);
}

void DEBAPT::updateS()
{
  if (kprun->run("apt-get update", "APT update")) {
    if (kprun->exec())
      kpackage->management->collectData(TRUE);
  }
}

void DEBAPT::upgradeS()
{
  if (kprun->run(env + "apt-get dist-upgrade", "APT upgrade")) {
    if (kprun->exec())
      kpackage->management->collectData(TRUE);
  }
}

void DEBAPT::fixupS()
{
  if (kprun->run(env + "apt-get -f install", "APT fixup")) {
    if (kprun->exec())
      kpackage->management->collectData(TRUE);
  }
}

void DEBAPT::fileS()
{
  if (ifExe("apt-file") || !hostName.isEmpty()) {
    if (kprun->run(env + "apt-file update", "APT file update")) {
      kprun->exec();
   } 
  } else {
    KpMsg("Error",i18n("The %1 program needs to be installed").arg("apt-file"), TRUE);
  }
}

void DEBAPT::listPackages(QPtrList<packageInfo> *pki)
{
  if (hostName.isEmpty()) {
    listInstalledPackages(pki);
  } else {
    listRemotePackages(pki);
  }
  listAvail(pki);
  if (hostName.isEmpty() && packageLoc) {
    listUnIPackages(pki, packageLoc);
  }
}

void DEBAPT::listRemotePackages(QPtrList<packageInfo> *pki)
{
  listRPack(pki);
}

void DEBAPT::listRPack(QPtrList<packageInfo> *pki)
{
  int NLINES =  70000;

  packageInfo *p;
  QStringList  plist;

  kpackage->setStatus(i18n("Querying DEB APT remote package list: %1").arg(hostName));
  kpackage->setPercent(0);

  QString cmd = "cat " STATUS;

  QStringList list = kpty->run(cmd);
  kpackage->setStatus(i18n("Processing DEB APT remote package list: %1").arg(hostName));
  //  kdDebug() << "P=" << list.count() <<"\n";
  kpackage->setPercent(50);


  if (list.count() > 0) {

    QString s;

    kpackage->setPercent(50 );

    int cnt = 0;
    for ( QStringList::Iterator it = list.begin();  it != list.end(); ++it ) {
      cnt++;
      if (cnt % (NLINES/20) == 0) {
	kpackage->setPercent(((cnt * 100)/ NLINES ) + 50);
	//	kdDebug() << cnt << "_" << ((cnt * 100) / NLINES) <<"\n";
      }

      if (!(*it).isEmpty()) {
	s = *it;
	//	kdDebug() << s.length() << "<" << s << ">\n";
	plist << s;
      } else {
	//	kdDebug() << "---------\n";
	p = collectInfo(plist);
	if (p) {
	  if (!p->pkgInsert(pki, typeID, TRUE)) {
	    delete p;
	  }
	}
	plist.clear();
      }
    }
  }

  list.clear();
  kpackage->setStatus(i18n("DEB APT"));
  kpackage->setPercent(100);
}

void DEBAPT::listAvail(QPtrList<packageInfo> *pki)
{
  int NLINES =  150000;

  packageInfo *p;
  QStringList  plist;

  //  kdDebug() << "H=" << hostName << "\n";
  if (hostName.isEmpty())
    kpackage->setStatus(i18n("Querying DEB APT available list"));
  else
    kpackage->setStatus(i18n("Querying DEB APT available list: %1").arg(hostName));
  kpackage->setPercent(0);

  QStringList list = kpty->run("apt-cache dumpavail");
  if (hostName.isEmpty())
    kpackage->setStatus(i18n("Processing DEB APT available list"));
  else
     kpackage->setStatus(i18n("Processing DEB APT available list: %1").arg(hostName));

  //  kdDebug() << "A=" << list.count() <<"\n";
  kpackage->setPercent(50);

  if (list.count() > 0) {

    QString s;

    kpackage->setPercent(50 );

    int cnt = 0;
    for ( QStringList::Iterator it = list.begin();  it != list.end(); ++it ) {
      cnt++;
      if (cnt % (NLINES/20) == 0) {
	kpackage->setPercent(((cnt * 100)/ NLINES ) + 50);
      }

      if (!(*it).isEmpty()) {
	s = *it;
	plist << s;
      } else {
	p = collectInfo(plist);
	if (p) {
	  if (!p->pkgInsert(pki, typeID, FALSE)) {
	    delete p;
	  }
	}
	plist.clear();
      }
    }
  }

  list.clear();
  kpackage->setStatus(i18n("DEB APT"));
  kpackage->setPercent(100);
}

QStringList  DEBAPT::listInstalls(const QStringList &packs, bool install, bool &cancel)
{
  bool extras=FALSE, found=FALSE;

  QString match;
  QString s = "apt-get -s ";
  if (install) {
    s += "install ";
    match = " extra packages ";
  } else {
    match = "packages will be REMOVED:";
    s += "remove ";
  }

  for ( QStringList::ConstIterator it = packs.begin(); it != packs.end(); ++it ) {
    s +=  *it;
    s += " ";
  }

  QStringList list = kpty->run(s, TRUE, TRUE);
  if (!kpty->inSession) {
    cancel = TRUE; // Root login did not work
  } else {
    cancel = FALSE;
  }
  //  kdDebug() << "LS=" << list.count()  << "\n";

  QString packAll;
  for ( QStringList::ConstIterator it = list.begin(); it != list.end(); ++it ) {
    //        kdDebug() << "M=" << *it << "\n";
    if ((*it).find(match) >= 0 || extras) {
      if (extras) {
	if ((*it)[0] == ' ') {
	  packAll += *it;
	  found = true;
	} else {
	  break;
	}
      }
      extras=TRUE;
    }
  }

  if (!found) {
    QStringList nill;
    return nill;
  } else {
    QStringList plist = QStringList::split(' ',packAll);
    return plist;
  }
}

//////////////////////////////////////////////////////////////////////////////
QStringList DEBAPT::FindFile(const QString &name, bool searchAll)
{
  if (searchAll) {
    if (ifExe("apt-file") || !hostName.isEmpty()) {
      QString s = "apt-file search  ";
      s += name;

      QStringList filelist = kpty->run(s);

      for ( QStringList::Iterator it = filelist.begin(); it != filelist.end(); ++it ) {
       int p =  (*it).find(": ");
        if( p !=-1 )
          (*it).replace(p, 2, "\t");
      }

      if (filelist.count() == 1) {
       QStringList::Iterator it = filelist.begin();
        if ((*it).find("not found") >= 0) {
	 filelist.remove(it);
        }
      }

      return filelist;
    } else {
      KpMsg("Error",i18n("The %1 program needs to be installed").arg("apt-file"), TRUE);
      QStringList nill;
      return nill;
    }
  } else {
    return DEB::FindFile(name);
  }
}


QStringList DEBAPT::getFileList(packageInfo *p)
{
  QString fn( p->getFilename());
  if(!fn.isEmpty())
    return getUFileList(fn);
  else {
    if (hostName.isEmpty())
      return getIFileList(p);
    else {
      if (p->packageState == packageInfo::INSTALLED) {
	return getRFileList(p);
      } else
	return "";
    }
  }
}

 QStringList DEBAPT::getRFileList(packageInfo *p)
{
  QString from;
  QString name = p->getProperty("name");

  from = "cat " INFODIR;
  from += name;
  from += ".list";

  return kpty->run(from);
}

//////////////////////////////////////////////////////////////////////////////
QString DEBAPT::doUninstall(int uninstallFlags, const QString &packs, bool &test)
{
  QString s = env + "apt-get remove ";
  s += setOptions(uninstallFlags, paramsUninst);
  s +=  packs;

  kdDebug() << "uCMD=" << s << "\n";

  if (uninstallFlags>>2 & 1)
    test = TRUE;

  return s;
}

//////////////////////////////////////////////////////////////////////////////
QString DEBAPT::install(int installFlags, QPtrList<packageInfo> *p,
			      bool &test)
{
  QString packs = "";
  QString files = "";
  packageInfo *i;

  for (i = p->first(); i!= 0; i = p->next())  {
    QString file = i->getFilename();
    QString fname = i->fetchFilename();

    if (!file.isEmpty()) {
      files += KProcess::quote(file);
      files += " ";
    } else if (!fname.isEmpty()) {
      packs += KProcess::quote(fname);
      packs += " ";
    }
  }

  if (!files.isEmpty()) { // What if mixed?
    return DEB::doInstall(installFlags, files, test);
  } else {
    return doInstall(installFlags, packs, test);
  }
}

QString DEBAPT::doInstall(int installFlags, const QString &packs, bool &test)
{
  QString s = env + "apt-get install ";
  s += setOptions(installFlags, paramsInst);
  s +=  packs;

  kdDebug() << "iCMD=" << s << "\n";

  if ((installFlags>>0 & 1) || (installFlags>>5 & 1))
    test = TRUE;

  return s;
}


//////////////////////////////////////////////////////////////////////////////

QStringList DEBAPT::readApt()
{
  if (hostName.isEmpty()) {
    return readAptF();
  } else {
    return readAptS();
  }
}

QStringList DEBAPT::readAptS()
{
  QString cmd = "cat -E "  APT_SOURCE;

  QStringList list = kpty->run(cmd);
  if (!kpty->Result) {
    QString s;
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
      (*it).truncate((*it).length() - 1);
      (*it) = (*it).stripWhiteSpace();
    }
    return list;
  } else {
    return 0;
  }
}

QStringList DEBAPT::readAptF()
{
  QStringList lines;
  QFile file( "/etc/apt/sources.list" );
  if ( file.open( IO_ReadOnly ) ) {
    QTextStream stream( &file );
    QString line;
    while ( !stream.atEnd() ) {
      line = stream.readLine(); // line of text excluding '\n'
      line = line.stripWhiteSpace();
      lines += line;
    }
    file.close();
    return lines;
  } else {
    return 0;
  }
}

void DEBAPT::writeApt(const QStringList &list) {
  kdDebug() << "writeApt\n";
  QString cmd = "sh -c \"/bin/echo -e '";
  for ( QStringList::ConstIterator it = list.begin(); it != list.end(); ++it ) {
    QString s = *it;
    s.replace("\""," ");
    s.replace("'"," ");
    s.replace("!"," ");
    s.replace("`"," ");
    cmd += s;
    cmd += "\n";
  }
  cmd += "' > /etc/apt/sources.list.n; if [ $? = 0 ]; then ";
  cmd += "mv /etc/apt/sources.list /etc/apt/sources.list.b; mv /etc/apt/sources.list.n /etc/apt/sources.list; fi\" ";
  

 QStringList rlist =  kpty->run(cmd,TRUE,TRUE);
  //for ( QStringList::Iterator it = rlist.begin(); it != rlist.end(); ++it ) {
  // kdDebug() << "SL=" << *it << "\n";
  //}
}
  

#include "debAptInterface.moc"
