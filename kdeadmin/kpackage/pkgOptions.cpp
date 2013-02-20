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
// qt headers
#include <qlabel.h>

#include <klocale.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <klistview.h>
#include <kseparator.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

#include "pkgOptions.h"
#include "managementWidget.h"
#include "debInterface.h"
#include "kpackage.h"
#include "options.h"

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

pkgOptions::pkgOptions(pkgInterface *pki, QWidget *parent, const QString &caption)
  :  KDialog(parent,0,TRUE)
{
  //  setFrameStyle(QFrame::Raised | QFrame::Panel);

  pkgInt = pki;

  hide();

  setCaption(caption);
}

// Destructor
pkgOptions::~pkgOptions()
{
  //  int i;
  //  for (i = 0; i < bnumber; i++) {
  //    delete(Boxs[i]);
  //  }
}

// Set up the sub-widgets
void pkgOptions::setupWidgets(QPtrList<param> &pars)
{
  int i;

  // Create widgets
  title = new QLabel("", this);
  QFont f( KGlobalSettings::generalFont());
  f.setBold(true);
  f.setPointSize(f.pointSize()+6);
  title->setFont(f);
  //  title->setAutoResize(TRUE);
  //  title->update();

  installButton = new QPushButton(insType,this);
  cancelButton = new KPushButton(KStdGuiItem::cancel(),this);
  // count number of buttons
  bnumber = pars.count();

  Boxs = new QCheckBox *[bnumber];
  param *p;
  i = 0;
  for ( p=pars.first(); p != 0; p=pars.next(), i++ ) {
    Boxs[i] = new QCheckBox(p->name, this);
    Boxs[i]->setChecked(p->init);
  }

  Keep = new QCheckBox(i18n("Keep this window"), this);

  // Connections
  connect(installButton,SIGNAL(clicked()),SLOT(pkginstallButtonClicked()));
  connect(cancelButton,SIGNAL(clicked()),SLOT(cancelButtonClicked()));
  connect(Keep, SIGNAL(toggled(bool)), SLOT(keepToggle(bool)));

  // Do the layout
  vlayout = new QBoxLayout(this, QBoxLayout::TopToBottom, marginHint(), spacingHint());
  vlayout->addWidget(title,0);

  {
    hlayout = new QBoxLayout(vlayout,QBoxLayout::LeftToRight, spacingHint());

    {
      layout = new QBoxLayout(hlayout,QBoxLayout::TopToBottom, spacingHint());

      packages = new KListView(this);
      layout->addWidget(packages,20);
      packages->addColumn(i18n("PACKAGES"),200);

      connect(packages, SIGNAL(selectionChanged ( QListViewItem * )),
	    this, SLOT(slotSearch( QListViewItem * )));

      layout->addStretch(1);
      for (i = 0; i < bnumber; i++) {
	layout->addWidget(Boxs[i],1);
      }
      layout->addWidget(new KSeparator(KSeparator::HLine, this), 2);

      QBoxLayout *slayout = new QBoxLayout(layout, QBoxLayout::LeftToRight);
      slayout->addStretch(1);
      slayout->addWidget(Keep, 1);
      slayout->addStretch(1);

      layout->addWidget(new KSeparator(KSeparator::HLine, this), 2);

      QBoxLayout *buttons = new QBoxLayout(QBoxLayout::LeftToRight);
      layout->addLayout(buttons);

      buttons->addWidget(installButton,2);
      buttons->addStretch(1);
      buttons->addWidget(cancelButton,2);
    }
    {
      term = new kpTerm(kpty,this);
      hlayout->addWidget(term, 1000);
    }
  }
  resize(800, 400);
}

void pkgOptions::setup(packageInfo *p, const QString &type) {
  QPtrList<packageInfo> *pl = new QPtrList<packageInfo>;
  pl->append(p);
  setup(pl,type);
}

bool pkgOptions::setup(QPtrList<packageInfo> *pl, const QString &)
{
 QString s;
 modified = FALSE;

 packList = pl;

  packages->clear();
  packageInfo *p;

  QStringList plist, rlist, clist;
  QDict<QString> dict;
  QString mark("x");
  for ( p = pl->first(); p != 0; p = pl->next() ) {
    QString file =  p->getFilename();
    plist += p->getProperty("name");
    if (file.isEmpty()) {
      clist += p->getProperty("name");
    }
    dict.insert(p->getProperty("name"), &mark);
  }

  packageInfo *pk;
  bool cancel;
  if (clist.count() > 0) {
    rlist = pkgInt->listInstalls(clist, installer, cancel);
    if (cancel) {
      reject();
      return false;
    }
    for ( QStringList::Iterator it = rlist.begin(); it != rlist.end(); ++it ) {
      if (!dict[*it]) {
	plist.append(*it);
	QString dirIndex = *it + pkgInt->typeID;
	if (installer) {
	  pk = kpackage->management->dirUninstPackages->find(dirIndex);
	} else {
	  pk = kpackage->management->dirInstPackages->find(dirIndex);
	}
	if (pk) {
	  //	  kdDebug() << "FF=" << dirIndex << "\n";
	  pl->append(pk);
	} else {
	  //	  kdDebug() << "uF=" << dirIndex << "\n";
	}
      }
    }
  }

 s = i18n("%1: 1 %2 Package","%1: %n %2 Packages",plist.count()).arg(insType,pkgInt->name);
 title->setText(s);

  for (QStringList::Iterator pit = plist.begin(); pit != plist.end(); ++pit ) {
    //    kdDebug() << "P=" << *pit << "\n";
    new QListViewItem(packages, *pit);
  }
  cancelButton->setGuiItem(KStdGuiItem::cancel());
  return TRUE;
}

