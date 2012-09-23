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

#include <kapplication.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>

#include <qclipboard.h>

#include "spellcatalog.h"
#include "spelling.h"
#include "katapultitem.h"
#include "actioncopyspelling.h"

ActionCopySpelling::ActionCopySpelling()
	: KatapultAction(), _spelling(0)
{
}

ActionCopySpelling::~ActionCopySpelling()
{
}

QString ActionCopySpelling::text() const
{
	if (_spelling->parseError()) {
		return i18n("Parse Error");
	} else {
	  return _spelling->result();
	}
}

QPixmap ActionCopySpelling::icon(int size) const
{
	return KGlobal::iconLoader()->loadIcon("katapultspellcheck", KIcon::NoGroup, size);
}

bool ActionCopySpelling::accepts(const KatapultItem* item) const
{
	bool accept = strcmp(item->className(), "Spelling") == 0;
	if (accept) {
		_spelling = (const Spelling*)item;
	}
	return accept;
}

void ActionCopySpelling::execute(const KatapultItem* item) const
{
	if (strcmp(item->className(), "Spelling") == 0) {
		_spelling = (const Spelling*)item;

		_spelling->evaluate();

		// Copy calculation and result into clipboard (unless there's a parse error).
		if (!_spelling->parseError()) {
		  _spelling->copyToClipboard();
		}
	}
}
