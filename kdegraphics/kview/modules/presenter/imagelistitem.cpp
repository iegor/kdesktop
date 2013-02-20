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

// $Id: imagelistitem.cpp 465369 2005-09-29 14:33:08Z mueller $

#include "imagelistitem.h"

#include <qimage.h>

#include <klistview.h>

ImageListItem::ImageListItem( KListView * parent, const KURL & url )
	: KListViewItem( parent, parent->lastItem(), url.prettyURL() )
	, m_pImage( 0 )
	, m_filename( QString::null )
	, m_url( url )
{
	setDragEnabled( true );
	if( m_url.isLocalFile() )
	{
		m_filename = m_url.path();
	}
	else
	{
		// download file
		/*
		QString extension;
		QString fileName = m_url.fileName();
		int extensionPos = fileName.findRev( '.' );
		if ( extensionPos != -1 )
			extension = fileName.mid( extensionPos ); // keep the '.'
		delete m_pTempFile;
		m_pTempFile = new KTempFile( QString::null, extension );
		m_filename = m_pTempFile->name();

		m_pJob = KIO::get( m_url, m_pExtension->urlArgs().reload, false );
		*/
	}
}

ImageListItem::~ImageListItem()
{
	if( ! m_url.isLocalFile() )
	{
		// remove downloaded tempfile
		//KIO::NetAccess::removeTempFile( m_filename );
	}
}

const QImage * ImageListItem::image() const
{
	return m_pImage;
}

const QString & ImageListItem::file() const
{
	if( m_url.isLocalFile() )
		return QString::null;
	return m_filename;
}

const KURL & ImageListItem::url() const
{
	return m_url;
}

// vim:sw=4:ts=4
