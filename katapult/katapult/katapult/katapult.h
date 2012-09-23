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

#ifndef _KATAPULT_H_
#define _KATAPULT_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <ksystemtray.h>
#include <kglobalaccel.h>
#include <kactioncollection.h>
#include <dcopobject.h>

#include <qstring.h>
#include <qptrlist.h>
#include <qevent.h>
#include <qtimer.h>
#include <qdict.h>

#include <match.h>

#include "katapultdisplay.h"
#include "katapultcatalog.h"
#include "katapultaction.h"
#include "katapultiface.h"

class KatapultSettings;

class Katapult : public KSystemTray, virtual public KatapultIface
{
	Q_OBJECT
public:
	Katapult();
	~Katapult();
    
	void setQuery(QString);
	QString query() const;

public slots:
	void updateDisplay();
	void showLauncher();
	void hideLauncher();
	void completeQuery();
	void clearQuery();
	void showShortcutsDialog();
	void showGlobalShortcutsDialog();
	void showAboutDialog();
	void loadCatalogPlugins();
	void initDisplay();
	virtual void keyReleased(QKeyEvent *);
	void execute();
	void updateSystray();
	void showContextMenu();
	
protected:
	virtual void mousePressEvent(QMouseEvent *);
	
private:
	void initAccel(QWidget *);
	
	bool executing;
	int allStatus;
	KatapultDisplay *display;
	KGlobalAccel *globalAccel;
	KActionCollection *actions;
	KatapultSettings *settings;
	Match bestMatch;
	const KatapultAction *action;
	QString _query;
	QDict<KatapultCatalog> catalogs;
	QTimer *hideTimer;
	QTimer *clearTimer;
};

#endif // _KATAPULT_H_
