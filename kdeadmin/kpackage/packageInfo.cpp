/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
// Author: Damyan Pepper
// Author: Toivo Pedaste
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



#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Standard headers
#include <stdlib.h>

#include <qregexp.h>
#include <qdir.h>
#include <ctype.h>

// KDE headers
#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>

// kpackage headers
#include "kpackage.h"
#include "kplview.h"
#include "packageInfo.h"
#include "pkgInterface.h"
#include "managementWidget.h"
#include "utils.h"
#include "options.h"

// Global pixmap for
QPixmap *pict = NULL;

//////////////////////////////////////////////////////////////////////////////
// Constructor -- get the pixmap
packageInfo::packageInfo(QMap<QString, QString> _info, pkgInterface *type)
{
  interface = type;
  info = _info;

  item = NULL;
  packageState = UNSET;
  updated = FALSE;
  url = QString::null;
}

// Another constructor, for a packge with a url
packageInfo::packageInfo(QMap<QString, QString> _info, const QString &_url)
{
  info = _info;
  url = _url;
  item = NULL;
}

packageInfo::~packageInfo()
{
}

// Return a property
QString packageInfo::getProperty(const QString &property)
{
  QString result = info[property];
  if (result.isEmpty()) {
     return QString::null;
  }
  return result;
}

// Check for existance of a property
bool packageInfo::hasProperty(const QString &property)
{
  QString s = info[property];
  if (s.isEmpty()) 
     return false;
  else 
    return true;
}

// Initialize fields if missing
void packageInfo::fixup()
{
  if (info["name"].isEmpty()) {
    QString q;
    q.setNum((long)this);
    info.insert("name", q);
  }

  if (info["group"].isEmpty()) {
    info.insert("group", i18n("OTHER"));
    kdDebug() << "Package " << info["name"] << " has no group set." << endl;
  }

  if (!info["version"]) {
    info.insert("version", "");
  }
}

// Set the file name
void packageInfo::setFilename(const QString &f)
{
  url = f;
}

// Get the url
QString packageInfo::getUrl()
{
  if (url.isEmpty()) {
    if (hasProperty("base") && hasProperty("filename")) {
      url = getProperty("base") + "/" + getProperty("filename");
    }
  }
  return url;
}

QString packageInfo::fetchFilename()
{
  QString f = getFilename();

  if (!f.isEmpty()) {
    return f;
  } else {
    QString aurl = getUrl();
    if (!aurl.isEmpty()) {
      return kpackage->fetchNetFile(aurl);
    } else {
      return getProperty("name");
    }
  }
}

bool packageInfo::isFileLocal()
{
  QString aurl = getUrl();
  if (!aurl.isEmpty()) {
    return KPACKAGE::isFileLocal(aurl);
  } 
  return false;
}

bool packageInfo::isInstallable()
{
  if (packageState != packageInfo::INSTALLED &&
      !getProperty("filename").isNull() ) 
    return true;
  else
    return false;
}

bool packageInfo::isFetchable()
{
  if (interface->noFetch || !getFilename().isEmpty() ) 
    return true;
  else
    return false;
}

QString packageInfo::getFilename()
{
  QString cn = "";
  QString aurl = getUrl();
  if (!aurl.isEmpty()) {
    return KPACKAGE::getFileName(aurl,cn);
  } else {
    return "";
  }
}

int packageInfo::getDigElement(const QString &str, int *pos)
  // Extract the next element from the string
  // All digits
{
	QString s = str;

  if (*pos < 0)
    return -1;

  s = s.mid(*pos);
  if (s.isEmpty())
    return -1;

  QRegExp ndig("[^0-9]");

  int nf = 0;
  int val = 0;

  if ((s[0] >= '0') && (s[0] <= '9')) {
    nf = s.find(ndig);
    if (nf >= 0) {
      val = s.left(nf).toInt();
    } else {
      val = s.toInt();
      nf = s.length();
    }
  }

  //  printf("n=%s %d %d\n",s.mid(nf,999).data(),nf,val);
  *pos += nf;
  return val;
}

