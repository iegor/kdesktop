/***************************************************************************
 *   Copyright (C) 2007  Martin Meredith                                   *
 *   mez@ubuntu.com                                                        *
 *                                                                         *
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
#include <kurl.h>
#include <krun.h>

#include "googlecatalog.h"
#include "query.h"
#include "katapultitem.h"
#include "actionsearch.h"

ActionSearch::ActionSearch()
	: KatapultAction(), _searchquery(0)
{
}

ActionSearch::~ActionSearch()
{
}

QString ActionSearch::text() const
{
	return i18n("Search Google");
}

QPixmap ActionSearch::icon(int size) const
{
	return KGlobal::iconLoader()->loadIcon("viewmag", KIcon::NoGroup, size);
}

bool ActionSearch::accepts(const KatapultItem* item) const
{
	bool accept = strcmp(item->className(), "SearchQuery") == 0;
	if (accept) {
		_searchquery = (const SearchQuery*)item;
	}
	return accept;
}

void ActionSearch::execute(const KatapultItem* item) const
{
	if (strcmp(item->className(), "SearchQuery") == 0) {
		_searchquery = (const SearchQuery*)item;

	KURL _gotourl;
	_gotourl.setProtocol("http");
	_gotourl.setHost("www.google.com");
	_gotourl.setPath("/search");
	_gotourl.addQueryItem("q", _searchquery->text().mid(_searchquery->catalog()->triggerWordLength() + 1));
	_gotourl.addQueryItem("ie", "UTF-8");
	_gotourl.addQueryItem("oe", "UTF-8");
	new KRun(_gotourl);


	}
}
