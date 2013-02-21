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

#include <qlineedit.h>
#include <qpainter.h> 

#include <klocale.h>
#include <kdebug.h>
#include <kurldrag.h>
#include <kiconloader.h>

#include "kpackage.h"
#include "managementWidget.h"
#include "findf.h"
#include "options.h"
#include "pkgInterface.h"

extern pkgInterface *kpinterface[];
extern Opts *opts;

FindF::FindF(QWidget *parent)
    : KDialogBase(parent, "find_file", false,
		i18n("Find File"),
		User1 | Close, User1, true,
		KGuiItem(i18n("&Find"),"filefind"))
{
    tick = UserIcon("ptick");

    QFrame *page = makeMainWidget();

    setFocusPolicy(QWidget::StrongFocus);

    QVBoxLayout* vtop = new QVBoxLayout( page, 10, 10, "vtop");
    QFrame *frame1 = new QGroupBox(i18n("Find Package"), page, "frame1");
    vtop->addWidget(frame1,1);
 
    QGridLayout* gtop = new QGridLayout( frame1, 1, 1, 20 );
    //    gtop->setMargin( KDialog::marginHint() );
    gtop->setSpacing( KDialog::spacingHint() );

    value = new QLineEdit( frame1, "value" );
    connect(value,SIGNAL(textChanged ( const QString & )),this,SLOT(textChanged ( const QString & )));
    value->setFocus();

    QLabel *valueLabel = new QLabel(value, i18n("Find:"), frame1);
    valueLabel->setAlignment( AlignRight );

    tab = new KListView(frame1, "tab");
    connect(tab, SIGNAL(selectionChanged ( QListViewItem * )),
	    this, SLOT(search( QListViewItem * )));
    tab->addColumn(i18n("Installed"),18);
    tab->addColumn(i18n("Type"),110);
    tab->addColumn("",0);   // Hidden column for package type
    tab->addColumn(i18n("Package"),180);
    tab->addColumn(i18n("File Name"),330);
    tab->setAllColumnsShowFocus(TRUE);
    tab->setSorting(1);

    if (kpackage->management->dirInstPackages->find("apt-file/deb")) {
      searchAll = new QCheckBox(i18n("Also search uninstalled packages"), frame1, "searchAll");
    } else {
      searchAll = new QCheckBox(i18n("Also search uninstalled packages (apt-file needs to be installed)"), frame1, "searchAll");
    }
    searchAll->setChecked(FALSE);

    gtop->addWidget(valueLabel, 0, 0);
    gtop->addWidget(value, 0, 1);
    gtop->addMultiCellWidget(tab, 1, 1, 0, 1);

    gtop->addWidget(searchAll, 2, 0);

    connect(this, SIGNAL(user1Clicked()), this, SLOT(ok_slot()));
    connect(this, SIGNAL(closeClicked()), this, SLOT(done_slot()));
    enableButton(User1 , false);
    show();

    setAcceptDrops(true);
}

FindF::~FindF()
{
}

void FindF::checkSearchAll() 
{
    // button not enabled if no package interface has search uninstalled
    // packages for files ability
    bool hasAll = FALSE;
    for (int i = 0; i < kpinterfaceN; i++) {
      if (kpinterface[i] && opts->handlePackage[i]) {
	if (kpinterface[i]->hasSearchAll) 
	  hasAll = TRUE;
      }
    }
    
    searchAll->setEnabled(hasAll);
}

void FindF::textChanged ( const QString & text)
{
    enableButton(User1 , !text.isEmpty());
}

void FindF::ok_slot()
{
  doFind(value->text());
}

void FindF::doFind(const QString &str)
{
  QString t;
  int i, cnt = 0;

  bool all = searchAll->isChecked();

  QApplication::setOverrideCursor( waitCursor );

  tab->clear();

  for (i = 0; i < kpinterfaceN; i++) {
    if (kpinterface[i] && opts->handlePackage[i]) {
      QStringList filelist = kpinterface[i]->FindFile(str, all);

      if (filelist.count() > 0) {
	cnt++;

	for ( QStringList::Iterator it = filelist.begin(); it != filelist.end(); ++it ) {
	  if ((*it).find("diversion by") >= 0) {
	    new QListViewItem(tab, "", *it);
	  }

	  int t1 = (*it).find('\t');
	  QString s1 = (*it).left(t1);
	  QString s2 = (*it).right((*it).length()-t1);
	  s2 = s2.stripWhiteSpace();

	  QListViewItem *ql = new QListViewItem(tab, "", kpinterface[i]->name, kpinterface[i]->head, s1, s2);

	  QString tx = s1 + kpinterface[i]->typeID;
	  if (kpackage->management->dirInstPackages->find(tx)) {
	    ql->setPixmap(0,tick);
	  }
	}
      }
    }
  }

  if (!cnt) {
    new QListViewItem(tab, "", i18n("--Nothing found--"));
  }

  QApplication::restoreOverrideCursor();
}

void FindF::done_slot()
{
  hide();
}

void FindF::resizeEvent(QResizeEvent *){
}

void FindF::search(QListViewItem *item)
{
  int p;

  QString s = item->text(3);
  s = s.stripWhiteSpace();
  kdDebug() << "searchF=" << s << "\n";

  p = s.find(',');
  if (p > 0) {
    s.truncate(p);
  }

  KpTreeListItem *k =  kpackage->management->treeList->search(s ,item->text(2));
  if (k)
    kpackage->management->treeList->changePack(k);
}

void FindF::dragEnterEvent(QDragEnterEvent* e)
{
  e->accept(KURLDrag::canDecode(e));
}

void FindF::dropEvent(QDropEvent *de) // something has been dropped
{
  KURL::List list;
  if (!KURLDrag::decode(de, list) || list.isEmpty())
     return;

  const KURL &url = list.first();

  if (url.isLocalFile()) {
    QString file = url.path(-1);
    value->setText(file);
    doFind(file);
  } else {
    KpMsgE(i18n("Incorrect URL type"),FALSE);
  }
}

#include "findf.moc"
