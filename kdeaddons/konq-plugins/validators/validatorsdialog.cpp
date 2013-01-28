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

#include <qlabel.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <kconfig.h>

#include <klocale.h>

#include "validatorsdialog.h"
#include "validatorsdialog.moc"

ValidatorsDialog::ValidatorsDialog(QWidget *parent,  const char *name )
  : KDialogBase( parent, name, false, i18n("Configure"), Ok|Cancel, Ok, true )
{
  setCaption(i18n("Configure Validating Servers"));
  setMinimumWidth(300);

  QVBox *page = makeVBoxMainWidget ();

  QGroupBox *tgroup = new QGroupBox( i18n("HTML/XML Validator"), page );
  QVBoxLayout *vlay = new QVBoxLayout( tgroup, spacingHint() );
  vlay->addSpacing( fontMetrics().lineSpacing());

  vlay->addWidget(new QLabel( i18n("URL:"), tgroup));

  m_WWWValidatorCB = new QComboBox(true, tgroup);
  m_WWWValidatorCB->setDuplicatesEnabled(false);
  vlay->addWidget( m_WWWValidatorCB );

  vlay->addWidget(new QLabel( i18n("Upload:"), tgroup));

  m_WWWValidatorUploadCB = new QComboBox(true, tgroup);
  m_WWWValidatorUploadCB->setDuplicatesEnabled(false);
  vlay->addWidget( m_WWWValidatorUploadCB );

  ///

  QGroupBox *group2= new QGroupBox( i18n("CSS Validator"), page );
  QVBoxLayout *vlay2 = new QVBoxLayout( group2, spacingHint() );
  vlay2->addSpacing( fontMetrics().lineSpacing());

  vlay2->addWidget(new QLabel( i18n("URL:"), group2));

  m_CSSValidatorCB = new QComboBox(true, group2);
  m_CSSValidatorCB->setDuplicatesEnabled(false);
  vlay2->addWidget( m_CSSValidatorCB );

  vlay2->addWidget(new QLabel( i18n("Upload:"), group2));

  m_CSSValidatorUploadCB = new QComboBox(true, group2);
  m_CSSValidatorUploadCB->setDuplicatesEnabled(false);
  vlay2->addWidget( m_CSSValidatorUploadCB );

  ///

  QGroupBox *group3= new QGroupBox( i18n("Link Validator"), page );
  QVBoxLayout *vlay3 = new QVBoxLayout( group3, spacingHint() );
  vlay3->addSpacing( fontMetrics().lineSpacing());

  vlay3->addWidget(new QLabel( i18n("URL:"), group3));

  m_linkValidatorCB = new QComboBox(true, group3);
  m_linkValidatorCB->setDuplicatesEnabled(false);
  vlay3->addWidget( m_linkValidatorCB );

  load();
}

ValidatorsDialog::~ValidatorsDialog()
{
    delete m_config;
}

void ValidatorsDialog::load()
{
  m_config = new KConfig("validatorsrc");
  m_config->setGroup("Addresses");

  m_WWWValidatorCB->insertStringList(m_config->readListEntry("WWWValidatorUrl"));
  if (m_WWWValidatorCB->count()==0) {
    m_WWWValidatorCB->insertItem("http://validator.w3.org/check");
  }
  m_WWWValidatorCB->setCurrentItem(m_config->readNumEntry("WWWValidatorUrlIndex",0));

  m_CSSValidatorCB->insertStringList(m_config->readListEntry("CSSValidatorUrl"));
  if (m_CSSValidatorCB->count()==0) {
    m_CSSValidatorCB->insertItem("http://jigsaw.w3.org/css-validator/validator");
  }
  m_CSSValidatorCB->setCurrentItem(m_config->readNumEntry("CSSValidatorUrlIndex",0));

  m_linkValidatorCB->insertStringList(m_config->readListEntry("LinkValidatorUrl"));
  if (m_linkValidatorCB->count()==0) {
    m_linkValidatorCB->insertItem("http://validator.w3.org/checklink");
  }
  m_linkValidatorCB->setCurrentItem(m_config->readNumEntry("LinkValidatorUrlIndex",0));

  m_WWWValidatorUploadCB->insertStringList(m_config->readListEntry("WWWValidatorUploadUrl"));
  if (m_WWWValidatorUploadCB->count()==0) {
    m_WWWValidatorUploadCB->insertItem("http://validator.w3.org/file-upload.html");
  }
  m_WWWValidatorUploadCB->setCurrentItem(m_config->readNumEntry("WWWValidatorUploadUrlIndex",0));

  m_CSSValidatorUploadCB->insertStringList(m_config->readListEntry("CSSValidatorUploadUrl"));
  if (m_CSSValidatorUploadCB->count()==0) {
    m_CSSValidatorUploadCB->insertItem("http://jigsaw.w3.org/css-validator/validator-upload.html" );
  }
  m_CSSValidatorUploadCB->setCurrentItem(m_config->readNumEntry("CSSValidatorUploadUrlIndex",0));
}

void ValidatorsDialog::save()
{
  QStringList strList;
  for (int i = 0; i < m_WWWValidatorCB->count(); i++) {
    strList.append(m_WWWValidatorCB->text(i));
  }
  m_config->writeEntry( "WWWValidatorUrl", strList );
  strList.clear();
  for (int i = 0; i < m_CSSValidatorCB->count(); i++) {
    strList.append(m_CSSValidatorCB->text(i));
  }
  m_config->writeEntry( "CSSValidatorUrl", strList );
  strList.clear();
  for (int i = 0; i < m_linkValidatorCB->count(); i++) {
    strList.append(m_linkValidatorCB->text(i));
  }
  m_config->writeEntry( "LinkValidatorUrl", strList );
  strList.clear();
  for (int i = 0; i < m_WWWValidatorUploadCB->count(); i++) {
    strList.append(m_WWWValidatorUploadCB->text(i));
  }
  m_config->writeEntry( "WWWValidatorUploadUrl", strList );
  strList.clear();
  for (int i = 0; i < m_CSSValidatorUploadCB->count(); i++) {
    strList.append(m_CSSValidatorUploadCB->text(i));
  }
  m_config->writeEntry( "CSSValidatorUploadUrl", strList );

  m_config->writeEntry( "WWWValidatorUrlIndex", m_WWWValidatorCB->currentItem() );
  m_config->writeEntry( "CSSValidatorUrlIndex", m_CSSValidatorCB->currentItem() );
  m_config->writeEntry( "LinkValidatorUrlIndex", m_linkValidatorCB->currentItem() );
  m_config->writeEntry( "WWWValidatorUploadUrlIndex", m_WWWValidatorUploadCB->currentItem() );
  m_config->writeEntry( "CSSValidatorUploadUrlIndex", m_CSSValidatorUploadCB->currentItem() );
  m_config->sync();
}

void ValidatorsDialog::slotOk()
{
   save();
   hide();
}

void ValidatorsDialog::slotCancel()
{
   load();
   hide();
}
