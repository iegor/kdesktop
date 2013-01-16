/*
**
** Copyright (C) 2004 Richard Lärkäng <nouseforaname@home.se>
**
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

#include "gentooInterface.h"
#include "updateLoc.h"
#include "kpackage.h"
#include "cache.h"
#include "kpTerm.h"
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kfilterdev.h>

/* TODO

  Make it possible to 'emerge sync'

  Add possibilty for package manager plugins to have own compare version method,
  and use it here for: (alpha|beta|pre|rc|p)

  Slots, how to do that in a good way?

  Should we care about the world-file?

  Read masked packages from /usr/portage/profiles/package.mask

  Use flags and CFLAGS?

  Should read arch from make.conf along with directories etc
  "~arch" implies "arch"

  Do something about locateDialog, I don't want it here,
  but I would like it to be possible to add own configuration,
  and I don't like the crash when clicking locate ;-)
*/

Gentoo::Gentoo()
  : pkgInterface()
{
  head = "Gentoo";
  name = i18n("Gentoo");
  icon = "gentoo";

  pict = UserIcon(icon);
  updated_pict = UserIcon("kupdated");
  new_pict = UserIcon("knew");

  packagePattern = "*.ebuild";
  //typeID = "/kiss";

  queryMsg = i18n("Querying Gentoo package list: ");

  archesPossible << "~x86" << "x86";
  portageDir="/usr/portage/";
  QFile f(portageDir+"profiles/package.mask");
  if (f.open(IO_ReadOnly))
  {
    QTextStream stream( &f );

    QString line;
    while (!stream.atEnd())
    {
      line = stream.readLine();
      if (!line.startsWith("#") && !line.isEmpty())
      {
  //      kdDebug() << "Adding: " << line << " to packageMask" << endl;
        packageMask << line;
      }
    }
  }

    hasProgram = ifExe("emerge");
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Gentoo::~Gentoo()
{
}

//////////////////////////////////////////////////////////////////////////////
bool Gentoo::isType(char * /*buf*/, const QString &)
{
  return false;
}

bool Gentoo::parseName(const QString& name, QString *n, QString *v)
{
  // Taken from the portage code, should be correct
  QRegExp r("([^/]+)-((\\d+(\\.\\d+)*[a-z]*)(_(alpha|beta|pre|rc|p)\\d*)?(-r(\\d+))?)$");

  r.search(name);

  *n = r.cap(1);
  *v = r.cap(2);

  if (n->isEmpty() || v->isEmpty())
    return false;
  else
    return true;
}

void Gentoo::listInstalledPackages(QPtrList<packageInfo> *pki)
{
  QString vb;
  packageInfo *p;

  QString sline = i18n("Looking for Gentoo packages: ");

  kpackage->setStatus(sline);
  kpackage->setPercent(0);

  QFile f(portageDir+"profiles/categories");
  if (!f.open(IO_ReadOnly))
  {
    kdWarning() << "Couldn't open categories file" << endl;
    return;
  }

  QTextStream stream( &f );
  QStringList categories;
  while (!stream.atEnd())
  {
    categories.append(stream.readLine());
  }

  int categoriesCount = categories.count();
  int categoriesDone = 0;
  for (QStringList::Iterator category = categories.begin(); category != categories.end(); ++category)
  {
    kpackage->setPercent(categoriesDone/categoriesCount);
    categoriesDone += 100;
    
    if (*category == "packages" || *category == "local" || *category == "virtual")
      continue;

    QDir d("/var/db/pkg/"+*category);
    QStringList packages = d.entryList(QDir::Dirs);
    for (QStringList::Iterator it = packages.begin(); it != packages.end(); ++it)
    {
      if (*it != "." && *it != "..")
      {
        p = collectInstalledInfo(*it, *category);
        if (p)
        {
          if (!p->pkgInsert(pki, typeID, true))
          {
            delete p;
          }
        }
      }
    }
    d.setPath("/var/cache/edb/dep/"+*category);
    packages = d.entryList(QDir::Files);
    for (QStringList::Iterator it = packages.begin(); it != packages.end(); ++it)
    {
      if (*it != "." && *it != "..")
      {
	bool isMasked = false;
	QString version, name;
	if (!parseName(*it, &name, &version))
	{
	  kdDebug() << "Couldn't parse name: " << *it << endl;
	  continue;
	}

        for (QStringList::Iterator maskIt = packageMask.begin(); maskIt != packageMask.end(); ++maskIt)
        {
	  // FIXME Should all be handled, just not implemented yet
          if ((*maskIt).startsWith("<") || (*maskIt).startsWith("=") || (*maskIt).startsWith("~") || (*maskIt).startsWith(">"))
            continue;
          if (*category+"/"+name == *maskIt)
	  {
	    kdDebug() << "Package: " << name << "-" << version << " is masked" << endl;
	    isMasked = true;
	    break;
          }
        }

	if (isMasked)
	  continue;

        p = collectUninstalledInfo(name, *category, version);
        if (p)
	{
	  if (!p->pkgInsert(pki, typeID, false))
	  {
	    delete p;
	  }
	}
      }
    }
  }

/*
*/
  kpackage->setPercent(100);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// mode: i = query installed    u = query uninstalled
packageInfo *Gentoo::getPackageInfo(char mode, const QString &name, const QString &version)
{
  packageInfo *pki = 0;
  QString vb,search;

  switch(mode)
    {
      ////////////////////////////////////////////////////////////////////////
      // query an installed package!
    case 'i':
      pki = collectInstalledInfo(name, ""/*FIXME*/);
      break;

      ////////////////////////////////////////////////////////////////////
      // query an uninstalled package
    case 'u':
      pki = collectUninstalledInfo(name, ""/*FIXME*/, version);
      break;
    }
  return pki;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
packageInfo *Gentoo::collectInstalledInfo(const QString& name, const QString& category)
{
  QMap<QString, QString> a;

  QIODevice* iod(KFilterDev::deviceForFile("/var/db/pkg/"+category+"/"+name+"/environment.bz2","application/x-bzip2", true));
  QString line;
  if (!iod->open(IO_ReadOnly))
    return 0;
  QTextStream stream( iod );
  QString n, version, description;
  while (!stream.atEnd())
  {
    line = stream.readLine();
    if (line.startsWith("PN="))
    {
      n = line.mid(3);
      a.insert("name", n);
    }
    else if (line.startsWith("PVR="))
    {
      version = line.mid(4);
      a.insert("version", version);
    }
    else if (line.startsWith("DESCRIPTION="))
    {
      description = line.mid(12);
      a.insert("summary", description);
    }
    if (a.count() >= 3)
      break;
  }
  delete iod;
  a.insert("group", category);
//  a.insert("file-size", "File-size");
//  a.insert("size", "Installed-size");

  packageInfo *i = new packageInfo(a,this);
  i->packageState = packageInfo::INSTALLED;
  i->fixup();
  return i;
}

//////////////////////////////////////////////////////////////////////////////
packageInfo *Gentoo::collectUninstalledInfo(const QString& name, const QString& category, const QString& version)
{
  QMap<QString, QString> a;

  a.insert("name", name);
  a.insert("group", category);
  a.insert("version", version);
  a.insert("base", portageDir+category+"/"+name);
  a.insert("filename", name+"-"+version+".ebuild");

  QFile f("/var/cache/edb/dep/"+category+"/"+name+"-"+version);
  if (!f.open(IO_ReadOnly))
  {
    kdDebug() << "Couldn't read: " << name << "-" << version << endl;
    return 0;
  }

  // Dep format:
  // 1: DEPEND?
  // 2: RDEPEND?
  // 3: SLOT
  // 4: SOURCE_URI
  // 5: FEATURES
  // 6: HOMEPAGE
  // 7: LICENSE
  // 8: DESCRIPTION
  // 9: KEYWORDS (arch)
  // 10: ECLASSES (inherited)
  // 11: IUSE

  // Skip first 7 lines for now
  QTextStream stream( &f );
  for (int i=0; i < 7 && !stream.atEnd(); i++)
    stream.readLine();

  if (!stream.atEnd())
    a.insert("summary", stream.readLine());

  QStringList keywords = QStringList::split(' ', stream.readLine());

  bool works = false;
  for (QStringList::Iterator it = keywords.begin(); it != keywords.end(); ++it)
  {
    for (QStringList::Iterator it2 = archesPossible.begin(); it2 != archesPossible.end(); ++it2)
    {
      if (*it == *it2)
      {
        works = true;
	break;
      }
    }
    if (works)
      break;
  }

  if (!works)
  {
//    kdDebug() << name << "-" << version << ": Doesn't contain working arch" << endl;
    return 0;
  }

  packageInfo *i = new packageInfo(a,this);
  i->packageState = packageInfo::AVAILABLE;
  i->fixup();
  return i;
}

//////////////////////////////////////////////////////////////////////////////

QStringList Gentoo::getChangeLog(packageInfo *p) {
  QStringList clog;
  QFile f(portageDir+p->getProperty("group")+"/"+p->getProperty("name")+"/ChangeLog");
  if (!f.open(IO_ReadOnly))
    return clog;
  QTextStream stream(&f);
  while (!stream.atEnd())
    clog.append(stream.readLine());
  return clog;
}


bool Gentoo::filesTab(packageInfo *p) {
  return p->packageState == packageInfo::INSTALLED;
}

bool Gentoo::changeTab(packageInfo *) {
    return true;
}

//////////////////////////////////////////////////////////////////////////////
QStringList Gentoo::getFileList(packageInfo *p)
{
  QStringList filelist;

  QFile f("/var/db/pkg/"+p->getProperty("group")+"/"+p->getProperty("name")+"-"+p->getProperty("version")+"/CONTENTS");
  if (!f.open(IO_ReadOnly))
    return filelist;
  QTextStream stream(&f);
  QString line;
  QRegExp removeEnd("(.*)( [a-f0-9]{32} [0-9]+| -> [^ ] [0-9]+| -> [^\\(]*\\([^\\)]*\\))$");
  while (!stream.atEnd())
  {
    line = stream.readLine();

    int pos=0;
    while (line[pos] != ' ') pos++;

    line = line.mid(pos+1);

    removeEnd.search(line);
    QString cap = removeEnd.cap(1);
    if (!cap.isEmpty())
      line = cap;

    filelist.append(line);
  }

  return filelist;
}

//////////////////////////////////////////////////////////////////////////////
QString Gentoo::uninstall(int , QPtrList<packageInfo> *plist, bool &test)
{
  QString cmd;
  packageInfo *pk;

  if (test)
    cmd = "NOCOLOR=\"true\" emerge unmerge -p ";
  else
    cmd = "NOCOLOR=\"true\" emerge unmerge ";

  for (pk = plist->first(); pk != 0; pk = plist->next()) {
    cmd += "=" + pk->getProperty("group") + "/" + pk->getProperty("name") +
      "-" + pk->getProperty("version") + " ";
  }
  return cmd;
}

//////////////////////////////////////////////////////////////////////////////
QString Gentoo::install(int, QPtrList<packageInfo> *plist, bool &test)
{
  QString cmd;
  packageInfo *pk;

  if (test)
    cmd = "NOCOLOR=\"true\" emerge -p ";
  else
    cmd = "NOCOLOR=\"true\" emerge ";

  for (pk = plist->first(); pk != 0; pk = plist->next()) {
    cmd += "=" + pk->getProperty("group") + "/" + pk->getProperty("name") +
      "-" + pk->getProperty("version") + " ";
  }
  return cmd;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
QStringList Gentoo::FindFile(const QString &, bool)
{
  return QStringList();
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void Gentoo::setLocation()
{

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void Gentoo::setAvail(LcacheObj *slist)
{
  Q_UNUSED(slist)
}

#include "gentooInterface.moc"
