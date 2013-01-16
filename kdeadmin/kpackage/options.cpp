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

#include "kpackage.h"
#include "managementWidget.h"
#include "pkgInterface.h"
#include "options.h"
#include "cache.h"

#include <qvbox.h>
#include <qcheckbox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kurlrequester.h>
#include <qframe.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qtabdialog.h>
#include <kcombobox.h>
#include <klineedit.h>

#include <findf.h>

extern Opts *opts;
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Options::Options(QWidget *parent)
    : KDialogBase(Tabbed, i18n("Options"), Ok | Cancel, Ok, parent, 0, false){

    fRemote = false;

    DCache = dc = opts->DCache;
    PCache = pc = opts->PCache;
    privCmd = prc = opts->privCmd;

    if (DCache >= Opts::SESSION) {
      cacheObj::clearDCache(); // clear dir caches if needed
    }
    if (PCache >= Opts::SESSION) {
      cacheObj::clearPCache(); // clear package caches if needed
    }

    {
      QVBox *page = addVBoxPage(i18n("&Types"));

      framet = new QGroupBox(1,Qt::Horizontal,i18n("Handle Package Type"), page);

      hh =  new QGroupBox(1,Qt::Horizontal,i18n("Remote Host"),framet);
      huse = new QCheckBox(i18n("Use remote host (Debian APT only):"),hh);
      connect(huse,  SIGNAL(clicked()), this, SLOT(useRemote()));
      hosts = new KComboBox( true, hh, "combo" );
      KCompletion *comp = hosts->completionObject();
      connect(hosts,SIGNAL(returnPressed(const QString&)),comp,SLOT(addItem(const QString&)));
      connect(hosts,SIGNAL(returnPressed()),this,SLOT(insHosts()));
      hosts->setMaxCount(20);
      hosts->setDuplicatesEnabled(false);
      hosts->setInsertionPolicy(QComboBox::AtTop);
      //      hosts->setInsertionPolicy(QComboBox::NoInsertion);
      hosts->setTrapReturnKey(true);

      int i;
      QString msgStr;
      for (i = 0; i < kpinterfaceN; i++)  {
	if (kpinterface[i]) {
	  if (kpinterface[i]->hasProgram) {
	    msgStr = kpinterface[i]->name;
	  } else {
	    msgStr = kpinterface[i]->name;
	    msgStr = i18n("%1: %2 not found")
	      .arg(kpinterface[i]->name)
	      .arg(kpinterface[i]->errExe);
	  }
	  packageBox[i] = new QGroupBox(2,Qt::Horizontal,msgStr, framet, "box");
	  packageHandle[i] =  new QCheckBox(i18n("Enable"), packageBox[i]);
	  connect(packageHandle[i], SIGNAL(clicked()), this, SLOT(scanLocates()));
	  locate[i] = new QPushButton(i18n("Location of Packages"),packageBox[i]);
	  connect(locate[i], SIGNAL(clicked()), kpinterface[i], SLOT(setLocation()));
	} else {
	  packageHandle[i] =  0;
	}
      }
    }

    {
      QVBox *page = addVBoxPage(i18n("Cac&he"));

      bc = new QButtonGroup(page);
      bc->setTitle(i18n("Cache Remote Package Folders"));
      connect( bc, SIGNAL(clicked(int)), SLOT(PDCache(int)) );

      QVBoxLayout* vc = new QVBoxLayout( bc, 15, 10, "vc");
      vc->addSpacing( bc->fontMetrics().height() );

      dcache[0] = new QRadioButton(i18n("Always"),bc);
      vc->addWidget(dcache[0]);

      dcache[1] = new QRadioButton(i18n("During a session"),bc);
      vc->addWidget(dcache[1]);

      dcache[2] = new QRadioButton(i18n("Never"),bc);
      vc->addWidget(dcache[2]);

      bp = new QButtonGroup(page);
      bp->setTitle(i18n("Cache Remote Package Files"));
      connect( bp, SIGNAL(clicked(int)), SLOT(PPCache(int)) );

      QVBoxLayout* vp = new QVBoxLayout( bp, 15, 10, "vp");
      vp->addSpacing( bp->fontMetrics().height() );

      pcache[0] = new QRadioButton(i18n("Always"),bp);
      vp->addWidget(pcache[0]);

      pcache[1] = new QRadioButton(i18n("During a session"),bp);
      vp->addWidget(pcache[1]);

      pcache[2] = new QRadioButton(i18n("Never"),bp);
      vp->addWidget(pcache[2]);

      QGroupBox* cd = new QGroupBox (1, Qt::Horizontal, i18n("Cache Folder"), page) ;
      cd->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Fixed) ;

      cachedir = new KURLRequester("", cd, "cachedir");
    }

    {
      QWidget *page = addVBoxPage(i18n("&Misc"));
      QVBoxLayout *vf = new QVBoxLayout(page);

      //      vf->setSpacing(KDialog::spacingHint());
      vf->setMargin(0);

      bs = new QButtonGroup(page);
      bs->setTitle(i18n("Execute Privileged Commands Using"));
      connect( bs, SIGNAL(clicked(int)), SLOT(PPrivs(int)) );
      
      QVBoxLayout* vs = new QVBoxLayout( bs, 15, 10, "bs");
      vs->addSpacing( bs->fontMetrics().height() );
      
      privs[0] = new QRadioButton(i18n("su command"),bs);
      vs->addWidget(privs[0]);
      
      privs[1] = new QRadioButton(i18n("sudo command"),bs);
      vs->addWidget(privs[1]);
      
      privs[2] = new QRadioButton(i18n("ssh command"),bs);
      vs->addWidget(privs[2]);
      
      valid = new QCheckBox(i18n("Verify file list"), page, "valid");
      vf->addWidget(valid,0,AlignLeft);

      pkgRead = new QCheckBox(i18n("Read information from all local package files"), page, "pkgr");
      vf->addWidget(pkgRead,0,AlignLeft);

      vf->addSpacing(100);
    }

    connect( this, SIGNAL(okClicked()), SLOT(apply_slot()) );
    connect( this, SIGNAL(closeClicked()), SLOT(cancel_slot()) );
    connect( this, SIGNAL(cancelClicked()), SLOT(cancel_slot()) );

    setValues();

}


