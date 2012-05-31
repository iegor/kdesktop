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

extern "C"
{
#include <sys/wait.h>
}

#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <qtimer.h>
#include <qfile.h>
#include <qdir.h>

#include <connect.h>
#include <dynamicrequest.h>
#include <flowsystem.h>
#include <kartsdispatcher.h>
#include <kartsserver.h>
#include <kplayobjectfactory.h>
#include <soundserver.h>

#include "engine.h"
#include <string.h>

using namespace std;

class Kaboodle::Engine::EnginePrivate
{
public:
	EnginePrivate()
		: playobj(0)
		, dispatcher()
		, server()
	{
	}
	
	~EnginePrivate()
	{
		delete playobj;
	}

	KDE::PlayObject *playobj;
	KArtsDispatcher dispatcher;
	KArtsServer server;
	KURL file;
};

Kaboodle::Engine::Engine(QObject *parent)
	: QObject(parent)
	, d(new EnginePrivate)
{
}

Kaboodle::Engine::~Engine()
{
	stop();
	delete d;
}

bool Kaboodle::Engine::load(const KURL &file)
{
	if(file.path().length())
	{
		d->file = file;
		return reload();
	}
	else return false;
}

bool Kaboodle::Engine::reload(void)
{
	// Only You can prevent memory leaks
	delete d->playobj;
	d->playobj = 0;

	KDE::PlayObjectFactory factory(d->server.server());
	d->playobj = factory.createPlayObject(d->file, true);
	
	needReload = false;

	return !d->playobj->isNull();
}

void Kaboodle::Engine::play()
{
	if(d->playobj)
	{
		switch(d->playobj->state())
		{
		case Arts::posIdle:
			if(needReload)
				reload();
			d->playobj->play();
			break;
		case Arts::posPaused:
			d->playobj->play();
			break;
		default:
			break;
		}
	}
}

void Kaboodle::Engine::pause()
{
	if(d->playobj && !d->playobj->isNull())
		d->playobj->pause();
}

void Kaboodle::Engine::stop()
{
	if(d->playobj && !d->playobj->isNull())
	{
		d->playobj->halt();
		needReload = true;
	}
}

// pass time in msecs
void Kaboodle::Engine::seek(unsigned long msec)
{
	Arts::poTime t;

	t.ms = (long) msec % 1000;
	t.seconds = (long) ((long)msec - t.ms) / 1000;

	if(d->playobj && !d->playobj->isNull())
		d->playobj->seek(t);
}

// return position in milliseconds
long Kaboodle::Engine::position()
{
	if(!d->playobj || d->playobj->isNull()) return 0;

	Arts::poTime time(d->playobj->currentTime());
	return (time.ms + (time.seconds*1000));
}

// return track-length in milliseconds
unsigned long Kaboodle::Engine::length()
{
	if(!d->playobj || d->playobj->isNull()) return 0;

	Arts::poTime time(d->playobj->overallTime());
	return (time.ms + (time.seconds*1000));
}

KMediaPlayer::Player::State Kaboodle::Engine::state()
{
	if(!d->playobj || d->playobj->isNull()) return KMediaPlayer::Player::Empty;

	switch(d->playobj->state())
	{
	case Arts::posIdle:
		return KMediaPlayer::Player::Stop;
		break;
	case Arts::posPlaying:
		return KMediaPlayer::Player::Play;
		break;
	case Arts::posPaused:
		return KMediaPlayer::Player::Pause;
		break;
	default:
		return KMediaPlayer::Player::Stop;
		break;
	}
}

bool Kaboodle::Engine::seekable(void)
{
	if(!d->playobj || d->playobj->isNull()) return false;
	return d->playobj->capabilities() & Arts::capSeek;
}

Arts::PlayObject Kaboodle::Engine::playObject() const
{
	return d->playobj ? d->playobj->object() : Arts::PlayObject::null();
}

#include "engine.moc"
