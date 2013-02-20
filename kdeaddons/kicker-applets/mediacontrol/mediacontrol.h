/***************************************************************************
                         main file of mediacontrol applet
                             -------------------
    begin                : Tue Apr 25 11:53:11 CEST 2000
    copyright            : (C) 2000-2005 by Stefan Gehn
    email                : metz {AT} gehn {DOT} net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MEDIACONTROL_H
#define MEDIACONTROL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <klocale.h>
#include <kpanelapplet.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kaboutdata.h>
#include <kaboutapplication.h>

#include <qpalette.h>

// used everywhere :)
#include <qstring.h>

#include "playerInterface.h"
#include "configfrontend.h"
#include "mediacontroliface.h"
#include "simplebutton.h"

#define MEDIACONTROL_VERSION "0.4"

class MCSlider;
class KPopupMenu;
class KInstance;
class MediaControlConfig;
class KAboutData;
class MediaControlToolTip;

// =============================================================================

class TrayButton : public SimpleButton
{
	Q_OBJECT

	public:
		TrayButton(QWidget* parent, const char* name);
		virtual ~TrayButton() {}
		void setIconSet(const QIconSet &iconSet);
};

// =============================================================================

class MediaControl : public KPanelApplet, virtual public MediaControlIface
{
	Q_OBJECT

	public:
		MediaControl(const QString&, Type, int ,QWidget * = 0, const char * = 0);
		virtual ~MediaControl();

		int widthForHeight(int height) const;
		int heightForWidth(int width) const;
		void dragEnterEvent(QDragEnterEvent* event);
		void dropEvent(QDropEvent* event);
		virtual void reparseConfig();

	public slots:
		void about();
		void preferences();
		void reportBug();

	private:
		KInstance *mInstance;
		KAboutData *mAboutData;
		DCOPClient *_dcopClient;
		PlayerInterface *_player;
		ConfigFrontend *_configFrontend;
		MediaControlConfig *_prefsDialog;
		TrayButton *prev_button;  // GoTo Previous Playlist-Item
		TrayButton *playpause_button;  // Start/Pause playing
		TrayButton *stop_button;  // Stop the music
		TrayButton *next_button;  // GoTo Next Playlist-Item
		MCSlider *time_slider;
		int mLastLen, mLastTime, mLastStatus;
		KPopupMenu *rmbMenu;

		virtual void mousePressEvent(QMouseEvent* e);
		virtual void moveEvent(QMoveEvent*);
		virtual void paletteChange(const QPalette&);
		virtual void resizeEvent(QResizeEvent*);
		virtual bool eventFilter(QObject *watched, QEvent *e);

		friend class MediaControlToolTip;
		MediaControlToolTip *slider_tooltip;

	private slots:
		void setSliderPosition(int len, int time);
		void slotIconChanged();
		void disableAll();
		void enableAll();
		void slotClosePrefsDialog();
		void slotPrefsDialogClosing();
		void slotConfigChanged();
		void adjustTime(int);
		void slotPlayingStatusChanged(int status);

	signals:
		void newJumpToTime(int);
};
#endif
