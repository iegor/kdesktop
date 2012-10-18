/*
 * xine_part.h
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
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

#ifndef XINEPART_H
#define XINEPART_H


#include <kparts/factory.h>

#include <qtimer.h>

#include "kaffeinepart.h"
#include "xine_part_iface.h"

#define FORWARD_TIMER 0
#define BACKWARD_TIMER 1

class QWidget;
class QSlider;
class QLabel;
class QPushButton;
class MRL;
class KXineWidget;
class QPoint;
class KSelectAction;
class KToggleAction;
class Equalizer;
class VideoSettings;
class FilterDialog;
class PositionSlider;
class KProgressDialog;
class KPopupMenu;

/**
 * Kaffeine Part - xine based player part
 * @author Jürgen Kofler <kaffeine@gmx.net>
 *
 */
class XinePart : public KaffeinePart, public XinePartIface
{
	Q_OBJECT
public:
	XinePart(QWidget*, const char*, QObject*, const char*, const QStringList&);
	virtual ~XinePart();

	/*
	 *Reimplemented from KaffeinePart
	 */
	bool isPlaying();
	bool isPaused();

	bool hasChapters(); /* e.g. DVD */
	void playNextChapter();
	void playPreviousChapter();
	void setDVDChapter(uint chapter);

	bool hasVideo();
	QString supportedExtensions();
	void* engine();
	uint volume() const; /* percent */
	uint position() const; /* percent */

	bool closeURL(); /* stops playback and shows kaffeine logo */
	static KAboutData* createAboutData();

	/*
	 * DCOP functions...
	 */
	int getContrast();
	void setContrast(int c);
	int getBrightness();
	void setBrightness(int b);
	void dvdMenuUp();
	void dvdMenuDown();
	void dvdMenuLeft();
	void dvdMenuRight();
	void dvdMenuSelect();
	void dvdMenuToggle();
	void aspectRatioAuto();
	void aspectRatio4_3();
	void aspectRatioAnamorphic();
	void aspectRatioSquare();
	void aspectRatioDVB();
	void zoomInX();
	void zoomOutX();
	void zoomInY();
	void zoomOutY();
	void zoomIn();
	void zoomOut();
	void zoomOff();
	QString screenShot();
	void nextAudioChannel();
	void nextSubtitleChannel();
	void speedFaster();
	void speedSlower();

public slots:
	/*
	 * Reimplemented from KaffeinePart
	 */
	bool openURL(const MRL& mrl);
	void slotPrepareForFullscreen(bool);
	void slotPlay(bool forcePlay=false);
	void slotTogglePause(bool pauseLive=true);
	void slotSetVolume(uint); /* percent */
	void slotSetPosition(uint); /* percent */
	void slotPosPlusSmall();
	void slotPosMinusSmall();
	void slotSyncVolume();
	void slotStop();
	void slotMute(); /* toggle mute */

	void slotVolumeUp();
	void slotVolumeDown();
	void slotPosPlusMedium();
	void slotPosMinusMedium();
	void slotPosPlusLarge();
	void slotPosMinusLarge();
	void slotJumpIncrement(int);
	void slotDelaySubTitle();
	void slotAdvanceSubTitle();
	void slotAddSubtitle();
	void slotNextAudioChannel();
	void slotNextSubtitleChannel();
	/***************** Private ********************/

private slots:
	void slotFinalize();
	void slotTrackPlaying();
	void slotCheckMoved();
	void slotNext();
	void slotPrevious();
	void slotSaveStream();
	void slotChannelInfo(const QStringList&, const QStringList&, int, int);
	void slotSetSubtitle(int);
	void slotSetAudioChannel(int);
	void slotNewPosition(int, const QTime&);
	void slotVolumeChanged(int);
	void slotPictureSettings();
	void slotEqualizer();
	void slotDeinterlaceQuality();
	void slotFilterDialog();
	void slotInfo();
	void slotToggleBroadcastSend();
	void slotBroadcastReceive();
	void slotJumpToPosition();
	void slotButtonTimerPressed();
	void slotButtonTimerReleased();
	void slotToggleOsdTimer();
	void slotScreenshot();
	void slotConfigXine();
	void slotError(const QString&);
	void slotMessage(const QString&);
	void slotStatus(const QString&);
	void slotNewTitle();
	void slotNewLength();
	void slotNewFrameSize();
	void slotPlaybackFinished();
	void slotContextMenu(const QPoint&);
	void slotDisableAllActions();
	void slotEnableAllActions();
	void slotEnablePlayActions();
	void slotCopyToClipboard();
	void slotLaunchExternally();
	void slotLaunchDelayed();
	void slotFastForward();
	void slotSlowMotion();
	void slotSetDVDTitle(const QString&);
	void slotSetDVDChapter(const QString&);
	void slotSetDVDAngle(const QString&);
	void slotDVDMenuLeft();
	void slotDVDMenuRight();
	void slotDVDMenuUp();
	void slotDVDMenuDown();
	void slotDVDMenuSelect();
	void slotSetHue(int);
	void slotSetSaturation(int);
	void slotSetContrast(int);
	void slotSetBrightness(int);

private:
	void initActions();
	void initConnections();
	void loadConfig();
	void saveConfig();

private:
	QPoint m_oldPosition;
	QTimer m_posCheckTimer;
	QTimer m_osdTimerEnabler;  /* Provide Long click on timer button */
	bool m_isOsdTimer;  /* Status of Osd Timer (on/off) */
	int m_timerDirection;  /* Counting Up or Down */
	int m_brightness, m_hue, m_contrast, m_saturation;

	MRL m_mrl;
	QValueList<MRL> m_playlist;
	uint m_current;
	uint m_lastDeinterlaceQuality;
	QString m_lastDeinterlacerConfig;
	uint m_broadcastPort;
	QString m_broadcastAddress;

	QSlider* m_volume;
	PositionSlider* m_position;
	QPushButton* m_playTime;
	uint currentPosition;

	KSelectAction* m_audioChannels;
	KSelectAction* m_audioVisual;
	KSelectAction* m_subtitles;
	KSelectAction* m_dvdChapters;
	KSelectAction* m_dvdTitles;
	KSelectAction* m_dvdAngles;
	KToggleAction* m_deinterlaceEnabled;
	KToggleAction* m_broadcastSend;
	KToggleAction* m_pauseButton;

	KXineWidget* m_xine;
	VideoSettings* m_pictureSettings;
	Equalizer* m_equalizer;
	QWidget* m_deinterlacerConfigWidget;
	FilterDialog* m_filterDialog;

	KPopupMenu* m_embeddedContext;

	/*  dvb  */
public:
	QString TimeShiftFilename;

public slots:
	void slotDvbOpen( const QString &filename, const QString &chanName, int haveVideo );
	void getTimeShiftFilename( const QString &filename );
	void requestForOSD( const QString &text, int duration, int priority );
	void setDvbCurrentNext( const QString &channelname, const QStringList &list );

signals:
	void stopDvb();
	void playerPause();
	void dvbOSDHide();
};


#include <qslider.h>

class VolumeSlider : public QSlider
{

public:

	VolumeSlider();
	~VolumeSlider();

protected:

	void wheelEvent(QWheelEvent* e);
	//bool eventFilter(QObject *obj, QEvent *ev);
};

#endif /* XINEPART_H */
