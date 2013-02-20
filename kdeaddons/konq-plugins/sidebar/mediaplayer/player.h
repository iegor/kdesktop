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

#ifndef PLAYER_H
#define PLAYER_H

#include <qobject.h>
#include <qtimer.h>
#include <kurl.h>

class Engine;

/**
 * This class has slots for all the common media player buttons
 **/
class Player : public QObject
{
Q_OBJECT

friend class KaboodlePart;
public:
	Player(QObject *parent = 0);
	~Player();

	/**
	 * returns a string with the time that can
	 * be used in the UI:
	 * CC:CC/LL:LL (mm:ss)
	 **/
	QString lengthString(long _position = -1);

	bool looping(void) const { return mLooping; }

	/**
	 * the position in milliseconds
	 **/
	long getTime() const { return position; }

	/**
	 * the track-length in milliseconds
	 **/
	unsigned long getLength() const { return mLength; }

	/**
	 * true if we're playing
	 **/
	bool isPlaying()const;

	/**
	 * true if paused
	 **/
	bool isPaused()const;

	/**
	 * true if stopped
	 **/
	bool isStopped()const;

	/**
	 * get the current opened URL
	 * this may be empty
	 **/
	KURL current() const { return mCurrent; }

	/**
	 * Change the file to have open
	 * @return true if file is playable, false otherwise
	 */
	bool openFile(const KURL &f);

public slots:
	/**
	 * force the playing/paused/stopped/playlist shown signals to
	 * be sent out
	 **/
	void handleButtons();

	/**
	 * stop playing
	 **/
	void stop(void);

	/**
	 * start playing
	 **/
	void play();

	/**
	 * pause playing
	 **/
	void pause();

	/**
	 * skip to the position
	 **/
	void skipTo(unsigned long msec);

	/**
	 * Set whether to restart playing when the playing is done
	 */
	void setLooping(bool);

signals:
	/**
	 * Tells you to update display information
	 **/
	void timeout();

	/**
	 * State is to Empty
	 */
	void empty();

	/**
	 * State is to Stopped
	 */
	void stopped();

	/**
	 * State is to Playing
	 */
	void playing();

	/**
	 * State is to Paused
	 */
	void paused();

	/**
	 * The file ended, and looping was off
	 */
	void finished();

	void loopingChange(bool);

	/**
	 * The current URL changed
	 */
	void opened(const KURL &);

private slots:
	void tickerTimeout();

protected:
	Engine *engine()const { return mEngine; }

private:
	Engine *mEngine;
	QTimer ticker;
	long position;
	bool mLooping;
	unsigned long mLength;
	KURL mCurrent;
	bool unfinished;
};

#endif

