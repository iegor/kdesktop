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

#include <qfileinfo.h>
#include <kmimetype.h>
#include <kiconloader.h>
#include <kio/previewjob.h>

#include "document.h"

Document::Document(QString _path, QString _absPath, bool _showPreview)
		: KatapultItem()
{
	this->_path = _path;
	this->_absPath = _absPath;
	this->_gotpix = false;
	this->_showPreview=_showPreview;
	
	QFileInfo file(_absPath);
	_name = file.fileName();
}

Document::~Document()
{
}

QPixmap Document::icon(int size) const
{
	KURL u;
	KIO::PreviewJob *doc;
	u.setPath(_absPath);
	if (this->_showPreview)
	{

		doc=KIO::filePreview(u,size);
		connect(doc, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
			SLOT(gotPreview(const KFileItem*, const QPixmap&)));
	
		if(this->_gotpix)return this->_pix;
		return KMimeType::pixmapForURL(u,0, KIcon::NoGroup, size, KIcon::DefaultState, 0L);
	}
	else
	{
		return KMimeType::pixmapForURL(u,0, KIcon::NoGroup, size, KIcon::DefaultState, 0L);
	}
}

void Document::gotPreview(const KFileItem *item, const QPixmap& pixmap )
{
	this->_pix = pixmap; this->_gotpix=true; 
	emit itemChanged();
}

QString Document::text() const
{
	return _path;
}

QString Document::path() const
{
	return _absPath;
}

QString Document::name() const
{
	return _name;
}

#include "document.moc"