QString packageInfo::getNdigElement(const QString &string, int *pos)
  // Extract the next element from the string
  // All  all non-digits
{
  QString s(string);

  if (*pos < 0)
    return QString::null;

  s = s.mid(*pos);
  if (s.isEmpty())
    return QString::null;

  QString str;
  int nf = 0;

  QRegExp idig("[0-9]");

  if ((s[0] < '0') || (s[0] > '9') ) {
    nf = s.find(idig);
    if (nf <  0)
      nf = s.length();
    str = s.left(nf);
    for (unsigned int i = 0; i < str.length() ; i++) {
      // Strange Debian package sorting magic
      if (!str[i].isLetter()) {
	char t = str[i].latin1();
	t += 128;
	str[i] = t;
      }
    }
  }
  *pos += nf;
  return str;
}


int packageInfo::pnewer(const QString &s, const QString &sp)
{
  int ns = 0, nsp = 0, vs, vsp;

  //  kdDebug() << "S=" << s  << " SP=" << sp  << "\n";
  while (TRUE) {
    vs = getDigElement(s,&ns);
    vsp = getDigElement(sp,&nsp);
    //    kdDebug() << "s=" << ns << " " << vs << " sp=" << nsp << " " << vsp << "\n";
    if (vs < 0 && vsp < 0)
      return 0;
    if (vs < 0 && vsp < 0)
      return 1;
    if (vs < 0 && vsp < 0)
      return -1;
    if (vsp > vs)
      return 1;
    else if (vs > vsp)
      return -1;

    QString svs = getNdigElement(s,&ns);
    QString svsp = getNdigElement(sp,&nsp);
    //    kdDebug() << "vs=" << ns << " " << svs << " sp=" << nsp << " " << svsp << "\n";
    if (svs.isEmpty() && svsp.isEmpty())
      return 0;
    if (svs.isEmpty() && !svsp.isEmpty())
      return 1;
    if (!svs.isEmpty() && svsp.isEmpty())
      return -1;

    if (svsp.isNull()) { // Allow for QT strangeness comparing null string
      svsp = "";
    }
    if (svs.isNull()) {
      svs = "";
    }
    int n = svsp.compare(svs);
    //    kdDebug() << "svsp=" << svsp << "=" << svsp.length() <<  " svs=" << svs << "=" <<svs.length() <<  " n=" << n << "\n";
    if (n != 0)
      return n;
  }
}

static bool split(QString orig, char seperator, QString &first, QString &second)
{
  int pos = orig.find(seperator);
  if (pos > 0) {
    first = orig.mid(0,pos);
    second = orig.mid(pos+1);
    return true;
  }
  return false;
}

int packageInfo::newer(packageInfo *p)
{
  QString mySerial;  // Serial number of this package
  QString myVersion; // Version of this package
  QString myRelease; // Release of this package

// Version of this package
  QString s = getProperty("version");

  (void) split(s, ':', mySerial, s);
  if (!split(s, '-', myVersion, myRelease))
  {
    myVersion = s;
  }

// Version of other package
  QString hisSerial;  // Serial number of the other package
  QString hisVersion; // Version of the other package
  QString hisRelease; // Release of the other package

  s = p->getProperty("version");
  if (p->hasProperty("release")) {
    s = s + "-" + p->getProperty("release");
  }
  if (p->hasProperty("serial")) {
    s = p->getProperty("serial") + ":" + s;
  }

  (void) split(s, ':', hisSerial, s);
  if (!split(s, '-', hisVersion, hisRelease))
  {
    hisVersion = s;
  }

  //  kdDebug() << "mySerial=" <<  mySerial << " hisSerial=" <<  hisSerial <<"\n";
  //  kdDebug() << "myVersion=" <<  myVersion << " hisVersion=" <<  hisVersion <<"\n";
  //  kdDebug() << "myRelease=" <<  myRelease << " hisRelease=" <<  hisRelease <<"\n";

  int n =  pnewer(mySerial,hisSerial);
  if (n)
    return n;
  else {
    n = pnewer(myVersion,hisVersion);
    if (n)
      return n;
    else
      return pnewer(myRelease,hisRelease);
  }
}

