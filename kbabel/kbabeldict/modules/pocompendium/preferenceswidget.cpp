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
#include <qwhatsthis.h>
#include <klineedit.h>
#include <klocale.h>
#include <kurlrequester.h>

#include "preferenceswidget.h"
#include "pwidget.h"

CompendiumPreferencesWidget::CompendiumPreferencesWidget(QWidget *parent, const char* name)
		: PrefWidget(parent,name)
		, changed(false)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
		
	prefWidget = new CompendiumPWidget(this);
	layout->addWidget(prefWidget);

	connect(prefWidget->caseBtn, SIGNAL(toggled(bool))
					, this, SLOT(setChanged()));
	connect(prefWidget->equalBtn, SIGNAL(toggled(bool))
					, this, SLOT(setChanged()));
	connect(prefWidget->ngramBtn, SIGNAL(toggled(bool))
					, this, SLOT(setChanged()));
	connect(prefWidget->isContainedBtn, SIGNAL(toggled(bool))
					, this, SLOT(setChanged()));
	connect(prefWidget->containsBtn, SIGNAL(toggled(bool))
					, this, SLOT(setChanged()));
	connect(prefWidget->fuzzyBtn, SIGNAL(toggled(bool))
					, this, SLOT(setChanged()));
	connect(prefWidget->hasWordBtn, SIGNAL(toggled(bool))
					, this, SLOT(setChanged()));
	connect(prefWidget->wholeBtn, SIGNAL(toggled(bool))
					, this, SLOT(setChanged()));
	
	connect(prefWidget->urlInput->lineEdit(),SIGNAL(textChanged(const QString&))
					, this, SLOT(setChanged()));

	connect(prefWidget->equalBtn, SIGNAL(toggled(bool))
					, this, SLOT(equalBtnToggled(bool)));
	connect(prefWidget->ngramBtn, SIGNAL(toggled(bool))
					, this, SLOT(ngramBtnToggled(bool)));
	connect(prefWidget->isContainedBtn, SIGNAL(toggled(bool))
					, this, SLOT(isContainedBtnToggled(bool)));
	connect(prefWidget->containsBtn, SIGNAL(toggled(bool))
					, this, SLOT(containsBtnToggled(bool)));
	connect(prefWidget->hasWordBtn, SIGNAL(toggled(bool))
					, this, SLOT(hasWordBtnToggled(bool)));


	QString whatsthis=i18n("<qt><p><b>Parameters</b></p>"
		"<p>Here you can fine-tune searching within the PO file. "
		"For example if you want to perform a case sensitive search, or if "
		"you want fuzzy messages to be ignored.</p></qt>" );
	QWhatsThis::add(prefWidget->caseBtn,whatsthis);
	QWhatsThis::add(prefWidget->fuzzyBtn,whatsthis);
	QWhatsThis::add(prefWidget->wholeBtn,whatsthis);

	whatsthis = i18n("<qt><p><b>Comparison Options</b></p>"
		"<p>Choose here which messages you want to have treated as a matching "
		"message.</p></qt>");
	QWhatsThis::add(prefWidget->equalBtn,whatsthis);
	QWhatsThis::add(prefWidget->containsBtn,whatsthis);
	QWhatsThis::add(prefWidget->isContainedBtn,whatsthis);
	QWhatsThis::add(prefWidget->hasWordBtn,whatsthis);

	whatsthis = i18n("<qt><p><b>3-Gram-matching</b></p>"
			 "<p>A message matches another if most of its 3-letter groups are "
			 "contained in the other message. e.g. 'abc123' matches 'abcx123c12'.</p></qt>");
	QWhatsThis::add(prefWidget->ngramBtn,whatsthis);

	whatsthis = i18n("<qt><p><b>Location</b></p>"
					"<p>Configure here which file is to be used for searching."
					"</p></qt>");
	QWhatsThis::add(prefWidget->urlInput,whatsthis);
}

CompendiumPreferencesWidget::~CompendiumPreferencesWidget()
{
}


void CompendiumPreferencesWidget::apply()
{
	emit applySettings();
}

void CompendiumPreferencesWidget::cancel()
{
	emit restoreSettings();
}

void CompendiumPreferencesWidget::standard()
{
	prefWidget->urlInput->setURL("http://i18n.kde.org/po_overview/@LANG@.messages");
	prefWidget->caseBtn->setChecked(false);
	prefWidget->equalBtn->setChecked(true);
	prefWidget->ngramBtn->setChecked(true);
	prefWidget->isContainedBtn->setChecked(false);
	prefWidget->containsBtn->setChecked(false);
	prefWidget->wholeBtn->setChecked(true);
	prefWidget->hasWordBtn->setChecked(true);

	prefWidget->fuzzyBtn->setChecked(true);
	
	changed=true;
}

void CompendiumPreferencesWidget::setURL(const QString url)
{
	prefWidget->urlInput->setURL(url);
	changed=false;
}

