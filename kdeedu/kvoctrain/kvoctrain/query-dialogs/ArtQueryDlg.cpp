/***************************************************************************

                     query dialog for articles

    -----------------------------------------------------------------------

    begin          : Fri Dec 3 18:28:18 1999

    copyright      : (C) 1999-2001 Ewald Arnold <kvoctrain@ewald-arnold.de>
                     (C) 2001 The KDE-EDU team
                     (C) 2004-2005 Peter Hedlund <peter.hedlund@kdemail.net>

    -----------------------------------------------------------------------

 ***************************************************************************

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "ArtQueryDlg.h"

#include <kv_resource.h>

#include <kstandarddirs.h>
#include <klocale.h>
#include <kapplication.h>
#include <kprogress.h>

#include <qtimer.h>
#include <qradiobutton.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>

ArtQueryDlg::ArtQueryDlg
(
        QString type,
        int entry,
        int col,
        int query_cycle,
        int query_num,
        int query_startnum,
        kvoctrainExpr *exp,
        kvoctrainDoc  *doc,
        const Article &articles)
  : QueryDlgBase(i18n("Article Training"))
{
  mw = new ArtQueryDlgForm(this);
  setMainWidget(mw);

  qtimer = 0;

  mw->artGroup->insert (mw->natural);
  mw->artGroup->insert (mw->male);
  mw->artGroup->insert (mw->rb_fem);

  connect(mw->dont_know, SIGNAL(clicked()), SLOT(dontKnowClicked()) );
  connect(mw->know_it, SIGNAL(clicked()), SLOT(knowItClicked()) );
  connect(mw->verify, SIGNAL(clicked()), SLOT(verifyClicked()) );
  connect(mw->show_all, SIGNAL(clicked()), SLOT(showAllClicked()) );

  connect(mw->natural, SIGNAL(clicked()), SLOT(slotNaturalClicked()) );
  connect(mw->male, SIGNAL(clicked()), SLOT(slotMaleClicked()) );
  connect(mw->rb_fem, SIGNAL(clicked()), SLOT(slotFemClicked()) );

  setQuery (type, entry, col, query_cycle, query_num, query_startnum, exp, doc, articles);
  mw->countbar->setFormat("%v/%m");
  mw->timebar->setFormat("%v");
  resize(configDialogSize("ArtQueryDialog"));
}


ArtQueryDlg::~ArtQueryDlg()
{
  saveDialogSize("ArtQueryDialog");
}


void ArtQueryDlg::setQuery(QString,
                           int entry,
                           int col,
                           int q_cycle,
                           int q_num,
                           int q_start,
                           kvoctrainExpr *exp,
                           kvoctrainDoc  *doc,
                           const Article &art)
{
   //type_timeout = type_to;
   kv_exp = exp;
   kv_doc = doc;
   q_row = entry;
   q_ocol = col;
   mw->timebar->setEnabled(Prefs::showCounter());
   mw->timelabel->setEnabled(Prefs::showCounter());
   mw->show_all->setDefault(true);
   articles = art;

   QString s;
   if (col == 0)
     s = exp->getOriginal().stripWhiteSpace();
   else
     s = exp->getTranslation(q_ocol).stripWhiteSpace();

   QString def, indef;
   bool removed = false;

   articles.female(def, indef);
   mw->rb_fem->setText (i18n("&female:\t")+def+" / "+indef);
   mw->rb_fem->setEnabled (!QString(def+indef).isEmpty() );
   if (!removed && s.find(def+" ") == 0) {
     s.remove (0, def.length()+1);
     removed = true;
   }
   if (!removed && s.find(indef+" ") == 0) {
     s.remove (0, indef.length()+1);
     removed = true;
   }

   articles.male(def, indef);
   mw->male->setText (i18n("&male:\t")+def+" / "+indef);
   mw->male->setEnabled (!QString(def+indef).isEmpty() );
   if (!removed && s.find(def+" ") == 0) {
     s.remove (0, def.length()+1);
     removed = true;
   }
   if (!removed && s.find(indef+" ") == 0) {
     s.remove (0, indef.length()+1);
     removed = true;
   }

   articles.natural(def, indef);
   mw->natural->setText (i18n("&natural:\t")+def+" / "+indef);
   mw->natural->setEnabled (!QString(def+indef).isEmpty() );
   if (!removed && s.find(def+" ") == 0) {
     s.remove (0, def.length()+1);
     removed = true;
   }
   if (!removed && s.find(indef+" ") == 0) {
     s.remove (0, indef.length()+1);
     removed = true;
   }

   mw->orgField->setText(s);
   s.setNum (q_cycle);
   mw->progCount->setText (s);

   mw->countbar->setTotalSteps(q_start);
   mw->countbar->setProgress(q_start - q_num + 1);
   int mqtime = Prefs::maxTimePer();
   if (mqtime > 0) {
     if (qtimer == 0) {
       qtimer = new QTimer( this );
       connect( qtimer, SIGNAL(timeout()), this, SLOT(timeoutReached()) );
     }

     if (Prefs::queryTimeout() != Prefs::EnumQueryTimeout::NoTimeout) {
       timercount = mqtime;
       mw->timebar->setTotalSteps(timercount);
       mw->timebar->setProgress(timercount);
       qtimer->start(1000, TRUE);
     }
     else
       mw->timebar->setEnabled(false);
   }
   else
     mw->timebar->setEnabled(false);
}


void ArtQueryDlg::initFocus() const
{
  mw->rb_fem->setFocus();
}


void ArtQueryDlg::showAllClicked()
{
  resetButton(mw->rb_fem);
  resetButton(mw->male);
  resetButton(mw->natural);

  if (kv_exp->getType (q_ocol) == QM_NOUN  QM_TYPE_DIV  QM_NOUN_F) {
    mw->rb_fem->setChecked (true);
    verifyButton(mw->rb_fem, true);
  }
  else if (kv_exp->getType (q_ocol) == QM_NOUN  QM_TYPE_DIV  QM_NOUN_M) {
    mw->male->setChecked (true);
    verifyButton(mw->male, true);
  }
  else if (kv_exp->getType (q_ocol) == QM_NOUN  QM_TYPE_DIV  QM_NOUN_S) {
    mw->natural->setChecked (true);
    verifyButton(mw->natural, true);
  }
  mw->dont_know->setDefault(true);
}


void ArtQueryDlg::showMoreClicked()
{
}


void ArtQueryDlg::verifyClicked()
{
  bool known = false;
  if (kv_exp->getType (q_ocol) == QM_NOUN  QM_TYPE_DIV  QM_NOUN_F)
    known = mw->rb_fem->isChecked ();
  else if (kv_exp->getType (q_ocol) == QM_NOUN  QM_TYPE_DIV  QM_NOUN_M)
    known = mw->male->isChecked ();
  else if (kv_exp->getType (q_ocol) == QM_NOUN  QM_TYPE_DIV  QM_NOUN_S)
    known = mw->natural->isChecked ();

  if (mw->rb_fem->isChecked() ) {
    verifyButton(mw->rb_fem, known);
    resetButton(mw->male);
    resetButton(mw->natural);
  }
  else if (mw->male->isChecked() ) {
    verifyButton(mw->male, known);
    resetButton(mw->rb_fem);
    resetButton(mw->natural);
  }
  else if (mw->natural->isChecked() ) {
    verifyButton(mw->natural, known);
    resetButton(mw->male);
    resetButton(mw->rb_fem);
  }

  if (known)
//    know_it->setDefault(true);
    knowItClicked();
  else
    mw->dont_know->setDefault(true);
}


void ArtQueryDlg::knowItClicked()
{
   emit sigQueryChoice(Known);
}


void ArtQueryDlg::timeoutReached()
{
   if (timercount > 0) {
     timercount--;
     mw->timebar->setProgress(timercount);
     qtimer->start(1000, TRUE);
   }

   if (timercount <= 0) {
     mw->timebar->setProgress(0);
     if (Prefs::queryTimeout() == Prefs::EnumQueryTimeout::Show) {
       showAllClicked();
       mw->dont_know->setDefault(true);
     }
     else if (Prefs::queryTimeout() == Prefs::EnumQueryTimeout::Continue)
       emit sigQueryChoice(Timeout);
   }
}


void ArtQueryDlg::dontKnowClicked()
{
   emit sigQueryChoice (Unknown);
}


void ArtQueryDlg::slotUser2()
{

   if (qtimer != 0)
     qtimer->stop();

   emit sigEditEntry (q_row, KV_COL_ORG+q_ocol);
}


void ArtQueryDlg::keyPressEvent( QKeyEvent *e )
{
  switch( e->key() )
  {
    case Key_Escape:
      dontKnowClicked();
    break;

    case Key_Return:
    case Key_Enter:
      if (mw->dont_know->isDefault() )
        dontKnowClicked();
      else if (mw->know_it->isDefault() )
        knowItClicked();
      else if (mw->show_all->isDefault() )
        showAllClicked();
      else if (mw->verify->isDefault() )
        verifyClicked();
    break;

    default:
      e->ignore();
    break;
  }
}


void ArtQueryDlg::returnPressed()
{
}


void ArtQueryDlg::slotFemClicked()
{
  resetButton(mw->rb_fem);
  resetButton(mw->male);
  resetButton(mw->natural);
  verifyClicked();
}


void ArtQueryDlg::slotMaleClicked()
{
  resetButton(mw->male);
  resetButton(mw->natural);
  resetButton(mw->rb_fem);
  verifyClicked();
}


void ArtQueryDlg::slotNaturalClicked()
{
  resetButton(mw->natural);
  resetButton(mw->male);
  resetButton(mw->rb_fem);
  verifyClicked();
}


#include "ArtQueryDlg.moc"
