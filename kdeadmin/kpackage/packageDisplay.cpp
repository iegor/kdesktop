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


#include "../config.h"
// Standard headers
#include <stdio.h>

// Qt headers

#include <qapplication.h>
#include <qfileinfo.h>
#include <qtextedit.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <krun.h>
#include <kopenwith.h>

// kpackage.headers
#include "kpackage.h"
#include "packageDisplay.h"
#include "packageProperties.h"
#include "pkgInterface.h"
#include "utils.h"
#include "options.h"
#include <klocale.h>

extern Opts *opts;

// constructor
packageDisplayWidget::packageDisplayWidget(QWidget *parent)
  : QTabWidget(parent)
{
  // Initially we're not dealing with any package
  package=NULL;

  // Set up the widgets
  setupWidgets();

  // Load the pixmaps
  tick = UserIcon("ptick");
  cross = UserIcon("cross");
  question = UserIcon("question");
  blank = new QPixmap();

}

packageDisplayWidget::~packageDisplayWidget()
{
    delete blank;
}

void packageDisplayWidget::setupWidgets()
{
  proptab = new QVBox( this);
  curTab = proptab;
  fltab = new QVBox( this);
  cltab = new QVBox( this);

  packageProperties = new packagePropertiesWidget(proptab);

  fileList = new kpFileList(fltab, this);
  connect(fileList, SIGNAL(executed(QListViewItem *)),
            this, SLOT( openBinding(QListViewItem *)) );
  connect(fileList, SIGNAL(returnPressed(QListViewItem *)),
            this, SLOT( openBinding(QListViewItem *)) );

  changeLog = new QTextEdit(cltab);

  addTab(proptab, i18n("Properties"));
  addTab(fltab, i18n("File List"));
  addTab(cltab, i18n("Change Log"));

  if (isTabEnabled(cltab))
    setTabEnabled(cltab,false);
  if (isTabEnabled(fltab))
    setTabEnabled(fltab,false);
  if (isTabEnabled(proptab))
    setTabEnabled(proptab,false);

  connect(this,SIGNAL(currentChanged(QWidget *)), this, SLOT(tabSelected(QWidget *)));
}

void packageDisplayWidget::tabSelected(QWidget *tab)
{
  curTab = tab;
  tabSet(tab);
}

void packageDisplayWidget::tabSet(QWidget *tab)
{
  disconnect(this,SIGNAL(currentChanged(QWidget *)), this, SLOT(tabSelected(QWidget *)));
  if(tab == proptab) {
    packageProperties->show();
    fileList->hide();
    changeLog->hide();
    setCurrentPage(0);
  } else if (tab == fltab) {
    packageProperties->hide();
    changeLog->hide();
    if (isTabEnabled(fltab)) {
      if (!initList) {
	updateFileList();
	initList = 1;
      } 
      fileList->show();
    } else {
      fileList->hide();
    }
    setCurrentPage(1);
  } else {
    fileList->hide();
    packageProperties->hide();
    if (isTabEnabled(cltab)) {
      updateChangeLog();
      changeLog->show();
    } else {
      changeLog->hide();
    }
    setCurrentPage(2);
  }
  connect(this,SIGNAL(currentChanged(QWidget *)), this, SLOT(tabSelected(QWidget *)));
}

void packageDisplayWidget::noPackage()
{
  disconnect(this,SIGNAL(currentChanged(QWidget *)), this, SLOT(tabSelected(QWidget *)));
  
  if (isTabEnabled(fltab)) {
    fileList->setColumnText(0,"");
    setTabEnabled(fltab,false);
  }
  if (isTabEnabled(proptab))
    setTabEnabled(proptab,false);
  if (isTabEnabled(cltab))
    setTabEnabled(cltab,false);

  packageProperties->changePackage(NULL);

  packageProperties->setText("");
  fileList->clear();
  changeLog->setText("");

  connect(this,SIGNAL(currentChanged(QWidget *)), this, SLOT(tabSelected(QWidget *)));
}

// Change packages
void packageDisplayWidget::changePackage(packageInfo *p)
{

// This is to stop selectionChanged firing off here

    disconnect(fileList, SIGNAL(executed(QListViewItem *)),
            this, SLOT( openBinding(QListViewItem *)) );
  disconnect(fileList, SIGNAL(returnPressed(QListViewItem *)),
            this, SLOT( openBinding(QListViewItem *)) );
  disconnect(fileList, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
            fileList, SLOT( openContext(KListView *, QListViewItem *, const QPoint &)) );


  if (package && package != p) {
    if (!package->getItem() && !kpackage->management->allPackages->find(package)) {
      delete package;
      package = 0;
    }
  }

  package = p;
  if (!p) {			// change to no package
    noPackage();
  } else {
    QString u = package->getFilename();
    if (!package->updated &&  !u.isEmpty()) {
      packageInfo *np = package->interface->getPackageInfo('u', u, 0);

      if (np) {
	QMap<QString, QString>::Iterator it; // update info entries in p
        for ( it = np->info.begin(); it != np->info.end(); ++it ) {
	  package->info.replace(it.key(),it.data());
	}
	package->interface = np->interface;
	delete np;
	package->updated = TRUE;
      }
    }

    initList = 0;
    packageProperties->changePackage(package);

    if (package->interface->changeTab(package))
      setTabEnabled(cltab,true);
    else
      setTabEnabled(cltab,false);

    if (package->interface->filesTab(package))
      setTabEnabled(fltab,true);
    else
      setTabEnabled(fltab,false);

    setTabEnabled(proptab,true);
 
    tabSet(curTab);


  }
    connect(fileList, SIGNAL(executed(QListViewItem *)),
            this, SLOT( openBinding(QListViewItem *)) );
  connect(fileList, SIGNAL(returnPressed(QListViewItem *)),
            this, SLOT( openBinding(QListViewItem *)) );
  connect(fileList, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
            fileList, SLOT( openContext(KListView *, QListViewItem *, const QPoint &)) );

 }

