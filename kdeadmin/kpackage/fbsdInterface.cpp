/*
** Copyright (C) 2000 by Alex Hayward <xelah@xelah.com>
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



#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/utsname.h>

#include <qstringlist.h>

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "fbsdInterface.h"
#include "kpackage.h"
#include "updateLoc.h"
#include "cache.h"
#include "options.h"

#define PKG_INFO_BIN "/usr/sbin/pkg_info"
#define PKG_ADD_BIN "/usr/sbin/pkg_add"
#define PKG_DELETE_BIN "/usr/sbin/pkg_delete"

#define INFO_SEPARATOR "f;g#z-@IqbX%"

fbsdInterface::fbsdInterface():pkgInterface() {
  head = "BSD";
  name = i18n("BSD");
  icon = "bsd";

  pict = UserIcon(icon);
  updated_pict = UserIcon("bupdated");
  new_pict = UserIcon("bnew");

  packagePattern = "*.tgz *.tbz";
  typeID = "/tbz";

  QDict <bsdPortsIndexItem> ports(17777, false);
  queryMsg = i18n("Querying package list: ");

  locatedialog = new Locations(i18n("Location of BSD Packages and Ports"));
  locatedialog->dLocations(1, 1, this, i18n("Ports"), "Pkg", "*.tbz",
                           i18n("Location of Ports Tree (e.g. /usr/ports or /usr/opt)"),FALSE);
  locatedialog->dLocations(1, 6, this, i18n("Packages"), "Pkg", "*.tbz",
                           i18n("Location of Folders Containing BSD Packages or Package Trees"));
  connect(locatedialog, SIGNAL(returnVal(LcacheObj *)), this, SLOT(setAvail(LcacheObj *)));
  locatedialog->apply_slot();

  paramsInst.append(new param(i18n("Ignore Scripts"),FALSE,FALSE,"-I"));
  paramsInst.append(new param(i18n("Check Dependencies"),TRUE,TRUE,"-f"));
  paramsInst.append(new param(i18n("Test (do not install)"),FALSE,FALSE,"-n"));

 paramsUninst.append(new param(i18n("Ignore Scripts"),FALSE,FALSE, "-I"));
 paramsUninst.append(new  param(i18n("Check Dependencies"),TRUE,TRUE, "-f"));
 paramsUninst.append(new param(i18n("Test (do not uninstall)"),FALSE,FALSE, "-n"));

 hasProgram = ifExe("pkg_info") && ifExe("pkg_add");
}

fbsdInterface::~fbsdInterface() {

}

bool fbsdInterface::isType(char *, const QString &fname) {
  // These files are .tgz or .tbz files. Pass it to pkg_info and see whether it
  // succeeds.
  if (hasProgram) { 
    QString cmd = PKG_INFO_BIN; // cmd += "_q";
    cmd += " -q ";
    cmd += fname;
    kpty->run(cmd);
    
    if (!kpty->Result)
      return true;
    else
      return false;
  } else {
    return false;
  }
}

static void insertGroups(QMap<QString, QString> *a, QString cats)
{
  /* Create the list of groups (which is space-separated), and then
  ** iterate through it with the iterator i. count is just to 
  ** distinguish the first entry (count==0) from the rest, since
  ** the key used in a->insert() needs to be different.
  */
  QStringList grlist = QStringList::split(' ',cats);
  unsigned int count = 0;
  for (QStringList::Iterator i = grlist.begin(); 
    i != grlist.end(); ++count,++i) {
    a->insert( (count ? "also in" : "group"), *i);
  }
}

