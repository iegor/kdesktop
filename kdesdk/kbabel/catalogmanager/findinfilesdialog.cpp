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
#include "findinfilesdialog.h"

#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qwhatsthis.h>

#include <kconfig.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

using namespace KBabel;

FindInFilesDialog::FindInFilesDialog(bool forReplace, QWidget* parent)
		:FindDialog(forReplace, parent)
{
	QGroupBox* box = new QGroupBox(2, Qt::Horizontal, i18n("File Options"), mainWidget());
	mainWidget()->layout()->add(box);

	_inAllFiles = new QCheckBox(i18n("&In all files"),box);
	_inMarked = new QCheckBox(i18n("&Marked files"),box);
	_inTemplates = new QCheckBox(i18n("In &templates"),box);
	_askForNextFile = new QCheckBox(i18n("Ask before ne&xt file"),box);
	_askForSave = new QCheckBox(i18n("Save &without asking"),box);

	QWhatsThis::add(box,i18n("<qt><p><b>File Options</b></p>"
	"<p>Here you can finetune where to find:"
	"<ul><li><b>In all files</b>: search in all files, otherwise searched "
	"is the selected file or files in the selected folder</li>"
	"<li><b>Ask before next file</b>: show a dialog asking to proceed to the next file</li>"
	"</ul></qt>"));

	readSettings();
}

FindInFilesDialog::~FindInFilesDialog()
{
   saveSettings();
}

int FindInFilesDialog::show(QString initialStr)
{

	FindDialog::show(initialStr);

	int r = result();

	if( r == QDialog::Accepted ) {
		if( isReplaceDialog() ) {
			ReplaceOptions options = replaceOpts();
			options.inAllFiles = _inAllFiles->isChecked();
			options.inMarkedFiles = _inMarked->isChecked();
			options.inTemplates = _inTemplates->isChecked();
			options.askForNextFile = _askForNextFile->isChecked();
			options.askForSave = !_askForSave->isChecked();
			FindDialog::setReplaceOpts(options);
		}
		else {
			FindOptions options = findOpts();
			options.inAllFiles = _inAllFiles->isChecked();
			options.inMarkedFiles = _inMarked->isChecked();
			options.inTemplates = _inTemplates->isChecked();
			options.askForNextFile = _askForNextFile->isChecked();
			options.askForSave = !_askForSave->isChecked();
			FindDialog::setFindOpts( options );
		}
	}
	
	return r;
}

int FindInFilesDialog::exec(QString initialStr)
{
	FindDialog::exec(initialStr);

	int r = result();

	if( r == QDialog::Accepted ) {
		if(isReplaceDialog()) {
			ReplaceOptions options = replaceOpts();
			options.inAllFiles = _inAllFiles->isChecked();
			options.inMarkedFiles = _inMarked->isChecked();
			options.inTemplates = _inTemplates->isChecked();
			options.askForNextFile = _askForNextFile->isChecked();
			options.askForSave = !_askForSave->isChecked();
			FindDialog::setReplaceOpts(options);
		}
		else {
			FindOptions options = findOpts();
			options.inAllFiles = _inAllFiles->isChecked();
			options.inMarkedFiles = _inMarked->isChecked();
			options.inTemplates = _inTemplates->isChecked();
			options.askForNextFile = _askForNextFile->isChecked();
			options.askForSave = !_askForSave->isChecked();
			FindDialog::setFindOpts( options );
		}
	}
	
	return r;
}

void FindInFilesDialog::setFindOpts(FindOptions options)
{
    FindDialog::setFindOpts(options);

    _inAllFiles->setChecked(options.inAllFiles);
    _inTemplates->setChecked(options.inTemplates);
    _inMarked->setChecked(options.inMarkedFiles);
    _askForNextFile->setChecked(options.askForNextFile);
    _askForSave->setChecked(!options.askForSave);
}

void FindInFilesDialog::setReplaceOpts(ReplaceOptions options)
{
    FindDialog::setReplaceOpts(options);

    _inAllFiles->setChecked(options.inAllFiles);
    _inTemplates->setChecked(options.inTemplates);
    _inMarked->setChecked(options.inMarkedFiles);
    _askForNextFile->setChecked(options.askForNextFile);
    _askForSave->setChecked(!options.askForSave);
}

void FindInFilesDialog::readSettings()
{
	KConfig* config = KGlobal::config();

	if(isReplaceDialog()) {
		KConfigGroupSaver cgs(config,"ReplaceDialog");
		
		ReplaceOptions options = replaceOpts();
	
		options.inAllFiles = config->readBoolEntry("AllFiles", false);
		options.inTemplates = config->readBoolEntry("InTemplates", false);
		options.inMarkedFiles = config->readBoolEntry("InMarked", false);
		options.askForNextFile = config->readBoolEntry("AskForNextFile", true);
		options.askForSave = config->readBoolEntry("AskForSave", true);
		
		_inAllFiles->setChecked(options.inAllFiles);
		_inTemplates->setChecked(options.inTemplates);
		_inMarked->setChecked(options.inMarkedFiles);
		_askForNextFile->setChecked(options.askForNextFile);
		_askForSave->setChecked(!options.askForSave);
		
		FindDialog::setReplaceOpts(options);
	}
	else {
		KConfigGroupSaver cgs(config,"FindDialog");

		FindOptions options = findOpts();

		options.inAllFiles = config->readBoolEntry("AllFiles", false);
		options.inTemplates = config->readBoolEntry("InTemplates", false);
		options.inMarkedFiles = config->readBoolEntry("InMarked", false);
		options.askForNextFile = config->readBoolEntry("AskForNextFile", true);
		options.askForSave = config->readBoolEntry("AskForSave", true);
		
		_inAllFiles->setChecked(options.inAllFiles);
		_inTemplates->setChecked(options.inTemplates);
		_inMarked->setChecked(options.inMarkedFiles);
		_askForNextFile->setChecked(options.askForNextFile);
		_askForSave->setChecked(!options.askForSave);
		
		FindDialog::setFindOpts(options);
	}
	
}

void FindInFilesDialog::saveSettings()
{
	KConfig* config = KGlobal::config();

	if(isReplaceDialog()) {
		KConfigGroupSaver cgs(config,"ReplaceDialog");
		ReplaceOptions options = replaceOpts();

		config->writeEntry("AllFiles", options.inAllFiles);
		config->writeEntry("InMarked", options.inMarkedFiles);
		config->writeEntry("InTemplates", options.inTemplates);
		config->writeEntry("AskForNextFile",options.askForNextFile);
		config->writeEntry("AskForSave",options.askForSave);
	}
	else {
		KConfigGroupSaver cgs(config,"FindDialog");
		
		FindOptions options = findOpts();

		config->writeEntry("AllFiles", options.inAllFiles);
		config->writeEntry("InMarked", options.inMarkedFiles);
		config->writeEntry("InTemplates", options.inTemplates);
		config->writeEntry("AskForNextFile",options.askForNextFile);
		config->writeEntry("AskForSave",options.askForSave);
	}
}

#include "findinfilesdialog.moc"
