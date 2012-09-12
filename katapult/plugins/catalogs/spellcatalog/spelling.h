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
#ifndef SPELLING_H
#define SPELLING_H

#include <kspell.h>

#include <katapultitem.h>

class SpellCatalog;

/**
@author Tobi Vollebregt
*/
class Spelling : public KatapultItem
{
	Q_OBJECT
	public:
		Spelling(SpellCatalog*, const QString&);
		~Spelling();

		virtual QPixmap icon(int) const;
		virtual QString text() const;

		void setText(const QString&);
		QString result() const;
		bool parseError() const;

		//evaluate() must be const, or ActionEvaluateSpelling::execute() can't call it.
		//It makes sense because evaluate() does _not_ change the expression,
		//it just calculates the result and remembers that.
		void evaluate() const;

		SpellCatalog* catalog() const;

		void copyToClipboard() const;

	private:
	        SpellCatalog* const _catalog;
		QString _text;
		mutable QString _result;
		mutable bool _parseError;
		KSpell* spellChecker;
		mutable bool misspelt;
		mutable bool corrected;
		QString suggestedWords;

	protected slots:
		void spellCheckerReady();
		void spellCheckerCorrected(const QString& originalword, const QString& newword, unsigned int pos);
		void spellCheckerMisspelling(const QString& originalword, const QStringList& suggestions, unsigned int pos);
};

#endif
