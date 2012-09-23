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
#ifndef SONG_H
#define SONG_H

#include "katapultitem.h"
#include "coverimage.h"
#include <kurl.h>
#include <qstring.h>
#include <qpixmap.h>

/**
@author Bastian Holst
*/
class Song : public KatapultItem
{
	Q_OBJECT
public:
	Song(const QString&);
	
	virtual QPixmap icon(int) const;
	virtual QString text() const;
	virtual KURL url() const;
	
	void setArtist(const QString&);
	void setName(const QString&);
	void setURL(const KURL);
	void setIcon(const QString);
	void setAlbum(const QString&);
	
protected:
	QString _artist;
	QString _name;
	KURL _url;
	
	CoverImage cover;
};

#endif //SONG_H
