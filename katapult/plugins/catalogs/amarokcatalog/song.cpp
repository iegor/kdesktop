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
#include <kurl.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qstring.h>
#include <amarokcatalog.h>

#include "song.h"

Song::Song(const QString&)
	: KatapultItem()
{
	setName(QString());
	setArtist(QString());
	setURL(KURL());
	setIcon(QString());
	setAlbum(QString::null);
}

void Song::setArtist(const QString& artist)
{
	_artist = artist;
	cover.setArtist(artist);
}

void Song::setName(const QString& name)
{
	_name = name;
}

void Song::setAlbum(const QString& album)
{
	cover.setAlbum(album);
}

void Song::setURL(const KURL url)
{
	_url = url;
}

void Song::setIcon(const QString icon)
{
	cover.setURL(icon);
}

QPixmap Song::icon(int size) const
{
	QImage image = cover.load(size);
	if ( image.isNull() )
		return KGlobal::iconLoader()->loadIcon("multimedia", KIcon::NoGroup, size);
	return QPixmap(image);
}

QString Song::text() const
{
	QString text;
	if( !_artist.isEmpty() ) {
		text.append(_artist);
		text.append(": ");
	}
	text.append(_name);
	return text;
}

KURL Song::url() const
{
	return _url;
}

#include "song.moc"
