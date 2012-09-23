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

#include <kapp.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kwin.h>
#include <kkeydialog.h>
#include <kaction.h>
#include <kparts/componentfactory.h>
#include <kpassivepopup.h>
#include <kpopupmenu.h>
#include <klocale.h>
#include <kaboutapplication.h>
#include <dcopclient.h>

#include <qpopupmenu.h>
#include <qmenudata.h>
#include <qcursor.h>

#include "katapult.h"
#include "actionregistry.h"
#include "katapultcatalog.h"
#include "katapultdisplay.h"
#include "katapultsettings.h"
#include "status.h"

Katapult::Katapult()
	: DCOPObject("Katapult"),
	  KSystemTray(0, "katapultSysTray")
{
	// Register with DCOP
	kapp->dcopClient()->registerAs("katapult", false);
	kapp->dcopClient()->setDefaultObject(objId());

	setPixmap(KGlobal::iconLoader()->loadIcon("katapult", KIcon::Small));

	display = 0;
	globalAccel = 0;
	actions = 0;
	action = 0;
	executing = FALSE;
	_query = "";
	
	settings = new KatapultSettings();

	hideTimer = new QTimer(this);
	connect(hideTimer, SIGNAL(timeout()), this, SLOT(hideLauncher()));
	clearTimer = new QTimer(this);
	connect(clearTimer, SIGNAL(timeout()), this, SLOT(clearQuery()));
	
	connect(settings, SIGNAL(catalogsChanged()), this, SLOT(loadCatalogPlugins()));
	connect(settings, SIGNAL(displayChanged()), this, SLOT(initDisplay()));
	connect(settings, SIGNAL(systrayChanged()), this, SLOT(updateSystray()));

	loadCatalogPlugins();
	initDisplay();

	if(settings->systrayIcon())
	{
		show();
	}

	//show popup (unless we're session restored)
	if (!settings->hideNotification()) {
		QString msg = i18n("Application successfully started !\nPress %1 to use it...")
				.arg(globalAccel->shortcut("show_katapult").toString());
		KPassivePopup::message(i18n("Katapult Notification"), msg, kapp->miniIcon(), this, 0, settings->notificationTimeout());
	}
}

Katapult::~Katapult()
{
	if(globalAccel != 0)
		delete globalAccel;
	if(actions != 0)
		delete actions;
	delete settings;
}

void Katapult::loadCatalogPlugins()
{
	catalogs = settings->activeCatalogs();
}

void Katapult::initDisplay()
{
	display = settings->display();
	if(display == 0) {
		initAccel(this);
	} else {
		initAccel(display);
		setQuery("");
		connect(display, SIGNAL(keyReleased(QKeyEvent *)), this, SLOT(keyReleased(QKeyEvent *)));
		connect(display, SIGNAL(focusOut()), this, SLOT(hideLauncher()));
	}
}

void Katapult::initAccel(QWidget *parent)
{
	globalAccel = new KGlobalAccel(parent);
	globalAccel->insert("show_katapult", i18n("Show Launcher"), i18n("Shows the Katapult launcher"), ALT+Key_Space, ALT+Key_Space,
		this, SLOT(showLauncher()));
	globalAccel->readSettings();
	globalAccel->updateConnections();
	
	actions = new KActionCollection(parent);
	actionCollection()->clear();
	
	contextMenu()->clear();
	
	new KAction(i18n("Execute"), "exec", Key_Return, this, SLOT(execute()), actions, "execute");
	new KAction(i18n("Clear"), "editclear", Key_Left, this, SLOT(clearQuery()), actions, "clear");
	new KAction(i18n("Close"), "fileclose", Key_Escape, this, SLOT(hideLauncher()), actions, "close");
	new KAction(i18n("Complete Query"), "next", Key_Right, this, SLOT(completeQuery()), actions, "complete_query");
	new KAction(i18n("Show Context Menu"), "menu", CTRL+Key_C, this, SLOT(showContextMenu()), actions, "show_menu");
	
	KAction *actGlobAccel = KStdAction::keyBindings(this, SLOT(showGlobalShortcutsDialog()), actions);
	actGlobAccel->setText(i18n("Configure &Global Shortcuts..."));
	actGlobAccel->plug((QWidget *) contextMenu());
	
	KStdAction::keyBindings(this, SLOT(showShortcutsDialog()), actions)->plug((QWidget *) contextMenu());
	KStdAction::preferences(settings, SLOT(configure()), actions)->plug((QWidget *) contextMenu());
	
	contextMenu()->insertSeparator();
	KStdAction::aboutApp(this, SLOT(showAboutDialog()), actions)->plug((QWidget *) contextMenu());
	contextMenu()->insertSeparator();
	
	KStdAction::quit(this, SLOT(close()), actions)->plug((QWidget *) contextMenu());
	
	actions->readShortcutSettings();
}

void Katapult::completeQuery()
{
	if(!bestMatch.isNull()) {
		QString newQuery = bestMatch.item()->text();
		setQuery("");
		setQuery(newQuery);
	}
}