void packageDisplayWidget::updateChangeLog()
{
  if (!package)
    return;

  QStringList lines;
  QString stmp;
  lines = package->interface->getChangeLog(package);


  changeLog->setTextFormat(Qt::LogText);
  changeLog->hide();
  if (lines.count() > 1) {
    changeLog->setText("");
    for (QStringList::ConstIterator it = lines.begin();
	 (it != lines.end());
	 it++) {
      if (! (*it).isEmpty())
	changeLog->append(*it);
      else
	changeLog->append(" ");
    }
  } else {
    changeLog->setText(i18n(" - No change log -"));
  }

  changeLog->show();
  changeLog->setContentsPos(0,0);

}

void packageDisplayWidget::updateFileList()
{
  if (!package)
    return;

  // Get a list of files in the package
  QStringList files;
  QStringList errorfiles;

  // set the status
  kpackage->setStatus(i18n("Updating File List"));

  // clear the file list
  fileList->clear();

  // Check if the package is installed
  int installed;
  if(package->getFilename().isEmpty()) {
    if(package->packageState == packageInfo::UPDATED) {
      fileList->setColumnText(0, "");
      return;
    } else
      installed=1;
  } else
    installed=0;

  files = package->interface->getFileList(package);

  if (files.count() == 0)
    return;

  // Get a list of files that failed verification
  if(installed && opts->VerifyFL) {
    errorfiles = package->interface->verify(package, files);
  }

  kpackage->setStatus(i18n("Updating File List"));

  uint c=0, p=0;
  uint step = (files.count() / 100) + 1;

  QString ftmp;
  ftmp.setNum(files.count());
  ftmp += i18n(" Files");

  fileList->setColumnText(0, ftmp);
  fileList->hide();
  fileList->setSorting(-1);

  QListViewItem *q;

  // Go through all the files
  for (QStringList::ConstIterator it = files.begin(); (it != files.end());  ) {
    // Update the status progress
    c++;
    if(c > step) {
      c=0;
      p++;
      kpackage->setPercent(p);
    }

    int error=0;
     
     QString cur = *it;
     it++;
     QPixmap pixmap;
     
    if (installed) { // see if file failed verification,

      if ( errorfiles.count() > 0) {
	for( QStringList::ConstIterator itError = errorfiles.begin();
	     (itError != errorfiles.end());
	     (itError++) ) {
	  if (cur == *itError) {
	    error = 1;
	  }
	}
      }
       if(error) pixmap=cross;
       else 
        pixmap=tick;
       
     } else 
       pixmap=question;    
     
     q = fileList->insert(cur, pixmap);   
 }

  fileList->setSorting(0);
  fileList->show();
  kpackage->setPercent(100);
}
  
 kpFileList::kpFileList(QWidget* parent, packageDisplayWidget* parent2) : KListView(parent)
 {
   hide();
   addColumn("name");
   setRootIsDecorated(TRUE);
   connect(this, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
             this, SLOT( openContext(KListView *, QListViewItem *, const QPoint &)) );
            
   FileListMenu = new KPopupMenu();
   openwith = FileListMenu->insertItem(i18n("&Open With..."),parent2,SLOT(__openBindingWith()));
   
   pkDisplay = parent2;
 }
 
 
 void packageDisplayWidget::__openBindingWith()
 {
        openBindingWith(fileList->selectedItem());
 }
 
 void packageDisplayWidget::openBindingWith(QListViewItem *index)
 {      
     if ( !index ) return; 
     KURL url;
     if (package && package->packageState == packageInfo::INSTALLED) {
       url.setPath( fileList->item2Path(index) ); // from local file to URL
       KRun::displayOpenWithDialog(KURL::List::List(url) );
      }      
 }
 
 
 void kpFileList::openContext(KListView *, QListViewItem *, const QPoint &p)
 {
        FileListMenu->setItemEnabled(openwith, 
        (selectedItem() && pkDisplay->package && pkDisplay->package->getFilename().isEmpty()) ? TRUE : FALSE);
        FileListMenu->exec(p);  
 
 }
 
 void kpFileList::clear() 
 {
        KListView::clear();
 }
 
 QString kpFileList::item2Path(QListViewItem *it)
 {
        QString res;
        res = it ? it->text(0) : NULL;  
        return res;
 }
 
 
 QListViewItem* kpFileList::insert(const QString &cur, const QPixmap &pixmap)
 {
   QListViewItem* q;

   q = new QListViewItem(this, cur);
   if (q) 
     q->setPixmap(0,pixmap);
   return q;       
 }
 
 void packageDisplayWidget::openBinding(QListViewItem *index)
 {
   if ( !index ) return; 
   KURL url;
   if (package && package->packageState == packageInfo::INSTALLED) {
     url.setPath( fileList->item2Path(index) ); // from local file to URL
     (void) new KRun ( url ); // run the URL
   }
 }


#include "packageDisplay.moc"