void Options::insHosts() {
  //  kdDebug() << "insHosts " <<  "\n";
  bool found = false;
  QString s = hosts->currentText();

  int i;
  for (i = 0; i < hosts->count(); i++) {
    if (s == hosts->text(i))
      found = true;
  }

  if (!found)
    hosts->insertItem(hosts->currentText(), 0);
}

void Options::setValues() {
  //  kdDebug() << "setValues:\n";
  DCache = dc = opts->DCache;
  PCache = pc = opts->PCache;
  privCmd = prc = opts->privCmd;
  CacheDir = opts->CacheDir;

  hosts->clear();
  hosts->completionObject()->clear();

  hosts->insertStringList(opts->hostList);
  if (hosts->completionObject()) {
    hosts->completionObject()->setItems(opts->hostList);
  }

  if (!hostName.isEmpty()) {
    huse->setChecked(TRUE);
  }
  //  kdDebug() << "C=" <<opts->hostList.count() << "\n";

  int i;
  for (i = 0; i < kpinterfaceN; i++)  {
    if (kpinterface[i]) {
      packageHandle[i]->setChecked(opts->handlePackage[i]);
    }
  }
  scanLocates();

  dcache[DCache]->setChecked(TRUE);
  pcache[PCache]->setChecked(TRUE);
  privs[privCmd]->setChecked(TRUE);
  cachedir->lineEdit()->setText(CacheDir);

  valid->setChecked(opts->VerifyFL);
  pkgRead->setChecked(opts->PkgRead);
}

Options::~Options()
{
}

void Options::scanLocates() {
  int i;

  bool remote = huse->isChecked();
  for (i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i]) {
      //      kdDebug() << i << " " << hostName << " " << kpinterface[i]->hasRemote << "\n";
      if (kpinterface[i]->hasProgram) {
	packageBox[i]->setEnabled(true);
	if (remote) {
	  if (kpinterface[i]->hasRemote) {
	    packageHandle[i]->setEnabled(true);
	    locate[i]->setEnabled(true);
	  } else {
	    packageHandle[i]->setEnabled(false);
	    locate[i]->setEnabled(false);
	  }
	} else {
	  packageHandle[i]->setEnabled(true);
	  if ( packageHandle[i]->isChecked()) {
	    locate[i]->setEnabled(true);
	  } else {
	    locate[i]->setEnabled(false);
	  }
	}
      } else {
	packageBox[i]->setEnabled(false);
      }
    }
  }
}

void Options::useRemote()
{
  scanLocates();
}

void Options::restore()
{
  show();
  fRemote = huse->isChecked();
}

