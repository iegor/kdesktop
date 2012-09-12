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
#ifndef KATAPULTCONFIGDLG_H
#define KATAPULTCONFIGDLG_H

#include <kdialogbase.h>

#include <qdict.h>

class KatapultSettings;
class KatapultCatalog;
class ConfGeneral;
class ConfCatalogs;
class ConfDisplay;
class QListBoxItem;
class QFrame;

/**
@author Joe Ferris
*/
class KatapultConfigDlg : public KDialogBase
{
Q_OBJECT
public:
	KatapultConfigDlg(KatapultSettings *settings);
	~KatapultConfigDlg();
	
public slots:
	void saveSettings();
	void activateCatalog(QListBoxItem *);
	void deactivateCatalog(QListBoxItem *);
	void activateDisplay(int);
	void nrDoNothingToggled(bool);

private:
	void addCatalogPage(QString name, KatapultCatalog *catalog);
	void addDisplaySettings();

	KatapultSettings *settings;
	ConfGeneral *general;
	ConfCatalogs *catalogConf;
	ConfDisplay *display;
	QWidget *displayConfig;
	QDict<QFrame> catalogPages;
};

#endif