packageInfo *fbsdInterface::getPackageInfo(char mode, const QString &pname, const QString &version) {
  QString name( pname);
  bool installed = false;
  kpackage->setStatus(i18n("Getting package info"));

  kdDebug() << "Looking at package " << pname << endl;

  if (mode == 'i' && !version.isEmpty()) {
    name += "-" + version;
  }

  QMap<QString, QString> a;

  // Get the package name first (for mode = 'u').
  if (mode == 'u') {
    QString cmd = PKG_INFO_BIN; // cmd += "_qf";
    cmd += " -qf ";
    cmd += name;
    QStringList list = kpty->run(cmd);
    
    int last_dir = name.find('/');
    if (last_dir != -1) {
      a["filename"] = name.mid(last_dir+1);
      a["base"] = name.left(last_dir + 1);
    } else {
      a["filename"] = name;
      a["base"] = "";
    }

    if (list.count() > 0) {
      for ( QStringList::Iterator it = list.begin();  it != list.end(); ++it ) {
    // Look for a line of the form '@name <pkgname>'
        if ((*it).left(5) == "@name") {
          QString n = (*it).mid(6);
          addNV(a, n);
          break;
        }
      }
    } else addNV(a, name);
  }

  // Open a pipe to a pkg_info process in order to read the one line comment
  // and description for the package. This works for both installed packages
  // and for files.
  QString cmd = PKG_INFO_BIN; // cmd += "_q";
  cmd += " -q ";
  cmd += name;
  QStringList list = kpty->run(cmd);
  
  QStringList::Iterator it = list.begin(); 
  
  
  if (list.count() > 0) {
    QStringList::Iterator it = list.begin(); 
    a["summary"] = *it;
    it++;
    QString desc;
    int prevlen = 0, len;
    for ( ;  it != list.end(); ++it ) {
      len = (*it).length();
      desc += (*it);
      // kdDebug() << len << " " << prevlen << "=" << *it << "\n";
      if (len > 0 || prevlen > 0)
        desc += "<br>\n";
      prevlen = (*it).length();
    }
    // kdDebug( << desc << "\n";
     bsdPortsIndexItem *inditem = ports[name];

    if (inditem) {
      installed = inditem->installed;
      a["maintainer"] = inditem->fields[bsdPortsIndexItem::MAINT];

      insertGroups(&a,inditem->fields[bsdPortsIndexItem::CATS]);

      a["build depends"] =  !inditem->fields[bsdPortsIndexItem::BDEPS].isEmpty() ? inditem->fields[bsdPortsIndexItem::BDEPS] : i18n("none");
      a["available as"] = inditem->bin ? (inditem->port? i18n("binary package and source port") : i18n("binary package")) : i18n("source port");
    }
    a["description"] = desc;
  } else {
    kpackage->setStatus(QString::null);
    return 0;
  }

  packageInfo *ret = new packageInfo(a, this);
  ret->packageState = installed? packageInfo::INSTALLED : packageInfo::AVAILABLE;
  ret->fixup();
  if (!installed) ret->smerge(typeID);
  kpackage->setStatus(QString::null);
  return ret;
}

QStringList fbsdInterface::getChangeLog(packageInfo *) {
  return 0;
}


bool fbsdInterface::filesTab(packageInfo *) {
  return TRUE;
}

bool fbsdInterface::changeTab(packageInfo *) {
    return FALSE;
}

QStringList fbsdInterface::getFileList(packageInfo *p) {

  // Run pkg_info on the package name to get the file list.
  // The file list is returned on stdout, one per line.
  kpackage->setStatus(i18n("Getting file list"));

  QStringList ret;

  // Find the full name 'name-version', or just 'name' if version is empty.
  // Check first that it is actually installed.
  QString name( p->getProperty("filename"));

  if (!name.isEmpty() && (p->packageState != packageInfo::INSTALLED)) {
    QString qbname( p->getProperty("base"));
    if (!qbname.isEmpty())
       name = qbname + "/" + name;
  } else {
    if (!p->hasProperty("name")) {
      ret.append(i18n("Can't find package name!"));
      kpackage->setStatus(QString::null);
      return ret;
    }

    name = p->getProperty("name");

    QString version( p->getProperty("version"));
    if (!version.isEmpty()) {
       name = name + "-" + version;
    }
  }

  // Open a pipe to a pkg_info process in order to read the file list.
  // This works for both installed packages and for files.
  QString cmd = PKG_INFO_BIN; // cmd += "_Lq";
  cmd += " -L -q  ";
  cmd += name;
  QStringList list = kpty->run(cmd);
  
  ret = list;
  
  kpackage->setStatus(QString::null);
  return ret;
}


