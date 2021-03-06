/***************************************************************************

                   dialog when in random query mode

    -----------------------------------------------------------------------

    begin          : Thu Mar 11 20:50:53 MET 1999

    copyright      : (C) 1999-2001 Ewald Arnold <kvoctrain@ewald-arnold.de>
                     (C) 2001 The KDE-EDU team
                     (C) 2005 Peter Hedlund <peter.hedlund@kdemail.net>

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


#ifndef Query_Dlg_H
#define Query_Dlg_H

#include "RandomQueryDlgForm.h"
#include "QueryDlgBase.h"

#include <qcombobox.h>
#include <qlineedit.h>

class kvoctrainDoc;

class RandomQueryDlg : public QueryDlgBase
{
  Q_OBJECT

public:
  RandomQueryDlg(
                 QString org,
                 QString trans,
                 int entry,
                 int orgcol,
                 int transcol,
                 int query_cycle,
                 int query_num,
                 int query_startnum,
                 kvoctrainExpr *exp,
                 kvoctrainDoc  *);

  ~RandomQueryDlg();

  void setQuery(QString org,
                      QString trans,
                      int entry,
                      int orgcol,
                      int transcol,
                      int query_cycle,
                      int query_num,
                      int query_startnum,
                      kvoctrainExpr *exp,
                      kvoctrainDoc  *);

public slots:
  virtual void initFocus() const;

public slots:
  void verifyClicked();
  void showMoreClicked();
  void showAllClicked();
  void knowItClicked();
  void dontKnowClicked();
  void timeoutReached();
  void slotUser2();
  void slotTransChanged(const QString&);
  void slotTransLostFocus();
  void slotFFClicked();
  void slotTypeClicked();
  void slotRemClicked();

protected:
  virtual void keyPressEvent(QKeyEvent *e);

  void setHintFields();
  QStringList extractTranslations (QString trans);

  kvoctrainDoc * kv_doc;

  QPtrList<QComboBox> transCombos;
  QPtrList<QLineEdit> transFields;

  QStringList translations;
  QStringList vocabulary;

  bool suggestion_hint;
  int  fields;

private:
  QueryDlgForm * mw;
};

#endif
