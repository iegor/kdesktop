/*
 * gstreamer_part.h
 *
 * Copyright (C) 2006 Christophe Thommeret <hftom@free.fr>
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

#ifndef GSTREAMER_PART_H
#define GSTREAMER_PART_H

#include <kparts/factory.h>

#include <gst/gst.h>

#include "kaffeinepart.h"

#include "timer.h"
#include "video.h"
#include "videosettings.h"
#include "gstreamerconfig.h"



class GStreamerPart : public KaffeinePart
{
	Q_OBJECT
public:
	GStreamerPart( QWidget*, const char*, QObject*, const char*, const QStringList& );
	virtual ~GStreamerPart();

	/*
	 *Reimplement from KaffeinePart
	 */
	bool isPlaying();
	bool isPaused();
	uint volume() const; /* percent */
	uint position() const; /* percent */
	bool hasVideo();

	bool closeURL();
	static KAboutData* createAboutData();

public slots:
	/*
	 * Reimplement from KaffeinePart
	 */
	bool openURL( const MRL& mrl );
	void slotPlay();
	void slotTogglePause( bool );
	void slotSetVolume( uint ); /* percent */
	void slotSetPosition( uint ); /* percent */
	void slotStop();
	void slotMute();
	void slotPrepareForFullscreen( bool fullscreen );

private slots:
	void slotNext();
	void slotPrevious();
	void slotVolume( int );
	void slotSaturation( int ); /* -1000...1000 */
	void slotHue( int ); /* -1000...1000 */
	void slotContrast( int ); /* -1000...1000 */
	void slotBrightness( int ); /* -1000...1000 */
	void slotContextMenu( const QPoint& pos );
	void slotInfo();
	void slotSetVisualPlugin( const QString& );
	void slotVideoSettings();
	void slotConfigDialog();
	void slotEngineError();

	void slotReadBus();

private:
	bool initGStreamer();
	bool createPlaybin();
	void deletePlaybin();
	void loadConfig();
	void saveConfig();
	void gstPlay( const QString& trackUrl, const QString& subtitleUrl );
	void initActions();
	void showError();
	void gstStateChanged();
	void foundTag( GstTagList *taglist );
	void processMetaInfo();
	void setAudioSink( QString sinkName );
	void setDevice( QString device );

private:
	GstElement *m_play;
	GstElement *m_videosink;
	GstElement *m_audiosink;
	GstElement *m_visual;
	GstBus *bus;
	GstState m_status;

	QTimer busTimer;

	VideoWindow* m_video;
	Timer* m_timer;
	VideoSettings* m_videoSettings;
	GStreamerConfig* m_gstConfig;
	QSlider* m_volume;
	KSelectAction* m_audioVisual;

	MRL m_mrl;
	QValueList<MRL> m_playlist;
	uint m_current;
	bool m_mute;
	QString m_logoPath;

	QString m_errorMsg;
	QString m_errorDetails;

	QString m_url;
	QString m_title;
	QString m_artist;
	QString m_album;
	QString m_track;
	QString m_year;
	QString m_genre;
	QString m_comment;
	QString m_audioCodec;
	QString m_videoCodec;
	QString errorMessage;
	QString errorDetails;

	QString m_audioSinkName;
	QString m_videoSinkName;
	QString m_visualPluginName;
	QStringList m_audioVisualPluginList;
	QStringList m_audioPluginList;
	QStringList m_videoPluginList;
	QString m_device, currentDevice;
	int m_savedVolume;
	int muteVolume;

	KToolBar* m_posToolbar;
};

#endif /* GSTREAMER_PART_H */
