/*****************************************************************

Copyright (c) 2000-2001 the noatun authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIAB\ILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#ifndef _ENGINE_H
#define _ENGINE_H

#include <qobject.h>
#include <kmedia2.h>
#include <kmediaplayer/player.h>
#include <kurl.h>

namespace Arts
{
class PlayObject;
class SoundServerV2;
}

namespace Kaboodle
{
/**
 * Handles all playing, connecting to aRts.
 * Does almost everything related to multimedia.
 * Most interfacing should be done with Player
 **/
class Engine : public QObject
{
Q_OBJECT

public:
	Engine(QObject *parent=0);
	~Engine();

	Arts::PlayObject playObject() const;

public slots:
	/**
	 * Load a file
	 **/
	bool load(const KURL &file);

	/**
	 * Pause while playing
	 **/
	void pause();
	
	/**
	 * Start
	 **/
	void play();
	
	/**
	 * stops, and unloads
	 **/
	void stop();

	/**
	 * skips to a time
	 **/
	void seek(unsigned long msec);

public:
	KMediaPlayer::Player::State state();
	long position(); // NOT unsigned
	unsigned long length();

	/**
	 * returns if the current track is seekable
	 */
	bool seekable(void);

private:
	bool reload(void);
	bool needReload;
	
	class EnginePrivate;
	EnginePrivate *d;
};

}
#endif
