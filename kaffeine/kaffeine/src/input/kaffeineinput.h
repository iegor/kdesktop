/*
 * kaffeineinput.h
 *
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

#ifndef KAFFEINEINPUT_H
#define KAFFEINEINPUT_H

#include <qstring.h>
#include <qwidget.h>

#include <kxmlguiclient.h>

class MRL;

class KaffeineInput : public QObject, public KXMLGUIClient
{
	Q_OBJECT
public:
	KaffeineInput(QObject* parent, const char* name);
	virtual ~KaffeineInput();

	// if you want the player window to be displayed on one of your widgets, return a pointer to that one.
	virtual QWidget *wantPlayerWindow() {return NULL;}

	// you MUST return your main widget (the one containing all your gui).
	virtual QWidget *inputMainWidget() {return NULL;}

	// called when the user want next track and you are the current input.
	// fill the MRL and return true to play your next track.
	virtual bool nextTrack( MRL& ) {return false;}

	// called when the user want previous track and you are the current input.
	// fill the MRL and return true to play your previous track.
	virtual bool previousTrack( MRL& ) {return false;}

	// called when the user starts playing and you are the current input.
	// fill the MRL and return true to play your current track.
	virtual bool currentTrack( MRL& ) {return false;}

	// called when the user enters a track number and you are the current input.
	// fill the MRL and return true to play that track.
	virtual bool trackNumber( int, MRL& ) {return false;}

	// called when the player has reached the end of current track and you are the current input.
	// fill the MRL and return true to play your next track.
	virtual bool playbackFinished( MRL& ) {return false;}

	// called when the player is paused and you are the current input.
	virtual void playerPaused() {}

	// called when the player is stopped and you are the current input.
	virtual void playerStopped() {}

	// called when playing failed and you are the current input.
	virtual void playbackFailed() {}

	// called when the player finds metadata(tags) and you are the current input.
	virtual void mergeMeta(const MRL&) {}

	// shortcuts buttons to be displayed in the start window.
	virtual void getTargets( QStringList&/*uiNames*/, QStringList&/*iconNames*/, QStringList&/*targetNames*/ ) {}

	// play shortcut, return false if the target is not yours;
	virtual bool execTarget( const QString&/*target*/ ) {return false;}

	// toggle your panel (show / hide) if any
	virtual void togglePanel() {}

	// called when kaffeine is about to quit. Here, you could ask the user to confirm if you have pending
	// operations. Return accordingly.
	virtual bool close() {return true;}

	// called when kaffeine is about to quit. Save your settings now.
	virtual void saveConfig() {}

	// called when a video is to be played, return false to not automatically switch to player window.
	virtual bool showPlayer() {return true;}

signals:
	// emit that to play your mrl (you'll become the current input, if not yet).
	void play(const MRL&, KaffeineInput*/*this*/);

	// emit that if you want a message to be displayed in statusbar. (do that only if you are the current input, e.g. after play())
	void statusBarMessage(const QString&);

	// emit that if you want to stop the player. Be aware that it'll end with a call to playerStopped().
	// you usually don't need that.
	void stop();

	// emit that if you want to pause the player. Be aware that it'll end with a call to playerPaused().
	// you usually don't need that.
	void pause();

	// for special uses.
	void setCurrentPlugin( KaffeineInput* );
	void showMe( KaffeineInput* );
};

#endif /* KAFFEINEINPUT_H */
