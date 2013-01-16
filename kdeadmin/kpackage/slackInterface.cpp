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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include <setjmp.h>

#include <qdir.h>
#include <qfileinfo.h>
#include <qregexp.h>

#include <kurl.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "packageInfo.h"
#include "slackInterface.h"
#include "updateLoc.h"
#include "kpackage.h"
#include "managementWidget.h"
#include "utils.h"
#include "procbuf.h"
#include "options.h"
#include "cache.h"
#include <klocale.h>


#define DIR "/var/log/packages/"
#define FILELIST "FILE LIST:\n"

enum {INITIAL, INSTALLED, UNINSTALLED};

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SLACK::SLACK():pkgInterface()
{
  head = "SLACK";
  name = i18n("Slackware");
  icon = "slack";

  pict = UserIcon(icon);
  updated_pict = UserIcon("supdated");
  new_pict = UserIcon("snew");

  packagePattern = "*.tgz *.tar.gz";
  typeID = "/slack";

  locatedialog = 0;

  queryMsg = i18n("Querying SLACK package list: ");
  procMsg = i18n("KPackage: Waiting on SLACK");

  locatedialog = new Locations(i18n("Location of Slackware Package Archives"));
  locatedialog->pLocations(1, 1, this,  i18n("Install location", "I"),
  "Slackware", "*.TXT *.txt *.tgz *.tar.gz",
  i18n("Location of a 'PACKAGES.TXT' File for Extended Information"));
  locatedialog->pLocations(4, 1, this,  i18n("Packages file", "P"),
  "Slackware", "*.tgz *.tar.gz",
  i18n("Location of 'PACKAGES.TXT' File for Slackware Distribution"),
  i18n("Location of Base Folder of Slackware Distribution"));
  locatedialog->dLocations(2, 6, this,  i18n("Folders", "F"),
  "Slackware", "*.tgz *.tar.gz",
  i18n("Location of Folders Containing Slackware Packages"));

  connect(locatedialog,SIGNAL(returnVal(LcacheObj *)),
  	  this,SLOT(setAvail(LcacheObj *)));
  locatedialog->apply_slot();

  paramsInst.append(new param(i18n("Test (do not install)"),FALSE,FALSE,"-warn"));

  paramsUninst.append(new param(i18n("Test (do not uninstall)"),FALSE,FALSE,"-warn"));

  hasProgram = ifExe("installpkg");

  initTranslate();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
SLACK::~SLACK()
{
}

//////////////////////////////////////////////////////////////////////////////
void SLACK::initTranslate()
{
  trl = new QDict<QString>(53);

  trl->insert("a",new QString(i18n("Base System")));
  trl->insert("ap",new QString(i18n("Linux Applications")));
  trl->insert("d",new QString(i18n("Program Development")));
  trl->insert("e",new QString(i18n("GNU EMacs")));
  trl->insert("f",new QString(i18n("FAQs")));
  trl->insert("k",new QString(i18n("Kernel Source")));
  trl->insert("n",new QString(i18n("Networking")));
  trl->insert("t",new QString(i18n("TeX Distribution")));
  trl->insert("tcl",new QString(i18n("TCL Script Language")));
  trl->insert("x",new QString(i18n("X Window System")));
  trl->insert("xap",new QString(i18n("X Applications")));
  trl->insert("xd",new QString(i18n("X Development Tools")));
  trl->insert("xv",new QString(i18n("XView and OpenLook")));
  trl->insert("y",new QString(i18n("Games")));
}

// check if slack file
bool SLACK::isType(char *buf, const QString &)
{
  if (hasProgram) {
    if ((unsigned char)buf[0] == 037 && (unsigned char)buf[1] == 0213 ) {
      return true;
    } else
      return false;
  } else {
    return false;
  }
}

bool SLACK::parseName(const QString &name, QString *n, QString *v)
{
  int  s1;
  s1 = name.findRev('.');
  if (s1 > 0) {
    *n = name.left(s1);
    v = new QString("");
    return TRUE;
  }
  return FALSE;
}

void SLACK::listPackages(QPtrList<packageInfo> *pki)
{
  QString s;
  cacheObj *cp;

  if (packageLoc) {
    for (cp = packageLoc->first(); cp != 0; cp = packageLoc->next()) {
      // first entry is special
      if (cp->cacheFile == "SLACK_0_0") {
	s = getPackList(cp);
	if (!s.isEmpty()) {
	  listPackList(pki, s, cp, INITIAL);
	}
      }
    }
  }

  listInstalledPackages(pki);

  if (packageLoc) {
    for (cp = packageLoc->first(); cp != 0; cp = packageLoc->next()) {
      if (cp->cacheFile == "SLACK_0_0") {
	// already done
      } else if ( !cp->base.isEmpty() ) {
	s = getPackList(cp);
	if (!s.isEmpty()) {
	  listPackList(pki, s, cp, UNINSTALLED);
	}
      } else {
	s = getDir(cp);
	if (!s.isEmpty()) {
	  listDir(pki,s,cp->location);
	}
      }
    }
  }
}

void SLACK::listInstalledPackages(QPtrList<packageInfo> *pki)
{
  FILE *file;
  char linebuf[1024];
  QString vb;
  packageInfo *p;
  QString fn, dr = DIR;

  QDir d(DIR);
  if (d.exists()) {
    QString sline = i18n("Querying SLACK package list: ");
    kpackage->setStatus(sline);

    const QFileInfoList *list = d.entryInfoList();
    int count = list->count();
    QFileInfoListIterator it( *list );      // create list iterator
    QFileInfo *fi;                          // pointer for traversing

    kpackage->setPercent(0);
    int cnt = 0;
    while ( (fi=it.current()) ) {           // for each file...
      int n = (cnt*100)/count;
      if (!(n % 5))
	kpackage->setPercent(n);

      if (!fi->isDir() && fi->isReadable()) {
	fn = dr + fi->fileName();
	file = fopen(QFile::encodeName(fn),"r");
	if (file) {
          vb = QString::null;
	  while (fgets(linebuf,sizeof(linebuf),file)) {
	    if (strcmp(linebuf,FILELIST)) {
	      vb += linebuf;
	    } else {
	      break;
	    }
	  }
	  fclose(file);
	  p = collectInfo(vb.ascii(), INSTALLED);
	  if (p) {
	    smerge(p);
	    if (!p->pkgInsert(pki, typeID, TRUE))
	      delete p;
	  }
	}
      }
      cnt++;
      ++it;                          // goto next list element
    }
    kpackage->setPercent(100);
  }
}

//////////////////////////////////////////////////////////////////////////////
void SLACK::listPackList(QPtrList<packageInfo> *pki, const QString &s,  cacheObj *cp, int insState)
{
  int np;
  QString vb;
  char linebuf[1024];
  FILE *file;
  packageInfo *p;

  QString sline = i18n("Querying SLACK package list: ");
  sline += cp->location;

  kpackage->setStatus(sline);
  kpackage->setPercent(0);

  np = 0;
  file= fopen(QFile::encodeName(s), "r");
  vb = "";

  if (file) {
    while (fgets(linebuf,sizeof(linebuf),file)) {
      int len = strlen(linebuf);
      if (len > 1) {
	if (linebuf[len - 2] == '\r') {
	  linebuf[len - 2] = '\n';
	  linebuf[len - 1] = 0;
	}
      }
      if (strcmp(linebuf,"\n")) {
	vb += linebuf;
      } else if ( !vb.isEmpty() ) {
	p = collectInfo(vb.ascii(), insState);
	if (p) {
	  if (!p->pkgInsert(pki, typeID, insState == INITIAL, insState == INITIAL)) {
	    delete p;
	  }  else if (cp && insState != INITIAL) {
	    p->info.insert("base", cp->base);
	  }
	  if (p && insState == INITIAL) {
	    p->packageState = packageInfo::NOLIST;
	    p->info.remove("summary");
	  }
	}
	vb.truncate(0);
      }
    }
    fclose(file);
  }
  kpackage->setPercent(100);
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// mode: i = query installed    u = query uninstalled
packageInfo *SLACK::getPackageInfo(char mode, const QString &name, const QString &)
{
  char linebuf[1024];
  packageInfo *pki = 0;
  QString vb, search, fn;
  QString n,v;
  FILE *file;

  switch(mode) {
    ////////////////////////////////////////////////////////////////////////
    // query an installed package!
  case 'i':
    fn = DIR + name;
    file = fopen(QFile::encodeName(fn),"r");
    if (file) {
      vb = QString::null;
      while (fgets(linebuf,sizeof(linebuf),file)) {
	if (strcmp(linebuf,FILELIST)) {
	  vb += linebuf;
	} else {
	  break;
	}
      }
      fclose(file);
      pki = collectInfo(vb.ascii(), INSTALLED);
      if (pki) {
	smerge(pki);
      }
     }
    break;

    ////////////////////////////////////////////////////////////////////
    // query an uninstalled package
  case 'u':
    QFile f(name);
    if (f.exists()) {
      QMap<QString, QString> a;

      a.insert("group", i18n("OTHER"));
      a.insert("filename", name);

      QFileInfo f(name);
      a.insert("name", f.baseName());

      QString st;
      st.setNum(f.size());
      a.insert("file-size", st);

      pki = new packageInfo(a,this);
      if (pki) {
	smerge(pki);
	pki->updated = TRUE;
      }
    }
    break;
  }
  return pki;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
packageInfo *SLACK::collectInfo(const char *_inp, int insState)
{
  QString stmp, fn = "";
  QMap<QString, QString> a;

  char *str, *xstr;
  QString qstr;

  char *inp = qstrdup(_inp);
  str = strtok(inp,"\n");

  if (str) {
    do {
      if (str[0] == 0)
	break;

      xstr = strchr(str,':');
      if (xstr) {
	*xstr++ = 0;
	xstr++;
	while (*xstr == ' ') {
	  xstr++;
	}

	for( int i = 0; str[ i ] != '\0'; ++i )
            str[ i ] = tolower( str[ i ] );

	if (*str == ' ')
	  str++;

	if (!strcmp("package name",str)) {
	  fn = xstr;
	  QString st = xstr;
	  if (st.right(4) == ".tgz")
	    a.insert("name", st.left(st.length() - 4));
	  else
	    a.insert("name", st);
	} else if (!strcmp("package description",str)) {
	  int i = 0;
	  QString qstr = "";

	  while ((str = strtok(NULL,"\n"))) {
	    xstr = strchr(str,':');
	    if (xstr) {
	      *xstr++ = 0;
	      if (*(xstr) != 0)
		xstr++;
	      while (*xstr == ' ') {
		xstr++;
	      }
	      if (i == 0) {
		a.insert("summary", xstr);
	      } else {
		if (!strcmp(xstr,"") && (i != 1)) {
		  qstr += "\n";
		} else {
		  qstr += xstr;
		  qstr += " ";
		}
	      }
	    }
	    i++;
	  }
	  a.insert("description", qstr);
	} else if (!strcmp("package location",str)) {
	  QString sl = xstr;
	  if (insState != INSTALLED) {
	    int sls = sl.findRev("/");
	    if (sls >= 0) {
	      QRegExp num("[0-9][0-9]*");
	      int slf = sl.find(num,sls);
	      if (slf >= 0) {
		sls++;
		QString gt = sl.mid(sls,slf-sls);
		if (trl->find(gt)) {
		  gt = *trl->find(gt);
		}
		a.insert("group",gt);
	      }
	    }
	    sl = sl.right(sl.length() - 2);
	    sl += "/";
	    sl += fn;
	  }
	  if (insState == UNINSTALLED) {
	    a.insert("filename", sl);
	  }
	} else if (!strcmp("section",str)) {
	  a.insert("group", xstr);
	} else if (!strcmp("compressed package size",str) ||
		   !strcmp("package size (compressed)",str)) {
	  QString stmp = xstr;
	  stmp.truncate(stmp.length() - 2);
	  stmp += "000";
	  a.insert("file-size", stmp);
	} else if (!strcmp("uncompressed package size",str) ||
		   !strcmp("package size (uncompressed)",str)) {
	  QString stmp = xstr;
	  stmp.truncate(stmp.length() - 2);
	  stmp += "000";
	  a.insert("size", stmp);
	} else {
	  a.insert(str, xstr);
	}
      }
    } while ((str = strtok(NULL,"\n")));
  }

  delete [] inp;

  if (a["name"].isEmpty()) {
    return 0;
  } else {
    packageInfo *i = new packageInfo(a,this);
    i->packageState = packageInfo::INSTALLED;
    i->fixup();
    return i;
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

QStringList SLACK::getChangeLog(packageInfo *) {
  return 0;
}

bool SLACK::filesTab(packageInfo *) {
  return TRUE;
}

bool SLACK::changeTab(packageInfo *) {
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////

QStringList SLACK::getFileList(packageInfo *p)
{
  char linebuf[1024];
  QString st, fn;
  FILE *file;
  QString name;
  char mode;

  fn = p->getFilename();
  if(!fn.isEmpty())
    mode = 'u';
  else
    mode = 'i';

  QStringList filelist;

  switch(mode) {
      ////////////////////////////////////////////////////////////////////////
      // query an installed package!
    case 'i':
      name = p->getProperty("name");

      fn = DIR + name;
      file = fopen(QFile::encodeName(fn),"r");
      if (file) {
	while (fgets(linebuf,sizeof(linebuf),file)) {
	  if (!strcmp(linebuf,FILELIST)) {
	    break;
	  }
	}
	while (fgets(linebuf,sizeof(linebuf),file)) {
	  st = "/";
	  st += linebuf;
	  st.truncate(st.length() -1);
	  if (st.left(8) != "/install") {
	    filelist.append(st);
	  }
	}
	fclose(file);
      }
      break;

      ////////////////////////////////////////////////////////////////////
      // query an uninstalled package
    case 'u':
           name = fn;

	   QString s = "sh -c 'cat ";
	   s += fn;
	   s += "|gunzip |tar -t -f -'";

	   filelist =  kpty->run(s);
      break;
    }

  return filelist;
}

//////////////////////////////////////////////////////////////////////////////
// Call the script to uninstall packages setting parameters
// to slack dependent on flags, returning whether everyting worked
//////////////////////////////////////////////////////////////////////////////
QString SLACK::doUninstall(int uninstallFlags, const QString &packs, bool &)
{
  QString s = "removepkg ";
  s += setOptions(uninstallFlags, paramsUninst);
  s +=  packs;

  kdDebug() << "uCMD=" << s << "\n";

  return  s;
}

//////////////////////////////////////////////////////////////////////////////
// Call the script to install packages setting parameters
// to slack dependent on flags, returning whether everyting worked
//////////////////////////////////////////////////////////////////////////////
QString SLACK::install(int installFlags, QPtrList<packageInfo> *plist, bool &test)
{
  packageInfo *pk;
  int i = 0;
  QString packs = "";
  for (pk = plist->first(); pk != 0; pk = plist->next()) {
    QString fname = pk->fetchFilename();
    if ( !fname.isEmpty() ) {
      packs += fname + " ";
      i++;
    }
  }
  return doInstall(installFlags, packs, test);
}

QString SLACK::doInstall(int installFlags, const QString &packs, bool &)
{

 QString s = "installpkg ";
 s += setOptions(installFlags, paramsInst);
 s +=  packs;

  kdDebug() << "iCMD=" << s << "\n";

  return  s;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
QStringList SLACK::FindFile(const QString &name, bool)
{
  FILE *file;
  char linebuf[1024];
  QString buf, st;
  QString fn, dr = DIR;
  QStringList filelist;

  QDir d(DIR);
  if (d.exists()) {
    QString sline = i18n("Querying SLACK package list: ");
    kpackage->setStatus(sline);

    const QFileInfoList *list = d.entryInfoList();
    int count = list->count();
    QFileInfoListIterator it( *list );      // create list iterator
    QFileInfo *fi;                          // pointer for traversing

    kpackage->setPercent(0);
    int cnt = 0;
    while ( (fi=it.current()) ) {           // for each file...
      int n = (cnt*100)/count;
      if (!(n % 5))
	kpackage->setPercent(n);

      if (!fi->isDir() && fi->isReadable()) {
	fn = dr + fi->fileName();
	file = fopen(QFile::encodeName(fn),"r");
	if (file) {
	  while (fgets(linebuf,sizeof(linebuf),file)) {
	    if (!strcmp(linebuf,FILELIST)) {
	      break;
	    }
	  }
	  while (fgets(linebuf,sizeof(linebuf),file)) {
	    if (QString::fromLocal8Bit(linebuf).find(name) != -1) {
	      st = "/";
	      st += linebuf;
	      st.truncate(st.length() -1);
	      if (st.left(8) != "/install") {
		QString s = fi->fileName();
		s += "\t";
		s += st;
		filelist.append(s);
	      }
	    }
	  }
	  fclose(file);
	}
      }
      cnt++;
      ++it;                          // goto next list element
    }
  }
  return filelist;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SLACK::setLocation()
{
    locatedialog->restore();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void SLACK::setAvail(LcacheObj *slist)
{
    delete packageLoc;
    packageLoc = slist;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void SLACK::smerge(packageInfo *p)
{
  p->smerge(typeID);
}
#include "slackInterface.moc"
