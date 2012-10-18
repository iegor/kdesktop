	/*

	Copyright (C) 1998-1999 Stefan Westerfeld
                            stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

    */

#ifndef __MAIN_H__
#define __MAIN_H__

#include "mwidget.h"

#include <kdockwidget.h>
#include <kapplication.h>
#include <kfiledialog.h>
#include <ksimpleconfig.h>

#include <list>

class PropertyPanel;
class Structure;
class MenuMaker;
class ExecDlg;
class KToggleAction;

class ArtsBuilderWindow: public KDockMainWindow
{
	Q_OBJECT

protected:
	KDockWidget* mainDock;
    ModuleWidget *modulewidget;

	KDockWidget* propertyDock;
	PropertyPanel *propertyPanel;
	KToggleAction *viewPropertiesAction;

	MenuMaker *menumaker;

	long mbroker_updateCount;
	Structure *structure;

	ExecDlg *execDlg;

	QString m_filename;
	bool modified;

protected:
	void setupActions();

public:
	ArtsBuilderWindow(const char *name);
	~ArtsBuilderWindow();

	void clear();
	void checkName();
	QString getOpenFilename(const char *pattern, const char *initialDir = 0);
	bool isModified();
	bool promptToSave();
	bool queryClose();

public slots:
	void fillModuleMenu();
	void viewMenuAboutToShow();

	/* ports menu */
	void createInAudioSignal();
	void createOutAudioSignal();
	void createInStringProperty();
	void createInAudioProperty();
	void addInterface();

	/* view menu */
	void viewAt50();
	void viewAt100();
	void viewAt150();
	void viewAt200();

	/* file menu */
	void fileNew();
	void open();
	void openSession();
	void openExample();
	void save();
	void saveAs();
	void quit();

	void open(QString filename);
	bool save(QString filename);
	void setModified(bool m = true);
	
	void execute();
	void endexecute();
	void rename();
	void retrieve();
	void publish();

	void addModule(const char *module);
	void changePortPositions();

	void oldFileNewWhatTheHellDoesItDo(int what);
};

#endif
