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

#include <klocale.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kiconloader.h>

#include "kpackage.h"
#include "pkgInterface.h"
#include "options.h"
#include "cache.h"
#include "updateLoc.h"
#include "kio.h"

extern Opts *opts;
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
param::param(const QString &nameP, bool initP,  bool invertP, const QString &flagP)
{
  name = nameP;
  init = initP;
  invert = invertP;
  flag = flagP;
  flagA = "";
}

param::param(const QString &nameP, bool initP,  bool invertP,  const QString &flagP,  const QString &flagAP )
{
  name = nameP;
  init = initP;
  invert = invertP;
  flag = flagP;
  flagA = flagAP;

}

param::~param()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
pkgInterface::pkgInterface( ) : QObject(), new_pict(), updated_pict()
{
  packageLoc = 0;

  DELMSG = i18n("'Delete this window to continue'");

  folder = SmallIcon("folder");
  markInst = UserIcon("tick");
  markUnInst = UserIcon("noball");
  bad_pict = UserIcon("dbad");

  hasRemote = FALSE;
  defaultHandle = 1;
  noFetch = FALSE;
  hasSearchAll = FALSE;
}

//////////////////////////////////////////////////////////////////////////////
pkgInterface::~pkgInterface()
{
  //  if (locatedialog)
  //    delete locatedialog;
  //  if (packageLoc)
  //    delete packageLoc;
}

//////////////////////////////////////////////////////////////////////////////
void pkgInterface::makeMenu(KActionCollection *)
{
}

void pkgInterface::setMenu(KActionCollection*, bool )
{
}

//////////////////////////////////////////////////////////////////////////////
QStringList pkgInterface::depends(const QString &, int ) {return 0;}

QString pkgInterface::doUninstall(int, const QString &, bool &) {return 0;}
QString pkgInterface::doInstall(int, const QString &, bool &) {return 0;}

////////////////////////////////////////////////////////////////////////////

bool pkgInterface::ifExe(QString exe) {
  if (!KGlobal::dirs()->findExe( exe ).isNull()) {
    return TRUE;
  } else {
    kdDebug() << "Program not found: " << exe << "\n";
    errExe = exe;
    return FALSE;
  }
}


void pkgInterface::listPackages(QPtrList<packageInfo> *pki)
{
  listInstalledPackages(pki);
  if (packageLoc) {
    for (cacheObj *cp = packageLoc->first(); cp != 0; cp = packageLoc->next()) {
      QString s = getDir(cp);
      if (!s.isEmpty())
          listDir(pki, s, cp->location, cp->subdirs);
    }
  }
}

void pkgInterface::smerge(packageInfo *)
{ }

void pkgInterface::listDir(QPtrList<packageInfo> *pki, const QString &fname, const QString &dir, bool subdirs)
{
  //  fname - path to directory or cached remote infromation file
  //  dir - url of directory

  QString name, size, rfile;
  packageInfo *p;

  QString sline( queryMsg + fname );
  kpackage->setStatus(sline);

  kdDebug() << "listDir fn=" << fname << " dir=" << dir << endl;

  QDir d(fname,packagePattern);

  if (subdirs)
    d.setMatchAllDirs( TRUE );        // list contains subdirs
  else
    d.setMatchAllDirs( FALSE );       // list contains no subdirs

  if (d.exists()) {
    if ( d.isReadable() ) {
      QString pn;
      const QFileInfoList *list = d.entryInfoList();
      QFileInfoListIterator it( *list );      // create list iterator
      QFileInfo *fi;                          // pointer for traversing

      while ( (fi=it.current()) ) {           // for each entry...
	if ( fi->isDir() ) {
	  // entry is a subdir
	  if ( fi->fileName() != QString::fromLatin1(".") &&
	       fi->fileName() != QString::fromLatin1("..") )
	    {
	      // not current dir and not parent dir
	      // -> recursive call:
	      listDir( pki, dir + "/" + fi->fileName(), dir + "/" + fi->fileName(), subdirs );
	    } else {
	      // current dir or parent dir
	      // -> notihng to do
	      ;
	    }
	} else {
	  // entry is a file
	  if (opts->PkgRead) {
	    rfile = fname + "/";
	    rfile += fi->fileName();
	    p = getPackageInfo('u',rfile, 0);
	    if (p) {
	      p->info.insert("filename", fi->fileName());
	      p->info.insert("base", dir);
	    }
	  } else {
	    p = collectDir(fi->fileName(),pn.setNum(fi->size()),dir);
	  }
	  if (p) {
	    smerge(p);
	    if (!p->pkgInsert(pki, typeID, FALSE))
	      delete p;
	  }
	}
	++it;                               // goto next list element
      }
    } else {
      // directory is not readable
      kdDebug() << QString("WARNING: directory '%1' not readable (will be ignored) !\n").arg(d.absPath() ) << endl;
    }
  } else {
      QFile f(fname);
      if ( f.open(IO_ReadOnly) ) {
	QTextStream t( &f );
	QString name;
	while ( !t.eof() ) {
	  name = t.readLine();
	  if (!t.eof() ) {
	    size = t.readLine();
	  } else
	    size = "";
	  packageInfo *p = collectDir(name,size,dir);
	  if (p) {
	    smerge(p);
	    if (!p->pkgInsert(pki, typeID, FALSE))
	      delete p;
	  }
	}
	f.close();
      }
    }
  }

