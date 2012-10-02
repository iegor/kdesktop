/* ****************************************************************************
  This file is part of KBabel

  Copyright (C) 1999-2000 by Matthias Kiefer
                            <matthias.kiefer@gmx.de>
		2002	  by Stanislav Visnovsky
			    <visnovsky@kde.org>

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
#include "finddialog.h"

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qgroupbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qhbox.h>
#include <qwhatsthis.h>

#include <kcombobox.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kparts/componentfactory.h>
#include <kregexpeditorinterface.h>

using namespace KBabel;

FindDialog::FindDialog(bool forReplace, QWidget* parent)
		:KDialogBase(parent, "finddialog",true, "", Ok|Cancel, Ok)
		, _regExpEditDialog(0), _replaceDlg(forReplace)
{
	QWidget* page = new QWidget(this);
	QVBoxLayout *layout = new QVBoxLayout(page, 0, spacingHint());

	QLabel *label = new QLabel(i18n("&Find:"),page);
	layout->addWidget(label);

	_findCombo = new KComboBox(true, page, "findCombo");
	_findCombo->setMaxCount(10);
	_findCombo->setInsertionPolicy(KComboBox::AtTop);
	layout->addWidget(_findCombo);
	label->setBuddy(_findCombo);

	QString msg=i18n("<qt><p><b>Find text</b></p>"
		"<p>Here you can enter the text you want to search for. "
		"If you want to search for a regular expression, "
		"enable <b>Use regular expression</b> below.</p></qt>");


	QWhatsThis::add(label,msg);
	QWhatsThis::add(_findCombo,msg);

	if(forReplace) {
		setCaption(i18n("Replace"));
                setButtonOK(i18n("&Replace"));

		_replaceLabel = new QLabel(i18n("&Replace with:"),page);
		layout->addWidget(_replaceLabel);
		_replaceCombo = new KComboBox(true, page, "replaceCombo");
		_replaceCombo->setMaxCount(10);
		_replaceCombo->setInsertionPolicy(KComboBox::AtTop);
		layout->addWidget(_replaceCombo);
		_replaceLabel->setBuddy(_replaceCombo);

	    	msg=i18n("<qt><p><b>Replace text</b></p>"
	    	"<p>Here you can enter the text you want the found text to get "
		"replaced with. The text is used as is. It is not possible to make a back "
		"reference, if you have searched for a regular expression.</p></qt>");

		QWhatsThis::add(_replaceLabel,msg);
		QWhatsThis::add(_replaceCombo,msg);
	}
	else {
		setCaption(i18n("Find"));
	        setButtonOK(KGuiItem(i18n("&Find"),"find"));

		_replaceLabel=0;
		_replaceCombo=0;
	}

	_buttonGrp = new QButtonGroup(3, Qt::Horizontal, i18n("Where to Search"), page);
	connect(_buttonGrp,SIGNAL(clicked(int)), this, SLOT(inButtonsClicked(int)));
	layout->addWidget(_buttonGrp);

	_inMsgid = new QCheckBox(i18n("&Msgid"),_buttonGrp);
	_inMsgstr = new QCheckBox(i18n("M&sgstr"),_buttonGrp);
	_inComment = new QCheckBox(i18n("Comm&ent"),_buttonGrp);

	QWhatsThis::add(_buttonGrp,i18n("<qt><p><b>Where to search</b></p>"
		"<p>Select here in which parts of a catalog entry you want "
		"to search.</p></qt>"));


	QGroupBox* box = new QGroupBox(2, Qt::Horizontal, i18n("Options"), page);
	layout->addWidget(box);

	_caseSensitive = new QCheckBox(i18n("C&ase sensitive"),box);
	_wholeWords = new QCheckBox(i18n("O&nly whole words"),box);
	_ignoreAccelMarker = new QCheckBox(i18n("I&gnore marker for keyboard accelerator"),box);
	_ignoreContextInfo = new QCheckBox(i18n("Ignore con&text information"),box);
	_fromCursor = new QCheckBox(i18n("From c&ursor position"),box);
	_backwards = new QCheckBox(i18n("F&ind backwards"),box);

	QHBox *regexp = new QHBox(box);

	_isRegExp = new QCheckBox(i18n("Use regu&lar expression"),regexp);
	_regExpButton = 0;

	if( !KTrader::self()->query("KRegExpEditor/KRegExpEditor").isEmpty() )
	{
	    _regExpButton = new QPushButton( i18n("&Edit..."), regexp );
	    connect( _regExpButton, SIGNAL( clicked() ), this, SLOT( regExpButtonClicked()));
	    connect( _isRegExp, SIGNAL( toggled(bool) ), _regExpButton, SLOT(setEnabled(bool)));
	}

	if(forReplace)
	{
		_inMsgid->setEnabled(false);
		_askForReplace = new QCheckBox(i18n("As&k before replacing"),box);
        _ignoreContextInfo->setEnabled(false);

		QWhatsThis::add(box,i18n("<qt><p><b>Options</b></p>"
		"<p>Here you can finetune replacing:"
		"<ul><li><b>Case sensitive</b>: does case of entered text have to be respected?</li>"
		"<li><b>Only whole words</b>: text found must not be part of a longer word</li>"
		"<li><b>From cursor position</b>: start replacing at the part of the document where "
		"the cursor is. Otherwise replacing is started at the beginning or the end.</li>"
		"<li><b>Find backwards</b>: Should be self-explanatory.</li>"
		"<li><b>Use regular expression</b>: use text entered in field <b>Find</b> "
		"as a regular expression. This option has no effect with the replace text, especially "
		"no back references are possible.</li>"
		"<li><b>Ask before replacing</b>: Enable, if you want to have control about "
		"what is replaced. Otherwise all found text is replaced without asking.</li>"
		"</ul></p></qt>"));
	}
	else {
		_askForReplace=0;

		QWhatsThis::add(box,i18n("<qt><p><b>Options</b></p>"
		"<p>Here you can finetune the search:"
		"<ul><li><b>Case sensitive</b>: does case of entered text have to be respected?</li>"
		"<li><b>Only whole words</b>: text found must not be part of a longer word</li>"
		"<li><b>From cursor position</b>: start search at the part of the document, where "
		"the cursor is. Otherwise search is started at the beginning or the end.</li>"
		"<li><b>Find backwards</b>: Should be self-explanatory.</li>"
		"<li><b>Use regular expression</b>: use entered text as a regular expression.</li>"
		"</ul></p></qt>"));
	}


	readSettings();


	setMainWidget(page);
}

FindDialog::~FindDialog()
{
   saveSettings();
}

int FindDialog::show(QString initialStr)
{
   if( !initialStr.isEmpty() ) {
		_findCombo->setEditText( initialStr );
	}
	_findCombo->lineEdit()->selectAll();
   _findCombo->setFocus();


	KDialogBase::show();

	int r = result();

	if( r == QDialog::Accepted ) {
		if(_replaceDlg) {
			_replaceList.remove(_replaceCombo->currentText());
			_replaceList.prepend(_replaceCombo->currentText());
			if(_replaceList.count() > 10 )
				_replaceList.remove(_replaceList.fromLast());

			_replaceCombo->clear();
			_replaceCombo->insertStringList(_replaceList);

			_replaceFindList.remove(_findCombo->currentText());
			_replaceFindList.prepend(_findCombo->currentText());
			if(_replaceFindList.count() > 10 )
				_replaceFindList.remove(_replaceFindList.fromLast());

			_findCombo->clear();
			_findCombo->insertStringList(_replaceFindList);

			_replaceOptions.findStr = _findCombo->currentText();
			_replaceOptions.replaceStr = _replaceCombo->currentText();

			_replaceOptions.inMsgstr = _inMsgstr->isChecked();
			_replaceOptions.inComment = _inComment->isChecked();
			_replaceOptions.inMsgid = false;

			_replaceOptions.caseSensitive = _caseSensitive->isChecked();
			_replaceOptions.wholeWords = _wholeWords->isChecked();
			_replaceOptions.ignoreAccelMarker = _ignoreAccelMarker->isChecked();
			_replaceOptions.ignoreContextInfo = false;
			_replaceOptions.backwards = _backwards->isChecked();
			_replaceOptions.fromCursor = _fromCursor->isChecked();
			_replaceOptions.isRegExp = _isRegExp->isChecked();
			_replaceOptions.ask = _askForReplace->isChecked();
		}
		else {
			_findList.remove(_findCombo->currentText());
			_findList.prepend(_findCombo->currentText());
			if(_findList.count() > 10 )
				_findList.remove(_findList.fromLast());

			_findCombo->clear();
			_findCombo->insertStringList(_findList);

			_findOptions.findStr = _findCombo->currentText();
			_findOptions.inMsgid = _inMsgid->isChecked();
			_findOptions.inMsgstr = _inMsgstr->isChecked();
			_findOptions.inComment = _inComment->isChecked();

			_findOptions.caseSensitive = _caseSensitive->isChecked();
			_findOptions.wholeWords = _wholeWords->isChecked();
			_findOptions.ignoreAccelMarker = _ignoreAccelMarker->isChecked();
			_findOptions.ignoreContextInfo = _ignoreContextInfo->isChecked();
			_findOptions.backwards = _backwards->isChecked();
			_findOptions.fromCursor = _fromCursor->isChecked();
			_findOptions.isRegExp = _isRegExp->isChecked();
		}
	}

	return r;
}

int FindDialog::exec(QString initialStr)
{
   if( !initialStr.isEmpty() ) {
		_findCombo->setEditText( initialStr );
	}
	_findCombo->lineEdit()->selectAll();
   _findCombo->setFocus();


	KDialogBase::exec();

	int r = result();

	if( r == QDialog::Accepted ) {
		if(_replaceDlg) {
			_replaceList.remove(_replaceCombo->currentText());
			_replaceList.prepend(_replaceCombo->currentText());
			if(_replaceList.count() > 10 )
				_replaceList.remove(_replaceList.fromLast());

			_replaceCombo->clear();
			_replaceCombo->insertStringList(_replaceList);

			_replaceFindList.remove(_findCombo->currentText());
			_replaceFindList.prepend(_findCombo->currentText());
			if(_replaceFindList.count() > 10 )
				_replaceFindList.remove(_replaceFindList.fromLast());

			_findCombo->clear();
			_findCombo->insertStringList(_replaceFindList);

			_replaceOptions.findStr = _findCombo->currentText();
			_replaceOptions.replaceStr = _replaceCombo->currentText();

			_replaceOptions.inMsgstr = _inMsgstr->isChecked();
			_replaceOptions.inComment = _inComment->isChecked();
			_replaceOptions.inMsgid = false;

			_replaceOptions.caseSensitive = _caseSensitive->isChecked();
			_replaceOptions.wholeWords = _wholeWords->isChecked();
			_replaceOptions.ignoreAccelMarker = _ignoreAccelMarker->isChecked();
			_replaceOptions.ignoreContextInfo = false;
			_replaceOptions.backwards = _backwards->isChecked();
			_replaceOptions.fromCursor = _fromCursor->isChecked();
			_replaceOptions.isRegExp = _isRegExp->isChecked();
			_replaceOptions.ask = _askForReplace->isChecked();
		}
		else {
			_findList.remove(_findCombo->currentText());
			_findList.prepend(_findCombo->currentText());
			if(_findList.count() > 10 )
				_findList.remove(_findList.fromLast());

			_findCombo->clear();
			_findCombo->insertStringList(_findList);

			_findOptions.findStr = _findCombo->currentText();
			_findOptions.inMsgid = _inMsgid->isChecked();
			_findOptions.inMsgstr = _inMsgstr->isChecked();
			_findOptions.inComment = _inComment->isChecked();

			_findOptions.caseSensitive = _caseSensitive->isChecked();
			_findOptions.wholeWords = _wholeWords->isChecked();
			_findOptions.ignoreAccelMarker = _ignoreAccelMarker->isChecked();
			_findOptions.ignoreContextInfo = _ignoreContextInfo->isChecked();
			_findOptions.backwards = _backwards->isChecked();
			_findOptions.fromCursor = _fromCursor->isChecked();
			_findOptions.isRegExp = _isRegExp->isChecked();
		}
	}

	return r;
}

FindOptions FindDialog::findOpts()
{
   return _findOptions;
}

void FindDialog::setFindOpts(FindOptions options)
{
    _findOptions = options;
    _inMsgid->setChecked(_findOptions.inMsgid);
    _inMsgstr->setChecked(_findOptions.inMsgstr);
    _inComment->setChecked(_findOptions.inComment);

    _caseSensitive->setChecked(_findOptions.caseSensitive);
    _wholeWords->setChecked(_findOptions.wholeWords);
    _ignoreAccelMarker->setChecked(_findOptions.ignoreAccelMarker);
    _ignoreContextInfo->setChecked(_findOptions.ignoreContextInfo);
    _backwards->setChecked(_findOptions.backwards);
    _fromCursor->setChecked(_findOptions.fromCursor);
    _isRegExp->setChecked(_findOptions.isRegExp);
    if( _regExpButton ) _regExpButton->setEnabled( _findOptions.isRegExp );

    _findCombo->setEditText(_findOptions.findStr);
}

ReplaceOptions FindDialog::replaceOpts()
{
   return _replaceOptions;
}

void FindDialog::setReplaceOpts(ReplaceOptions options)
{
   _replaceOptions = options;
    _inMsgid->setChecked(_replaceOptions.inMsgid);
    _inMsgstr->setChecked(_replaceOptions.inMsgstr);
    _inComment->setChecked(_replaceOptions.inComment);

    _caseSensitive->setChecked(_replaceOptions.caseSensitive);
    _wholeWords->setChecked(_replaceOptions.wholeWords);
    _ignoreAccelMarker->setChecked(_replaceOptions.ignoreAccelMarker);
    _ignoreContextInfo->setChecked(_replaceOptions.ignoreContextInfo);
    _backwards->setChecked(_replaceOptions.backwards);
    _fromCursor->setChecked(_replaceOptions.fromCursor);
    _isRegExp->setChecked(_replaceOptions.isRegExp);
    _askForReplace->setChecked(_replaceOptions.ask);
    if( _regExpButton ) _regExpButton->setEnabled( _replaceOptions.isRegExp );

    _findCombo->setEditText(_replaceOptions.findStr);
    _replaceCombo->setEditText(_replaceOptions.replaceStr);
}

void FindDialog::readSettings()
{
	KConfig* config = KGlobal::config();

	if(_replaceDlg) {
		KConfigGroupSaver cgs(config,"ReplaceDialog");
		_replaceOptions.inMsgstr = config->readBoolEntry("InMsgstr",true);
		_replaceOptions.inComment = config->readBoolEntry("InComment",false);

		_replaceOptions.caseSensitive =
            config->readBoolEntry("CaseSensitive",true);
		_replaceOptions.wholeWords = config->readBoolEntry("WholeWords",false);
		_replaceOptions.ignoreAccelMarker =
            config->readBoolEntry("IgnoreAccelMarker",true);
		_replaceOptions.backwards = config->readBoolEntry("Backwards",false);
		_replaceOptions.fromCursor = config->readBoolEntry("FromCursor",true);
		_replaceOptions.isRegExp = config->readBoolEntry("RegExp",false);
		_replaceOptions.ask = config->readBoolEntry("AskForReplace",true);
		_replaceFindList = config->readListEntry("FindList");
		_replaceList = config->readListEntry("ReplaceList");

		_inMsgstr->setChecked(_replaceOptions.inMsgstr);
		_inComment->setChecked(_replaceOptions.inComment);

		_caseSensitive->setChecked(_replaceOptions.caseSensitive);
		_wholeWords->setChecked(_replaceOptions.wholeWords);
		_ignoreAccelMarker->setChecked(_findOptions.ignoreAccelMarker);
		_backwards->setChecked(_replaceOptions.backwards);
		_fromCursor->setChecked(_replaceOptions.fromCursor);
		_isRegExp->setChecked(_replaceOptions.isRegExp);
		_askForReplace->setChecked(_replaceOptions.ask);
		if( _regExpButton ) _regExpButton->setEnabled( _findOptions.isRegExp );

		_replaceCombo->insertStringList(_replaceList);
		_findCombo->insertStringList(_replaceFindList);
	}
	else {
		KConfigGroupSaver cgs(config,"FindDialog");

		_findOptions.inMsgid = config->readBoolEntry("InMsgid",true);
		_findOptions.inMsgstr = config->readBoolEntry("InMsgstr",true);
		_findOptions.inComment = config->readBoolEntry("InComment",false);

		_findOptions.caseSensitive = config->readBoolEntry("CaseSensitive"
                ,false);
		_findOptions.wholeWords = config->readBoolEntry("WholeWords",false);
		_findOptions.ignoreAccelMarker =
            config->readBoolEntry("IgnoreAccelMarker",true);
		_findOptions.ignoreContextInfo =
            config->readBoolEntry("IgnoreContextInfo",true);
		_findOptions.backwards = config->readBoolEntry("Backwards",false);
		_findOptions.fromCursor = config->readBoolEntry("FromCursor",false);
		_findOptions.isRegExp = config->readBoolEntry("RegExp",false);
		_findList = config->readListEntry("List");
		if( _regExpButton ) _regExpButton->setEnabled( _findOptions.isRegExp );

		_inMsgid->setChecked(_findOptions.inMsgid);
		_inMsgstr->setChecked(_findOptions.inMsgstr);
		_inComment->setChecked(_findOptions.inComment);

		_caseSensitive->setChecked(_findOptions.caseSensitive);
		_wholeWords->setChecked(_findOptions.wholeWords);
		_ignoreAccelMarker->setChecked(_findOptions.ignoreAccelMarker);
		_ignoreContextInfo->setChecked(_findOptions.ignoreContextInfo);
		_backwards->setChecked(_findOptions.backwards);
		_fromCursor->setChecked(_findOptions.fromCursor);
		_isRegExp->setChecked(_findOptions.isRegExp);

		_findCombo->insertStringList(_findList);
	}

}

void FindDialog::saveSettings()
{
	KConfig* config = KGlobal::config();

	if(_replaceDlg) {
		KConfigGroupSaver cgs(config,"ReplaceDialog");
		config->writeEntry("InMsgstr",_replaceOptions.inMsgstr);
		config->writeEntry("InComment",_replaceOptions.inComment);

		config->writeEntry("CaseSensitive",_replaceOptions.caseSensitive);
		config->writeEntry("WholeWords",_replaceOptions.wholeWords);
		config->writeEntry("IgnoreAccelMarker"
                ,_replaceOptions.ignoreAccelMarker);
		config->writeEntry("Backwards",_replaceOptions.backwards);
		config->writeEntry("FromCursor",_replaceOptions.fromCursor);
		config->writeEntry("RegExp",_replaceOptions.isRegExp);
		config->writeEntry("AskForReplace",_replaceOptions.ask);
		config->writeEntry("FindList",_replaceFindList);
		config->writeEntry("ReplaceList",_replaceList);
	}
	else {
		KConfigGroupSaver cgs(config,"FindDialog");

		config->writeEntry("InMsgid",_findOptions.inMsgid);
		config->writeEntry("InMsgstr",_findOptions.inMsgstr);
		config->writeEntry("InComment",_findOptions.inComment);

		config->writeEntry("CaseSensitive",_findOptions.caseSensitive);
		config->writeEntry("WholeWords",_findOptions.wholeWords);
		config->writeEntry("IgnoreAccelMarker"
                ,_findOptions.ignoreAccelMarker);
		config->writeEntry("IgnoreContextInfo"
                ,_findOptions.ignoreContextInfo);
		config->writeEntry("Backwards",_findOptions.backwards);
		config->writeEntry("FromCursor",_findOptions.fromCursor);
		config->writeEntry("RegExp",_findOptions.isRegExp);
		config->writeEntry("List",_findList);
	}
}



void FindDialog::inButtonsClicked(int id)
{
 	// check if at least one button is checked
	if(! _buttonGrp->find(id)->isOn() ) {
		if(!_inMsgstr->isOn() && !_inComment->isOn() ) {
			if(_inMsgid->isEnabled()) {
				if( !_inMsgid->isOn() ) {
					_buttonGrp->setButton(id);
				}
			}
			else {
					_buttonGrp->setButton(id);
			}

		}
	}
}

void FindDialog::regExpButtonClicked()
{
    if ( _regExpEditDialog == 0 )
         _regExpEditDialog = KParts::ComponentFactory::createInstanceFromQuery<QDialog>( "KRegExpEditor/KRegExpEditor", QString::null, this );

    KRegExpEditorInterface *iface = dynamic_cast<KRegExpEditorInterface *>( _regExpEditDialog );
    if( iface )
    {
	iface->setRegExp( _findCombo->currentText() );
	if( _regExpEditDialog->exec() == QDialog::Accepted )
	    _findCombo->setCurrentText( iface->regExp() );
    }
}

ReplaceDialog::ReplaceDialog(QWidget* parent)
		:KDialogBase(Plain, "", Close|User1|User2|User3, User1, parent,"finddialog"
                   , true,false,i18n("&Replace"),i18n("&Goto Next"),i18n("R&eplace All"))
{
	QWidget* page = plainPage();
	QVBoxLayout *layout = new QVBoxLayout(page, 0, spacingHint());

	QLabel *label = new QLabel(i18n("Replace this string?"),page);
	layout->addWidget(label);

	connect(this,SIGNAL(user1Clicked()),this,SIGNAL(replace()));
	connect(this,SIGNAL(user2Clicked()),this,SIGNAL(next()));
	connect(this,SIGNAL(user3Clicked()),this,SIGNAL(replaceAll()));
}

ReplaceDialog::~ReplaceDialog()
{
}

#include "finddialog.moc"
