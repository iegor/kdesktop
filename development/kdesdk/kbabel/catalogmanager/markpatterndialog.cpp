/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2003 by Marco Wegner <mail@marcowegner.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  In addition, as a special exception, the copyright holders give
  permission to link the code of this program with any edition of
  the Qt library by Trolltech AS, Norway (or with modified versions
  of Qt that use the same license as Qt), and distribute linked
  combinations including the two.  You must obey the GNU General
  Public License in all respects for all of the code used other than
  Qt. If you modify this file, you may extend this exception to
  your version of the file, but you are not obligated to do so.  If
  you do not wish to do so, delete this exception statement from
  your version.
	  
**************************************************************************** */


#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>

#include <kcombobox.h>
#include <kcompletion.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kparts/componentfactory.h>
#include <kregexpeditorinterface.h>

#include "markpatterndialog.h"
#include "markpatternwidget.h"

MarkPatternDialog::MarkPatternDialog(QWidget * parent, const char * name)
  : KDialogBase(parent, name, true, 0, Ok|Cancel, Ok)
{
  actionButton(Ok)->setEnabled(false);
  
  mainWidget = new MarkPatternWidget(this);
  
  connect (mainWidget->combo, SIGNAL(textChanged(const QString&)), 
            this, SLOT(slotComboTextChanged(const QString&)));
  
  comboCompletion = mainWidget->combo->completionObject( );
  
  regexpEditDialog = 0;
  if (!KTrader::self( )->query("KRegExpEditor/KRegExpEditor").isEmpty( )) {
    connect(mainWidget->regexpButton, SIGNAL(clicked( )), this, SLOT(slotRegexpButtonClicked( )));
  } else {
    disconnect(mainWidget->useRegExp, 0, mainWidget->regexpButton, 0);
    delete mainWidget->regexpButton;
    mainWidget->regexpButton = 0;
  }
  
  restoreSettings( );
  
  actionButton(Ok)->setEnabled(!mainWidget->combo->currentText( ).isEmpty( ));
  mainWidget->combo->setFocus( );
  setMainWidget( mainWidget);
}

void MarkPatternDialog::accept( )
{
  // Update the list of patterns.
  patternList.remove(mainWidget->combo->currentText( ));
  patternList.prepend(mainWidget->combo->currentText( ));
  while (patternList.count( ) > 10)
    patternList.remove(patternList.last( ));
    
  saveSettings( );
  KDialogBase::accept( );
}

QString MarkPatternDialog::pattern( )
{
  return mainWidget->combo->currentText( );
}

bool MarkPatternDialog::isCaseSensitive( )
{
  return mainWidget->caseSensitive->isChecked( );
}

bool MarkPatternDialog::includeTemplates( )
{
  return mainWidget->inclTemplates->isChecked( );
}

bool MarkPatternDialog::useRegExp( )
{
  return mainWidget->useRegExp->isChecked( );
}

void MarkPatternDialog::setMode(bool markMode)
{
  if (markMode) {
    mainWidget->mainLabel->setText(i18n("Ma&rk files which match the following pattern:"));
    setButtonOKText(i18n("&Mark Files"));
  } else {
    mainWidget->mainLabel->setText(i18n("Unma&rk files which match the following pattern:"));
    setButtonOKText(i18n("Un&mark Files"));
  }
}

void MarkPatternDialog::slotComboTextChanged(const QString& text)
{
  actionButton(Ok)->setEnabled(!text.isEmpty( ));
}

void MarkPatternDialog::slotRegexpButtonClicked( )
{
  if (!regexpEditDialog)
    regexpEditDialog = KParts::ComponentFactory::createInstanceFromQuery<QDialog>(
      "KRegExpEditor/KRegExpEditor", QString::null, this);

  KRegExpEditorInterface * iface = dynamic_cast<KRegExpEditorInterface *>(regexpEditDialog);
  
  if (iface) {
    iface->setRegExp(mainWidget->combo->currentText( ));
    if (regexpEditDialog->exec( ) == QDialog::Accepted)
      mainWidget->combo->setCurrentText(iface->regExp( ));
  }
}

void MarkPatternDialog::restoreSettings( )
{
  KConfig * config = KGlobal::config( );
  config->setGroup("MarkPatternDialog");
  
  patternList = config->readListEntry("Patterns");
  mainWidget->combo->insertStringList(patternList);
  comboCompletion->insertItems(patternList);
  mainWidget->caseSensitive->setChecked(config->readBoolEntry("CaseSensitive", false));
  mainWidget->inclTemplates->setChecked(config->readBoolEntry("IncludeTemplates", false));
  
  bool rx = config->readBoolEntry("UseRegExp", false);
  if (rx)
    mainWidget->useRegExp->setChecked(true);
  else
    mainWidget->useWildcards->setChecked(true);
  if (mainWidget->regexpButton)
    mainWidget->regexpButton->setEnabled(mainWidget->useRegExp->isChecked( ));
}

void MarkPatternDialog::saveSettings( )
{
  KConfig * config = KGlobal::config( );
  config->setGroup("MarkPatternDialog");
  
  config->writeEntry("Patterns", patternList);
  config->writeEntry("CaseSensitive", mainWidget->caseSensitive->isChecked( ));
  config->writeEntry("IncludeTemplates", mainWidget->inclTemplates->isChecked( ));
  config->writeEntry("UseRegExp", mainWidget->useRegExp->isChecked( ));
}

#include "markpatterndialog.moc"
