/***************************************************************************
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

#include <kbookmark.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <krun.h>
#include <kurl.h>
#include <klocale.h>

#include <qstring.h>
#include <qpixmap.h>

#include "actionopenbookmark.h"
#include "katapultitem.h"
#include "bookmark.h"
#include "mozillabookmark.h"

QPixmap ActionOpenBookmark::icon(int size) const
{
	return KGlobal::iconLoader()->loadIcon("fileopen", KIcon::NoGroup, size);
}

QString ActionOpenBookmark::text() const
{
	return i18n("Open Bookmark");
}

void ActionOpenBookmark::execute(const KatapultItem *item) const
{
	if(strcmp(item->className(), "Bookmark") == 0) {
		const Bookmark *bookmark = (const Bookmark *) item;
		new KRun(bookmark->bookmark().url());
	} else if(strcmp(item->className(), "MozillaBookmark") == 0) {
		const MozillaBookmark *bookmark = (const MozillaBookmark *) item;
		new KRun(bookmark->url());
	}
}

bool ActionOpenBookmark::accepts(const KatapultItem *item) const
{
	return strcmp(item->className(), "Bookmark") == 0 || strcmp(item->className(), "MozillaBookmark") == 0;
}
