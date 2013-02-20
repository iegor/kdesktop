/*
** Copyright (C) 1999,2000 Toivo Pedaste <toivo@ucs.uwa.edu.au>
**
// Author: Damyan Pepper
// Author: Toivo Pedaste
//
// See managementWidget.h for more information
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


#include <qsplitter.h>
#include <qtoolbutton.h>

#include <klocale.h>
#include <kdebug.h>
#include <klistviewsearchline.h>
#include <kaction.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <kiconloader.h>

// kpackage.headers
#include "kpackage.h"
#include "kplview.h"
#include "managementWidget.h"
#include "pkgInterface.h"
#include "pkgOptions.h"
#include "packageDisplay.h"
#include "packageProperties.h"
#include "options.h"

extern Opts *opts;

KpListViewSearchLine::KpListViewSearchLine(QWidget *parent, KpTreeList *listView)  
  :KListViewSearchLine(parent, listView)
{
  list = listView;
}

KpListViewSearchLine::~KpListViewSearchLine()
{
}

void KpListViewSearchLine::updateSearch(const QString &s)
{
    list->expand();
    KListViewSearchLine::updateSearch(s);
    KListViewSearchLine::updateSearch(s); // Yes both are needed
    list->sweep(false);
}


// constructor -- initialise variables
managementWidget::managementWidget(QWidget *parent)
  : QFrame(parent)
{
  install_action = 0;
  uninstall_action = 0;

  allPackages  = new QPtrList<packageInfo>;

  tType[0] = i18n("Installed");
  tType[1] = i18n("Updated");
  tType[2] = i18n("New");
  tType[3] = i18n("All");

  dirInstPackages = new QDict<packageInfo>(7717);
  dirUninstPackages = new QDict<packageInfo>(7717);
  dirInfoPackages = new QDict<packageInfo>(7717);

  setupWidgets();

  connect(treeList,SIGNAL(updateMarked()), 
	  this, SLOT( checkMarked()));
}

managementWidget::~managementWidget()
{
  //  if(allPackages)
  //      delete allPackages;
  //  delete dirInstPackages;
  //  delete dirUninstPackages;
}

void managementWidget::resizeEvent(QResizeEvent *)
{
  arrangeWidgets();
}


void managementWidget::setupWidgets()
{
  QTab t;

  top = new QBoxLayout(this,QBoxLayout::TopToBottom);
  vPan  = new QSplitter(QSplitter::Horizontal, this);
  top->addWidget(vPan);

  // the left panel
  leftpanel = new QFrame(vPan);
  leftbox = new QBoxLayout(leftpanel,QBoxLayout::TopToBottom);

  QTabBar *ltab = new QTabBar(leftpanel);

  treeList = new KpTreeList(leftpanel);


  for (int i = 0; i < 4; i++) {
    QTab *t = new QTab();
    t->setText( tType[i] );
    ltab->addTab(t);
  }
  // Quick Search Bar
  searchToolBar = new KToolBar( leftpanel, "search toolbar");

  QToolButton *clearSearch = new QToolButton(searchToolBar);
  clearSearch->setTextLabel(i18n("Clear Search"), true);
  clearSearch->setIconSet(SmallIconSet(QApplication::reverseLayout() ? "clear_left"
                                            : "locationbar_erase"));
  (void) new QLabel(i18n("Search: "),searchToolBar);

  searchLine = new KpListViewSearchLine(searchToolBar, treeList);
  //  searchLine->setKeepParentsVisible(false);
  connect( clearSearch, SIGNAL( pressed() ), searchLine, SLOT( clear() ));

  QValueList<int> clist;  clist.append(0);  clist.append(2);
  searchLine->setSearchColumns(clist);

  searchToolBar->setStretchableWidget( searchLine );
  connect( treeList, SIGNAL( cleared() ), searchLine, SLOT( clear() ));

  connect(ltab,SIGNAL(selected (int)),SLOT(tabChanged(int)));
  ltab->setCurrentTab(treeList->treeType);

  leftbox->addWidget(ltab,10);
  leftbox->addWidget(searchToolBar,10);
  leftbox->addWidget(treeList,10);

  leftbox->addStretch();

  lbuttons = new QBoxLayout(QBoxLayout::LeftToRight);

  luinstButton = new QPushButton(i18n("Uninstall Marked"),leftpanel);
  luinstButton->setEnabled(FALSE);
  connect(luinstButton,SIGNAL(clicked()),
	  SLOT(uninstallMultClicked()));
  linstButton = new QPushButton(i18n("Install Marked"),leftpanel);
  linstButton->setEnabled(FALSE);
  connect(linstButton,SIGNAL(clicked()),
	  SLOT(installMultClicked()));

  leftbox->addLayout(lbuttons,0); // top level layout as child

  // Setup the `buttons' layout
  lbuttons->addWidget(linstButton,1,AlignBottom);
  lbuttons->addWidget(luinstButton,1,AlignBottom);
  lbuttons->addStretch(1);

  connect(treeList, SIGNAL(selectionChanged(QListViewItem *)),
	  SLOT(packageHighlighted(QListViewItem *)));

  // the right panel
  rightpanel = new QFrame(vPan);
  rightbox = new QBoxLayout(rightpanel,QBoxLayout::TopToBottom);

  packageDisplay = new packageDisplayWidget(rightpanel);
  //  connect(this, SIGNAL(changePackage(packageInfo *)),
  //  packageDisplay, SLOT(changePackage(packageInfo *)));

  rbuttons = new QBoxLayout(QBoxLayout::LeftToRight);

  uinstButton = new QPushButton(i18n("Uninstall"),rightpanel);
  uinstButton->setEnabled(FALSE);
  connect(uinstButton,SIGNAL(clicked()),
	  SLOT(uninstallSingleClicked()));
  instButton = new QPushButton(i18n("Install"),rightpanel);
  instButton->setEnabled(FALSE);
  connect(instButton,SIGNAL(clicked()),
	  SLOT(installSingleClicked()));


  // Setup the `right panel' layout
  rightbox->addWidget(packageDisplay,10);
  rightbox->addLayout(rbuttons,0); // top level layout as child

  // Setup the `buttons' layout
  rbuttons->addWidget(instButton,1);
  rbuttons->addWidget(uinstButton,1);
  rbuttons->addStretch(1);

}

////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
void managementWidget::writePSeparator()
{
  KConfig *config = kapp->config();

  config->setGroup("Kpackage");

  config->writeEntry("panel1Width",vPan->sizes().first());
  config->writeEntry("panel2Width",vPan->sizes().last());
}

void managementWidget::readPSeparator()
{
  KConfig *config = kapp->config();

  config->setGroup("Kpackage");

  int w1 = config->readNumEntry("panel1Width",200);
  int w2 = config->readNumEntry("panel2Width",200);

  QValueList<int> size;
  size << w1 << w2;
  vPan->setSizes(size);
}

///////////////////////////////////////////////////////////////////
void managementWidget::setupMultButton(int &cntInstall, int &cntUnInstall)
{
  if (cntInstall)
    linstButton->setEnabled(true);
  else
    linstButton->setEnabled(false);

  if (cntUnInstall)
    luinstButton->setEnabled(true);
  else
    luinstButton->setEnabled(false);
}

void managementWidget::setupInstButton()
{
  bool u,i;

  packageInfo *package = packageDisplay->package;

  if (!package) {
    i = false;
    u = false;
  } else {
    if (package->isFetchable() ) {
      instButton->setText(i18n("Install"));
    } else {
      instButton->setText(i18n("Fetch"));
    }
    if (package->isInstallable() ) {

      i = true;
      u = false;
    } else {
      i = false;
      u = true;
    }
  }
  instButton->setEnabled(i);
  if (install_action)
    install_action->setEnabled(i);

  uinstButton->setEnabled(u);
  if (uninstall_action)
    uninstall_action->setEnabled(u);
}

void managementWidget::arrangeWidgets()
{
  // this is done automatically by the layout managers
}

void managementWidget::tabChanged(int tab)
{
   treeList->treeType = tab;
   searchLine->updateSearch();
}


// Collect data from package.
void managementWidget::collectData(bool refresh)
{
  int i;

  if (!refresh && allPackages) {
    treeList->sweep(true);
    return; // if refresh not required already initialised
  }

  QApplication::setOverrideCursor( waitCursor );

// stop clear() sending selectionChanged signal
  disconnect(treeList, SIGNAL(selectionChanged(QListViewItem *)),
	  this, SLOT(packageHighlighted(QListViewItem *)));
  treeList->hide();    // hide list tree
  treeList->clear();   // empty it
  connect(treeList, SIGNAL(selectionChanged(QListViewItem *)),
	  SLOT(packageHighlighted(QListViewItem *)));

  packageDisplay->changePackage(0);

  // Delete old list if necessary
  if(allPackages) {
    delete allPackages;
  }

  allPackages = new QPtrList<packageInfo>;
  allPackages->setAutoDelete(TRUE);

  dirInstPackages->clear();
  dirUninstPackages->clear();
  // List installed packages
  for (i = 0; i < kpinterfaceN; i++)  {
    if (kpinterface[i] && kpinterface[i]->hasProgram && opts->handlePackage[i]) {
      if (hostName.isEmpty() || (kpinterface[i]->hasRemote)) {
	kpinterface[i]->listPackages(allPackages);
      }
    }
  }

  // Rebuild the list tree
  rebuildListTree();

  QApplication::restoreOverrideCursor();
}

// Rebuild the list tree
void managementWidget::rebuildListTree()
{
  packageInfo *i;
  int  n = 0;

  kpackage->setStatus(i18n("Building package tree"));
  kpackage->setPercent(0);

  treeList->setSorting(-1);

  // place all the packages found
  int count = allPackages->count();
  int incr = count/50;
  if (incr == 0)
    incr = 1;

  for(i=allPackages->first(); i!=0; i=allPackages->next())
    {
      i->place(treeList,TRUE);

      if (!(n % incr)) {
	kpackage->setPercent(int (n*100/count));
      }
      n++;
    }
  treeList->sweep(true);
  treeList->show();		// show the list tree

  treeList->setSorting(0);

  kpackage->setPercent(100);	// set the progress
  kpackage->setStatus("");

  checkMarked();
}

// A package has been highlighted in the list tree
void managementWidget::packageHighlighted(QListViewItem *item)
{
 
  treeList->packageHighlighted(item, packageDisplay);
  setupInstButton();

  kpackage->setPercent(100);
}

/////////////////////////////////////////////////////////////////////////
// install has been clicked

void managementWidget::installSingleClicked()
{
  int result;
  QPtrList<packageInfo> plist;

  packageInfo *package = packageDisplay->package;

  if (package) {
    QString filename = package->getFilename();
    kdDebug() << "File=" << filename  <<"\n";
    pkgInterface *interface = package->interface;
    if (interface->noFetch || !filename.isEmpty()) {
      plist.append(package);
      if (!interface->installation->setup(&plist, interface->head)) {
        return;
      }
      result = interface->installation->exec();

      if (interface->installation->result() == QDialog::Accepted ||
	  interface->installation->modified) { 
	// it was accepted, so the package has been installed
	packageInfo *inf;
	for (inf = plist.first(); inf != 0; inf = plist.next()) {
	  updatePackage(inf,TRUE);
	}

	if (treeList->currentItem()) {
	  KpTreeListItem *p = treeList->currentItem();
	  packageDisplay->changePackage(p->info);
	} else {
	  packageDisplay->changePackage(0); // change package to no package
	}
	setupInstButton();	  
      }

      //      kdDebug() << "Result=" << result <<"\n";
    } else {
      QString url = package->getUrl();
      if (!url.isEmpty()) {
	QString s = kpackage->fetchNetFile(url);
	if (!s.isEmpty()) {
	  packageDisplay->changePackage(package);
	  setupInstButton();
	}
      } else {
	KpMsgE(i18n("Filename not available\n"),TRUE);
      }
    }
  }
  kpackage->setPercent(100);

  searchLine->updateSearch();
  checkMarked();
}

// install has been clicked
void managementWidget::installMultClicked()
{
  int  i;
  KpTreeListItem *it;
  packageInfo *inf;
  QPtrList<packageInfo> **lst = new QPtrList<packageInfo>*[kpinterfaceN];

  selList.clear();
  treeList->findMarked(treeList->firstChild(), selList);

  for (i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i]) {
      lst[i] = new QPtrList<packageInfo>;
      for (it = selList.first(); it != 0; it = selList.next()) {
	if (it->info->interface == kpinterface[i] &&
	    it->childCount() == 0 &&
	    (it->info->packageState == packageInfo::UPDATED ||
	     it->info->packageState == packageInfo::NEW)
	    ) {
	  lst[i]->insert(0,it->info);
	}
      }
    }
  }
  selList.clear();

  for (i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i]) {
      if (lst[i]->count() > 0) {
        if (kpinterface[i]->installation->setup(lst[i],kpinterface[i]->head)) {
	 if (kpinterface[i]->installation->exec() ||
	    kpinterface[i]->installation->modified) {
	   for (inf = lst[i]->first(); inf != 0; inf = lst[i]->next()) {
	     updatePackage(inf,TRUE);
	   }
          }
	}
	delete lst[i];
      }
    }
  }
  delete [] lst;

  searchLine->updateSearch();
  checkMarked();
}

/////////////////////////////////////////////////////////////////////////////
// Uninstall has been clicked

void managementWidget::uninstallSingleClicked()
{
  int result;
  QPtrList<packageInfo> plist;

  packageInfo *package = packageDisplay->package;

  if (package) {			// check that there is a package to uninstall
    pkgInterface *interface = package->interface;
    plist.append(package);
    if (!interface->uninstallation->setup(&plist, interface->head)) {
      return;
    }
    result = interface->uninstallation->exec();

    if(result == QDialog::Accepted ||
       interface->installation->modified) { 
      packageInfo *inf;
      for (inf = plist.first(); inf != 0; inf = plist.next()) {
	updatePackage(inf,FALSE);
      }

      if (treeList->currentItem()) {
	KpTreeListItem *p = treeList->currentItem();
	packageDisplay->changePackage(p->info);
      } else {
	packageDisplay->changePackage(0); // change package to no package
      }
      setupInstButton();
    }
    //    kdDebug() << "Result=" << result <<"\n";
  }
  kpackage->setPercent(100);

  searchLine->updateSearch();
  checkMarked();
}

void managementWidget::uninstallMultClicked()
{
  int  i;
  KpTreeListItem *it;
  packageInfo *inf;
  QPtrList<packageInfo> **lst = new QPtrList<packageInfo>*[kpinterfaceN];

  selList.clear();
  treeList->findMarked(treeList->firstChild(), selList);
  for (i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i]) {
      lst[i] = new QPtrList<packageInfo>;
      for (it = selList.first(); it != 0; it = selList.next()) {
	if (it->info->interface == kpinterface[i] &&
	    it->childCount() == 0 &&
	    (it->info->packageState == packageInfo::INSTALLED ||
	     it->info->packageState == packageInfo::BAD_INSTALL)
	    ) {
	  lst[i]->insert(0,it->info);
	}
      }
    }
  }
  selList.clear();

  for (i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i]) {
      if (lst[i]->count() > 0) {
        if (kpinterface[i]->uninstallation->setup(lst[i],kpinterface[i]->head)) {
	 if (kpinterface[i]->uninstallation->exec()||
	    kpinterface[i]->installation->modified ) {
	   for (inf = lst[i]->first(); inf != 0; inf = lst[i]->next()) {
	     updatePackage(inf,FALSE);
	   }
         }
       }
      delete lst[i];
      }
    }
  }
  delete [] lst;

  searchLine->updateSearch();
  checkMarked();
}


///////////////////////////////////////////////////////////////////////////

void managementWidget::doChangePackage(packageInfo *p)
{
   packageDisplay->changePackage(p);
}

///////////////////////////////////////////////////////////////////////////

KpTreeListItem *managementWidget::search(QString str, bool subStr, bool wrap,
			     bool start)
{
  return treeList->search(str, subStr, wrap, start);
}


///////////////////////////////////////////////////////////////////////////
KpTreeListItem *managementWidget::updatePackage(packageInfo *pki, bool install)
{
  QString version;
  KpTreeListItem *q;

  if (allPackages) {
    QString name(pki->getProperty("name"));
    if (pki->hasProperty("version"))
      version = pki->getProperty("version");
    else
      version = "";
    pkgInterface *interface = pki->interface;
    packageInfo *pnew = interface->getPackageInfo('i', name, version);
    packageInfo *ptree;
    QString pkgId =  name + interface->typeID;

    if (install) {
      if (pnew) {
	if (pnew->packageState !=  packageInfo::BAD_INSTALL) {
	  ptree = dirInstPackages->find(pkgId); // remove installed entry
	  dirInstPackages->remove(pkgId);
	  if (ptree) {
	    if (ptree->getItem()) {
	      delete ptree->getItem();
	      ptree->item = 0;
	    }
	  }

	  ptree = dirUninstPackages->find(pkgId); // remove uninstalled entry
	  if (ptree) {
	    ptree->packageState = packageInfo::HIDDEN;
	    if (ptree->getItem()) {
	      delete ptree->getItem();
	      ptree->item = 0;
	    }
	  }
	}

	dirInstPackages->insert(pkgId,pnew);

	q = pnew->place(treeList,TRUE);
	allPackages->insert(0,pnew);
	if (!q) {
	  printf("NOTP=%s \n",pnew->getProperty("name").ascii());
	} else {
	  return q;
	}
      }
    } else { // uninstalling
      if (!pnew) {
	dirInstPackages->remove(pkgId);
	KpTreeListItem  *qt = pki->getItem();
	if (qt) {
	  treeList->stackRemove(qt);
	  treeList->setSelected(qt,false);
	  if (treeList->markPkg == qt)
	    treeList->markPkg = 0;
	  delete qt;
	} else {
	  kdDebug() << "DEL=" << name << endl;
	}
	packageInfo *pb = dirUninstPackages->find(pkgId);
	if (pb) { // available package matching the one just uninstalled
	  pb->packageState = packageInfo::NEW;
	  q = pb->place(treeList,TRUE);
	  if (!q) {
	    printf("NOTP=%s \n",pb->getProperty("name").ascii());
	  } else {
	    return q;
	  }
	}

      } else {
	delete pnew;
      }
    }
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////
void managementWidget::checkMarked()
{
int  cntInstall = 0;
int  cntUnInstall = 0;

  treeList->countMarked(treeList->firstChild(), cntInstall, cntUnInstall);
  setupMultButton(cntInstall, cntUnInstall);
}


#include "managementWidget.moc"
