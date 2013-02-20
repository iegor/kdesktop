/*
    Copyright (C) 2004 Teemu Rytilahti <tpr@d5k.net>
    Copyright (C) 2005 George Staikos <staikos@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    As a special exception, permission is given to link this program
    with any edition of Qt, and distribute the resulting executable,
    without including the source code for Qt in the source distribution.
*/

#ifndef KONQFEEDICON_H
#define KONQFEEDICON_H

#include <khtml_part.h>
#include <kparts/plugin.h>
#include <kpopupmenu.h>
#include "pluginbase.h"

/**
@author Teemu Rytilahti
*/
class KURLLabel;
class QStringList;

namespace KParts {
    class StatusBarExtension;
}

class KonqMFIcon : public KParts::Plugin, PluginBase {
	Q_OBJECT
	public:
		KonqMFIcon(QObject *parent, const char *name, const QStringList &);

		~KonqMFIcon();

	private:
		bool mfFound();
		bool hasMicroFormat(DOM::NodeList nodes);
		void extractCard(DOM::Node node);
		void extractEvent(DOM::Node node);

		QGuardedPtr<KHTMLPart> m_part;
		KURLLabel *m_mfIcon;
		KParts::StatusBarExtension *m_statusBarEx;
		QGuardedPtr<KPopupMenu> m_menu;
		QValueList<QPair<QString, QString> > _events, _cards;

	private slots:
		void waitPartToLoad();
		void contextMenu();
		void addMFIcon();
		void removeMFIcon();
		void addMFs();
		void addMF(int id);
};

#endif