void Katapult::clearQuery()
{
	setQuery("");
}

void Katapult::setQuery(QString _query)
{
	allStatus=0;
	bestMatch = Match();
		
	this->_query = _query;
	if(display != 0)
		display->setQuery(_query);
	if(_query == "")
	{
		QDictIterator<KatapultCatalog> it(catalogs);
		KatapultCatalog *catalog;
		while((catalog = it.current()) != 0)
		{
			++it;
			catalog->setQuery("");
		}
		display->setItem(0);
		display->setAction(0);
		display->setStatus(0);
		display->setSelected(0);
		action = 0;
	} else if(catalogs.count() == 0) {
		allStatus = S_Active | S_NoResults;
		display->setStatus(allStatus);
	} else {
		QDictIterator<KatapultCatalog> it(catalogs);
		KatapultCatalog *catalog;
		int status;
		
		while((catalog = it.current()) != 0)
		{
			++it;
			catalog->setQuery(_query);
				
			status = catalog->status();
			if(status & S_HasResults)
			{
				if(allStatus & S_HasResults)
					allStatus |= S_Multiple;
				Match match = catalog->bestMatch();
				if(!match.isNull())
				{
					if(bestMatch.isNull() || bestMatch.rank() < match.rank())
						bestMatch = match;
				}
			}
			allStatus |= status;
		}
		if(bestMatch.isNull() || bestMatch.rank() == 0)
			bestMatch = Match();
		if(!bestMatch.isNull()) {
			QPtrList<KatapultAction> itemActions = ActionRegistry::self()->actionsForItem(bestMatch.item());
			action = itemActions.at(0);
			connect(bestMatch.item(), SIGNAL(itemChanged()), this, SLOT(updateDisplay()));
		}
	}
	if(!(allStatus & S_HasResults) && allStatus & S_Active) {
			// no results
		switch(settings->noResultsAction()) {
			case KatapultSettings::NR_HideDisplay:
				hideTimer->start(settings->noResultsDelay(), TRUE);
				break;
			case KatapultSettings::NR_ClearQuery:
				clearTimer->start(settings->noResultsDelay(), TRUE);
				break;
			default:
				break;
		}
	} else {
			//stop timers if a catalog has suddenly realized it
			//does accept the input after all.
		if (hideTimer->isActive())
			hideTimer->stop();
		if (clearTimer->isActive())
			clearTimer->stop();
	}
	
	if(!executing && settings->isAutoExecute() && allStatus & S_HasResults && !(allStatus & S_Multiple)) {
		execute();
	}
	
	updateDisplay();
}

void Katapult::showAboutDialog()
{
	KAboutApplication *aboutDialog = new KAboutApplication(this);
	aboutDialog->exec();
	delete aboutDialog;
}

void Katapult::updateDisplay()
{
	if(display != 0 && executing == FALSE)
	{
		display->setItem(bestMatch.item());
		if(bestMatch.isNull()) {
			display->setAction(0);
			display->setSelected(0);
		} else {
			display->setAction(action);
			display->setSelected(bestMatch.matched());
		}
		display->setStatus(allStatus);
		display->update();
	}
}

void Katapult::showLauncher()
{
	if(!settings->isConfiguring() && display != 0)
	{
		setQuery("");
		display->setStatus(0);
		display->setQuery("");
		display->show();
		display->update();
		KWin::forceActiveWindow(display->winId());
	}
}

void Katapult::hideLauncher()
{
	if(display != 0)
		display->hide();
	setQuery("");
}

void Katapult::showShortcutsDialog()
{
	KKeyDialog::configure(actions);
}

void Katapult::showGlobalShortcutsDialog()
{
	KKeyDialog::configure(globalAccel);
}

void Katapult::execute()
{
	executing = TRUE;
	if(action == 0)
	{
		display->setStatus(S_Active | S_NoResults);
		display->update();
		hideTimer->start(settings->hideDelay(), TRUE);
	} else {
		completeQuery();
		if(!bestMatch.isNull())
			action->execute(bestMatch.item());
		hideTimer->start(settings->hideDelay(), TRUE);
	}
	executing = FALSE;
}

void Katapult::updateSystray()
{
	if(settings->systrayIcon())
		show();
	else
		hide();
}

void Katapult::showContextMenu()
{
	contextMenu()->popup(QCursor::pos());
}

void Katapult::keyReleased(QKeyEvent *e)
{
	if(e->key() == Key_BackSpace) {
		if(hideTimer->isActive())
			hideTimer->stop();
		if(clearTimer->isActive())
			clearTimer->stop();
		QString newQuery = _query.left(_query.length()-1);
		setQuery("");
		setQuery(newQuery);
	} else {
		QString t = e->text();
		if ( !t.isEmpty() && (!e->ascii() || e->ascii()>=32) &&
				    e->key() != Key_Delete) 
			setQuery(_query + t);
	}
}

void Katapult::mousePressEvent(QMouseEvent *e)
{
	if(e->button() == LeftButton)
		showLauncher();
	else
		KSystemTray::mousePressEvent(e);
}

#include "katapult.moc"
