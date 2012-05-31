/*

    Copyright (C) 2000-2001 Stefan Westerfeld
	                        <stefan@space.twc.de>
                  2003 Arnold Krille <arnold@arnoldarts.de

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

#ifndef MAIN_H
#define MAIN_H
#include <qiomanager.h>
#include <dispatcher.h>
#include <qmessagebox.h>
#include <kapplication.h>
#include <qframe.h>
#include <kmainwindow.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kstdaction.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <artsflow.h>
#include <soundserver.h>
#include <qobject.h>
#include <kartswidget.h>
#include <flowsystem.h>
#include <artsmodules.h>
#include <kartsserver.h>

class LevelMeter;
class PeakBar;
class PeakLevelMeter;
class VScale;

class FreeVerbView : public QWidget {
	Q_OBJECT
protected:
	Arts::Synth_FREEVERB freeverb;
	Arts::SimpleSoundServer server;
	long effectID;

	void closeEvent(QCloseEvent *e);
public:
	FreeVerbView(Arts::SimpleSoundServer server);
	~FreeVerbView();
signals:
	void closed();
};

class KArtsWidget;

class VControl : public QFrame {
	Q_OBJECT
protected:
	class StereoLevelMeter *stereoMeter;
	FreeVerbView *freeVerbView;
	Arts::StereoVolumeControl svc;
	Arts::StereoVolumeControlGui svcgui;
	KArtsWidget *svcguiw;
	KArtsServer *server;
	
	QBoxLayout *boxLayout;

public:
	VControl( KArtsServer*, QWidget *parent);
	~VControl();

public slots:
	void useOldVolumeBar(int old);
	void showFreeVerbView();
private slots:
	void initaRtsConnections();
};

#include "artsactions.h"

class MainWindow : public KMainWindow {
	Q_OBJECT
protected:
	VControl *vc;
	KToggleAction *showOldVolumeDisplay;
	ArtsActions* artsactions;
	KArtsServer *kartsserver;

public slots:
	void toggleVolumeBar();

	void serverRestarted();
public:
	MainWindow();
};

//Arts::Environment::Container defaultEnvironment();

#endif /* MAIN_H */
