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

#include <kurl.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "packageInfo.h"
#include "kissInterface.h"
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
KISS::KISS():pkgInterface()
{
  head = "KISS";
  name = i18n("KISS");
  icon = "kiss";

  pict = UserIcon(icon);
  updated_pict = UserIcon("kupdated");
  new_pict = UserIcon("knew");

  packagePattern = "*.installer";
  typeID = "/kiss";

  locatedialog = new Locations(i18n("Location of KISS Packages"));
  locatedialog->dLocations(2, 6, this,  i18n("Folders", "F"),
  "KISS", "*.installer",
  i18n("Location of Folders Containing KISS Packages"));

  connect(locatedialog,SIGNAL(returnVal(LcacheObj *)),
         this,SLOT(setAvail(LcacheObj *)));
  locatedialog->apply_slot();

  queryMsg = i18n("Querying KISS package list: ");
  procMsg = i18n("KPackage: Waiting on KISS");

  param paramsInst[] = {
			param(0,FALSE,FALSE,0)
  };

  param paramsUninst[] = {
			  param(0,FALSE,FALSE,0)
  };
  hasProgram = ifExe("kiss");
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
KISS::~KISS()
{
}

//////////////////////////////////////////////////////////////////////////////
// check if kiss file
bool KISS::isType(char *buf, const QString &)
{
  if (hasProgram) {
    QString tmp = buf;
    if (tmp.find("perl",0,false) >= 0)
      return true;
    else
      return false;
  } else {
    return false;
  }
}

bool KISS::parseName(const QString& name, QString *n, QString *v)
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

void KISS::listInstalledPackages(QPtrList<packageInfo> *pki)
{
  QString vb;
  packageInfo *p;

  QString sline = i18n("Querying KISS package list: ");

  reader.setup("kiss");
  *reader.proc << "-qq";
  if (!reader.start(0,FALSE))
    return;

  kpackage->setStatus(sline);
  kpackage->setPercent(0);

  vb = "" ;

  int sc, sp = 0;
  while ((sc  = reader.buf.find("\n\n",sp)) >= 0) {
    if (sc+1 == (signed int)reader.buf.length())
      break;
    p = collectInfo(reader.buf.mid(sp,sc-sp).ascii());
    if (p) {
      if (!p->pkgInsert(pki, typeID, TRUE)) {
	delete p;
      }
    }
    sp = sc + 2;
  }

  kpackage->setPercent(100);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// mode: i = query installed    u = query uninstalled
packageInfo *KISS::getPackageInfo(char mode, const QString &name, const QString &)
{
  packageInfo *pki = 0;
  QString vb,search;

  switch(mode)
    {
      ////////////////////////////////////////////////////////////////////////
      // query an installed package!
    case 'i':
      reader.setup("kiss");
      *reader.proc << "-q" << name;
      if (reader.start(0,FALSE)) {
	reader.buf += "package: " + name + "\n";
	pki = collectInfo(reader.buf.ascii());
      }
      break;

      ////////////////////////////////////////////////////////////////////
      // query an uninstalled package
    case 'u':
      reader.setup("perl");
      *reader.proc << name << "-q";
      if (reader.start(0,TRUE)) {
	pki = collectInfo(reader.buf.ascii());

	QFileInfo fi(name);
	QString s;
	s.setNum(fi.size());
	pki->info.insert("file-size", s);
      }
      break;
    }
  return pki;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
packageInfo *KISS::collectInfo(const char *_inp)
{
  QMap<QString, QString> a;

  char *str, *xstr;
  QString qstr;

  char *inp = qstrdup(_inp);
  str = strtok(inp,"\n");
  do {
    xstr = strchr(str,':');
    if (*str == ' ')
      str++;
    if (!strncmp("package",str,7))
      break;
  } while ((str = strtok(NULL,"\n")));

  // parse 'name: text' elements

  if (str) {
    do {
      if (str[0] == 0)
	break;

    xstr = strchr(str,':');
    if (xstr) {
      *xstr++ = 0;
      xstr++;

      for( int i = 0; str[ i ] != '\0'; ++i )
        str[ i ] = tolower( str[ i ] );

      if (*str == ' ')
	str++;

      if (!strcmp("package",str)) {
	a.insert("name", xstr);
      } else if (!strcmp("name",str)) {
	a.insert("summary", xstr);
      } else if (!strcmp("section",str)) {
	a.insert("group", xstr);
      } else if (!strcmp("size",str)) {
	a.insert("file-size", xstr);
      } else if (!strcmp("installed-size",str)) {
	QString str = xstr;
	a.insert("size", str + "000");
      } else {
	a.insert(str, xstr);
      }
    }
    } while ((str = strtok(NULL,"\n")));
  }

  packageInfo *i = new packageInfo(a,this);
  i->packageState = packageInfo::INSTALLED;
  i->fixup();
  delete [] inp;
  return i;

}

//////////////////////////////////////////////////////////////////////////////

QStringList KISS::getChangeLog(packageInfo *) {
  return 0;
}


bool KISS::filesTab(packageInfo *) {
  return TRUE;
}

bool KISS::changeTab(packageInfo *) {
    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
QStringList KISS::getFileList(packageInfo *p)
{
  QString vb, fn;
  QString name;
  char mode;

  fn = p->getFilename();
  if(!fn.isEmpty())
    mode = 'u';
  else
    mode = 'i';

  QStringList filelist;

  switch(mode)
    {
      ////////////////////////////////////////////////////////////////////////
      // query an installed package!
    case 'i':
      name = p->getProperty("name");

      reader.setup("kiss");
      *reader.proc << "-f" << name;
      if (reader.start(0,FALSE)) {
	char *buffer = qstrdup(reader.buf.ascii());
	char *str = strtok(buffer,"\n");
	if (str) {
	  do {
	    filelist.append(str);
	  } while ((str = strtok(NULL,"\n")));
	}
	delete [] buffer;
      }
      break;

      ////////////////////////////////////////////////////////////////////
      // query an uninstalled package
    case 'u':
      reader.setup("perl");
      *reader.proc << fn << "-f";
      if (reader.start(0,TRUE)) {
	char *buffer = qstrdup(reader.buf.ascii());
	char *str = strtok(buffer,"\n");
	if (str) {
	  do {
	    filelist.append(strdup(str));
	  } while ((str = strtok(NULL,"\n")));
	}
	delete [] buffer;
      }
      break;
    }

  return filelist;
}

//////////////////////////////////////////////////////////////////////////////
// Call the script to install packages setting parameters
// to kiss dependent on flags
//////////////////////////////////////////////////////////////////////////////
QString KISS::uninstall(int uninstallFlags, QPtrList<packageInfo> *plist, bool &test)
{
  QString packs;
  packageInfo *pk;

  for (pk = plist->first(); pk != 0; pk = plist->next()) {
    packs = pk->getProperty("name");
    doUninstall(uninstallFlags, packs, test);
  }
  return 0;
}

QString KISS::doUninstall(int, const QString &packs, bool &)
{
  return "kiss -d " + packs;
}

//////////////////////////////////////////////////////////////////////////////
// Call the script to install packages setting parameters
// to kiss dependent on flags, returning whether everyting worked
//////////////////////////////////////////////////////////////////////////////
QString KISS::install(int installFlags, QPtrList<packageInfo> *plist, bool &test)
{
  packageInfo *pk;
  int i = 0;
  for (pk = plist->first(); pk != 0; pk = plist->next()) {
    QString fname = pk->fetchFilename();
    if (!fname.isEmpty()) {
      doInstall(installFlags, fname, test);
      i++;
    }
  }
  return 0;
}

QString KISS::doInstall(int, const QString &packs, bool &)
{
  return "perl " + packs;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
QStringList KISS::FindFile(const QString &name, bool)
{
  QString s = "kiss -p ";
  s += name;

  QStringList filelist;
  //  filelist = kpty->run(s);

  //  for ( QStringList::Iterator it = filelist.begin(); it != filelist.end(); ++it ) {
  //       *it = *it + '\t' + name ;
  //  }

  return filelist;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void KISS::setLocation()
{
    locatedialog->restore();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void KISS::setAvail(LcacheObj *slist)
{
  if (packageLoc)
    delete packageLoc;
  packageLoc = slist;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "kissInterface.moc"
