/*
 * kaffeine.h
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 * Copyright (C) 2005-2006 Christophe Thommeret <hftom@free.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef KAFFEINE_H
#define KAFFEINE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qtimer.h>
#include <qvbox.h>

#include <kmainwindow.h>
#include <kparts/part.h>

#include "kaffeineiface.h"

class KActionMenu;
class KCmdLineOptions;
class KRecentFilesAction;
class KToggleAction;
class KToggleFullScreenAction;

class CdWidget;
class Disc;
class DvbPanel;
class InputManager;
class KaffeinePart;
class MRL;
class PlayList;
class PlayerContainer;
class StartWindow;
class SystemTray;

extern const KCmdLineOptions cmdLineOptions[];

class Kaffeine : public KMainWindow, private KaffeineIface
{
	Q_OBJECT
public:
	Kaffeine();
	virtual ~Kaffeine();

	void updateArgs();

private:
	/**
	 * Use this method to load whatever file/URL you have
	 */
	void load(const QStringList&);
	void load(const QString&);
	/**
	  * Load urls from command line into the "NEW" playlist
	  * @returns if any url like "DVD", "VCD" was found
	  */
	bool loadTMP(const QStringList&);

	void setDevice(const QString& device)
	{
		m_device = device;
	}

	/*
	 * DCOP functions...
	 */
	void openURL(QString url);
	void appendURL(QString url);
	void play();
	void playAudioCD();
	void playVCD();
	void playDVD();
	void pause();
	void stop();
	void next();
	void previous();
	void changePlaylist();
	bool isPlaying();
	bool isVideo();
	QString title();
	QString artist();
	QString album();
	QString track();
	QString getFileName();
	void random();
	void fullscreen();
	void minimal();
	int getLength();
	int getTimePos();
	void posPlus();
	void posMinus();
	void volUp();
	void volDown();
	void mute();
	void quit();
	void setNumber( int num );
	void dvbNewTimer( QString name, QString channel, QString datetime, QString duration );
	int dvbSNR( int device );
	void dvbOSD();
	void dvbOSDNextChannel();
	void dvbOSDPreviousChannel();
	void dvbOSDNextProgram();
	void dvbOSDPreviousProgram();
	void dvbOSDZap();
	void playDvb();

public slots:
	void slotPlay(const MRL&);
	void slotLoadURLS(const QStringList&);
	void slotSwitchToPlayerWindow();
	void slotChangeStatusbar(const QString&);
	void slotChangePlaylistStatus(const QString&);
	void slotStop();
	void slotPlayUnPause();

private:
	void showEvent(QShowEvent*);
	void hideEvent(QHideEvent*);

	void closeEvent(QCloseEvent*);
	//  void dragEnterEvent(QDragEnterEvent*);
	//  void dropEvent(QDropEvent*);
	void mouseDoubleClickEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mousePressEvent(QMouseEvent*);
	void resizeEvent(QResizeEvent*);

private slots:
	void slotOpenFile();
	void slotQuit();
	void optionsConfigureKeys();
	void optionsConfigureToolbars();
	void applyNewToolbarConfig();
	void slotFakeKeyEvent();
	void slotChangeCaption(const QString&);
	void slotOpenURL();
	void slotOpenDirectory();
	void slotPlaylistPlay();
	void slotNext();
	void slotRequestForCurrentTrack();
	void slotRequestForNextTrack();
	void slotRequestForPreviousTrack();
	void slotRequestForTrackNumber();
	void slotPlayRecent(const KURL&);
	void slotMetaFromPlayer(const MRL &mrl);
	void slotToggleFullscreen();
	void slotEscapeFullscreen();
	void slotToggleMinimalMode();
	void slotToggleMinimalModeFromPlayer();
	void slotTogglePlaylist();
	void slotPreferences();
	void slotOriginalAspect();
	void slotAutoresizeOff();
	void slotAutoresizeOriginal();
	void slotAutoresizeDouble();
	void slotAutoresizeTriple();
	void slotNewFrameSize(const QSize&);
	void slotCurrentTabChanged(QWidget*);
	void slotClearRecent();
	void slotLoadPart(const QString&);
	void slotLoadingCanceled(const QString&);
	void slotPlaybackFailed();
	void slotSystemTray(bool);
	void slotUseAlternateEncoding(bool);
	void slotAlternateEncoding(const QString&);
	void slotMute();
	void slotSleepAfterPlay();
	void slotSleepAfterPlayMenu();
	void slotQuitAfterPlay();
	void slotQuitAfterPlayMenu();
	void slotQuitAfterPlaylistMenu();
	void slotSetOSDTimeout(uint);
	void slotPauseVideo(bool);
	void slotDvbClient(bool,const QString&,int,int,const QString&);
	void slotNumKeyInput( int );
	void slotDVBNextBack( int );

private:
	void autoresize();
	void setupAccel();
	void setupActions();
	void loadConfig();
	void saveConfig();
	void hideToolbars(bool);
	QString askForOtherDevice(const QString&);
	void unloadCurrentPart();

	KaffeinePart* m_mediaPart;

	InputManager *inplug;

	QHBox *mainbox;
	QVBox *playerWidget;
	PlayerContainer* m_playerContainer;
	PlayList* m_playlist;
	Disc *cddisc;
#ifdef HAVE_DVB
	DvbPanel *dvbPanel;
#endif
	CdWidget *dvbClient;
	StartWindow *m_startWindow;

	bool m_dvbClientEnabled;
	QString m_dvbClientAddress;
	int m_dvbClientPort;
	int m_dvbClientInfo;
	QString m_dvbClientShiftPath;
	SystemTray* m_systemTray;
	QSize m_videoSize;
	bool m_noResize;
	bool m_embedSystemTray;
	bool m_sleepInfo;
	uint m_osdDuration;
	uint m_autoResizeFactor;

	bool m_useAlternateEncoding;
	QString m_alternateEncoding;

	bool m_autoPaused;
	bool m_pauseVideo;

	bool m_haveKWin;

	bool haveXTest;
	int fakeKeycode;

	QString m_currentPartService;

	QString m_device;
	QString m_filter;
	QStringList m_engineParameters;

	KRecentFilesAction* m_recent;
	KToggleFullScreenAction* m_fullscreen;
	KToggleAction* m_minimal;
	KToggleAction* m_autoResizeOff;
	KToggleAction* m_autoResizeOriginal;
	KToggleAction* m_autoResizeDouble;
	KToggleAction* m_autoResizeTriple;
	KToggleAction* m_originalAspect;
	KToggleAction* m_toggleLayout;
	KActionMenu* m_playersMenu;
	KToggleAction* m_sleepAfterPlay;
	KToggleAction* m_quitAfterPlay;
	KToggleAction* m_quitAfterPlaylist;
	bool m_statusBarVisible;

	QTimer m_screensaverTimer;
	QTimer m_numKeyHideTimer;
	int m_numKey;
	QString m_captionCache;

signals:
	void showOSD( const QString &text, int duration, int priority );

private:
	Kaffeine(const Kaffeine &);
	Kaffeine &operator=(const Kaffeine &);
};

#endif /* KAFFEINE_H */
