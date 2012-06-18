/*
 * kaffeinepart.h
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

#ifndef KAFFEINEPART_H
#define KAFFEINEPART_H

#include <kparts/part.h>

#include "mrl.h"

/*
 * Base-Class for Kaffeine-Parts. Derive from this class if you want to develop a player part for Kaffeine.
 * At least you have to reimplement openURL(const MRL&) and to emit signalTrackFinished()!
 *
 * IMPORTANT: Forward the double-click, middle-click and wheel mouse-events to the parent widget (don't use
 * QMouseEvent::accept())!
 *
 * @author Jürgen Kofler
 */

class KDE_EXPORT KaffeinePart : public KParts::ReadOnlyPart
{
	Q_OBJECT
public:
	KaffeinePart(QObject* parent, const char* name);
	virtual ~KaffeinePart();

	/*
	 * Playback in progress?
	 */
	virtual bool isPlaying()
	{
		return false;
	}

	/*
	 * Playback paused?
	 */
	virtual bool isPaused()
	{
		return false;
	}

	/*
	 * Should look like "*.mp3 *.MP3 *.avi *.AVI", used as file filter for directory import
	 */
	virtual QString supportedExtensions()
	{
		return QString::null;
	}

	/*
	 * Current volume. In percent.
	 */
	virtual uint volume() const
	{
		return 0;
	}
	/*
	 * Current position. In percent.
	 */
	virtual uint position() const
	{
		return 0;
	}

	/*
	 * DVD etc.
	 */
	virtual bool hasChapters()
	{
		return false;
	}

	virtual void setDVDChapter(uint) {}

	/*
	 * Contain the stream video?
	 */
	virtual bool hasVideo()
	{
		return false;
	}

signals:
	/*
	 * Frame size of video changed
	 */
	void signalNewFrameSize(const QSize& frame);

	/*
	 * New meta information available
	 */
	void signalNewMeta(const MRL &mrl);

	/*
	 * Playback of current track finished
	 */
	void signalTrackFinished();

	/*
	 *  Playback failed
	 */
	void signalPlaybackFailed();

	/*
	 * User pressed play button but queue is empty
	 */
	void signalRequestCurrentTrack();

	/*
	 * User pressed next button but no track left in queue
	 */
	void signalRequestNextTrack();

	/*
	 * User pressed previous button but queue is empty
	 */
	void signalRequestPreviousTrack();

	void signalToggleMinimalMode();

public slots:
	/*
	 * You have to reimplement this! Opens an url and plays it.
	 */
	virtual bool openURL(const MRL &mrl) = 0;

	/*
	 * Tells the player we go fullscreen now. Hide controls etc.
	 */
	virtual void slotPrepareForFullscreen(bool)
	{}

	/*
	 * DVD etc.
	 */
	virtual void playNextChapter()
	{}

	/*
	 * DVD etc.
	 */
	virtual void playPreviousChapter()
	{}

	/*
	 * Toggle play/pause
	 */
	virtual void slotTogglePause(bool pauseLive = true)
	{
		pauseLive = true;
	}

	/*
	 * Stop playback
	 */
	virtual void slotStop()
	{}

	/*
	 * Sets Volume. In percent.
	 */
	virtual void slotSetVolume(uint)
	{}

	/*
	 * Sets Position. In percent.
	 */
	virtual void slotSetPosition(uint)
	{}

	virtual void slotPosPlusSmall()
	{}

	virtual void slotPosMinusSmall()
	{}

	/*
	 * Turn mute on/off
	 */
	virtual void slotMute()
	{}

private:
	/*
	 * Don't reimplement this, a player should be able to stream media
	 */
	bool openFile()
	{
		return false;
	}

	bool openURL(const KURL &url)
	{
		return openURL(MRL(url));
	}
};

#endif /* KAFFEINEPART_H */
