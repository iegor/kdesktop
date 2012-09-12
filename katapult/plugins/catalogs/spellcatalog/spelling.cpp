/***************************************************************************
 *   Copyright (C) 2006  Jonathan Riddell                                  *
 *   jriddell@ubuntu.com                                                   *
 *                                                                         *
 *   Copyright (C) 2005 by Joe Ferris                                      *
 *   jferris@optimistictech.com                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <kservice.h> 
#include <kglobal.h>
#include <kiconloader.h>
#include <kapplication.h>

#include <qclipboard.h>

#include "spellcatalog.h"
#include "spelling.h"


Spelling::Spelling(SpellCatalog* catalog, const QString& text): KatapultItem(), _catalog(catalog), _text(text)
{
	spellChecker = new KSpell( 0, "caption", this, SLOT(spellCheckerReady()) );

	connect( spellChecker, SIGNAL(misspelling(const QString&, const QStringList&, unsigned int)),
		 this, SLOT(spellCheckerMisspelling(const QString&, const QStringList&, unsigned int)) );

	connect( spellChecker, SIGNAL(corrected(const QString&, const QString&, unsigned int)), 
		 this, SLOT(spellCheckerCorrected(const QString&, const QString&, unsigned int)) );

	evaluate();
}

Spelling::~Spelling() {
	delete spellChecker;
}

void Spelling::spellCheckerReady() {
}

void Spelling::spellCheckerCorrected(const QString& /*originalword*/, const QString& /*newword*/, unsigned int /*pos*/) {
	corrected = true;
}

void Spelling::spellCheckerMisspelling(const QString& /*originalword*/, const QStringList& suggestions, unsigned int /*pos*/) {
	misspelt = true;
	suggestedWords = suggestions.join(",");
}

QPixmap Spelling::icon(int size) const
{
	const char* icon = "checkmark";
	if (_parseError || misspelt) {
		icon = "no";
	}
	return KGlobal::iconLoader()->loadIcon(icon, KIcon::NoGroup, size);
}

QString Spelling::text() const
{
	return _text;
}

void Spelling::setText(const QString& text)
{
	_text = text;
	evaluate();
}

QString Spelling::result() const
{
	return _result;
}

bool Spelling::parseError() const
{
	return _parseError;
}

SpellCatalog* Spelling::catalog() const
{
	return _catalog;
}

void Spelling::evaluate() const
{
	int length = catalog()->triggerWordLength();
	
	QString text = _text.mid(length + 1); // + 1 for space
	
	misspelt = false;
	corrected = false;
	
	_parseError = false;
	if (!_text.isEmpty()) {
		_result = "my result";
		spellChecker->checkWord(text);
	
		while (corrected == false) {
			kapp->processEvents();
		}
	
		if (misspelt) {
			_result = suggestedWords;
		} else {
			_result = "Correct";
		}
	} else {
		_parseError = true;
	}
}

void Spelling::copyToClipboard() const {
	QClipboard* clipBoard = QApplication::clipboard();
	clipBoard->setText(suggestedWords, QClipboard::Clipboard);
	clipBoard->setText(suggestedWords, QClipboard::Selection);
}

#include "spelling.moc"