bool packageInfo::display(int treeType)
{
  switch (treeType) {
  case Opts::INSTALLED:
    if (packageState == INSTALLED || packageState == BAD_INSTALL)
      return TRUE;
    break;
  case Opts::UPDATED:
    if (packageState == UPDATED)
      return TRUE;
    break;
  case Opts::NEW:
    if  ((packageState == UPDATED) || (packageState == NEW))
      return TRUE;
    break;
  case Opts::ALL:
      return TRUE;
    break;
  };
  return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Place the package in a QListView

KpTreeListItem *packageInfo::place(KpTreeList *tree, bool insertI)
{
  KpTreeListItem *search = tree->firstChild(), *parent=NULL, *child=NULL;
  QString qtmp, tmp;
  bool doit = FALSE;

  doit = TRUE;
  if (packageState == NOLIST || packageState == HIDDEN)
    doit = FALSE;

  if (doit) {
    qtmp = interface->head;
    qtmp += "/";
    qtmp += getProperty("group");
    int cnt = 0;

    QStringList list = QStringList::split("/",qtmp);
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
      KpTreeListItem *group;

      if( search && (group=findGroup(*it, search)) ) {
	parent = group;
	parent->setOpen(TRUE);
	search = group->firstChild();
      } else {
	if (parent) {
	  group = new KpTreeListItem(parent, 0, interface->folder, *it);
	} else {
	  group = new KpTreeListItem(tree, 0, interface->folder, *it);
	}
	parent = group;
	parent->setOpen(TRUE);
	search = NULL;
      }
      cnt++;
    }

    tmp = *info.find("name");

    if(item)
      delete item;

    QString sz = "";
    if (!info["size"].isEmpty()) {
      sz = info["size"].stripWhiteSpace();
      if (sz.length() > 3)
	sz.truncate(sz.length() - 3);
      else
	sz = "0";
      sz += "K";
    } else if (!info["file-size"].isEmpty()) {
      sz = info["file-size"].stripWhiteSpace();
      if (sz.length() > 3)
	sz.truncate(sz.length() - 3);
      else
	sz = "0";
      sz += "k";
    }
    sz = sz.rightJustify(6,' ');

    QString ver = "";
    if (!info["version"].isEmpty()) {
      ver = info["version"];
    }

    QString over = "";
    if (!info["old-version"].isEmpty()) {
      over = info["old-version"];
    }
    QString summary = "";
    if (!info["summary"].isEmpty()) {
      summary = info["summary"];
    }
       

    QPixmap pic;
    if (packageState == BAD_INSTALL) {
      pic = interface->bad_pict;
    } else if (packageState == UPDATED) {
      pic = interface->updated_pict;
    } else if (packageState == NEW) {
      pic = interface->new_pict;
    } else if (packageState == INSTALLED) {
      pic = interface->pict;
    } else {
      pic = interface->pict;
    }

    if (child) {
      item =  new KpTreeListItem(child, this, pic, tmp, "", summary, sz, ver, over);
    } else {
      item = new KpTreeListItem(parent, this, pic, tmp, "", summary, sz, ver, over);
    }

    if (insertI) {
       parent->setOpen(TRUE);
    } else {
       parent->setOpen(FALSE);
    }

    return item;
  } else {
    return 0;
  }
}

//////////////////////////////////////////////////////////////////////

// Get the QListViewItem
KpTreeListItem *packageInfo::getItem()
{
  return item;
}

