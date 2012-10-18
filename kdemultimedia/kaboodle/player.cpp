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

#include <kaction.h>
#include <kdebug.h>
#include <klocale.h>
#include <kurl.h>

#include "engine.h"
#include "kaboodleapp.h"
#include "kaboodle_factory.h"
#include "player.h"
#include "view.h"

Kaboodle::Player::Player(QObject *parent, const char *name)
	: KMediaPlayer::Player(parent, name)
	, engine(new Engine(this))
	, widget(0)
	, uncompleted(true)
	, embedded(false)
{
	setInstance(KaboodleFactory::instance());

	connect(&ticker, SIGNAL(timeout()), SLOT(tickerTimeout()));
	ticker.start(500);
	setState(Empty);
}

Kaboodle::Player::Player(QWidget *widgetParent, const char *widgetName,
                         QObject *parent, const char *name)
	: KMediaPlayer::Player(widgetParent, widgetName, parent, name)
	, engine(new Engine(this))
	, widget(new View(widgetParent, widgetName, this))
	, uncompleted(true)
	, embedded(false)
{
	setInstance(KaboodleFactory::instance());

	connect(&ticker, SIGNAL(timeout()), SLOT(tickerTimeout()));
	ticker.start(500);
	setState(Empty);

	playAction = new KAction(i18n("&Play"), 0, this, SLOT(play()), actionCollection(), "play");
	pauseAction = new KAction(i18n("&Pause"), 0, this, SLOT(pause()), actionCollection(), "pause");
	stopAction = new KAction(i18n("&Stop"), 0, this, SLOT(stop()), actionCollection(), "stop");
	loopAction = new KToggleAction(i18n("&Looping"), 0, this, SLOT(loop()), actionCollection(), "loop");
	stopAction->setEnabled(false);
	playAction->setEnabled(false);
	pauseAction->setEnabled(false);
	connect(this, SIGNAL(loopingChanged(bool)), loopAction, SLOT(setChecked(bool)));

	KParts::Part::setWidget(widget);
	setXMLFile("kaboodlepartui.rc");

	extension = new BrowserExtension(this);
	extension->setURLDropHandlingEnabled(true);
}

Kaboodle::Player::~Player()
{
}

KMediaPlayer::View *Kaboodle::Player::view(void)
{
	return widget;
}

// notice how this is just an expanded stop() ? weird.
bool Kaboodle::Player::openURL(const KURL &f)
{
	if(!current.isEmpty())
	{
		uncompleted = false;
		engine->stop();
	}

	emit started(0);
	current = f;
	if(!engine->load(current))
	{
		current = KURL();
		setState(Empty);
		emit canceled(i18n("aRts could not load this file."));
		return false;
	}

	stopAction->setEnabled(false);
	playAction->setEnabled(true);
	pauseAction->setEnabled(false);
	setState(Empty); // so stateChanged() is emitted and autoPlay works
	setState(Stop);
        
	tickerTimeout();
	return true;
}

KURL Kaboodle::Player::currentURL(void)
{
	return current;
}

bool Kaboodle::Player::openFile(void)
{
	return true;
}

QString Kaboodle::Player::timeString(unsigned long time)
{
	int posSecs = (int)(time / 1000);
	int posSeconds = posSecs % 60;
	int posMinutes = (posSecs - posSeconds) / 60;

	QString result;
	result.sprintf("%.2d:%.2d", posMinutes, posSeconds);
	return result;
}

QString Kaboodle::Player::positionString(void)
{
	return timeString(engine->position());
}

QString Kaboodle::Player::lengthString(void)
{
	return timeString(engine->length());
}

void Kaboodle::Player::pause()
{
	if(engine->state() == Play)
	{
		stopAction->setEnabled(true);
		playAction->setEnabled(true);
		pauseAction->setEnabled(false);
		engine->pause();
		setState(Pause);
	}
}

void Kaboodle::Player::play()
{
	stopAction->setEnabled(true);
	playAction->setEnabled(false);
	pauseAction->setEnabled(true);
	engine->play();
	setState(Play);
	uncompleted = true;
}

void Kaboodle::Player::stop(void)
{
	engine->stop();
	uncompleted = false;
	stopAction->setEnabled(false);
	playAction->setEnabled(true);
	pauseAction->setEnabled(false);
	setState(Stop);
}

void Kaboodle::Player::loop(void)
{
	setLooping(!isLooping());
}

void Kaboodle::Player::seek(unsigned long msec)
{
	if(!current.isEmpty())
		engine->seek(msec);
}

bool Kaboodle::Player::isSeekable(void) const
{
	return engine->seekable();
}

unsigned long Kaboodle::Player::position(void) const
{
	return engine->position();
}

bool Kaboodle::Player::hasLength(void) const
{
	// TODO: replace this weird assumption with something nice in aRts
	return engine->seekable();
}

unsigned long Kaboodle::Player::length(void) const
{
	return engine->length();
}

void Kaboodle::Player::tickerTimeout(void)
{
	if(engine->state() == Stop)
	{
		if ( uncompleted )
		{
			stop();
			if( isLooping() )
			{
				play();
			}
			else
			{
				uncompleted = false;
				emit completed();
			}
		}
		if(embedded)
		{
			widget->embed(Arts::PlayObject::null());
			embedded = false;
		}
	}
	else if(engine->state() != Stop && engine->state() != Empty)
	{
		if(!embedded)
		{
			widget->embed(engine->playObject());
			embedded = true;
		}
		
		emit timeout();

		if(extension)
			emit setStatusBarText(i18n("Playing %1 - %2")
			                      .arg(current.prettyURL())
			                      .arg(positionString() + "/" + lengthString()));

	}
	updateTitle();
}

void Kaboodle::Player::updateTitle()
{
	if(!current.isEmpty() && (lastEmitted != current))
	{
		lastEmitted = current;
		emit setWindowCaption(current.prettyURL());
	}
}

Kaboodle::BrowserExtension::BrowserExtension(Player *parent)
	: KParts::BrowserExtension(parent, "Kaboodle Browser Extension")
{
}

#include "player.moc"