//  QPtrList<char> *verify(packageInfo *p, QPtrList<char> *files);

QString fbsdInterface::doUninstall(int uninstallFlags, const QString &packs, bool &)
{

  QString s = PKG_DELETE_BIN;
  s += " ";
  s += setOptions(uninstallFlags, paramsUninst);
  s += packs;

  kdDebug() << "uCMD=" << s << "\n";

  return  s;
}


QString fbsdInterface::doInstall(int installFlags, const QString &packs, bool &)
{

  QString s = PKG_ADD_BIN;
  s += " ";
  s += setOptions(installFlags, paramsInst);
  s += packs;

  kdDebug() << "iCMD=" << s << "\n";

  return  s;
}

QString fbsdInterface::uninstall(int uninstallFlags, packageInfo *p, bool &test)
{
  QString packs( p->getProperty("name"));
  QString vers( p->getProperty("version"));
  if (vers.length() > 0) packs += "-" + vers;

  return doUninstall(uninstallFlags, packs, test);
}

QString fbsdInterface::uninstall(int uninstallFlags, QPtrList<packageInfo> *p, bool &test)
{
  QString packs ;
  packageInfo *i;

  for (i = p->first(); i!= 0; i = p->next())  {
    packs += i->getProperty("name");
    QString vers( i->getProperty("version"));
    if (vers.length() != 0) packs += "-" + vers;
    packs += " ";
  }
  return doUninstall( uninstallFlags, packs, test);
}

QStringList fbsdInterface::FindFile(const QString &, bool) {
  QStringList tmp;
  return tmp;
}

bool fbsdInterface::parseName(const QString &name, QString *n, QString *v) {
  int m1;

  m1 = name.findRev('-');
  if (m1 <= 0) return false;
  *n = name.left(m1);
  *v = name.right(name.length() - m1 - 1);
  return true;
}

void fbsdInterface::addNV(QMap<QString, QString> &d, const QString &name) {
  QString n, v;

  if (!parseName(name, &n, &v)) {
    n = name;
    v = QString::null;
  }

  d.insert("name", n);
  d.insert("version", v);
}

  //public slots
void fbsdInterface::setLocation() {
  locatedialog->restore();
}

void fbsdInterface::setAvail(LcacheObj *slist) {
  kdDebug() << k_funcinfo << endl;
  
  if (packageLoc) delete packageLoc;
  packageLoc = slist;

  cacheObj *cp = packageLoc->first();

  if (cp && !cp->location.isEmpty()) {
    for (; cp != 0; cp = packageLoc->next()) {
      QString oldloc = cp->location;
      cp->location += "/INDEX";
      QString s = getPackList(cp);
      if (!s.isEmpty()) bsdPortsIndexItem::processFile(this, QFile::encodeName(s), true, oldloc);
      cp->location = oldloc;
    }
  }

    // Try /usr/port/INDEX-<major version> on FreeBSD
    struct utsname fbsdName;
    if(uname(&fbsdName) != -1 && !strcmp(fbsdName.sysname, "FreeBSD"))
    	bsdPortsIndexItem::processFile(this, QString("/usr/ports/INDEX-").append(*fbsdName.release), false, "/usr/ports");

    // Try the standard ports tree locations.
    bsdPortsIndexItem::processFile(this, "/usr/ports/INDEX", false, "/usr/ports"); // FreeBSD/OpenBSD
    bsdPortsIndexItem::processFile(this, "/usr/opt/INDEX", false, "/usr/opt");  // NetBSD
}


