/* This file is part of the KDE project

   Copyright (C) 2001 Andreas Schlapbach <schlpbch@iam.unibe.ch>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef __validatorsdialog_h
#define __validatorsdialog_h

#include <qcombobox.h>

#include <kconfig.h>
#include <kdialogbase.h>

class ValidatorsDialog : public KDialogBase
{
 Q_OBJECT

 public:
  ValidatorsDialog(QWidget *parent=0, const char *name=0 );
  ~ValidatorsDialog();

  const QString getWWWValidatorUrl() const {return m_WWWValidatorCB->currentText();}
  const QString getCSSValidatorUrl() const {return m_CSSValidatorCB->currentText();}
  const QString getWWWValidatorUploadUrl() const {return m_WWWValidatorUploadCB->currentText();}
  const QString getCSSValidatorUploadUrl() const {return m_CSSValidatorUploadCB->currentText();}
  const QString getLinkValidatorUrl() const {return m_linkValidatorCB->currentText();}

 protected slots:
  void slotOk();
  void slotCancel();

 private:
  void load();
  void save();

  QComboBox *m_WWWValidatorCB;
  QComboBox *m_WWWValidatorUploadCB;
  QComboBox *m_CSSValidatorCB;
  QComboBox *m_CSSValidatorUploadCB;
  QComboBox *m_linkValidatorCB;
  KConfig *m_config;
};

#endif
