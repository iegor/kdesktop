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

/* $Id: kviewpresenter.h 465369 2005-09-29 14:33:08Z mueller $ */

#ifndef __kviewpresenter_h
#define __kviewpresenter_h

#include <kparts/plugin.h>
#include <kurl.h>

#include <qsortedlist.h>

namespace KImageViewer { class Viewer; }

class ImageListDialog;
class ImageListItem;
class QListViewItem;
class QTimer;
class KToggleAction;
class KAction;

class KViewPresenter : public KParts::Plugin
{
	Q_OBJECT
public:
	KViewPresenter( QObject* parent, const char* name, const QStringList & );
	virtual ~KViewPresenter();

protected:
	struct ImageInfo
	{
		KURL url;
		ImageInfo( const KURL & url )
			: url( url )
		{
		}
		bool operator==( const ImageInfo & i1 )
		{
			return url.prettyURL() == i1.url.prettyURL();
		}
		bool operator!=( const ImageInfo & i1 )
		{
			return url.prettyURL() == i1.url.prettyURL();
		}
		bool operator>( const ImageInfo & i1 )
		{
			return url.prettyURL() > i1.url.prettyURL();
		}
		bool operator<( const ImageInfo & i1 )
		{
			return url.prettyURL() < i1.url.prettyURL();
		}
	};
	bool eventFilter( QObject *, QEvent * );

private slots:
	void slotImageOpened( const KURL & );
	void slotImageList();
	void slotOpenFiles();
	void slotClose();
	void changeItem( QListViewItem * );
	void prev();
	void next();
	void slideshow( bool );
	void setSlideshowInterval( int );
	void shuffle();
	void closeAll();
	void loadList();
	void saveList();

private:
	void makeCurrent( QListViewItem * );

	KImageViewer::Viewer * m_pViewer;
	ImageListDialog * m_pImageList;
	KToggleAction * m_paSlideshow;
	KAction * m_paFileOpen;
	KAction * m_paFileClose;

	QSortedList<ImageInfo> m_imagelist;
	bool m_bDontAdd;
	ImageListItem * m_pCurrentItem;
	QTimer * m_pSlideshowTimer;
};

// vim:sw=4:ts=4:cindent
#endif