void fbsdInterface::listPackages(QPtrList<packageInfo> *pki) {
  kdDebug() << k_funcinfo << endl;
  
  listInstalledPackages(pki);
  
    
  QDictIterator<bsdPortsIndexItem> it( ports ); // See QDictIterator
  for( ; it.current(); ++it ) {
    bsdPortsIndexItem *scan = it.current();
    if (!scan->installed  /*&& scan->bin */) {
      QMap<QString, QString> a;

      addNV(a, scan->fields[bsdPortsIndexItem::NAME]);
      a["summary"] = scan->fields[bsdPortsIndexItem::COMMENT];
      a["maintainer"] = scan->fields[bsdPortsIndexItem::MAINT];

      insertGroups(&a,scan->fields[bsdPortsIndexItem::CATS]);

      a["run depends"] = !scan->fields[bsdPortsIndexItem::RDEPS].isEmpty() ? scan->fields[bsdPortsIndexItem::RDEPS] : i18n("none");
      a["build depends"] = !scan->fields[bsdPortsIndexItem::BDEPS].isEmpty() ? scan->fields[bsdPortsIndexItem::BDEPS] : i18n("none");
      a["available as"] = scan->bin ? (scan->port? i18n("binary package and source port") : i18n("binary package")) : i18n("source port");

      a["filename"] = scan->bin_filename;
      a["base"] = scan->bin_filename_base;

      packageInfo *info = new packageInfo(a, this);
      info->packageState = packageInfo::AVAILABLE;
      info->smerge(typeID);
      info->fixup();
      info->pkgInsert(pki, typeID, false);
//	pki->append(info);
    }
  }
  
}

int fbsdInterface::parseItem(QStringList::Iterator &it, QString &name, QString &value, QString separator, QStringList list ) {
  if ((*it).left(separator.length()) == separator) {
    name = *it;
    name = name.mid(separator.length());
  } else {
    return -1;
  }
  if (it == list.end()) 
    return -1;
  it++;
  
  value = "";
  int prevlen = 0, len;
  while ((*it).left(separator.length()) != separator) {
    len = (*it).length();
    value += *it;
    if (len > 0 || prevlen > 0)
      value += "<br>";
    if (it == list.end()) 
      return -1;
    prevlen = (*it).length();
    it++;
  }
  return 1;
}

