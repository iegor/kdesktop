/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
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
#include "spelldlg.h"
#include "spelldlgwidget.h"

#include <qcheckbox.h>
#include <qradiobutton.h>

#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>

SpellDlg::SpellDlg(bool haveMarkedText,QWidget *parent,const char *name)
		: KDialogBase(parent,name,true,i18n("Caption of dialog","Spelling")
						, Ok|Cancel)
{
	setButtonOK(KGuiItem(i18n("&Spell Check"),"spellcheck"));

	_mainWidget = new SpellDlgWidget(this);
	setMainWidget(_mainWidget);

	if(haveMarkedText)
	{
		_mainWidget->markedBtn->setChecked(true);
		_mainWidget->defaultBtn->setChecked(false);
		_mainWidget->defaultBtn->setEnabled(false);
	}
	else
	{
		_mainWidget->markedBtn->setEnabled(false);

		KConfig *config = KGlobal::config();
		KConfigGroupSaver cs(config,"SpellDlg");
		QString what=config->readEntry("Default","All");

		if(what=="All")
			_mainWidget->allBtn->setChecked(true);
		else if(what=="Current")
			_mainWidget->currentBtn->setChecked(true);
		else if(what=="Begin")
			_mainWidget->beginBtn->setChecked(true);
		else if(what=="End")
			_mainWidget->endBtn->setChecked(true);
		else
			_mainWidget->allBtn->setChecked(true);

	}

}

SpellDlg::~SpellDlg()
{
	if(_mainWidget->defaultBtn->isChecked())
	{
		KConfig *config=KGlobal::config();
		KConfigGroupSaver cs(config,"SpellDlg");

		QString what="All";
		if(_mainWidget->endBtn->isChecked())
			what="End";
		else if(_mainWidget->beginBtn->isChecked())
			what="Begin";
		else if(_mainWidget->currentBtn->isChecked())
			what="Current";

		config->writeEntry("Default",what);
	}
}

bool SpellDlg::all() const
{
	return _mainWidget->allBtn->isChecked();
}

bool SpellDlg::current() const
{
	return _mainWidget->currentBtn->isChecked();
}

bool SpellDlg::begin() const
{
	return _mainWidget->beginBtn->isChecked();
}

bool SpellDlg::end() const
{
	return _mainWidget->endBtn->isChecked();
}

bool SpellDlg::marked() const
{
	return _mainWidget->markedBtn->isChecked();
}


void SpellDlg::markedChecked(bool on)
{
	if(on)
	{
		_mainWidget->defaultBtn->setChecked(false);
	}

	_mainWidget->defaultBtn->setEnabled(!on);
}

bool SpellDlg::beginCurrent( ) const
{
	return _mainWidget->beginCurrentMsgBtn->isChecked( );
}


#include "spelldlg.moc"
