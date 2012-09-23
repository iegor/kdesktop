/*  This file is part of the KDE project
    Copyright (C) 2002 Matthias Kretz <kretz@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

*/

// $Id: imagelistitem.h 465369 2005-09-29 14:33:08Z mueller $

#ifndef IMAGELISTITEM_H
#define IMAGELISTITEM_H

#include <klistview.h>
#include <kurl.h>
#include <qstring.h>

class QImage;

class ImageListItem : public KListViewItem
{
	public:
		ImageListItem( KListView * parent, const KURL & url );
		~ImageListItem();

		const QImage * image() const;
		const QString & file() const;
		const KURL & url() const;

		virtual int rtti() const { return 48294; }

	private:
		QImage * m_pImage;
		QString m_filename;
		KURL m_url;
};

// vim:sw=4:ts=4
#endif // IMAGELISTITEM_H