// install button has been clicked....so install the package
void pkgOptions::pkginstallButtonClicked()
{
  int i;
  QStringList r;
  modified = TRUE;

  // Collect data from check boxes
  int installFlags = 0;

  for (i = 0; i < bnumber; i++) {
    installFlags |= (Boxs[i]->isChecked()) << i;
  }

  test = FALSE;
  QString s = doPackages(installFlags, packList, test);
  // A "0=" or "1=" indicates it was actually (un)installed by the doPackages
  // routine instead of just returning a command to execute

  kdDebug() <<  "S=" << s << "\n";
  if (s == "0=") {
    cancelButtonClicked();
  } else if (s.left(2) == "1=") {
    term->textIn(s.mid(2), true);
  } else {
    connect(term,SIGNAL(result(QStringList &, int)),
	 this,SLOT(slotResult(QStringList &, int)));

    installButton->setEnabled(FALSE);

    if (term->run(s, r)) {
      running = TRUE;
      cancelButton->setGuiItem(KStdGuiItem::cancel());
    } else {
      reset();
    }
  }
}

void pkgOptions::slotSearch(QListViewItem *item)
{
  QString s = item->text(0);
  kdDebug() << "searchI=" << s << "h=" << pkgInt->head <<"\n";

  packageInfo *p;
  for ( p = packList->first(); p != 0; p = packList->next() ) {
    if (s == p->getProperty("name")) {
      kpackage->management->doChangePackage(p);
      break;
    }
  }
}

void pkgOptions::reset() {
  installButton->setEnabled(TRUE);
  cancelButton->setGuiItem(KGuiItem(i18n("Done")));  //clear icon
  disconnect(term,SIGNAL(result(QStringList &, int)),
	 this,SLOT(slotResult(QStringList &, int)));
  running = FALSE;
}

void pkgOptions::slotResult(QStringList &, int ret)
{
  reset();
  if (ret == 0 && !test && !keep) {
    term->done();
    accept();
  }
}

void pkgOptions::terminate() {
  if (running) {
    term->cancel();
    reset();
  }
}

void pkgOptions::cancelButtonClicked()
{
  terminate();
  term->done();

  if (!modified || test)
    reject();
  else
    accept();
}

void pkgOptions::closeEvent ( QCloseEvent * e ) {
  kdDebug() << "pkgOptions::QCloseEvent\n";
  terminate();
   
  QWidget::closeEvent (e);
}

void pkgOptions::showEvent ( QShowEvent *e ) {
  //  kdDebug() << "pkgOptions::showEvent\n";
  getKeep();

  modified = FALSE;
  running = FALSE;

  QWidget::showEvent(e);
}

void pkgOptions::keepToggle(bool kp)
{
  //  kdDebug() << "KEEP " << kp << "\n";

  KConfig *config = kapp->config();

  config->setGroup("Kpackage");
  config->writeEntry("keepIWin", kp);

  keep = kp;
}

void pkgOptions::getKeep()
{
    KConfig *config = kapp->config();
    config->setGroup("Kpackage");
    keep =  config->readBoolEntry("keepIWin", true);
  kdDebug() << "getKEEP " << keep << "\n";
    Keep->setChecked(keep);

}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
pkgOptionsI::pkgOptionsI(pkgInterface *pkg, QWidget *parent):
  pkgOptions(pkg, parent, i18n("Install"))
{
  insType = i18n("Install");
  installer = TRUE;
  setupWidgets(pkg->paramsInst);
}

QString pkgOptionsI::doPackages(int installFlags, QPtrList<packageInfo> *p, bool &test)
{
  return pkgInt->install(installFlags, p, test);
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
pkgOptionsU::pkgOptionsU(pkgInterface *pkg, QWidget *parent):
  pkgOptions(pkg, parent, i18n("Uninstall"))
{
  insType = i18n("Uninstall");
  installer = FALSE;
  setupWidgets(pkg->paramsUninst);
}

QString pkgOptionsU::doPackages(int installFlags, QPtrList<packageInfo> *p, bool &test)
{
  return pkgInt->uninstall(installFlags, p, test);
}
#include "pkgOptions.moc"
