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

#include <kglobal.h>
#include <kiconloader.h>
#include <krun.h>
#include <klocale.h>

#include "actionopendocument.h"
#include "document.h"
#include "directory.h"

ActionOpenDocument::ActionOpenDocument()
 : KatapultAction()
{
}

bool ActionOpenDocument::accepts(const KatapultItem* item) const
{
	return strcmp(item->className(), "Document") == 0 || strcmp(item->className(), "Directory") == 0;
}

QPixmap ActionOpenDocument::icon(int size) const
{
	return KGlobal::iconLoader()->loadIcon("fileopen", KIcon::NoGroup, size);
}

QString ActionOpenDocument::text() const
{
	return i18n("Open");
}

void ActionOpenDocument::execute(const KatapultItem* item) const
{
	if(strcmp(item->className(), "Document") == 0 || strcmp(item->className(), "Directory") == 0) {
	    const Document *document = (const Document *) item;
		qDebug("%s", QString("Running: %1").arg(document->path()).latin1());
		new KRun(document->path());
	}
}

