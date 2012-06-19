/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>

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
#include <qlayout.h>

#include <kfiledialog.h>
#include <qpushbutton.h>
#include <klineedit.h>
#include <klocale.h>
#include <kurlrequester.h>

#include "preferenceswidget.h"
#include "pwidget.h"

AuxiliaryPreferencesWidget::AuxiliaryPreferencesWidget(QWidget *parent, const char* name)
        : PrefWidget(parent,name)
        , changed(false)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
        
    prefWidget = new PWidget(this);
    layout->addWidget(prefWidget);


    connect(prefWidget->urlInput->lineEdit(),SIGNAL(textChanged(const QString&))
                    , this, SLOT(setChanged()));
}

AuxiliaryPreferencesWidget::~AuxiliaryPreferencesWidget()
{
}


void AuxiliaryPreferencesWidget::apply()
{
    emit applySettings();
}

void AuxiliaryPreferencesWidget::cancel()
{
    emit restoreSettings();
}

void AuxiliaryPreferencesWidget::standard()
{
    prefWidget->urlInput->setURL("@PACKAGE@.po");
    changed=true;
}

void AuxiliaryPreferencesWidget::setURL(const QString url)
{
    prefWidget->urlInput->setURL(url);
    changed=false;
}

QString AuxiliaryPreferencesWidget::url()
{
    changed = false;
    return prefWidget->urlInput->url();
}

bool AuxiliaryPreferencesWidget::ignoreFuzzy()
{
    changed=false;
    return prefWidget->fuzzyBtn->isChecked();
}

void AuxiliaryPreferencesWidget::setIgnoreFuzzy(bool flag)
{
    prefWidget->fuzzyBtn->setChecked(flag);
}

bool AuxiliaryPreferencesWidget::settingsChanged() const
{
    return changed;
}

void AuxiliaryPreferencesWidget::setChanged()
{
    changed=true;
}


#include "preferenceswidget.moc"
