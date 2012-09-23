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

#include <kmdcodec.h>
#include <qimage.h>
#include <qdir.h>

#include "coverimage.h"

CoverImage::CoverImage() 
{
	setArtist(QString());
	setAlbum(QString());
	setURL(QString());
	
	_coverfolder = QDir::homeDirPath();
	_coverfolder.append("/.kde/share/apps/amarok/albumcovers/");
	_largefolder = QString(_coverfolder).append("large/");
	_cachefolder = QString(_coverfolder).append("cache/");
	
	QDir foldertest;
	foldertest.setPath(_coverfolder);
	if ( !foldertest.exists() )
		foldertest.mkdir(_coverfolder);
	if ( !foldertest.exists(_largefolder) )
		foldertest.mkdir(_largefolder);
	if ( !foldertest.exists(_cachefolder) )
		foldertest.mkdir(_cachefolder);
}

QImage CoverImage::load(int size) const
{
	QString imagepath(_cachefolder);
	KMD5 md5sum(artist().lower().utf8()+album().lower().utf8());
	imagepath.append(QString::number ( size )).append("@").append(md5sum.hexDigest());
	QImage image( imagepath );
	if ( image.isNull() )
	{
		if ( !url().isEmpty() ) {
			image.load(url());
		} else {
			imagepath = _largefolder;
			imagepath.append(md5sum.hexDigest());
			image.load( imagepath );
		}
		
		if ( !image.isNull() ) {
			QString savepath(_cachefolder);
			savepath.append(QString::number ( size )).append("@").append(md5sum.hexDigest());
			image = image.smoothScale(size, size, QImage::ScaleMin);
			if ( !url().isEmpty() ) {
				image.save( savepath, QImage::imageFormat ( url() ));
			} else {
				image.save( savepath, QImage::imageFormat ( imagepath ));
			}
		}
	}
	return image;
}

QString CoverImage::artist() const
{
	return _artist;
}

QString CoverImage::album() const
{
	return _album;
}

QString CoverImage::url() const
{
	return _url;
}

void CoverImage::setArtist(const QString& artist)
{
	_artist = artist;
}

void CoverImage::setAlbum(const QString& album)
{
	_album = album;
}

void CoverImage::setURL(const QString& url)
{
	_url = url;
}