//////////////////////////////////////////////////////////////////////////////
bool packageInfo::smerge( const QString &exp) {

  QDict<packageInfo> *dirInfoPackages = kpackage->management->dirInfoPackages;
  QString pname = getProperty("name") + exp;

  packageInfo *pi = dirInfoPackages->find(pname);
  if (pi) {
    QMap<QString,QString>::Iterator it;

    for ( it = pi->info.begin(); it != pi->info.end(); ++it ) {
      if (!(it.key() == "size" && !info["size"].isEmpty()) ||
	  !(it.key() == "file-size"  && !info["file-size"].isEmpty())) {
	info.insert(it.key(), it.data());
      }
      ++it;
    }
    return TRUE;
  }
  return FALSE;
}

//////////////////////////////////////////////////////////////////////////////
void packageInfo::pkgFileIns(const QString &fileName)
{
  info.insert("filename", fileName);
  info.insert("base", "/");

  if (pkgInsert(kpackage->management->allPackages, interface->typeID, FALSE)) {
    packageState = packageInfo::NEW;
    place(kpackage->management->treeList,TRUE);

    QString pname = getProperty("name") + interface->typeID;
    kpackage->management->dirUninstPackages->insert(pname,this);
  }
}

//////////////////////////////////////////////////////////////////////////////
bool packageInfo::pkgInsert(QPtrList<packageInfo> *pki, const QString &exp,
			 bool installed, bool infoPackage)
{
  QDict<packageInfo> *dirInstPackages = kpackage->management->dirInstPackages;
  QDict<packageInfo> *dirUninstPackages = kpackage->management->dirUninstPackages;
  QDict<packageInfo> *dirInfoPackages = kpackage->management->dirInfoPackages;

  QString pname = getProperty("name") + exp;
  //  printf("U1=%s\n",pname.data());

  bool shouldUpdate = TRUE;
  bool hidden = FALSE;

  packageInfo *pi = dirInstPackages->find(pname);
  if (pi) { // installed version exists
    if ((pi->packageState != BAD_INSTALL)
	&& (pi->packageState != NOLIST))  {
      if (newer(pi) >= 0) {
	hidden = TRUE;
      }
    }
  }

  packageInfo *pu = dirUninstPackages->find(pname);
  if (pu) { // available version exists
    if ((pu->packageState != BAD_INSTALL)
	&& (pu->packageState != NOLIST))  {
      if (newer(pu) >= 0) {
	shouldUpdate = FALSE;
      } else if (!installed) { // If older available package exists, remove it
	dirUninstPackages->remove(*(pu->info.find("name")));
	pki->remove(pu);
      }
    }
  }

  if (getProperty("version").isEmpty()) {
    shouldUpdate = TRUE;
  }

  if (shouldUpdate) {
    if (packageState != BAD_INSTALL) {
      if (installed)
	packageState = INSTALLED;
      else if (pi) { // installed version exists
	if (hidden) {
	  packageState = HIDDEN;
	} else {
	  QString version = pi->getProperty("version");
	  if (version.isEmpty()) {
	    if (pi->packageState == NOLIST)
	      packageState = NEW;
	    else
	      packageState = UPDATED;
	  } else {
	    packageState = UPDATED;
	    if (pi->hasProperty("old-version")) {
	      info.insert("old-version",
			   pi->getProperty("old-version"));
	    } else {
	      info.insert("old-version",version);
	    }
	    QString group = getProperty("group");
	    if (group == "NEW") {
	      if (pi->hasProperty("group")) {
		info.replace("group",
			      pi->getProperty("group") );
	      }
	    }
	  }
	}
      } else
	packageState = NEW;
    }

    pki->insert(0,this);
    if (installed) {
      if (infoPackage)
	dirInfoPackages->insert(pname,this);
      else
	dirInstPackages->insert(pname,this);
    } else
      dirUninstPackages->insert(pname,this);
    return TRUE;
  } else {
    return FALSE;
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