packageInfo *pkgInterface::collectDir(const QString &name, const QString &size, const QString &dir)
{
    kdDebug() << "collectDir " << name << " " << size << " " << dir << endl;
    QString n,v;

  if (parseName(name, &n, &v)) {
    QMap<QString, QString> a;

    a.insert("group", "NEW");
    a.insert("name", n);
    a.insert("version", v);
    a.insert("file-size", size);
    a.insert("filename", name);
    a.insert("base", dir);

    packageInfo *i = new packageInfo(a,this);
    i->packageState = packageInfo::AVAILABLE;
    //    i->packageState = packageInfo::NEW;
    return i;
    }
  return 0;
}

QString pkgInterface::getPackList(cacheObj *cp)
{
  QString tmpf;
  int res;
  QString url = cp->location;
  kdDebug() << "pkgInterface::getPackList " << url << " " << cp->cacheFile << "\n";
  if ((res = cacheObj::newDCache(url, cp->cacheFile, tmpf))) {
    if (res < 0)
      return 0;

    unlink(QFile::encodeName(tmpf));
    if (kpkg)
      kpackage->setStatus(i18n("Starting Kio"));

    Kio kio;
    if (kio.download(url, tmpf)) {
      if (kpkg)
	kpackage->setStatus(i18n("Kio finished"));
      QFileInfo f(tmpf);
      if (!(f.exists() && f.size() > 0)) {
	unlink(QFile::encodeName(tmpf));
	return "";
      } else {
	return tmpf;
      }
    } else {
      if (kpkg)
	kpackage->setStatus(i18n("Kio failed"));
      return "";
    }
  } else {
    return tmpf;
  }
}

QString pkgInterface::getDir(cacheObj *cp) {
  int res;
  QString tmpDir;
  QString url = cp->location;

  if ((res = cacheObj::newDCache(url, cp->cacheFile, tmpDir))) {
    if (res < 0)
      return QString::null;

    Kiod kiod;
    if (kiod.listDir(url,tmpDir, cp->subdirs)) {
      QFileInfo fi (tmpDir);
      CacheList cl (fi.dirPath());
      cl.append (fi.fileName());
      cl.write();
      return tmpDir;
    } else {
      KpMsgE(i18n("Cannot read folder %1").arg(url),FALSE);
      unlink(tmpDir.ascii());
      return QString::null;
    }
  } else {
      return tmpDir;
  }
}


//////////////////////////////////////////////////////////////////////////////

QString  pkgInterface::provMap(const QString &p)
{
  //  kdDebug() << "provMap=>" << p << endl;
  return p;
}

//////////////////////////////////////////////////////////////////////////////
QStringList pkgInterface::verify(packageInfo *, const QStringList &files)
{
  int  p = 0;
  uint c = 0;
  QStringList errorlist;
  QDir d;

  if (hostName.isEmpty()) {

    uint step = (files.count() / 100) + 1;

    kpackage->setStatus(i18n("Verifying"));
    kpackage->setPercent(0);

    for( QStringList::ConstIterator it = files.begin();
	 it != files.end();
	 it++)
      {
	// Update the status progress
	c++;
	if(c > step) {
	  c=0; p++;
	  kpackage->setPercent(p);
	}

	if (!d.exists(*it)) {
	  errorlist.append(*it);
	}
      }

    kpackage->setPercent(100);
  }
  return errorlist;
}

//////////////////////////////////////////////////////////////////////////////
QString pkgInterface::uninstall(int uninstallFlags, packageInfo *p, bool &test)
{
  QString packs( p->getProperty("name"));

  return doUninstall(uninstallFlags, packs, test);
}

//////////////////////////////////////////////////////////////////////////////
QString pkgInterface::uninstall(int uninstallFlags, QPtrList<packageInfo> *p, bool &test)
{
  QString packs;
  packageInfo *i;

  for (i = p->first(); i!= 0; i = p->next())  {
    packs += i->getProperty("name");
    packs += " ";
  }
  return doUninstall( uninstallFlags, packs, test);
}
//////////////////////////////////////////////////////////////////////////////

QString pkgInterface::install(int installFlags, packageInfo *p, bool &test)
{
  QString fname = p->fetchFilename();

  return doInstall(installFlags, fname, test);
}

//////////////////////////////////////////////////////////////////////////////
QString pkgInterface::install(int installFlags, QPtrList<packageInfo> *p, bool &test)
{
  QString packs = "";
  packageInfo *i;

  for (i = p->first(); i!= 0; i = p->next())  {
    QString fname = i->fetchFilename();
    if (!fname.isEmpty()) {
      packs += fname;
      packs += " ";
    }
  }
  return doInstall(installFlags, packs, test);
}

//////////////////////////////////////////////////////////////////////////////
QStringList  pkgInterface::listInstalls(const QStringList &packs, bool , bool &cancel)
{
  cancel = FALSE;
  return packs;
}

//////////////////////////////////////////////////////////////////////////////

QString pkgInterface::setOptions(int flags, QPtrList<param> &params)
{
  int i;
  QString s;

  param *p;
  i = 0;
  for ( p=params.first(); p != 0; p=params.next(), i++ ) {
    if ((flags>>i & 1) ^ p->invert) {
      s += p->flag + " ";
    } else {
      if (!p->flagA.isEmpty())
	s += p->flagA + " ";
    }
  }
  return s;
}

 QStringList  pkgInterface::readApt()
{
  return 0;
}

 void  pkgInterface::writeApt(const QStringList &)
{
}

#include "pkgInterface.moc"