void CompendiumPreferencesWidget::setCaseSensitive(bool on)
{
	prefWidget->caseBtn->setChecked(on);
	changed=false;
}

void CompendiumPreferencesWidget::setMatchEqual(bool on)
{
	prefWidget->equalBtn->setChecked(on);
	changed=false;
}

void CompendiumPreferencesWidget::setMatchNGram(bool on)
{
	prefWidget->ngramBtn->setChecked(on);
	changed=false;
}

void CompendiumPreferencesWidget::setMatchIsContained(bool on)
{
	prefWidget->isContainedBtn->setChecked(on);
	changed=false;
}

void CompendiumPreferencesWidget::setMatchContains(bool on)
{
	prefWidget->containsBtn->setChecked(on);
	changed=false;
}

void CompendiumPreferencesWidget::setIgnoreFuzzy(bool on)
{
	prefWidget->fuzzyBtn->setChecked(on);
	changed=false;
}

void CompendiumPreferencesWidget::setWholeWords(bool on)
{
	prefWidget->wholeBtn->setChecked(on);
	changed=false;
}


void CompendiumPreferencesWidget::setMatchWords(bool on)
{
	prefWidget->hasWordBtn->setChecked(on);
	changed=false;
}



QString CompendiumPreferencesWidget::url()
{
	changed=false;
	return prefWidget->urlInput->url();
}

bool CompendiumPreferencesWidget::caseSensitive()
{
	changed=false;

	return prefWidget->caseBtn->isChecked();
}

bool CompendiumPreferencesWidget::matchEqual()
{
	changed=false;

	return prefWidget->equalBtn->isChecked();
}

bool CompendiumPreferencesWidget::matchNGram()
{
	changed=false;

	return prefWidget->ngramBtn->isChecked();
}

bool CompendiumPreferencesWidget::matchIsContained()
{
	changed=false;

	return prefWidget->isContainedBtn->isChecked();
}

bool CompendiumPreferencesWidget::matchContains()
{
	changed=false;

	return prefWidget->containsBtn->isChecked();
}

bool CompendiumPreferencesWidget::ignoreFuzzy()
{
	changed=false;

	return prefWidget->fuzzyBtn->isChecked();
}


bool CompendiumPreferencesWidget::wholeWords()
{
	changed=false;

	return prefWidget->wholeBtn->isChecked();
}


bool CompendiumPreferencesWidget::matchWords()
{
	changed=false;

	return prefWidget->hasWordBtn->isChecked();
}



bool CompendiumPreferencesWidget::settingsChanged() const
{
	return changed;
}

void CompendiumPreferencesWidget::setChanged()
{
	changed=true;
}


void CompendiumPreferencesWidget::equalBtnToggled(bool on)
{
	if(!on)
	{
		if(!prefWidget->isContainedBtn->isChecked() 
		   && !prefWidget->ngramBtn->isChecked()
		   && !prefWidget->containsBtn->isChecked()
		   && !prefWidget->hasWordBtn->isChecked())
		{
			prefWidget->equalBtn->setChecked(true);
		}
	}
}

void CompendiumPreferencesWidget::ngramBtnToggled(bool on)
{
	if(!on)
	{
		if(!prefWidget->isContainedBtn->isChecked() 
		   && !prefWidget->equalBtn->isChecked()
		   && !prefWidget->containsBtn->isChecked()
		   && !prefWidget->hasWordBtn->isChecked())
		{
		  prefWidget->equalBtn->setChecked(true);
		}
	}
}

void CompendiumPreferencesWidget::isContainedBtnToggled(bool on)
{
	if(!on)
	{
		if(!prefWidget->equalBtn->isChecked() 
		   && !prefWidget->ngramBtn->isChecked()
		   && !prefWidget->containsBtn->isChecked()
		   && !prefWidget->hasWordBtn->isChecked())
		{
		  prefWidget->isContainedBtn->setChecked(true);
		}
	}
}

void CompendiumPreferencesWidget::containsBtnToggled(bool on)
{
	if(!on)
	{
		if(!prefWidget->isContainedBtn->isChecked() 
		   && !prefWidget->ngramBtn->isChecked()
		   && !prefWidget->equalBtn->isChecked()
		   && !prefWidget->hasWordBtn->isChecked())
		{
		  prefWidget->containsBtn->setChecked(true);
		}
	}
}

void CompendiumPreferencesWidget::hasWordBtnToggled(bool on)
{
	if(!on)
	{
		if(!prefWidget->isContainedBtn->isChecked() 
		   && !prefWidget->ngramBtn->isChecked()
		   && !prefWidget->equalBtn->isChecked()
		   && !prefWidget->containsBtn->isChecked())
		{
		  prefWidget->hasWordBtn->setChecked(true);
		}
	}
}



#include "preferenceswidget.moc"
