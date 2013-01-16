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
#include "debInterface.h"
#include "updateLoc.h"
#include "kpackage.h"
#include "managementWidget.h"
#include "utils.h"
#include "options.h"
#include "cache.h"
#include <klocale.h>

extern KApplication *app;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DEB::DEB():pkgInterface()
{
  head = "DEB";
  icon = "deb";

  pict = UserIcon(icon);
  bad_pict = UserIcon("dbad");
  updated_pict = UserIcon("dupdated");
  new_pict = UserIcon("dnew");

  packagePattern = "*.deb";
  typeID = "/deb";

  locatedialog = 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
DEB::~DEB()
{
}

//////////////////////////////////////////////////////////////////////////////

// check if debian file
bool DEB::isType(char *buf, const QString &)
{
  if (hasProgram) {
    if  (!strcmp(buf,"!<arch>\n")) {
      return true;
    } else if (!strncmp(buf,"0.9",3)) {
      return true;
    } else
      return false;
  } else {
    return false;
  }
}

void DEB::distPackages(QPtrList<packageInfo> *, cacheObj *)
{
}

void DEB::listUnIPackages(QPtrList<packageInfo> *pki, LcacheObj *pCache)
{
  QString s;
  cacheObj *cp;

  for (cp = pCache->first(); cp != 0; cp = pCache->next()) {
    kdDebug() << cp->base << ":: " << cp->option << "\n";
    if (!cp->option.isEmpty()) {
      distPackages(pki, cp);
    } else if (!cp->base.isEmpty()) {
      s = getPackList(cp);
      if (!s.isEmpty()) {
	listPackList(pki,s,cp);
      }
    } else {
      s = getDir(cp);
      if (!s.isEmpty()) {
	listDir(pki,s,cp->location,cp->subdirs);
      }
    }
  }
}

bool DEB::parseName(const QString &name, QString *n, QString *v)
{
  int d1, d2, s1;

  s1 = name.findRev('.');
  if (s1 > 0) {
      d2 = name.findRev('-',s1-1);
      if (d2 > 0) {
	d1 = name.findRev('_',d2-1);
	if (d1 < 0)
	  d1 = d2;
	*n = name.left(d1);
	*v = name.mid(d1+1,s1-d1-1);
	return TRUE;
      }
  }
  return FALSE;
}

void DEB::listInstalledPackages(QPtrList<packageInfo> *pki)
{
  listPackList(pki,STATUS,0);
}

void DEB::listPackList(QPtrList<packageInfo> *pki, const QString &fname, cacheObj *cp)
{
  bool local = FALSE;
  packageInfo *p;
  QStringList list;
  QString sline( i18n("Querying DEB package list: ")+fname );

  if (cp) {
    KURL u(cp->base);
    local = u.isLocalFile();
  }

  kpackage->setStatus(sline);
  kpackage->setPercent(0);

  QFile file(STATUS);
  QString s;

  bool fileOpened= file.open(IO_ReadOnly);
  if (fileOpened) {
    QTextStream stream( &file );
    s = "";
    while ( !s.isNull() ) {
      s = stream.readLine();
      if ( !s.isEmpty() ) {
	list << s;
      } else {
	p = collectInfo(list);
	if (p) {
	  if (!p->pkgInsert(pki, typeID, cp == 0)) {
	    delete p;
	  } else if (cp) {
	    p->info.insert("base", cp->base);
	  }
	}
	list.clear();
      }
    }
    file.close();
  }

  kpackage->setPercent(100);
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
packageInfo *DEB::getPackageInfo(char mode, const QString &name, const QString &)
{
  if (mode == 'i') {
    if (hostName.isEmpty()) {
      return getIPackageInfo(name);
    } else {
      return getIRPackageInfo(name);
    }
  } else
    return getUPackageInfo(name);
}

packageInfo *DEB::getIPackageInfo( const QString &name)
{
  // query an installed package!
  packageInfo *pki = 0;
  QString search;
  QStringList list;

  search = "Package: "+ name;

  QFile f(STATUS);
  if ( f.open(IO_ReadOnly) ) {
    QTextStream t( &f );
    QString s;
    while ( !t.eof() ) {
      s = t.readLine();

      if ( s == search) {
	list << s;
	break;
      }
    }

    while ( !t.eof() ) {
      s = t.readLine();
      if (s.length()) {
	list << s;
      } else {
	pki = collectInfo(list);
	break;
      }
    }
  }
  f.close();
  return pki;
}

packageInfo *DEB::getIRPackageInfo( const QString &name)
{
  // query an remote installed package
  packageInfo *pki = 0;

  QString s = "dpkg --status ";
  s += name;
  QStringList list =  kpty->run(s);
  for ( QStringList::Iterator it = list.begin();  it != list.end(); ++it ) {
    //    kdDebug() << "U=" << *it << "\n";
    if ((*it).find("Package:") >= 0) {
      kdDebug() << "found\n";
      while (it != list.begin()) {
	list.remove(list.begin());
      }
      break;
    }
  }

  if (list.count() > 1) {
    pki = DEB::collectInfo(list);
    if (pki) {
      pki->updated = TRUE;
      if (pki->getFilename().isEmpty())
	pki->setFilename(name);
    }
  }

  return pki;
}

packageInfo *DEB::getUPackageInfo( const QString &name)
{
  // query an uninstalled package
  packageInfo *pki = 0;

  QString s = "dpkg --info ";
  s += KProcess::quote(name);

  QStringList list =  kpty->run(s);
  for ( QStringList::Iterator it = list.begin();  it != list.end(); ++it ) {
    //    kdDebug() << "U=" << *it << "\n";
    if ((*it).find("Package:") >= 0) {
      //      kdDebug() << "found\n";
      while (it != list.begin()) {
	list.remove(list.begin());
      }
      break;
    }
  }

  if (list.count() > 1) {
    pki = DEB::collectInfo(list, kpinterface[0]); // To be fixed up later, assumes order of kpinterface
    if (pki)
      pki->updated = TRUE;
  }

  return pki;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
packageInfo *DEB::collectInfo(QStringList &ln, pkgInterface *pkgInt)
{
  if (!pkgInt) {
    pkgInt = this;
  }

  QMap<QString, QString> a;

  QString key, val;
  bool bad_install = FALSE;
  bool available = FALSE;
  bool haveName = FALSE;

  for ( QStringList::Iterator it = ln.begin();  it != ln.end(); ++it ) {
  loop:
    int col = (*it).find(':');
    key = ((*it).left(col)).lower();
    if (key[0] == ' ') {
      key.remove(0,1);
    }
    val = (*it).mid(col+2);

    //    val.truncate(val.length() - 1);

    if (key == "conffiles") {
      while (++it != ln.end()) {
	if ((*it)[0] == ' ') {
	} else {
	  goto loop;
	}
      }
    } else if (key == "description") {
      a.insert("summary", val);
      QString desc;
      while (++it != ln.end()) {
	if ((*it)[0] == ' ') {
	  desc += *it;
	} else {
	  a.insert("description", desc);
	  goto loop;
	}
      }
      a.insert("description", desc);
      break;
    } else if (key == "package") {
      a.insert("name", val);
      haveName = TRUE;
    } else if (key == "md5sum") {
      available = TRUE;
      bad_install = FALSE;
    } else if (key == "section") {
      a.insert("group",  val);
    } else if (key == "status") {
      if ((val.find("not-installed")  >= 0) || (val.find("config-files") >= 0)) {
	return 0;
      }
      if (val != "install ok installed" &&
	  val != "deinstall ok installed" &&
	  val != "deinstall ok config-files" &&
	  val != "purge ok installed") {
	bad_install = TRUE;
      }
      a.insert("status", val);
    } else if (key == "version") {
      a.insert("version", val);
    } else if (key == "size") {
      a.insert("file-size", val);
    } else if (key == "installed-size") {
      a.insert("size",  val + "000");
    } else {
       a.insert(key, val);
    }
    //    kdDebug() << "C=" << key << "," << val <<"\n";
  }

  if (haveName) {
    packageInfo *i = new packageInfo(a,pkgInt);
    if (bad_install) {
      i->packageState = packageInfo::BAD_INSTALL;
    } else if (available) {
      i->packageState = packageInfo::AVAILABLE;
    } else {
      i->packageState = packageInfo::INSTALLED;
    }
    i->fixup();
    return i;
  } else {
    return 0;
  }
}

//////////////////////////////////////////////////////////////////////////////

QStringList DEB::getChangeLog(packageInfo *p) {
  QString fn( p->getFilename());
  if(!fn.isEmpty())
    return 0;
  else
    return getIChangeLog(p);
}

QStringList DEB::getIChangeLog(packageInfo *p)
{
  QString from;
  QStringList ret;
  QString name = p->getProperty("name");

  from = "zcat /usr/share/doc/";
  from += name;
  from += "/changelog.Debian.gz";

  ret = kpty->run(from);

  if (!kpty->Result)
    return ret;
  else {
    from = "zcat /usr/share/doc/";
    from += name;
    from += "/changelog.gz";

    ret = kpty->run(from);
    if (!kpty->Result)
      return ret;
    else
      return 0;
  }
}

bool DEB::filesTab(packageInfo *p) {
  if (p->packageState == packageInfo::INSTALLED) {
    return true;
  } else if (p->isFileLocal()) {
    return true;
  }
  return false;
}

bool DEB::changeTab(packageInfo *p) {
  if (p->packageState == packageInfo::INSTALLED) {
    return true;
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////

QStringList DEB::getFileList(packageInfo *p)
{
  QString fn( p->getFilename());
  if(!fn.isEmpty())
    return getUFileList(fn);
  else
    return getIFileList(p);
}

// query an installed package
QStringList DEB::getIFileList(packageInfo *p)
{
  FILE *file;
  QString name = p->getProperty("name");
  QStringList filelist;

  QString vb( INFODIR + name + ".list");
  file= fopen(vb.ascii(),"r");

  if (file) {
    char linebuf[1024];
    while (fgets(linebuf,sizeof(linebuf),file)) {
      linebuf[strlen(linebuf) - 1] = 0;    // remove new line
      filelist.append(linebuf);
    }
    fclose(file);
  }
  return filelist;
}

// query an uninstalled package
QStringList DEB::getUFileList(const QString &fn)
{
  QString s = "dpkg --contents ";
  s += "'";
  s += fn;
  s += "'";

  QStringList filelist = kpty->run(s);

  int pt = -1;
  for ( QStringList::Iterator it = filelist.begin();
	it != filelist.end(); ++it ) {
    //    kdDebug() << "F=" << *it << "\n";
    if (pt < 0) {
      pt = (*it).findRev(' ');
    }
    (*it) = (*it).mid(pt + 1);
  }
  return filelist;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
QStringList DEB::FindFile(const QString &name, bool)
{
  QString s = "dpkg -S ";
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
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void DEB::setLocation()
{
    locatedialog->restore();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void DEB::setAvail(LcacheObj *slist)
{
  if (packageLoc)
    delete packageLoc;
  packageLoc = slist;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
QString DEB::uninstall(int uninstallFlags, QPtrList<packageInfo> *p,
				bool &test)
{
  QString packs = "";
  packageInfo *i;

  for (i = p->first(); i!= 0; i = p->next())  {
    packs += i->getProperty("name");
    packs += " ";
  }
  return doUninstall( uninstallFlags, packs, test);
}

//////////////////////////////////////////////////////////////////////////////
QString DEB::install(int installFlags, QPtrList<packageInfo> *p,
			      bool &test)
{
  QString packs = "";
  packageInfo *i;

  for (i = p->first(); i!= 0; i = p->next())  {
    QString fname = i->fetchFilename();
    if (!fname.isEmpty()) {
      packs += KProcess::quote(fname);
      packs += " ";
    }
  }
  return doInstall(installFlags, packs, test);
}

//////////////////////////////////////////////////////////////////////////////
// Call the script to install packages setting parameters
// to dpkg dependent on flags, returning whether everyting worked
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

QString DEB::doInstall(int installFlags, const QString &packs, bool &test)
{
  QString s = "dpkg -i ";
  s += setOptions(installFlags, paramsInst);
  s +=  packs;

  kdDebug() << "iCMD=" << s << "\n";

  if (installFlags>>4 & 1)
    test = 1;

  return s;
}

#include "debInterface.moc"
