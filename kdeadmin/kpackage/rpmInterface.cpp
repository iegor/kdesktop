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


//////////////////////////////////////////////////////////////////////////////
///
///               RPM Program version
///
//////////////////////////////////////////////////////////////////////////////

#include <kdebug.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "kpPty.h"
#include "kpackage.h"
#include "rpmInterface.h"
#include "updateLoc.h"
#include "cache.h"

RPM::RPM():pkgInterface()
{  
 head = "RPM";
 name = i18n("RPM");
 icon = "rpm";

 pict = UserIcon(icon);
 updated_pict = UserIcon("rupdated");
 new_pict = UserIcon("rnew");

 packagePattern = "*.rpm";
 typeID = "/rpm";

 locatedialog = new Locations(i18n("Location of RPM Package Archives"));
 locatedialog->dLocations(7,6, this, i18n("Folder","F"),
			  "Rpm","*.rpm", i18n("Location of Folders Containing RPM Packages"));

 connect(locatedialog,SIGNAL(returnVal(LcacheObj *)),
	 this,SLOT(setAvail(LcacheObj *)));
 locatedialog->apply_slot();

 paramsInst.append(new param(i18n("Upgrade"),TRUE,FALSE,"-U","-i"));
 paramsInst.append(new param(i18n("Replace Files"),FALSE,FALSE,"--replacefiles"));
 paramsInst.append(new param(i18n("Replace Packages"),TRUE,FALSE,"--replacepkgs"));
 paramsInst.append(new param(i18n("Check Dependencies"),TRUE,TRUE,"--nodeps"));
 paramsInst.append(new param(i18n("Test (do not install)"),FALSE,FALSE,"--test"));

 paramsUninst.append(new  param(i18n("Remove all versions"),FALSE,FALSE,"--allmatches"));
 paramsUninst.append(new  param(i18n("Use Scripts"),TRUE,TRUE,"--noscripts"));
 paramsUninst.append(new param(i18n("Check Dependencies"),TRUE,TRUE,"--nodeps"));
 paramsUninst.append(new param(i18n("Test (do not uninstall)"),FALSE,FALSE,"--test"));


 queryMsg = i18n("Querying RPM package list: ");

 QDict<QString> provides(1433,false);

 infoList.append("name/%{NAME}");
 infoList.append("version/%{VERSION}");
 infoList.append("release/%{RELEASE}");
 infoList.append("summary/%{SUMMARY}");
 infoList.append("url/%{URL}");
 infoList.append("architecture/%{ARCH}");
 infoList.append("group/%{GROUP}");
 infoList.append("distribution/%{DISTRIBUTION}");
 infoList.append("vendor/%{VENDOR}");
 infoList.append("packager/%{PACKAGER}");
 infoList.append("installtime/%{INSTALLTIME:date}");
 infoList.append("buildtime/%{BUILDTIME:date}");
 infoList.append("size/%{SIZE}");
 infoList.append("provides/[%{PROVIDES}, ]");
 infoList.append("requires/[%{REQUIRENAME} (%{REQUIREFLAGS:depflags} %{REQUIREVERSION}), ]");
 infoList.append("description/[%{DESCRIPTION}]");

 hasProgram = ifExe("rpm");
}

 RPM::~RPM(){}

bool RPM::isType(char *buf, const QString & /* fname */)
{
  if (hasProgram) {
    if ((unsigned char)buf[0] == 0355 && (unsigned char)buf[1] == 0253 &&
        (unsigned char)buf[2] == 0356 && (unsigned char)buf[3] == 0333 ) {
     return true;
    } else
      return false;
  } else {
    return false;
  }
}

bool RPM::parseName(const QString &name, QString *n, QString *v)
{
  int d1, d2, s1, s2;

  s2 = name.findRev('.');
  if (s2 > 0) {
    s1 = name.findRev('.',s2-1);
    if (s1 > 0) {
      d2 = name.findRev('-',s1-1);
      if (d2 > 0) {
	d1 = name.findRev('-',d2-1);
	if (d1 < 0)
	  d1 = d2;
	*n = name.left(d1);
	*v = name.mid(d1+1,s1-d1-1);
	return TRUE;
      }
    }
  }
  return FALSE;
}

