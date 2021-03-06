/***************************************************************************

                    dialog when in query mode

    -----------------------------------------------------------------------

    begin          : Thu Mar 11 20:50:53 MET 1999

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

#include "RandomQueryDlg.h"
#include "common-dialogs/ProgressDlg.h"

#include <kv_resource.h>

#include <kstandarddirs.h>
#include <klocale.h>
#include <kapplication.h>
#include <kprogress.h>

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qregexp.h>
#include <qtimer.h>

QStringList RandomQueryDlg::extractTranslations (QString trans)
{
   QRegExp full_stop ("^(.*[^\\. ])\\.* *$");
   //full_stop.setMinimal (true);
   if ( full_stop.search (trans) >= 0 )
     trans = full_stop.cap (1);
   int i;
   QStringList translations;
   if ( Prefs::periods() )
     for ( i = fields - 1; i > 0; i -- )
     {
       QString regexp ("^ *");
       for ( int j = 0; j < i; j ++ )
         regexp += "([^ ][^.]*)\\.[. ]*";
       regexp += "([^. ].*)$";
       QRegExp regex (regexp);
       if ( regex.search (trans) >= 0 )
       {
         translations = regex.capturedTexts();
         translations.remove (translations.at (0));
         break;
       }
     }
   if ( Prefs::colons() && translations.count() <= 1 )
   {
     translations.clear();
     for ( i = fields - 1; i > 0; i -- )
     {
       QString regexp ("^ *");
       for ( int j = 0; j < i; j ++ )
         regexp += "([^ ][^:]*):[: ]*";
       regexp += "([^: ].*)$";
       QRegExp regex (regexp);
       if ( regex.search (trans) >= 0 )
       {
         translations = regex.capturedTexts();
         translations.remove (translations.at (0));
         break;
       }
     }
   }
   if ( Prefs::semicolons() && translations.count() <= 1 )
   {
     translations.clear();
     for ( i = fields - 1; i > 0; i -- )
     {
       QString regexp ("^ *");
       for ( int j = 0; j < i; j ++ )
         regexp += "([^ ][^;]*);[; ]*";
       regexp += "([^; ].*)$";
       QRegExp regex (regexp);
       if ( regex.search (trans) >= 0 )
       {
         translations = regex.capturedTexts();
         translations.remove (translations.at (0));
         break;
       }
     }
   }
   if ( Prefs::commas() && translations.count() <= 1 )
   {
     translations.clear();
     for ( i = fields - 1; i > 0; i -- )
     {
       QString regexp ("^ *");
       for ( int j = 0; j < i; j ++ )
         regexp += "([^ ][^,]*),[, ]*";
       regexp += "([^, ].*)$";
       QRegExp regex (regexp);
       if ( regex.search (trans) >= 0 )
       {
         translations = regex.capturedTexts();
         translations.remove (translations.at (0));
         break;
       }
     }
   }
   if ( translations.count() <= 1 )
     translations = QStringList (trans);
   return translations;
}

RandomQueryDlg::RandomQueryDlg(
                   QString org,
                   QString trans,
                   int entry,
                   int orgcol,
                   int transcol,
                   int q_cycle,
                   int q_num,
                   int q_start,
                   kvoctrainExpr *exp,
                   kvoctrainDoc  *doc)
  : QueryDlgBase(i18n("Random Query"))
{
  mw = new QueryDlgForm(this);
  setMainWidget(mw);

  connect( mw->c_type, SIGNAL(clicked()), SLOT(slotTypeClicked()) );
  connect( mw->c_remark, SIGNAL(clicked()), SLOT(slotRemClicked()) );
  connect( mw->c_falsefriend, SIGNAL(clicked()), SLOT(slotFFClicked()) );
  connect( mw->dont_know, SIGNAL(clicked()), SLOT(dontKnowClicked()) );
  connect( mw->know_it, SIGNAL(clicked()), SLOT(knowItClicked()) );
  connect( mw->verify, SIGNAL(clicked()), SLOT(verifyClicked()) );
  connect( mw->show_all, SIGNAL(clicked()), SLOT(showAllClicked()) );
  connect( mw->show_more, SIGNAL(clicked()), SLOT(showMoreClicked()) );

  mw->show_more -> setEnabled (Prefs::showMore());
  mw->know_it -> setEnabled (Prefs::iKnow());

  bool split = Prefs::split();
  fields = Prefs::fields();
  if ( ! split || fields < 1 )
    fields = 1;
  else if ( fields > 10 )
    fields = 10;

  QVBoxLayout * vb = new QVBoxLayout(mw->TranslationFrame, 0, KDialog::spacingHint());

  bool suggestions = Prefs::suggestions();
  int i;
  if ( suggestions )
  {
    for ( i = 0; i < fields; i ++ )
    {
      transCombos.append (new QComboBox (false, mw->TranslationFrame, QCString ("transCombo") + QCString().setNum (i)));
      transCombos.at(i) -> setSizePolicy (QSizePolicy ((QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, transCombos.at(i) -> sizePolicy().hasHeightForWidth()));
      transCombos.at(i) -> setEditable (true);
      transCombos.at(i) -> setInsertionPolicy (QComboBox::NoInsertion);
      transCombos.at(i) -> setDuplicatesEnabled (false);
      vb->addWidget(transCombos.at(i));
      connect (transCombos.at(i), SIGNAL (textChanged (const QString&)), SLOT (slotTransChanged (const QString&)));
      connect (transCombos.at(i) -> lineEdit(), SIGNAL (lostFocus()), SLOT (slotTransLostFocus()));
    }
  }
  else
  {
    for ( i = 0; i < fields; i ++ )
    {
      transFields.append (new QLineEdit (mw->TranslationFrame, QCString ("transField") + QCString().setNum (i)));
      transFields.at(i) -> setSizePolicy (QSizePolicy ((QSizePolicy::SizeType)7, (QSizePolicy::SizeType)1, 0, 0, transFields.at(i) -> sizePolicy().hasHeightForWidth()));
      vb->addWidget(transFields.at(i));
      connect (transFields.at(i), SIGNAL (textChanged (const QString&)), SLOT (slotTransChanged (const QString&)));
      connect (transFields.at(i), SIGNAL (lostFocus()), SLOT (slotTransLostFocus()));
    }
  }

  kv_doc = 0;
  qtimer = 0;
  //setCaption (kapp->makeStdCaption(i18n("Random Query")));
  setQuery (org, trans, entry, orgcol, transcol, q_cycle, q_num, q_start, exp, doc);
  mw->countbar->setFormat("%v/%m");
  mw->timebar->setFormat("%v");

  if ( suggestions )
  {
    ProgressDlg* pdlg = 0;
    if ( split && kv_doc -> numEntries() >= 500 )
    {
      pdlg = new ProgressDlg (QString(), QString(), kapp -> makeStdCaption (i18n("Loading Random Query")));
      pdlg -> resize (pdlg -> width(), pdlg -> minimumSize().height());
      pdlg -> show();
      kapp -> processEvents();
    }
    for ( i = 0; i < kv_doc -> numEntries(); i ++ )
    {
      kvoctrainExpr* expr = kv_doc -> getEntry (i);
      if ( split )
        vocabulary += extractTranslations (q_tcol ? expr -> getTranslation (q_tcol) : expr -> getOriginal());
      else
        vocabulary += q_tcol ? expr -> getTranslation (q_tcol) : expr -> getOriginal();
      if ( Prefs::swapDirection() )
      {
        if ( split )
          vocabulary += extractTranslations (q_ocol ? expr -> getTranslation (q_ocol) : expr -> getOriginal());
        else
          vocabulary += q_ocol ? expr -> getTranslation (q_ocol) : expr -> getOriginal();
      }
      if ( pdlg )
      {
        pdlg -> setValue (doc, i * 100 / kv_doc -> numEntries());
        kapp -> processEvents();
      }
    }
    vocabulary.sort();
    for ( uint k = 1; k < vocabulary.count(); k ++ )
      if ( vocabulary [k - 1] == vocabulary [k] )
        vocabulary.remove (vocabulary.at (k --));
    if ( pdlg )
      delete pdlg;
  }
  resize(configDialogSize("RandomQueryDialog"));
}


RandomQueryDlg::~RandomQueryDlg()
{
  saveDialogSize("RandomQueryDialog");
}


void RandomQueryDlg::setQuery(QString org,
                         QString trans,
                         int entry,
                         int orgcol,
                         int transcol,
                         int q_cycle,
                         int q_num,
                         int q_start,
                         kvoctrainExpr *,
                         kvoctrainDoc  *doc)
{
   //type_timeout = type_to;
   kv_doc = doc;
   q_row = entry;
   q_ocol = orgcol;
   q_tcol = transcol;
   translation = trans;
   if ( Prefs::split() )
     translations = extractTranslations (trans);
   else
     translations = trans;
   mw->timebar->setEnabled(Prefs::showCounter());
   mw->timelabel->setEnabled(Prefs::showCounter());
   int i;
   uint k;
   if ( Prefs::suggestions() )
   {
     for ( i = 0; i < fields; i ++ )
     {
       transCombos.at(i) -> clearEdit();
       resetField (transCombos.at(i) -> lineEdit());
     }
     for ( k = 0; k < translations.count(); k ++ )
       transCombos.at(k) -> show();
     for ( i = k; i < fields; i ++ )
       transCombos.at(i) -> hide();
   }
   else
   {
     for ( i = 0; i < fields; i ++ )
     {
       transFields.at(i) -> clear();
       transFields.at(i) -> setFont (Prefs::tableFont());
       resetField (transFields.at(i));
     }
     for ( k = 0; k < translations.count(); k ++ )
       transFields.at(k) -> show();
     for ( i = k; i < fields; i ++ )
       transFields.at(i) -> hide();
   }
   mw->verify -> setEnabled (true);
   mw->orgField->setFont(Prefs::tableFont());
   mw->orgField->setText (org);
   mw->show_all->setDefault(true);
   QString s;
   s.setNum (q_cycle);
   mw->progCount->setText (s);

   mw->remark->hide();
   mw->falseFriend->hide();
   mw->type->hide();

   mw->c_remark->setChecked(false);
   mw->c_falsefriend->setChecked(false);
   mw->c_type->setChecked(false);
   setHintFields();

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

   mw->status -> clear();
   suggestion_hint = false;
}


void RandomQueryDlg::initFocus() const
{
  RandomQueryDlg* that = (RandomQueryDlg*) this;
  if ( Prefs::suggestions() )
    that -> transCombos.at(0) -> setFocus();
  else
    that -> transFields.at(0) -> setFocus();
}


void RandomQueryDlg::verifyClicked()
{
  QStringList trans (translations);
  uint i, j;
  if ( Prefs::suggestions() )
  {
    QPtrList<QComboBox> combos (transCombos);
    for ( i = combos.count() - 1; i >= translations.count(); i -- )
      combos.remove (i);
    for ( i = 0; i < combos.count(); i ++ )
      for ( j = 0; j < trans.count(); j ++ )
        if ( smartCompare (trans[j], combos.at(i) -> currentText(), 0) )
        {
          verifyField (combos.at(i) -> lineEdit(), trans[j]);
          trans.remove (trans.at(j));
          combos.remove (i --);
          break;
        }
    if ( trans.count() == 0 )
    {
      int percent = (mw->countbar->progress()/mw->countbar->totalSteps()) * 100;
      //status->setText(getOKComment(countbar->getPercentage()));
      mw->status->setText(getOKComment(percent));
      knowItClicked();
    }
    else
    {
      for ( i = 0; i < combos.count(); i ++ )
        verifyField (combos.at(i) -> lineEdit(), "a\na"); // always fail
      mw->status->setText(getNOKComment((mw->countbar->progress()/mw->countbar->totalSteps()) * 100));
      mw->dont_know->setDefault(true);
    }
  }
  else
  {
    QPtrList<QLineEdit> fields (transFields);
    for ( i = fields.count() - 1; i >= translations.count(); i -- )
      fields.remove (i);
    for ( i = 0; i < fields.count(); i ++ )
      for ( j = 0; j < trans.count(); j ++ )
        if ( smartCompare (trans[j], fields.at(i) -> text(), 0) )
        {
          verifyField (fields.at(i), "a\na"); // always fail
          trans.remove (trans.at(j));
          fields.remove (i --);
          break;
        }
    if ( trans.count() == 0 )
    {
      mw->status->setText(getOKComment((mw->countbar->progress()/mw->countbar->totalSteps()) * 100));
      knowItClicked();
    }
    else
    {
      for ( i = 0; i < fields.count(); i ++ )
        verifyField (fields.at(i), trans[i]);
      mw->status->setText(getNOKComment((mw->countbar->progress()/mw->countbar->totalSteps()) * 100));
      mw->dont_know->setDefault(true);
    }
  }
  suggestion_hint = false;
}


void RandomQueryDlg::showMoreClicked()
{
  if ( Prefs::suggestions() )
    for ( uint i = 0; i < translations.count(); i ++ )
    {
      QComboBox* combo = transCombos.at(i);
      if ( ! smartCompare (combo -> currentText(), translations[i], 0) )
      {
        uint length = combo -> currentText().length() + 1;
        if ( length >= translations[i].length() )
        {
          combo -> setEditText (translations[i]);
          verifyField (combo -> lineEdit(), translations[i]);
          mw->verify -> setEnabled (false);
        }
        else
        {
          combo -> setEditText (translations[i].left (length));
          resetField (combo -> lineEdit());
        }
        mw->dont_know -> setDefault (true);
        break;
      }
    }
  else
    for ( uint i = 0; i < translations.count(); i ++ )
    {
      QLineEdit* field = transFields.at(i);
      if ( ! smartCompare (field -> text(), translations[i], 0) )
      {
        uint length = field -> text().length() + 1;
        if ( length >= translations[i].length() )
        {
          field -> setText (translations[i]);
          verifyField (field, translations[i]);
          mw->verify -> setEnabled (false);
        }
        else
        {
          field -> setText (translations[i].left (length));
          resetField (field);
        }
        mw->dont_know -> setDefault (true);
        break;
      }
    }
  mw->status -> clear();
  suggestion_hint = false;
}


void RandomQueryDlg::showAllClicked()
{
  if ( Prefs::suggestions() )
    for ( uint i = 0; i < translations.count(); i ++ )
    {
      transCombos.at(i) -> setEditText (translations[i]);
      verifyField (transCombos.at(i) -> lineEdit(), translations[i]);
    }
  else
    for ( uint i = 0; i < translations.count(); i ++ )
    {
      transFields.at(i) -> setText (translations[i]);
      verifyField (transFields.at(i), translations[i]);
    }
  mw->verify -> setEnabled (false);
  mw->dont_know->setDefault(true);
  mw->status -> clear();
  suggestion_hint = false;
}


void RandomQueryDlg::slotTransChanged(const QString&)
{
  mw->verify->setDefault(true);
  bool suggestions = Prefs::suggestions();
  if ( suggestions && sender() && sender() -> isA ("QComboBox") )
  {
    QLineEdit* edit = ((QComboBox*) sender()) -> lineEdit();
    resetField (edit);
    suggestion_hint = ! edit -> text().isEmpty() && edit -> text().length() <= 10;
    if ( suggestion_hint )
      mw->status -> setText (QString (i18n("Press F5 for a list of translations starting with '%1'\n"
        "Press F6 for a list of translations containing '%2'")).arg (edit -> text()).arg (edit -> text()));
    else
      mw->status -> clear();
  }
  else if ( ! suggestions && sender() && sender() -> isA ("QLineEdit") )
    resetField ((QLineEdit*) sender());
}

void RandomQueryDlg::slotTransLostFocus()
{
  if ( suggestion_hint )
    mw->status -> clear();
  suggestion_hint = false;
}


void RandomQueryDlg::knowItClicked()
{
  mw->status -> clear();
  suggestion_hint = false;
  emit sigQueryChoice (Known);
}


void RandomQueryDlg::timeoutReached()
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
     else if (Prefs::queryTimeout() == Prefs::EnumQueryTimeout::Continue) {
       emit sigQueryChoice (Timeout);
     }
     mw->status->setText(getTimeoutComment((mw->countbar->progress()/mw->countbar->totalSteps()) * 100));
   }

   suggestion_hint = false;
}


void RandomQueryDlg::dontKnowClicked()
{
  mw->status -> clear();
  suggestion_hint = false;
  emit sigQueryChoice (Unknown);
}


void RandomQueryDlg::setHintFields()
{
   QString s;
   kvoctrainExpr *exp = kv_doc->getEntry(q_row);

   s = exp->getRemark(q_ocol);
   mw->remark->setText (s);
   mw->c_remark->setEnabled(!s.isEmpty() );

   s = exp->getFauxAmi(q_ocol, q_ocol != 0);
   mw->falseFriend->setText (s);
   mw->c_falsefriend->setEnabled(!s.isEmpty() );

   s = "";
   vector<TypeRelation> all_types = QueryManager::getRelation(false);
   for (int i = 0; i < (int) all_types.size(); i++) {
     if ( exp->getType(q_ocol) == all_types[i].shortStr()) {
       s = all_types[i].longStr();
       break;
     }
   }
   mw->type->setText (s);
   mw->c_type->setEnabled(!s.isEmpty() );
}


void RandomQueryDlg::slotUser2()
{
   if (qtimer != 0)
     qtimer->stop();

   emit sigEditEntry (q_row, KV_COL_ORG+q_ocol);

   kvoctrainExpr *exp = kv_doc->getEntry(q_row);
   mw->orgField->setText (q_ocol == 0
                        ? exp->getOriginal()
                        : exp->getTranslation(q_ocol));

   if ( Prefs::suggestions() )
     for ( int i = 0; i < fields; i ++ )
       transCombos.at(i) -> clearEdit();
   else
     for ( int i = 0; i < fields; i ++ )
       transFields.at(i) -> clear();
   mw->status -> clear();
   suggestion_hint = false;

   setHintFields();
}


void RandomQueryDlg::slotFFClicked()
{
   if (mw->c_falsefriend->isChecked() )
     mw->falseFriend->show();
   else
     mw->falseFriend->hide();
}


void RandomQueryDlg::slotRemClicked()
{
   if (mw->c_remark->isChecked())
     mw->remark->show();
   else
     mw->remark->hide();
}


void RandomQueryDlg::slotTypeClicked()
{
   if (mw->c_type->isChecked() )
     mw->type->show();
   else
     mw->type->hide();
}


void RandomQueryDlg::keyPressEvent( QKeyEvent *e )
{
  if ( Prefs::suggestions() )
  {
    QComboBox* combo = 0;
    if ( e -> key() == Key_F4 || e -> key() == Key_F5 || e -> key() == Key_F6 )
      for ( uint i = 0; i < translations.count(); i ++ )
        if ( transCombos.at(i) -> hasFocus() )
        {
          combo = transCombos.at(i);
          break;
        }
    switch( e->key() )
    {
      case Key_F5:
      case Key_F6:
        if ( combo && ! combo -> currentText().isEmpty() )
        {
          QString curText (combo -> currentText());
          combo -> clear();
          for ( uint i = 0; i < vocabulary.count(); i ++ )
          {
            QString trans (vocabulary[i]);
            if ( (e -> key() == Key_F5 && trans.startsWith (curText, false)
                || e -> key() == Key_F6 && trans.contains (curText, false)) )
              combo -> insertItem (trans);
          }
          combo -> setEditText (curText);
        }
      case Key_F4:
        if ( combo )
          combo -> popup();
      break;
    }
  }

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

#include "RandomQueryDlg.moc"