void Options::cancel_slot()
{
  //  kdDebug() << "Cancel\n";
  opts->readSettings();
  setValues();
}

void Options::apply_slot()
{
  bool doReload = false;
  bool newHost  = false;

  opts->VerifyFL = valid->isChecked();
  opts->PkgRead = pkgRead->isChecked();

  insHosts();
  opts->hostList.clear();
  int i;
  QString prev;
  for (i = 0; i < hosts->count(); i++) {
    //    kdDebug() << "=" << prev << "=" << hosts->text(i) << "=\n";
    if (prev != hosts->text(i))
      opts->hostList.append(hosts->text(i));
    prev = hosts->text(i);
  }

  QString remoteHost = hosts->currentText();

  if ((fRemote != huse->isChecked()) || huse->isChecked() && (remoteHost != hostName)) {
    newHost = true;
    doReload = true;
  }
  if (huse->isChecked()) {
    hostName = remoteHost;
  } else {
    hostName = "";
  }
  kpkg->setCaption(hostName);

  opts->DCache = dc;
  opts->PCache = pc;
  opts->privCmd = prc;
  cachedir->lineEdit()->setText (QDir(cachedir->lineEdit()->text()).path().append("/")) ; // make sure that cache directory ends with "/"
  opts->CacheDir = cachedir->lineEdit()->text() ;

  for (i = 0; i < kpinterfaceN; i++)  {
    if (packageHandle[i]) {
      if ( opts->handlePackage[i] != packageHandle[i]->isChecked()) {
	doReload = true;
      }
      opts->handlePackage[i] = packageHandle[i]->isChecked();
    }
  }

  if (kpackage->findialog)
    kpackage->findialog->checkSearchAll();
  scanLocates();
  opts->writeSettings();

  if (doReload) {
    if (newHost) {
      kpty->close();
    }
    kpackage->reload();
  }
}

void Options::PDCache(int r)
{
  dc = r;
}

void Options::PPCache(int r)
{
  pc = r;
}

void Options::PPrivs(int r)
{
  kdDebug() << "Privs=" << r << "\n";
  prc = r;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Opts::Opts(const QString &initHost)
{
    readSettings(initHost);
}

Opts::~Opts()
{
}

void Opts::readSettings(const QString &initHost)
{

  KConfig *config = kapp->config();

  config->setGroup("Kpackage");

  //  kdDebug() << "readSettings: " << initHost << "\n";
  hostList = config->readListEntry("Host_list");
  if (!initHost.isEmpty() && !hostList.contains(initHost)) {
    hostList.prepend(initHost);
    config->writeEntry("Host_list", hostList);
  }
  hostList.sort();

  DCache = config->readNumEntry("Dir_Cache",1);
  if (DCache >2) {
    DCache = 1;
  }
  PCache = config->readNumEntry("Package_Cache",0);
  if (PCache >2) {
    PCache = 0;
  }
  CacheDir = config->readPathEntry("Cache_Directory", QDir::homeDirPath() + "/.kpackage/");

  // Backward compatability
  bool useSSH = config->readNumEntry("Use_SSH",0);
  privCmd = config->readNumEntry("Priv_Command", -1);
  
  if (privCmd == -1) {
    if (useSSH) {
      privCmd = SSHcmd;
    } else {
      privCmd = SUcmd;
    }
  }
  VerifyFL = config->readNumEntry("Verify_File_List",1);
  PkgRead = config->readNumEntry("Read_Package_files",0);
}

void Opts::readLaterSettings()
{
  KConfig *config = kapp->config();
  config->setGroup("Kpackage");

  int i;
  for (i = 0; i < kpinterfaceN; i++)  {
    if (kpinterface[i])
      handlePackage[i] = config->readBoolEntry(kpinterface[i]->head,
						      kpinterface[i]->defaultHandle);
  }
}

void Opts::writeSettings()
{

  KConfig *config = kapp->config();

  config->setGroup("Kpackage");

  config->writeEntry("Host_list", hostList);

  config->writeEntry("Dir_Cache", DCache);
  config->writeEntry("Package_Cache", PCache);
  config->writePathEntry("Cache_Directory", CacheDir);

  config->writeEntry("Priv_Command",privCmd );
  
  config->writeEntry("Verify_File_List",VerifyFL );
  config->writeEntry("Read_Package_files", PkgRead);

  int i;
  for (i = 0; i < kpinterfaceN; i++)  {
    if (kpinterface[i])
      config->writeEntry(kpinterface[i]->head, handlePackage[i]);
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "options.moc"
