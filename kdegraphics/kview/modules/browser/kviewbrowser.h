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
// $Id: kviewbrowser.h 465369 2005-09-29 14:33:08Z mueller $

#ifndef __kviewbrowser_h
#define __kviewbrowser_h

#include <kparts/plugin.h>
#include <kfileitem.h>

namespace KImageViewer { class Viewer; }

namespace KParts { class BrowserExtension; }
class KAction;
class KDirLister;
class KMyFileItemList;

class KViewBrowser : public KParts::Plugin
{
	Q_OBJECT
public:
	KViewBrowser( QObject* parent, const char* name, const QStringList & );
	virtual ~KViewBrowser();

private slots:
	void slotBack();
	void slotForward();

	void slotNewItems( const KFileItemList & );
	void slotDeleteItem( KFileItem * item );

private:
	void setupDirLister();
	void openURL(const KURL &);

	KImageViewer::Viewer * m_pViewer;
	KDirLister * m_pDirLister;
	KMyFileItemList * m_pFileItemList;
	KParts::BrowserExtension * m_pExtension;
	bool m_bShowCurrent;

	KAction * m_paBack;
	KAction * m_paForward;
};

// vim:sw=4:ts=4:cindent
#endif