int fbsdInterface::pathInfo(QMap<QString, QString> &a)
{
  int pkg_state = packageInfo::INSTALLED;
  if (a["group"].isEmpty()) {
    QString s, ps;
    ps = a["name"];
    if (ps.isEmpty()) 
      s = ps;
    else 
      s = "<anonymous>";

    ps = a["version"];
    if (!ps.isEmpty()) 
      s.append(QString("-")+(ps));

    kdDebug() << "Package " << (s) << " has no group." << endl;
 
    /* This must be an installed package with no INDEX entry,
    ** which usually means that the port has been updated.
    */
    QString cmd = PKG_INFO_BIN;
    //   cmd += "2";
    cmd += " -ol ";
    cmd += INFO_SEPARATOR;
    QStringList list = kpty->run(cmd);

    if (list.count() > 0) {
      QStringList::Iterator it = list.begin();
      QString name, value;
      parseItem(it, name, value, INFO_SEPARATOR, list); // Information
      parseItem(it, name, value, INFO_SEPARATOR, list); // Path
      
      int pos = value.findRev('/');
      value.truncate(pos);
      a["group"] = value;
    } else {
      kdDebug() << "Could not read package origin info." << endl;
    }
  }
  return pkg_state;
}

  void fbsdInterface::listInstalledPackages(QPtrList<packageInfo> *pki) {
    kdDebug() << k_funcinfo << endl;
  
  // Open a pipe to a pkg_info process in order to read the comment, name
  // and description for the packages.

  kpackage->setStatus(i18n("Querying BSD packages database for installed packages"));

  QString cmd = PKG_INFO_BIN;
  cmd += " -acdl ";
  cmd += INFO_SEPARATOR;
  QStringList list = kpty->run(cmd);
  
    // We should now get:
    //  INFO_SEPARATORInformation for pkgname:
    //
    //  INFO_SEPARATORComment:
    //   <one line description>
    //
    //  INFO_SEPARATORDescription:
    //   <description>
    //
    //
    //  INFO_SEPARATOR
    //  INFO_SEPARATORInformation for [etc]

  QMap<QString, QString> a;
  QString name, value;
  if (list.count() > 0) {
    for ( QStringList::Iterator it = list.begin();  it != list.end(); ++it ) {
      
      parseItem(it, name, value, INFO_SEPARATOR, list); // Information 
     // Find the last word on this line (which should be the package name) minus a trailing :.
      QString pkg = name.section(' ',-1);
      if (pkg.isEmpty()) {
        KpMsgE(i18n("Unexpected output from pkg_info (looking for package name): %1").arg(value), TRUE);
        kpackage->setStatus(QString::null);
        return;
      } else {
        if (pkg[pkg.length()-1] == ':') {
          pkg.truncate(pkg.length()-1);
        }
      }
      addNV(a, pkg);

      parseItem(it, name, value, INFO_SEPARATOR, list); //Comment
      a["summary"] = value;


      parseItem(it, name, value, INFO_SEPARATOR, list); //Description

      bsdPortsIndexItem *inditem = ports[pkg];
      
      if (inditem) {
	inditem->installed = true;

        a["maintainer"] = inditem->fields[bsdPortsIndexItem::MAINT];

        insertGroups(&a,inditem->fields[bsdPortsIndexItem::CATS]);
	if (a["group"].isEmpty()) {
          kdDebug() << "Line <" << name << "=" << value << "> has no group?" << endl;
        }

        a["run depends"] = !inditem->fields[bsdPortsIndexItem::RDEPS].isEmpty() ? 
          inditem->fields[bsdPortsIndexItem::RDEPS] : i18n("none");
        a["build depends"] = !inditem->fields[bsdPortsIndexItem::BDEPS].isEmpty() ? 
          inditem->fields[bsdPortsIndexItem::BDEPS] : i18n("none");
	a["available as"] = inditem->bin ? (inditem->port? i18n("binary package and source port") : i18n("binary package")) : i18n("source port");
      }

      a["description"] = value;

      int pkg_state = pathInfo(a);
      packageInfo *info = new packageInfo(a, this);
      info->packageState = pkg_state;

      info->fixup();
      //pki->append(info);
      info->pkgInsert(pki, typeID, true);

      }
    }
}


bsdPortsIndexItem::bsdPortsIndexItem(fbsdInterface *parent, char *desc, bool binaries, const QString &dname) : bin(binaries), port(!binaries), installed(false) {
  fields = QStringList::split('|', desc, TRUE);
  QString name = fields[NAME];
  
  bsdPortsIndexItem *port = parent->ports[name];
  if (port) {
    port->bin = port->bin || bin;
    port->port = port->port || port;
    if (binaries) {
      port->bin_filename = QString(name) + ".tbz";
      port->bin_filename_base = dname + "/";
    }
    fields[NAME] = ""; // Acts as a 'not used' tag.
    return;
    
  }
  if (binaries) {
    bin_filename = QString(name) + ".tbz";
    bin_filename_base = dname + "/";
  }
  
}

void bsdPortsIndexItem::processFile(fbsdInterface *parent, const QString &fname, bool binaries, const QString &dname) {
  // Read the file in to a buffer and null terminate it.

  struct stat s;

  if (stat(fname.ascii(), &s) == -1) {
    // Error message?
    return;
  }

  char *index = (char *) malloc(s.st_size);
  int fd;

  fd = open(fname.ascii(), O_RDONLY);
  if (fd == -1) {
    // Error message?
    return;
  }

  int size = read(fd, index, s.st_size);
  index[size] = 0;
  close(fd);


  // Go through each line and create a new bsdPortsIndexItem.
  char *line = strtok(index, "\n");
  while (line != 0) {
     bsdPortsIndexItem *i = new bsdPortsIndexItem(parent, line, binaries, dname + "/All");
    if (i->fields[NAME].isEmpty()) {
      delete i;
    } else {
      parent->ports.insert(i->fields[NAME] , i);
    }
    line = strtok(0, "\n");
  }
}


#include "fbsdInterface.moc"
