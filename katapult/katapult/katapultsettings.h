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
#ifndef KATAPULTSETTINGS_H
#define KATAPULTSETTINGS_H

#include <qobject.h>
#include <qptrlist.h>
#include <qdict.h>

class KatapultCatalog;
class KatapultDisplay;
class QStringList;
class QString;

/**
@author Joe Ferris
*/
class KatapultSettings : public QObject
{
	Q_OBJECT
public:
	enum NRAction {
		NR_DoNothing = 0,
		NR_ClearQuery = 1,
		NR_HideDisplay = 2
	};
	
	KatapultSettings();
	~KatapultSettings();
	
	bool isConfiguring() const;
	
	// general settings
	bool isAutoExecute() const;
	void setAutoExecute(bool);
	int hideDelay() const;
	void setHideDelay(int);
	int noResultsDelay() const;
	void setNoResultsDelay(int);
	NRAction noResultsAction() const;
	void setNoResultsAction(NRAction);
	bool systrayIcon() const;
	void setSystrayIcon(bool);
	bool hideNotification() const;
	int notificationTimeout() const;

	// display settings
	QString displayName() const;
	QStringList displayNames() const;
	QStringList displayIds() const;
	int displayNumber() const;
	KatapultDisplay * display() const;
	void setDisplayName(QString name);
	
	// catalog settings
	QStringList catalogNames() const;
	QStringList catalogIds() const;
	QStringList activeCatalogNames() const;
	QStringList inactiveCatalogNames() const;
	QDict<KatapultCatalog> activeCatalogs() const;
	void activateCatalog(QString catalogName);
	void deactivateCatalog(QString catalogName);
	KatapultCatalog * catalog(QString catalogName) const;

public slots:	
	void readSettings();
	void writeSettings();
	void configure();
	
signals:
	void systrayChanged();
	void displayChanged();
	void catalogsChanged();
	
private:
	void loadCatalogPlugins();
	void loadDisplayPlugins();

	bool _isConfiguring;
	bool _forceWrite;

	// general settings
	int _hideSessionNotification; // Hide Katapult Notification? (session restore / manual start)
	int _hideUserNotification;    // if 0: show always, if 1: show once, else: show never
	int _notificationTimeout;     // Delay before notification hides.
	bool _autoExec;
	int _hideDelay;
	int _noResultsDelay;
	NRAction _noResultsAction;
	bool _systrayIcon;

	// display settings
	QString _displayName;
	QStringList _displayNames;
	QStringList _displayIds;
	KatapultDisplay *_display;
	
	// catalog settings
	QStringList _catalogNames, _catalogIds, _activeCatalogNames;
	QDict<KatapultCatalog> _activeCatalogs;
};

#endif