QString RPM::packageQuery() {
  QString cmd =  " --queryformat '";
   for ( QStringList::Iterator it = infoList.begin(); it != infoList.end(); ++it ) {
     QStringList s = QStringList::split("/",*it);
     cmd += "==";
     cmd += s[0];
     cmd += "\\n";
     cmd += s[1];
     cmd += "\\n";
   }
   cmd += "==\\n'";
   return cmd;
}

void RPM::listInstalledPackages(QPtrList<packageInfo> *pki)
{
  int NLINES =  70000;

  packageInfo *p;
  QStringList  plist;

  QString  cmd = "rpm  -q -a";
  cmd += packageQuery();

  kpackage->setStatus(i18n("Querying RPM package list"));
  kpackage->setPercent(0);

  QStringList list = kpty->run(cmd);
  kpackage->setStatus(i18n("Processing RPM package list"));
  //  kdDebug() << "P=" << list.count() <<"\n";
  kpackage->setPercent(50);


  if (list.count() > 0) {

    QString s;

    kpackage->setPercent(0 );
    int cnt = 0;
    for ( QStringList::Iterator it = list.begin();  it != list.end(); ++it ) {
      cnt++;
      if (cnt % (NLINES/20) == 0) {
	kpackage->setPercent((cnt * 100)/ NLINES );
      }
      if (*it != "==") {
	s = *it;
	//	kdDebug() << s.length() << "<" << s << ">\n";
	plist << s;
      } else {
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

packageInfo* RPM::collectInfo(QStringList &ln) {

  bool haveName = FALSE;
  QMap<QString, QString> a;

  QString name, value;

  for ( QStringList::Iterator it = ln.begin();  it != ln.end(); ++it ) {
    if ((*it).left(2) == "==" && (*it).length() >= 2) {
      name = (*it).right((*it).length() - 2);
    }
    value = "";
    it++;
    while (it != ln.end() && (*it).left(2) != "==") {
      value += *it;
      value += " ";
      it++;
    }
    it--;

    // kdDebug() << "name=" << name << " value='" << value << "'\n";
    if (name == "installtime") {
      a.insert("install time", value);
    } else if (name == "name") {
      if (!value.isEmpty())
	haveName = TRUE;
      a.insert("name", value.stripWhiteSpace());    
    } else if (name == "buildtime") {
      a.insert("build-time", value);
    } else if (name == "requires") {
      value = value.replace(QRegExp("\\(\\)"),"");
      value = value.replace(QRegExp("\\( \\)"),"");
      value = value.stripWhiteSpace();
      if (value.endsWith(",")) {  
	value.truncate(value.length()-1);
      }
      a.insert("depends", value);
    } else if (name == "provides") {
      int s = 0, n;
      QString t;

      if (!(*a.find("name")).isEmpty()) {
	while ((n = value.find(",",s)) > 0) {
	  t = value.mid(s,n-s);
	  t = t.stripWhiteSpace();
	  if (!t.isEmpty())
	    provides.insert(t,new QString(*a.find("name")));
	  s = n+1;
	}
	t = value.mid(s);
	t = t.stripWhiteSpace();
	if (!t.isEmpty())
	  provides.insert(t,new QString(*a.find("name")));

	value = value.stripWhiteSpace();
	if (value.endsWith(",")) {  
	  value.truncate(value.length()-1);
	}
	a.insert("provides", value);
      }
    } else {
      if (!name.isEmpty())
        a.insert(name, value.stripWhiteSpace());    
    }
    
  }

  QString vers = a["version"];
  QString rel = a["release"];
  if (!vers.isEmpty() && !rel.isEmpty()) {
    vers += "-";
    vers += rel;
    a["version"] = vers;
    a.remove("release");
  }

  if (haveName) {
    packageInfo *i = new packageInfo(a,this);
    i->packageState = packageInfo::INSTALLED;
    i->fixup();
    return i;
  } else {
    return 0;
  }
}

//////////////////////////////////////////////////////////////////////////////

QStringList RPM::getChangeLog(packageInfo *p)
{
  QStringList clog;
  QString fn( p->getFilename());

  if(!fn.isEmpty())
    return getUChangeLog(fn);
  else
    return getIChangeLog(p);

  return clog;
}


// query an installed package
QStringList RPM::getIChangeLog(packageInfo *p)
{
  QString name = p->getProperty("name");

  QString cmd = "rpm -q --changelog ";
  cmd += name;

  QStringList filelist = kpty->run(cmd);

  return filelist;
}


// query an uninstalled package
QStringList RPM::getUChangeLog(const QString &fn)
{
  QString cmd = "rpm -q --changelog -p ";
  cmd += quotePath(fn);

  QStringList filelist = kpty->run(cmd);

  return filelist;
}


bool RPM::filesTab(packageInfo *p) {
  if (p->packageState == packageInfo::INSTALLED) {
    return true;
  } else if (p->isFileLocal()) {
    return true;
  } 
  return false;
}

bool RPM::changeTab(packageInfo *p) {
  if (p->packageState == packageInfo::INSTALLED) {
    return true;
  } else if (p->isFileLocal()) {
    return true;
  } 
  return false;
}

//////////////////////////////////////////////////////////////////////////////


QStringList RPM::getFileList(packageInfo *p)
{
  QStringList filelist;
  QString fn( p->getFilename());

  if(!fn.isEmpty())
    return getUFileList(fn);
  else
    return getIFileList(p);

  return filelist;
}



// query an installed package
QStringList RPM::getIFileList(packageInfo *p)
{
  QString name = p->getProperty("name");

  QString cmd = "rpm -q -l ";
  cmd += name;

  QStringList filelist = kpty->run(cmd);

  return filelist;
}


// query an uninstalled package
QStringList RPM::getUFileList(const QString &fn)
{
  QString cmd = "rpm -q -l -p ";
  cmd += quotePath(fn);

  QStringList filelist = kpty->run(cmd);

  return filelist;
}

//////////////////////////////////////////////////////////////////////////////
packageInfo *RPM::getPackageInfo(char mode, const QString &name, const QString &)
{
  if (mode == 'i') {
    return getIPackageInfo(name);
  } else
    return getUPackageInfo(name);
}

packageInfo *RPM::getIPackageInfo( const QString &name )
{
  // query an installed package!
  QString cmd = "rpm -q";
  cmd += packageQuery();
  cmd += " ";
  cmd += name;

  QStringList infoList = kpty->run(cmd);
  packageInfo *pki = collectInfo(infoList);
  if (pki) {
    pki->packageState = packageInfo::INSTALLED;
    collectDepends(pki,name,0);
  }
  return pki;
}

packageInfo *RPM::getUPackageInfo( const QString &name )
{
  // query an uninstalled package
  QString cmd = "rpm -q";
  cmd += packageQuery();
  cmd += " -p ";
  cmd += quotePath(name);

  QStringList infoList =  kpty->run(cmd);
  packageInfo *pki = collectInfo(infoList);
  if (pki) {
    pki->updated = TRUE;
    pki->packageState = packageInfo::AVAILABLE;
    if (pki->hasProperty("install time"))
      pki->info.remove("install time");
    collectDepends(pki,name,1);
  }

  return pki;
}

QString RPM::provMap( const QString &p )
{
  QString *r = provides[p];
  if (r) {
    QString s = *r;
    //    printf("%s=>%s\n",p.data(),s.data());
    return s;
  } else {
    return p;
  }
}


//////////////////////////////////////////////////////////////////////////////
void RPM::collectDepends(packageInfo *p, const QString &name, int src)
{
  QString cmd = "rpm -V --nofiles ";
  if (src) {
    cmd += "-p ";
  }
  cmd += quotePath(name);

  //  cmd = "cat /home/toivo/rpm.deps";
  QStringList list =  kpty->run(cmd);

  if (list.count() > 0) {
     QStringList::Iterator it = list.begin();
     int pt = (*it).find(":");
     if (pt > 0) {
       QString s = (*it).mid(pt+1);
       if (!s.isEmpty()) {
	 //	 kdDebug() << "S=" << s << "\n";
	 p->info.insert("unsatisfied dependencies", s);
       }
     }
  }
}

//////////////////////////////////////////////////////////////////////////////
void RPM::setLocation()
{
    locatedialog->restore();
}

void RPM::setAvail(LcacheObj *slist)
{
  if (packageLoc)
    delete packageLoc;
  packageLoc = slist;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

QString RPM::uninstall(int uninstallFlags, QPtrList<packageInfo> *plist, bool &test)
{
  QStringList files;

  packageInfo *pk;
  for (pk = plist->first(); pk != 0; pk = plist->next()) {
    files.append( pk->getProperty("name") );
  }

  if (getuid() == 0) {
    return  doUninst(uninstallFlags,files, test);
  } else {
    return  doUninstP(uninstallFlags,files, test);
  }
}

QString RPM::uninstall(int uninstallFlags, packageInfo *p, bool &test)
{
  QStringList files;
  files.append( p->getProperty("name") );

  if (getuid() == 0) {
    return  doUninstP(uninstallFlags,files, test);
  } else {
    return  doUninstP(uninstallFlags,files, test);
  }
}

QString RPM::doUninstP(int uninstallFlags, const QStringList &files, bool &test)
{
    QString s = "rpm -e ";
    s += setOptions(uninstallFlags, paramsUninst);

    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it ) {
      s += " ";
      s += *it;
    }

    if (uninstallFlags>>3 & 1)
      test = TRUE;

    kdDebug() << "uCMD=" << s << " test=" << test << "\n";

    return s;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
QString RPM::install(int installFlags, QPtrList<packageInfo> *plist, bool &test)
{
  QStringList files;

  for (packageInfo *pk = plist->first(); pk != 0; pk = plist->next()) {
    QString fname( pk->fetchFilename() );
    if (!fname.isEmpty()) {
       files.append(quotePath(fname));
    }
  }

  if (getuid() == 0) {
    return doinstP(installFlags,files,test);
  } else {
    return doinstP(installFlags,files,test);
  }
}

QString RPM::install(int installFlags, packageInfo *p, bool &test)
{
  QStringList files;
  files.append(quotePath(p->fetchFilename()));
  if (getuid() == 0) {
    return doinstP(installFlags,files,test);
  } else {
    return doinstP(installFlags,files,test);
  }
}


QString RPM::doinstP(int installFlags, const QStringList &files, bool &test)
{
    QString s = "rpm ";
    s += setOptions(installFlags, paramsInst);

    for (QStringList::ConstIterator it = files.begin(); it != files.end(); ++it ) {
      s += " ";
      s += *it;
    }

    if (installFlags>>4 & 1)
      test = TRUE;

    kdDebug() << "iCMD=" << s << " test=" << test << "\n";

    return s;
}

 QStringList RPM::verify(packageInfo *p, const QStringList &files){
   return pkgInterface::verify(p,files);}

//////////////////////////////////////////////////////////////////////////////
QStringList RPM::FindFile(const QString &name, bool) {
  QString cmd = "rpm -q -a --filesbypkg";
    
  QStringList list =  kpty->run(cmd);
  QStringList retlist;
  if (kpty->Result > 0) {
    list.clear();
  } else {
    for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
      int p =  (*it).find(" ");
      int nm = (*it).find(name,p);
      if (nm >= 0) {
	(*it).replace(p, 1, "\t");
	retlist.append(*it);
      }
    }
  }

  return retlist;
}

//////////////////////////////////////////////////////////////////////////////
QString RPM::quotePath( const QString &path) {
  QString s = path;
  s = s.replace(" ","\\ ");
  return ( "'" + s + "'" );
}

//////////////////////////////////////////////////////////////////////////////
  QStringList RPM::depends(const QString &, int){return 0;}

  QString RPM::doUninst(int, const QStringList &, bool &){return "0=";}
  QString RPM::doinst(int, const QStringList &, bool &){return "0=";}


#include "rpmInterface.moc"
