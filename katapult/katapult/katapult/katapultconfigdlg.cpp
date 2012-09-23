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

#include <kactionselector.h>
#include <kconfig.h>
#include <kapplication.h>
#include <kcombobox.h>
#include <klocale.h>

#include <qframe.h>
#include <qlayout.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qstringlist.h>
#include <qradiobutton.h>
 
#include "katapultconfigdlg.h"
#include "katapultsettings.h"
#include "katapultcatalog.h"
#include "katapultdisplay.h"

#include "confgeneral.h"
#include "confcatalogs.h"
#include "confdisplay.h"

KatapultConfigDlg::KatapultConfigDlg(KatapultSettings *settings)
 : KDialogBase(TreeList, "Configure", Ok|Cancel, Ok)
{
	this->settings = settings;
	setTreeListAutoResize(TRUE);

	// general settings	
	QFrame *fConfGeneral = addPage(QString("Katapult"), i18n("General Settings"));
	general = new ConfGeneral(fConfGeneral);
	(new QVBoxLayout(fConfGeneral, 0, KDialog::spacingHint()))->addWidget(general);
	
	general->hideDelay->setValue(settings->hideDelay());
	general->autoExec->setChecked(settings->isAutoExecute());
	general->noResultsDelay->setValue(settings->noResultsDelay());
	general->systrayIcon->setChecked(settings->systrayIcon());
	switch(settings->noResultsAction()) {
		case KatapultSettings::NR_DoNothing:	general->nrDoNothing->setChecked(TRUE);		break;
		case KatapultSettings::NR_HideDisplay:	general->nrHideDisplay->setChecked(TRUE);	break;
		default:				general->nrClearQuery->setChecked(TRUE);	break;
	}
	connect(general->nrDoNothing, SIGNAL(toggled(bool)), this, SLOT(nrDoNothingToggled(bool)));
	general->noResultsDelay->setEnabled(!general->nrDoNothing->isChecked());
	
	// catalog settings
	QFrame *fConfCatalogs = addPage(i18n("Catalogs"), i18n("Enabled Catalogs"));
	catalogConf = new ConfCatalogs(fConfCatalogs);
	(new QVBoxLayout(fConfCatalogs, 0, KDialog::spacingHint()))->addWidget(catalogConf);
	
	catalogConf->catalogs->selectedListBox()->insertStringList(settings->activeCatalogNames());
	catalogConf->catalogs->availableListBox()->insertStringList(settings->inactiveCatalogNames());
	
	catalogPages.setAutoDelete(TRUE);
	QDict<KatapultCatalog> catalogs;
	catalogs = settings->activeCatalogs();
	QDictIterator<KatapultCatalog> it(catalogs);
	KatapultCatalog *catalog;
	while((catalog = it.current()) != 0)
	{
		QString catalogName = it.currentKey();
		addCatalogPage(catalogName, catalog);
		++it;
	}
	
	// display settings
	QFrame *fConfDisplay = addPage(i18n("Display"), i18n("Display"));
	display = new ConfDisplay(fConfDisplay);
	(new QVBoxLayout(fConfDisplay, 0, KDialog::spacingHint()))->addWidget(display);
	new QVBoxLayout((QWidget *) display->displaySettings, 0, KDialog::spacingHint());
	
	QStringList displays = settings->displayNames();
	display->displayName->insertStringList(displays);
	display->displayName->setCurrentItem(settings->displayNumber());
	displayConfig = 0;
	addDisplaySettings();
	
	unfoldTreeList(TRUE);
	
	// connect signals	
	connect(catalogConf->catalogs, SIGNAL(added(QListBoxItem *)), this, SLOT(activateCatalog(QListBoxItem* )));
	connect(catalogConf->catalogs, SIGNAL(removed(QListBoxItem *)), this, SLOT(deactivateCatalog(QListBoxItem* )));
	connect(display->displayName, SIGNAL(activated(int)), this, SLOT(activateDisplay(int)));
	connect(this, SIGNAL(okClicked()), this, SLOT(saveSettings()));
}

KatapultConfigDlg::~KatapultConfigDlg()
{
}

void KatapultConfigDlg::nrDoNothingToggled(bool on)
{
	general->noResultsDelay->setEnabled(!on);
}

void KatapultConfigDlg::saveSettings()
{
	settings->setHideDelay(general->hideDelay->value());
	settings->setAutoExecute(general->autoExec->isChecked());
	settings->setNoResultsDelay(general->noResultsDelay->value());
	settings->setSystrayIcon(general->systrayIcon->isChecked());
	if (general->nrDoNothing->isChecked()) {
		settings->setNoResultsAction(KatapultSettings::NR_DoNothing);
	} else if (general->nrHideDisplay->isChecked()) {
		settings->setNoResultsAction(KatapultSettings::NR_HideDisplay);
	} else {
		settings->setNoResultsAction(KatapultSettings::NR_ClearQuery);
	}
}

void KatapultConfigDlg::activateCatalog(QListBoxItem *i)
{
	settings->activateCatalog(i->text());
	KatapultCatalog *catalog = settings->catalog(i->text());
	if(catalog != 0)
		addCatalogPage(i->text(), catalog);
}

void KatapultConfigDlg::deactivateCatalog(QListBoxItem *i)
{
	catalogPages.remove(i->text());
	settings->deactivateCatalog(i->text());
}

void KatapultConfigDlg::activateDisplay(int index)
{
	settings->setDisplayName(settings->displayIds()[index]);
	addDisplaySettings();
}

void KatapultConfigDlg::addCatalogPage(QString name, KatapultCatalog *catalog)
{
	QWidget *catalogConfig = catalog->configure();
	if(catalogConfig != 0)
	{
		QStringList path;
		path << i18n("Catalogs") << name;
		QFrame *fCatalog = addPage(path, name);
		catalogConfig->reparent(fCatalog, QPoint(0, 0));
		(new QVBoxLayout(fCatalog, 0, KDialog::spacingHint()))->addWidget(catalogConfig);
		catalogPages.insert(name, fCatalog);
	}
}

void KatapultConfigDlg::addDisplaySettings()
{
	if(displayConfig != 0)
	{
		delete displayConfig;
		displayConfig = 0;
	}
	displayConfig = settings->display()->configure();
	if(displayConfig != 0)
	{
		displayConfig->reparent((QWidget *) display->displaySettings, QPoint(0, 0));
		display->displaySettings->layout()->add(displayConfig);
		displayConfig->show();
	}
}

#include "katapultconfigdlg.moc"
