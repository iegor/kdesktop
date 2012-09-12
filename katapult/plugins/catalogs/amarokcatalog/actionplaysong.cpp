/***************************************************************************
 *   Copyright (C) 2005 by Bastian Holst                                   *
 *   bastianholst@gmx.de                                                   *
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
#include <qpixmap.h>
#include <qstring.h>
#include <qcstring.h>
#include <qdatastream.h>
#include <kapp.h>
#include <dcopclient.h>
#include <klocale.h>

#include <katapultitem.h>

#include "actionplaysong.h"
#include "song.h"

QPixmap ActionPlaySong::icon(int size) const
{
	return KGlobal::iconLoader()->loadIcon("amarok", KIcon::NoGroup, size);
}

QString ActionPlaySong::text() const
{
	return i18n("Play Song");
}

bool ActionPlaySong::accepts(const KatapultItem *item) const
{
	return strcmp(item->className(), "Song") == 0;
}
		
void ActionPlaySong::execute(const KatapultItem *item) const
{
	const Song *song = (const Song *) item;
	QByteArray buffer;
	QDataStream stream(buffer, IO_WriteOnly);
	stream << song->url();
	kapp->dcopClient()->send("amarok", "playlist", "playMedia(KURL)", buffer);
}
