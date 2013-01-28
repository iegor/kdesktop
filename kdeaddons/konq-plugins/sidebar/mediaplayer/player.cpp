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

#include <kdebug.h>
#include <klocale.h>
#include <kurl.h>
#include <kmediaplayer/player.h>

#include "engine.h"
#include "player.h"

Player::Player(QObject *parent)
	: QObject(parent)
	, position(0)
	, unfinished(false)
{
	mEngine = new Engine;
	mLooping = false;
	connect(&ticker, SIGNAL(timeout()), SLOT(tickerTimeout()));
	ticker.start(500);
	stop();
}

Player::~Player()
{
	delete mEngine;
}

bool Player::isPlaying()const
{
	return mEngine->state() == KMediaPlayer::Player::Play;
}

bool Player::isPaused()const
{
	return mEngine->state() == KMediaPlayer::Player::Pause;
}

bool Player::isStopped()const
{
	return mEngine->state() == KMediaPlayer::Player::Stop || mEngine->state() == KMediaPlayer::Player::Empty;
}

void Player::handleButtons()
{
	switch(mEngine->state())
	{
	case KMediaPlayer::Player::Play:
		emit playing();
		break;
	case KMediaPlayer::Player::Pause:
		emit paused();
		break;
	case KMediaPlayer::Player::Stop:
		emit stopped();
		break;
	case KMediaPlayer::Player::Empty:
		emit empty();
		break;
	}
}

void Player::stop(void)
{
	unfinished = false;
	position = 0;
	mEngine->stop();
}

void Player::play()
{
	mEngine->play();
}

void Player::pause()
{
	if(mEngine->state() == KMediaPlayer::Player::Play)
	{
		mEngine->pause();
	}
}

// skip to a certain time in the track
void Player::skipTo(unsigned long msec)
{
	if(!mCurrent.isEmpty())
		mEngine->seek(msec);
}

void Player::tickerTimeout()
{
	position = mEngine->position();
	mLength = mEngine->length();

	handleButtons();

	if(mEngine->state() == KMediaPlayer::Player::Stop && unfinished)
	{
		if(mLooping)
		{
			play();
		}
		else
		{
			stop();
			emit finished();
		}
	}
	else if(mEngine->state() != KMediaPlayer::Player::Stop)
	{
		emit timeout();
		unfinished = true;
	}

}

QString Player::lengthString(long _position)
{
	if(_position == -1) _position = position;

	int posSecs = (int)(_position / 1000);
	int posSeconds = posSecs % 60;
	int posMinutes = (posSecs - posSeconds) / 60;

	int totSecs = (int)(mLength / 1000);
	int totSeconds = totSecs % 60;
	int totMinutes = (totSecs - totSeconds) / 60;

	QString result;
	result.sprintf("%.2d:%.2d/%.2d:%.2d", posMinutes, posSeconds, totMinutes, totSeconds);
	return result;
}

bool Player::openFile(const KURL &f)
{
	stop();
	mCurrent = f;

	bool work = mEngine->load(mCurrent);

	if(!work)
	{
		mCurrent = KURL();
		return false;
	}
	else
	{
		emit opened(mCurrent);
		tickerTimeout();
		return true;
	}
}

void Player::setLooping(bool b)
{
	mLooping = b;
	emit loopingChange(mLooping);
}

#include "player.moc"
