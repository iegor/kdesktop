/***************************************************************************
 *   Copyright (C) 2007  Martin Meredith                                   *
 *   mez@ubuntu.com                                                        *
 *                                                                         *
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

#include <kapplication.h>
#include <kconfig.h>
#include <kparts/componentfactory.h>
#include <klocale.h>

#include <qstring.h>
#include <qstringlist.h>
 
#include "katapultsettings.h"
#include "katapultcatalog.h"
#include "katapultconfigdlg.h"
#include "katapultdisplay.h"
#include "actionregistry.h"

KatapultSettings::KatapultSettings()
{
	_forceWrite = FALSE;
	readSettings();
	_isConfiguring = FALSE;
	_activeCatalogs.setAutoDelete(TRUE);
	_display = 0;
	
	loadCatalogPlugins();
	loadDisplayPlugins();
}


KatapultSettings::~KatapultSettings()
{
	if (_forceWrite)
		writeSettings();

	if(_display != 0)
		delete _display;
}

void KatapultSettings::readSettings()
{
	KConfig *config = kapp->config();
	_hideSessionNotification = config->readNumEntry("HideSessionNotification", 1);
	_hideUserNotification = config->readNumEntry("HideUserNotification", 0);
	_notificationTimeout = config->readNumEntry("NotificationTimeout", 5000);
	_hideDelay = config->readUnsignedNumEntry("HideDelay", 1000);
	_noResultsDelay = config->readUnsignedNumEntry("NoResultsDelay", 2500);
	_noResultsAction = (NRAction) config->readUnsignedNumEntry("NoResultsAction", NR_ClearQuery);
	_systrayIcon = config->readBoolEntry("SystrayIcon", TRUE);
	_autoExec = config->readBoolEntry("AutoExec", FALSE);
	_displayName = config->readEntry("Display", "Pure Display");

	QStringList defCatalogs;
	defCatalogs << "Program Catalog" << "Bookmark Catalog" << "Document Catalog" << "Calculator Catalog" << "Amarok Catalog" << "Spell Catalog" << "Google Catalog" << "Execution Catalog";
	_activeCatalogNames = config->readListEntry("Catalogs", defCatalogs);

	// Force a config write if notification is to be shown once.
	// (in writeSettings(), _hideSessionNotification or _hideUserNotification
	// is changed before writing [if necessary]).
	_forceWrite = ((kapp->isSessionRestored() && _hideSessionNotification == 1) ||
			(!kapp->isSessionRestored() && _hideUserNotification == 1));
}

void KatapultSettings::writeSettings()
{
	KConfig *config = kapp->config();
	int hideSession = _hideSessionNotification;
	int hideUser = _hideUserNotification;

	// If notification had to be shown once, set it to hide always.
	if (kapp->isSessionRestored()) {
		if (hideSession == 1) hideSession = 2;
	} else {
		if (hideUser == 1) hideUser = 2;
	}

	config->writeEntry("HideSessionNotification", hideSession);
	config->writeEntry("HideUserNotification", hideUser);
	config->writeEntry("NotificationTimeout", _notificationTimeout);
	config->writeEntry("HideDelay", _hideDelay);
	config->writeEntry("NoResultsDelay", _noResultsDelay);
	config->writeEntry("NoResultsAction", (int) _noResultsAction);
	config->writeEntry("SystrayIcon", _systrayIcon);
	config->writeEntry("AutoExec", _autoExec);
	config->writeEntry("Catalogs", _activeCatalogNames);
	config->writeEntry("Display", _displayName);

	QDictIterator<KatapultCatalog> it(_activeCatalogs);
	KatapultCatalog *catalog;
	while((catalog = it.current()) != 0)
	{
		KConfigGroup group(config, QString("Catalogs/%1").arg(it.currentKey()));
		catalog->writeSettings(&group);
		++it;
	}
	
	if(_display != 0)
	{
		KConfigGroup group(config, QString("Displays/%1").arg(_displayName));
		_display->writeSettings(&group);
	}

	config->sync();
}

void KatapultSettings::configure()
{
	if(!_isConfiguring)
	{
		_isConfiguring = TRUE;
		
		KatapultConfigDlg *dlg = new KatapultConfigDlg(this);
		if(dlg->exec() == QDialog::Accepted) {
			writeSettings();
			emit systrayChanged();
			loadCatalogPlugins();
			emit catalogsChanged();
			
		} else {
			readSettings();
			loadCatalogPlugins();
			loadDisplayPlugins();
			emit catalogsChanged();
			emit displayChanged();
		}
		delete dlg;
		
		_isConfiguring = FALSE;
	}
}

bool KatapultSettings::isConfiguring() const
{
	return _isConfiguring;
}

bool KatapultSettings::isAutoExecute() const
{
	return _autoExec;
}

void KatapultSettings::setAutoExecute(bool _autoExec)
{
	this->_autoExec = _autoExec;
}

int KatapultSettings::hideDelay() const
{
	return _hideDelay;
}

void KatapultSettings::setHideDelay(int _hideDelay)
{
	this->_hideDelay = _hideDelay;
}

int KatapultSettings::noResultsDelay() const
{
	return _noResultsDelay;
}

void KatapultSettings::setNoResultsDelay(int _noResultsDelay)
{
	this->_noResultsDelay = _noResultsDelay;
}

KatapultSettings::NRAction KatapultSettings::noResultsAction() const
{
	return _noResultsAction;
}

void KatapultSettings::setNoResultsAction(NRAction _noResultsAction)
{
	this->_noResultsAction = _noResultsAction;
}

bool KatapultSettings::systrayIcon() const
{
	return _systrayIcon;
}

void KatapultSettings::setSystrayIcon(bool _systrayIcon)
{
	this->_systrayIcon = _systrayIcon;
}

bool KatapultSettings::hideNotification() const
{
	int hide = kapp->isSessionRestored() ? _hideSessionNotification : _hideUserNotification;
	return (hide != 0 && hide != 1);
}

int KatapultSettings::notificationTimeout() const
{
	return _notificationTimeout;
}

QString KatapultSettings::displayName() const
{
	return _displayName;
}

QStringList KatapultSettings::displayNames() const
{
	return _displayNames;
}

QStringList KatapultSettings::displayIds() const
{
	return _displayIds;
}

int KatapultSettings::displayNumber() const
{
	return _displayIds.findIndex(_displayName);
}

KatapultDisplay * KatapultSettings::display() const
{
	return _display;
}

void KatapultSettings::setDisplayName(QString name)
{
	_displayName = name;
	loadDisplayPlugins();
	emit displayChanged();
}

QStringList KatapultSettings::catalogNames() const
{
	return _catalogNames;
}

QStringList KatapultSettings::catalogIds() const
{
	return _catalogIds;
}

QStringList KatapultSettings::activeCatalogNames() const
{
	return _activeCatalogNames;
}

QStringList KatapultSettings::inactiveCatalogNames() const
{
	QStringList inactiveCatalogs, catalogs = _catalogIds;
	for(QStringList::Iterator it = catalogs.begin(); it != catalogs.end(); ++it)
	{
		if(!_activeCatalogNames.contains(*it))
			inactiveCatalogs.append(*it);
	}
	return inactiveCatalogs;
}

QDict<KatapultCatalog> KatapultSettings::activeCatalogs() const
{
	return _activeCatalogs;
}

void KatapultSettings::activateCatalog(QString catalogName)
{
	_activeCatalogNames.append(catalogName);
	loadCatalogPlugins();
	emit catalogsChanged();
}

void KatapultSettings::deactivateCatalog(QString catalogName)
{
	_activeCatalogNames.remove(catalogName);
	loadCatalogPlugins();
	emit catalogsChanged();
}

void KatapultSettings::loadCatalogPlugins()
{
	_activeCatalogs.clear();
	_catalogNames.clear();
	_catalogIds.clear();
	ActionRegistry::self()->clear();

	KTrader::OfferList offers = KTrader::self()->query("Katapult/Catalog");
	KTrader::OfferList::ConstIterator it;
	for(it = offers.begin(); it != offers.end(); ++it)
	{
		QString id;
		
		KService::Ptr service = *it;
		KatapultCatalog *catalog;
		
		if(!service->property("X-Katapult-ID", QVariant::String).toString().isEmpty())
			id = service->property("X-Katapult-ID", QVariant::String).toString();
		else
			id = service->name();
		
		_catalogNames.append(service->name());
		_catalogIds.append(id);
		
		if(_activeCatalogNames.contains(id))
		{
			int errCode = 0;
			catalog = KParts::ComponentFactory::createInstanceFromService<KatapultCatalog>
				(service, 0, 0, QStringList(), &errCode);
			if(catalog != 0)
			{
				KConfigGroup group(kapp->config(), QString("Catalogs/%1").arg(id));
				catalog->readSettings(&group);
				catalog->initialize();
				_activeCatalogs.insert(id, catalog);
			}
		}
	}
}

void KatapultSettings::loadDisplayPlugins()
{
	if(_display != 0)
	{
		delete _display;
		_display = 0;
	}
	
	_displayNames.clear();
	_displayIds.clear();
	
	KTrader::OfferList offers = KTrader::self()->query("Katapult/Display");
	KTrader::OfferList::ConstIterator it;
	for(it = offers.begin(); it != offers.end(); ++it)
	{
		KService::Ptr service = *it;
		
		_displayNames.append(service->name());
		if(!service->property("X-Katapult-ID", QVariant::String).toString().isEmpty())
			_displayIds.append(service->property("X-Katapult-ID", QVariant::String).toString());
		else
			_displayIds.append(service->name());

		if(_displayIds.last() == _displayName)
		{
			int errCode = 0;
			_display = KParts::ComponentFactory::createInstanceFromService<KatapultDisplay>
				(service, 0, 0, QStringList(), &errCode);
		}
	}
	if(_display != 0)
	{
		KConfigGroup group(kapp->config(), QString("Displays/%1").arg(_displayName));
		_display->readSettings(&group);
	}
}

KatapultCatalog * KatapultSettings::catalog(QString catalogName) const
{
	return _activeCatalogs.find(catalogName);
}

#include "katapultsettings.moc"
