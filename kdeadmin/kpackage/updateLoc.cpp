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
#include "updateLoc.h"
#include "pkgInterface.h"
#include "options.h"
#include "cache.h"

#include <qvbox.h>
#include <qscrollview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kcombobox.h>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
apanel::apanel( QWidget *parent,
              const char * name )
   : QWidget( parent, name )
{
  puse = 0;

  pack = new QHBoxLayout(this);
  {
    puse = new QCheckBox(i18n("Use"),this);
    pack->addWidget(puse,0);

    pent = new QLineEdit(this);
    pent->setMinimumWidth(600);
    pack->addWidget(pent,0);

  }
}

apanel::~apanel()
{
}

QString apanel::getText() const
{
  QString s = pent->text();
  return s;
}

void apanel::setText(const QString &s)
{
 pent->setText(s);
}

bool apanel::getUse()
{
  if (puse)
    return puse->isChecked();
  else
    return FALSE;
}

void apanel::setUse(int n)
{
  if (puse) {
    if (n)
      puse->setChecked(TRUE);
    else
      puse->setChecked(FALSE);
  }
}

void apanel::clear()
{
      puse->setChecked(FALSE);
      pent->clear();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
dpanel::dpanel( QWidget *parent, const char * name )
   : QWidget( parent, name )
{
  puse = 0;
  psubdirs = 0;

  pack = new QHBoxLayout(this);
  {
    pent = new QLineEdit(this);
    pent->setMinimumWidth(280);
    pack->addWidget(pent,0);

    pbut = new QPushButton("...",this);
    pack->addWidget(pbut,0);

    connect(pbut, SIGNAL(clicked()), this, SLOT(dirOpen()));

  }
}

dpanel::dpanel(dpanel *basep,  const QString &Pfilter,  bool bsubdirs,
	       QWidget *parent, const char * name )
   : QWidget( parent, name )
{
  filter = Pfilter;
  puse = 0;
  psubdirs = 0;

  base = basep;

  pack = new QHBoxLayout(this);
  {
    puse = new QCheckBox(i18n("Use"),this);
    pack->addWidget(puse,0);

    pent = new QLineEdit(this);
    pent->setMinimumWidth(280);
    pack->addWidget(pent,0);

    if (bsubdirs) {
      psubdirs = new QCheckBox(i18n("Subfolders"),this);
      psubdirs->setFixedSize(psubdirs->sizeHint());
      pack->addWidget(psubdirs,0);
    }

    pbut = new QPushButton("...",this);
    pack->addWidget(pbut,0);

    if (base)
      connect(pbut, SIGNAL(clicked()), this, SLOT(fileOpen()));
    else
      connect(pbut, SIGNAL(clicked()), this, SLOT(dirOpen()));

  }
}

dpanel::~dpanel()
{
}

QString dpanel::getText() const
{
  QString s = pent->text();
  return s;
}

void dpanel::setText(const QString &s)
{
 pent->setText(s);
}

bool dpanel::getUse() const
{
  if (puse)
    return puse->isChecked();
  else
    return FALSE;
}

void dpanel::setUse(int n)
{
  if (puse) {
    if (n)
      puse->setChecked(TRUE);
    else
      puse->setChecked(FALSE);
  }
}

bool dpanel::getSubdirs() const
{
  if (psubdirs)
    return psubdirs->isChecked();
  else
    return FALSE;
}

void dpanel::setSubdirs(int n)
{
  if (psubdirs)
  {
    if (n)
      psubdirs->setChecked(TRUE);
    else
      psubdirs->setChecked(FALSE);
  }
}

void dpanel::fileOpen()
{
  QString st;

  if (base && getText().isEmpty()) {
    st = base->getText();
  } else {
    st = getText();
  }
  if (st.right(8) == "Packages") {
    st.truncate(st.length() - 8);
  }

  KURL url =  KFileDialog::getOpenURL
    (st, filter, 0, i18n("Package File"));

  if( url.isEmpty() )
    return;

  pent->setText( url.url() );
}

void dpanel::dirOpen()
{
  QString st;

  if (base && getText().isEmpty()) {
    st = base->getText();
  } else {
    st = getText();
  }

  KURL url  =  KFileDialog::getExistingURL
    (st, 0, i18n("Package Archive"));


  if( url.isEmpty() )
    return;

  pent->setText( url.url() );

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
updateLoc::updateLoc(QWidget *p, int panelNumber, pkgInterface *inter, const QString &iname)
    : QWidget(p,"updateLoc",FALSE)
{
  interName = iname;
  interface = inter;
  panNumber = panelNumber;

  vf = new QVBoxLayout( this, 15, 10, "vf");
}

updateLoc::~updateLoc()
{
}

void updateLoc::doBase(const QString & bmsg)
{
  base = 0;
  if (haveBase) {
    fbase = new QGroupBox(bmsg, this);
    fbase->setColumnLayout(0, Qt::Vertical );
    fbase->layout()->setSpacing( KDialog::spacingHint() );
    fbase->layout()->setMargin( KDialog::marginHint() );
    vbase = new QVBoxLayout(fbase->layout());
    vf->addWidget(fbase,1);
    base = new dpanel(fbase);
    vbase->addWidget(base,0);
    vbase->activate();
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

aUpdateLoc::aUpdateLoc(QWidget *p, int panelNumber, pkgInterface *inter, const QString &iname,
			 int numberLines, const QString &label)
  : updateLoc(p, panelNumber, inter, iname)
{

  if (numberLines > PNUM)
    numLines = PNUM;
  else
    numLines = numberLines;

  ap[0] = 0;

  QGroupBox *floc = new QGroupBox(1,Qt::Vertical, label, this);
  vf->addWidget(floc,1);
  QScrollView* sv = new QScrollView(floc);
    sv->setHScrollBarMode(QScrollView::AlwaysOff);
    sv->setResizePolicy(QScrollView::AutoOneFit);

      QFrame *f = new QFrame(sv->viewport());
    sv->addChild(f);

  QVBoxLayout *vloc = new QVBoxLayout(f, 0, 3, "vloc");

  for (int i = 0; i < numLines; i++) {
    ap[i] = new apanel( f);
    vloc->addWidget(ap[i],0);
  }

}


aUpdateLoc::~aUpdateLoc()
{
}

void aUpdateLoc::readSettings()
{
  int i = 0;
  QString a,b;

  for (int i = 0; i < numLines; i++) {
    ap[i]->clear();
  }

  QStringList list = interface->readApt();
  for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) {
    if (!(*it).isEmpty()) {
      if ((*it).left(1) == "#") {
	ap[i]->setText(((*it).mid(1)).stripWhiteSpace());
	ap[i]->setUse(0);
      } else {
	ap[i]->setText((*it));
	ap[i]->setUse(1);
      }
      i++;
      if (i >= numLines) {
	ap[numLines - 1]->setText(i18n("File truncated..."));
	break;
      }
    }
  }
}

void aUpdateLoc::writeSettings() {
  QStringList list;
  QString s;

  QString ln;
  for (int i = 0; i < numLines; i++) {
    if (!ap[i])
      break;

    ln = ap[i]->getText();
    if (!ln.isEmpty()) {
      if (ap[i]->getUse()) {
	s = "";
      } else {
	s = "# ";
      }
      s += ln;
      list.append(s);
    }
  }
  interface->writeApt(list);
}

void aUpdateLoc::applyS(LcacheObj *) {
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

pdUpdateLoc::pdUpdateLoc(QWidget *p, int panelNumber, pkgInterface *inter, const QString &iname,
			 int numberLines, const QString &filter,
			 const QString &lmsg, QString bmsg,
			 bool subdirs )
  : updateLoc(p, panelNumber, inter, iname)
{

  if (numberLines > PNUM)
    numLines = PNUM;
  else
    numLines = numberLines;

  haveBase = !bmsg.isNull();

  dp[0] = 0;

  QString pn;
  pn.setNum(panNumber);
  pn += "_";

  packL = "Package_L_";
  packL += pn;
  packU = "Package_U_";
  packU += pn;
  packS = "Package_S_";
  packS += pn;
  availB = "Available_Base";
  availB += pn;

  doBase(bmsg);

  QGroupBox *floc = new QGroupBox(lmsg, this);
  floc->setColumnLayout(0, Qt::Vertical );
  floc->layout()->setSpacing( KDialog::spacingHint() );
  floc->layout()->setMargin( KDialog::marginHint() );
  vf->addWidget(floc,1);
  QVBoxLayout *vloc = new QVBoxLayout(floc->layout());

  for (int i = 0; i < numLines; i++) {
    dp[i] = new dpanel(base, filter,  subdirs, floc);
    vloc->addWidget(dp[i],0);
  }

  readSettings();
}


pdUpdateLoc::~pdUpdateLoc()
{
}

void pdUpdateLoc::applyS(LcacheObj *slist)
{
  QString t,pn,cn,pv,prev,opts;
  cacheObj *CObj;
  KConfig *config = kapp->config();
  config->setGroup(interName);

  cn = interface->head;
  cn += "_";
  cn += pn.setNum(panNumber);
  cn += "_";


  for (int i = 0; i < numLines; i++) {
    // delete chached dir if text changed
    pv = packL + pn.setNum(i);
    prev = config->readEntry(pv);
    if (prev != dp[i]->getText())
      cacheObj::rmDCache(QString(cn + pn.setNum(i)));

    // create cache object corresponding to this entry
    if (dp[i]->getUse()) {
      t = dp[i]->getText();
      if (!t.isEmpty()) {
	CObj = new cacheObj(haveBase ? base->getText() : QString::null,
			    t, cn + pn.setNum(i), "", dp[i]->getSubdirs());
	slist->append(CObj);
	//	printf("T=%s\n",t.data());
      }
    }
  }
  //  writeSettings();
}

void pdUpdateLoc::readSettings()
{
  QString pv, pn;

  KConfig *config = kapp->config();

  config->setGroup(interName);

  if (haveBase)
    base->setText(config->readEntry(availB));

  for (int i = 0; i < numLines; i++) {
    if (!dp[i])
      break;
    pv = packL + pn.setNum(i);
    dp[i]->setText(config->readEntry(pv));
    pv = packU + pn.setNum(i);
    dp[i]->setUse(config->readNumEntry(pv));
    pv = packS + pn.setNum(i);
    dp[i]->setSubdirs(config->readNumEntry(pv));
  }
}

void pdUpdateLoc::writeSettings()
{
  QString pv, pn;

  KConfig *config = kapp->config();

  config->setGroup(interName);
  if (haveBase) {
    if (!base->getText().isEmpty())
      config->writeEntry(availB,base->getText());
  }

  for (int i = 0; i < numLines; i++) {
    if (!dp[i])
      break;
    pv = packL + pn.setNum(i);
    config->writeEntry(pv,dp[i]->getText());
    pv = packU + pn.setNum(i);
    config->writeEntry(pv,(int)dp[i]->getUse());
    pv = packS + pn.setNum(i);
    config->writeEntry(pv,(int)dp[i]->getSubdirs());
  }

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
cUpdateLoc::cUpdateLoc(QWidget *p, int panelNumber, pkgInterface *inter, const QString &iname,
		       int numberLines, const QString &baseMsg,
		       const QString &boxLabels, const QString &boxValues)
  : updateLoc(p, panelNumber, inter, iname)
{
  if (numberLines > PNUM)
    numLines = PNUM;
  else
    numLines = numberLines;

  haveBase = !baseMsg.isNull();

  combo[0] = 0;

  QString pn;
  pn.setNum(panNumber);
  pn += "_";

  packC = "Package_C_";
  packC += pn;
  availB = "Available_Base";
  availB += pn;

  QStringList kc = QStringList::split('\n', boxLabels);
  QStringList::Iterator kcIt = kc.begin();

  QStringList oc = QStringList::split('\n', boxValues);
  QStringList::Iterator ocIt = oc.begin();


  doBase(baseMsg);

  for (int i = 0; i < numLines; i++) {
    QGroupBox *floc = new QGroupBox(*kcIt, this);
    floc->setColumnLayout(0, Qt::Vertical );
    floc->layout()->setSpacing( KDialog::spacingHint() );
    floc->layout()->setMargin( KDialog::marginHint() );
    vf->addWidget(floc,1);
    QVBoxLayout *vloc = new QVBoxLayout(floc->layout());

    combo[i] = new KComboBox( true, floc);
    KCompletion *comp = combo[i]->completionObject();
    connect(combo[i],SIGNAL(returnPressed(const QString&))
	    ,comp,SLOT(addItem(const QString&)));
    combo[i]->insertStringList(QStringList::split(' ',*ocIt));
    vloc->addWidget(combo[i]);
    if (kcIt != kc.end()) {
      ++kcIt;
    }
    if (ocIt != oc.end()) {
      ++ocIt;
    }
  }

  readSettings();
}

cUpdateLoc::~cUpdateLoc()
{
}

 void cUpdateLoc::applyS(LcacheObj *slist)
{
  QString t,pn,cn,pv,prev,opts;
  cacheObj *CObj;
  KConfig *config = kapp->config();
  config->setGroup(interName);

  cn = interface->head;
  cn += "_";
  cn += pn.setNum(panNumber);
  cn += "_";

    if (!base->getText().isEmpty()) {
      for (int i = 0; i < numLines; i++) {
	opts +=  combo[i]->currentText();
	opts += "\n";
      }
      CObj = new cacheObj(base->getText(),
			  "", cn + ":", opts);
      slist->append(CObj);
    }
    //  writeSettings();
}

void cUpdateLoc::readSettings()
{
  QString pv, pn;

  KConfig *config = kapp->config();

  config->setGroup(interName);

  if (haveBase)
    base->setText(config->readEntry(availB));

  for (int i = 0; i < numLines; i++) {
    if (!combo[i])
      break;
    pv = packC + pn.setNum(i);
    if (!config->readEntry(pv).isEmpty()) {
      combo[i]->insertItem(config->readEntry(pv), 0);
    }
  }

}

void cUpdateLoc::writeSettings()
{
  QString pv, pn;

  KConfig *config = kapp->config();

  config->setGroup(interName);

  if (haveBase) {
      config->writeEntry(availB,base->getText());
  }

  for (int i = 0; i < numLines; i++) {
    if (!combo[i])
      break;
    pv = packC + pn.setNum(i);
    config->writeEntry(pv,combo[i]->currentText());
  }

}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
Locations::Locations(const QString &msg)
    : KDialogBase(Tabbed, msg, Ok | Cancel, Ok, 0, "Locations", false)
{
  numPanels = 0;

  connect( this, SIGNAL(okClicked()), SLOT(write_slot()) );
}

void Locations::dLocations(int numberDirs,  int numberLines,
		     pkgInterface *inter, const QString &iname,
		     const QString &label, const QString &filter, const QString &dirMsg,
		     bool subdirs)
{
  QString nm;

  for (int i = numPanels ; (i <  numPanels + numberDirs) && (i < PANNUM); i++) {

    QString mp = iname;
    nm.setNum(i+1);
    mp += nm;
    QVBox *page = addVBoxPage(mp);
    pn[i] = new pdUpdateLoc(page, i, inter,
			    label, numberLines,
			    filter, dirMsg, NULL, subdirs);
  }
  numPanels += numberDirs;
}

void Locations::pLocations(int numberDirs,  int numberLines,
		     pkgInterface *inter, const QString &iname, const QString &label,
		     const QString &filter,
		     const QString &packMsg, QString baseMsg, bool subdirs)
{
  QString nm;

  for (int i = numPanels; (i < numPanels + numberDirs) && (i < PANNUM); i++) {
    QString mp = iname;
    nm.setNum(i+1);
    mp += nm;
    QVBox *page = addVBoxPage(mp);
    pn[i] = new pdUpdateLoc(page, i, inter, label,
			    numberLines,
			    filter, packMsg, baseMsg, subdirs);
  }
  numPanels += numberDirs;
}

void Locations::cLocations(int numberDirs,   int numberLines,
		     pkgInterface *inter, const QString &iname, const QString &label,
		     const QString &boxLables, const QString &baseMsg, const QString &boxValues)
{
  QString nm;

  for (int i = numPanels; (i < numPanels + numberDirs) && (i < PANNUM); i++) {
    QString mp = iname;
    nm.setNum(i+1);
    mp += nm;
    QVBox *page = addVBoxPage(mp);
    pn[i] = new cUpdateLoc(page, i, inter, label,
			   numberLines, baseMsg,
			   boxLables, boxValues);
  }
  numPanels += numberDirs;
}

void Locations::aLocations(int numberDirs,   int numberLines,
		     pkgInterface *inter, const QString &iname, const QString &label)
{
  QString nm;

  for (int i = numPanels; (i < numPanels + numberDirs) && (i < PANNUM); i++) {
    QString mp = iname;
    nm.setNum(i+1);
    mp += nm;
    QVBox *page = addVBoxPage(mp);
    pn[i] = new aUpdateLoc(page, i, inter, iname,
			   numberLines, label);
  }
  numPanels += numberDirs;
}

Locations::~Locations() {
  //  for (int i = 0; (i < numPanels) && (i < PANNUM) && pn[i]; i++) {
  //    delete pn[i];
  //  }
}

void Locations::restore()
{
  for (int i = 0; (i <  numPanels) && (i < PANNUM); i++) {
    pn[i]->readSettings();
  }
  show();
}

void Locations::apply_slot()
{
   LcacheObj *slist = new LcacheObj();

  for (int i = 0; (i <  numPanels) && (i < PANNUM); i++) {
    pn[i]->applyS(slist);
  }
  emit returnVal(slist);
}

void Locations::write_slot()
{
  apply_slot();

  for (int i = 0; (i <  numPanels) && (i < PANNUM); i++) {
    pn[i]->writeSettings();
  }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#include "updateLoc.moc"
