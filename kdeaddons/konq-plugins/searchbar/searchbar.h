/* This file is part of the KDE project
   Copyright (C) 2004 Arend van Beelen jr. <arend@auton.nl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef SEARCHBAR_PLUGIN
#define SEARCHBAR_PLUGIN

#include <kcombobox.h>
#include <klibloader.h>
#include <kparts/plugin.h>
#include <kparts/mainwindow.h>

#include <qguardedptr.h>
#include <qpixmap.h>
#include <qstring.h>

class KHTMLPart;
class KProcess;
class QPopupMenu;

/**
 * Combo box which catches mouse clicks on the pixmap.
 */
class SearchBarCombo : public KHistoryCombo
{
	Q_OBJECT

	public:
		/**
		 * Constructor.
		 */
		SearchBarCombo(QWidget *parent, const char *name);

		/**
		 * Returns the icon currently displayed in the combo box.
		 */
		const QPixmap &icon() const;

		/**
		 * Sets the icon displayed in the combo box.
		 */
		void setIcon(const QPixmap &icon);

		/**
		 * Finds a history item by its text.
		 * @return The item number, or -1 if the item is not found.
		 */
		int findHistoryItem(const QString &text);

		/**
		 * Sets whether the plugin is active. It can be inactive
		 * in case the current Konqueror part isn't a KHTML part.
		 */
		void setPluginActive(bool pluginActive);

	public slots:
		virtual void show();

	signals:
		/**
		 * Emitted when the icon was clicked.
		 */
		void iconClicked();

	protected:
		/**
		 * Captures mouse clicks and emits iconClicked() if the icon
		 * was clicked.
		 */
		virtual void mousePressEvent(QMouseEvent *e);

	private slots:
		void historyCleared();

	private:
		QPixmap m_icon;
		bool    m_pluginActive;
};

/**
 * Plugin that provides a search bar for Konqueror. This search bar is located
 * next to the location bar and will show a small icon indicating the search
 * provider it will use.
 *
 * @author Arend van Beelen jr. <arend@auton.nl>
 * @version $Id: searchbar.h 465369 2005-09-29 14:33:08Z mueller $
 */
class SearchBarPlugin : public KParts::Plugin
{
	Q_OBJECT

	public:
		/** Possible search modes */
		enum SearchModes { FindInThisPage = 0, UseSearchProvider };

		SearchBarPlugin(QObject *parent, const char *name,
		                const QStringList &);
		virtual ~SearchBarPlugin();

	protected:
		bool eventFilter(QObject *o, QEvent *e);

	private slots:
		/**
		 * Starts a search by putting the query URL from the selected
		 * search provider in the locationbar and calling goURL()
		 */
		void startSearch(const QString &search);

		/**
		 * Sets the icon to indicate which search engine is used.
		 */
		void setIcon();

		/**
		 * Opens the selection menu.
		 */
		void showSelectionMenu();

		void useFindInThisPage();
		void useSearchProvider(int);
		void selectSearchEngines();
		void searchEnginesSelected(KProcess *process);
		void configurationChanged();

		/**
		 * We keep track of part activations to know when to show or hide ourselves
		 */
		void partChanged(KParts::Part *newPart);

		/**
		 * Show or hide the combo box
		 */
		void updateComboVisibility();

		void focusSearchbar();
	private:
		void nextSearchEntry();
		void previousSearchEntry();

		QGuardedPtr<KHTMLPart> m_part;
		SearchBarCombo        *m_searchCombo;
                KWidgetAction         *m_searchComboAction;
		QPopupMenu            *m_popupMenu;
		QPixmap                m_searchIcon;
		SearchModes            m_searchMode;
		QString                m_providerName;
		bool                   m_urlEnterLock;
		QString                m_currentEngine;
		QStringList            m_searchEngines;
};

#endif // SEARCHBAR_PLUGIN
