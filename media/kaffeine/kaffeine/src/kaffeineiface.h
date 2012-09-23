/*
 * kaffeineiface.h
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

#ifndef KAFFEINEIFACE_H
#define KAFFEINEIFACE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <dcopobject.h>

class KaffeineIface : virtual public DCOPObject
{
	K_DCOP
k_dcop:
	/* Add url to playlist and play it */
	virtual void openURL(QString url) = 0;

	/* Append url to playlist */
	virtual void appendURL(QString url) = 0;

	/* Play next in playlist */
	virtual void play() = 0;

	/* Start playing audio CD */
	virtual void playAudioCD() = 0;

	/* Start playing video CD */
	virtual void playVCD() = 0;

	/* Start playing DVD */
	virtual void playDVD() = 0;

	/* Toggle pause / play */
	virtual void pause() = 0;

	/* Stop playback */
	virtual void stop() = 0;

	/* Play next in playlist */
	virtual void next() = 0;

	/* Play previous in playlist */
	virtual void previous() = 0;

	/* Change to next playlist */
	virtual void changePlaylist() = 0;

	/* Is currently playback in progress? */
	virtual bool isPlaying() = 0;

	/* Is a video being played? */
	virtual bool isVideo() = 0;

	/* Returns current track title */
	virtual QString title() = 0;

	/* Returns current artist */
	virtual QString artist() = 0;

	/* Returns current album */
	virtual QString album() = 0;

	/* Returns current track */
	virtual QString track() = 0;

	/* Returns current track file (media) name */
	virtual QString getFileName() = 0;

	/* Toggle random play */
	virtual void random() = 0;

	/* Toggle fullscreen / windowed mode */
	virtual void fullscreen() = 0;

	/* Switch to/from minimal mode */
	virtual void minimal() = 0;

	/* Returns current track length in seconds */
	virtual int getLength() = 0;

	/* Returns current track position in seconds */
	virtual int getTimePos() = 0;

	/* Increase stream position */
	virtual void posPlus() = 0;

	/* Decrease stream position */
	virtual void posMinus() = 0;

	/* Volume up */
	virtual void volUp() = 0;

	/* Volume down */
	virtual void volDown() = 0;

	/* Toggle mute */
	virtual void mute() = 0;

	/* Quit Kaffeine */
	virtual void quit() = 0;

	/* Jump to number */
	virtual void setNumber(int num) = 0;

	/* Create new DVB timer. datetime=2006-08-28T19:45:00, duration=01:55:00 */
	virtual void dvbNewTimer(QString name, QString channel, QString datetime, QString duration) = 0;

	virtual int dvbSNR(int device) = 0;

	virtual void dvbOSD() = 0;
	virtual void dvbOSDNextChannel() = 0;
	virtual void dvbOSDPreviousChannel() = 0;
	virtual void dvbOSDNextProgram() = 0;
	virtual void dvbOSDPreviousProgram() = 0;
	virtual void dvbOSDZap() = 0;

	virtual void playDvb() = 0;
};

#endif /* KAFFEINEIFACE_H */
