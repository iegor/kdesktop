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
#ifndef COVERIMAGE_H
#define COVERIMAGE_H

#include<qstring.h>
#include<qimage.h>

/**
@author Bastian Holst
*/
class CoverImage 
{
public:
	CoverImage();
	
	QImage load(int) const;
	
	virtual QString artist() const;
	virtual QString album() const;
	virtual QString url() const;
	
	void setArtist(const QString&);
	void setAlbum(const QString&);
	void setURL(const QString&);
private:
	QString _artist;
	QString _album;
	QString _url;
	
	QString _coverfolder;
	QString _largefolder;
	QString _cachefolder;
};

#endif // COVERIMAGE_H

