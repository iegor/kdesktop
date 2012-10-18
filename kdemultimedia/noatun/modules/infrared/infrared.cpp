
#include <stdio.h>

#include <unistd.h>
#include <noatun/player.h>
#include <noatun/app.h>

#include <klocale.h>
#include <qtimer.h>

#include "infrared.h"
#include "lirc.h"
#include "irprefs.h"

extern "C"
{
	KDE_EXPORT Plugin *create_plugin()
	{
		return new InfraRed();
	}
}


InfraRed::InfraRed()
	: QObject(),
	  Plugin()
{
	NOATUNPLUGINC(InfraRed);
	m_lirc = new Lirc(this);
	connect(m_lirc,
	        SIGNAL(commandReceived(const QString &, const QString &, int)),
	        SLOT(slotCommand(const QString &, const QString &, int)));

	IRPrefs::s_lirc = m_lirc;
	volume=0;
	QTimer::singleShot(0, this, SLOT(start()));
}

InfraRed::~InfraRed()
{
}

void InfraRed::start()
{
	new IRPrefs(this);
}

void InfraRed::slotCommand(const QString &remote, const QString &button, int repeat)
{
	switch (IRPrefs::actionFor(remote, button, repeat))
	{
	case IRPrefs::None:
		break;
	
	case IRPrefs::Play:
		napp->player()->play();
		break;
	
	case IRPrefs::Stop:
		napp->player()->stop();
		break;
	
	case IRPrefs::Pause:
		napp->player()->playpause();
		break;
	
	case IRPrefs::Mute:
		if (napp->player()->volume())
		{
			volume=napp->player()->volume();
			napp->player()->setVolume(0);
		}
		else
		{
			napp->player()->setVolume(volume);
		}
		break;
	
	case IRPrefs::Previous:
		napp->player()->back();
		break;
	
	case IRPrefs::Next:
		napp->player()->forward();
		break;
	
	case IRPrefs::VolumeDown:
		napp->player()->setVolume(napp->player()->volume() - 4);
		break;
	
	case IRPrefs::VolumeUp:
		napp->player()->setVolume(napp->player()->volume() + 4);
		break;

	case IRPrefs::SeekBackward:  // - 3 seconds
		napp->player()->skipTo( QMAX(0, napp->player()->getTime() - 3000) );
		break;

	case IRPrefs::SeekForward:  // + 3 seconds
		napp->player()->skipTo( QMIN(napp->player()->getLength(),
		                             napp->player()->getTime() + 3000) );
		break;
	case IRPrefs::ShowPlaylist:
		napp->player()->toggleListView();
		break;
	case IRPrefs::NextSection:
		// This and the next case theoretically shouldn't bypass player()
		// but I'm making this change as inobtrusive as possible. That
		// means restricting the change to infrared and not messing around
		// in libnoatun -- Neil
		napp->playlist()->nextSection();
		break;
	case IRPrefs::PreviousSection:
		napp->playlist()->previousSection();
		break;
	}
}

#include "infrared.moc"

