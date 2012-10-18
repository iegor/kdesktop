// $Id: engine.cpp 356602 2004-10-21 19:59:59Z metz $

#include <noatun/engine.h>
#include <noatun/equalizer.h>
#include <noatun/player.h>
#include <noatun/plugin.h>
#include <noatun/effects.h>
#include "titleproxy.h"

#include <string.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kdebug.h>
#include <qfile.h>
#include <qdir.h>
#include <sys/wait.h>
#include <kplayobject.h>
#include <kplayobjectfactory.h>

#include <dynamicrequest.h>
#include <soundserver.h>
#include <kmedia2.h>
#include <flowsystem.h>
#include <noatunarts.h>
#include <connect.h>
#include <cpuinfo.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>


static Arts::PlayObject nullPO() { return Arts::PlayObject::null(); }

#define HARDWARE_VOLUME

#if defined(__osf__)
#undef HARDWARE_VOLUME
#elif defined(__linux__)
#include <sys/soundcard.h>
#elif defined(__FreeBSD__)
#include <sys/soundcard.h>
#elif defined(__NetBSD__)
#include <soundcard.h>
#elif defined(___SOMETHING_UNKNOWN__)
#include <sys/soundcard.h>
#elif defined(_UNIXWARE)
#include <sys/soundcard.h>
#else
#undef HARDWARE_VOLUME
#endif

using namespace std;

namespace VolumeControls
{
	struct VC
	{
		virtual ~VC() {}
		virtual void setVolume(int percent)=0;
		virtual int volume() const =0;
	};

#ifdef HARDWARE_VOLUME

	struct Hardware : public VC
	{
		Hardware(Engine *)
		{
			if ((fd=::open( "/dev/mixer", O_RDWR)) < 0)
			{
				return;
			}
			else
			{
#define ERROR { fd=-1; return; }
				int devmask, recmask, i_recsrc, stereodevs;
				// Mixer is open. Now define properties
				if (ioctl(fd, SOUND_MIXER_READ_DEVMASK, &devmask) == -1) ERROR
				if (ioctl(fd, SOUND_MIXER_READ_RECMASK, &recmask) == -1) ERROR
				if (ioctl(fd, SOUND_MIXER_READ_RECSRC, &i_recsrc) == -1) ERROR
				if (ioctl(fd, SOUND_MIXER_READ_STEREODEVS, &stereodevs) == -1) ERROR
				if (!devmask) ERROR
#undef ERROR
			}

		}

		virtual ~Hardware() { ::close(fd); }

		virtual void setVolume(int percent)
		{
			percent=percent+(percent<<8);
			ioctl(fd, MIXER_WRITE(4), &percent);
		}

		virtual int volume() const
		{
			int volume, left, right;
			left=100;
			if (ioctl(fd, MIXER_READ(4), &volume) != -1)
			{
				left=volume & 0x7f;
				right=(volume>>8) & 0x7f;
				left=(left+right)/2;
			}
			return left;
		}
	private:
		int fd;
	};
#endif

	struct SoftwareSSE : public VC
	{
		SoftwareSSE(Engine *e) : mVolume(100)
		{
			volumeControl = Arts::DynamicCast(e->server()->createObject("Noatun::StereoVolumeControlSSE"));

			if(volumeControl.isNull())
				volumeControl = Arts::DynamicCast(e->server()->createObject("Noatun::StereoVolumeControl"));

			volumeControl.start();

			id=e->globalEffectStack()->insertBottom(volumeControl,"Volume Control");
		}

		virtual void setVolume(int percent)
		{
			mVolume=percent;
			float vol = pow(2.0, ((400 - (100-percent)*12)/200.0))/4.0;
			if (percent == 0) vol = 0.0;
			if (!volumeControl.isNull())
				volumeControl.percent(vol);
		}

		virtual int volume() const
		{
			return mVolume;
		}

	private:
		Noatun::StereoVolumeControlSSE volumeControl;
		long id;
		int mVolume;
	};

	struct Software : public VC
	{
		Software(Engine *e) : mVolume(100)
		{
			volumeControl = Arts::DynamicCast(e->server()->createObject("Noatun::StereoVolumeControl"));
			volumeControl.start();

			id=e->globalEffectStack()->insertBottom(volumeControl,"Volume Control");
		}

		virtual void setVolume(int percent)
		{
			mVolume=percent;
			if (!volumeControl.isNull())
				volumeControl.percent((float)percent/100.0);
		}

		virtual int volume() const
		{
			return mVolume;
		}

	private:
		Noatun::StereoVolumeControl volumeControl;
		long id;
		int mVolume;
	};

	static VC *volumeControl(Engine *e)
	{
#ifdef HARDWARE_VOLUME
		if (napp->fastMixer())
			return new Hardware(e);
		else
		{
#endif
			if (!getenv("NO_SSE") && Arts::CpuInfo::flags()&Arts::CpuInfo::CpuSSE)
				return new SoftwareSSE(e);
			else
				return new Software(e);

#ifdef HARDWARE_VOLUME
		}
#endif
	}
}


class Engine::EnginePrivate
{
public:
	EnginePrivate()
		: playobj(0),
		  server(Arts::SoundServerV2::null()),
		  globalEffectStack(Noatun::StereoEffectStack::null()),
		  effectsStack(Noatun::StereoEffectStack::null()),
		  visStack(Noatun::StereoEffectStack::null()),
		  volumeControl(0), session(Noatun::Session::null()),
		  pProxy(0)
		{
		}

	~EnginePrivate()
	{
		visStack=Noatun::StereoEffectStack::null();
	}

	KDE::PlayObject *playobj;
	Arts::SoundServerV2 server;
	Arts::Synth_AMAN_PLAY amanPlay;

	// globalEffectStack
	//  |- effectsStack
	//      |- Effects...
	//  |- visStack
	//      |- Visualizations
	//  |- Volume Control
	//
	Noatun::StereoEffectStack globalEffectStack;
	Noatun::StereoEffectStack effectsStack;
	Noatun::StereoEffectStack visStack;
	Noatun::Equalizer equalizer;

	int volumeID;
	VolumeControls::VC *volumeControl;
	Noatun::Session session;
	TitleProxy::Proxy *pProxy;
};

Arts::SoundServerV2 *Engine::server() const               { return &d->server;}
Arts::PlayObject Engine::playObject() const                  { return d->playobj ? d->playobj->object() : nullPO(); }
Arts::SoundServerV2 *Engine::simpleSoundServer() const    { return &d->server; }
Noatun::StereoEffectStack *Engine::effectStack() const        { return &d->effectsStack; }
Noatun::StereoEffectStack *Engine::visualizationStack() const { return &d->visStack; }
Noatun::StereoEffectStack *Engine::globalEffectStack() const  { return &d->globalEffectStack; }
Noatun::Equalizer *Engine::equalizer() const { return &d->equalizer; }
Noatun::Session *Engine::session() const { return &d->session; }

Engine::Engine(QObject *parent) : QObject(parent, "Engine"), mPlay(false)
{
	d=new EnginePrivate;
	// Connect to aRts
	if (!initArts())
	{
		KMessageBox::error(0, i18n("There was an error communicating to the aRts daemon."), i18n("aRts error"));
		exit(0);
	}

}

Engine::~Engine()
{
	stop();
	delete d->volumeControl;
	d->server=Arts::SoundServerV2::null();
	delete d;
}

void Engine::setInitialized()
{
	mPlay=true;
}

bool Engine::initialized() const
{
	return mPlay;
}

bool Engine::open(const PlaylistItem &file)
{
	if(!initArts())
		return false;

	d->playobj = 0;

	KDE::PlayObjectFactory factory(d->server);

	if (file.isProperty("stream_") && file.url().protocol() == "http")
	{
		deleteProxy();
		d->pProxy = new TitleProxy::Proxy(KURL(file.property("stream_")));
		d->playobj = factory.createPlayObject(d->pProxy->proxyUrl(), false);

		connect(d->playobj, SIGNAL(destroyed()), this, SLOT(deleteProxy()));
		connect(
			d->pProxy, SIGNAL(
				metaData(
				const QString &, const QString &,
				const QString &, const QString &,
				const QString &, const QString &)),
			this, SIGNAL(
				receivedStreamMeta(const QString &, const QString &,
				const QString &, const QString &,
				const QString &, const QString &))
		);
		connect(d->pProxy, SIGNAL(proxyError()), this, SLOT(slotProxyError()));
	}
	else
	{
		d->playobj = factory.createPlayObject(file.url(), false);
	}

	if (!d->playobj || d->playobj->isNull())
	{
		kdDebug(66666) << k_funcinfo <<
			"No playobject for '" << file.url().prettyURL() << "'" << endl;
		delete d->playobj;
		d->playobj=0;
		emit playingFailed();
		return false;
	}

	if ( !d->playobj->object().isNull() )
	{
		connectPlayObject();
	}
	else
	{
		connect( d->playobj, SIGNAL( playObjectCreated() ), this, SLOT( connectPlayObject() ) );
	}

	if (mPlay)
		d->playobj->play();

	return true;
}

void Engine::slotProxyError()
{
	kdDebug(66666) << k_funcinfo << endl;
	emit playingFailed();
	deleteProxy();
}

void Engine::deleteProxy()
{
	delete d->pProxy;
	d->pProxy = 0;
}

void Engine::connectPlayObject()
{
	if (d->playobj->object().isNull())
	{
		emit playingFailed();
		return;
	}
	d->playobj->object()._node()->start();

	// TODO: check for existence of left & right streams
	Arts::connect(d->playobj->object(),"left",d->globalEffectStack,"inleft");
	Arts::connect(d->playobj->object(),"right",d->globalEffectStack,"inright");

	emit aboutToPlay();
}

bool Engine::play()
{
	if (!mPlay) return true;
	if(!d->playobj)
		return false;
	d->playobj->play();
	return true;
}

void Engine::pause()
{
	d->playobj->pause();
}

void Engine::stop()
{
	if(!d->playobj) return;

	d->playobj->halt();
	delete d->playobj;
	d->playobj=0;
}

void Engine::seek(int msec) // pass time in msecs
{
	if(!d->playobj) return;

	Arts::poTime t;

	t.custom = 0.0;
	t.ms = (long) msec % 1000;
	t.seconds = (long) (msec - t.ms) / 1000;

	if(d->playobj)
		d->playobj->seek(t);
}

int Engine::position()
{
	if(!d->playobj) return -1;

	Arts::poTime time(d->playobj->currentTime());
	return (int)(time.ms + (time.seconds*1000)); // return position in milliseconds
}

int Engine::length()
{
	if(!d->playobj) return -1;
	if (!(d->playobj->capabilities() & Arts::capSeek))
		return -1;

	Arts::poTime time(d->playobj->overallTime());
	return (int)(time.ms + (time.seconds*1000)); // return track-length in milliseconds
}

int Engine::state()
{
	if(d->playobj)
		return d->playobj->state();
	else
		return Arts::posIdle;
}

void Engine::setVolume(int percent)
{
	if (percent>100)
		percent=100;
	if (percent<0)
		percent=0;
	d->volumeControl->setVolume(percent);
}

int Engine::volume() const
{
	return d->volumeControl->volume();
}

void Engine::useHardwareMixer(bool)
{
	delete d->volumeControl;
	d->volumeControl=VolumeControls::volumeControl(this);
}

bool Engine::initArts()
{
	if ( d->server.isNull() || d->server.error()  )
	{
		d->server = Arts::Reference("global:Arts_SoundServerV2");
		int volume = d->volumeControl ? d->volumeControl->volume() : -1;
		delete d->volumeControl;
		d->volumeControl=0;

		if( d->server.isNull() || d->server.error() )
		{
			// aRts seems not to be running, let's try to run it
			// First, let's read the configuration as in kcmarts
			KConfig config("kcmartsrc");
			QCString cmdline;

			config.setGroup("Arts");

			bool rt = config.readBoolEntry("StartRealtime",false);
			bool x11Comm = config.readBoolEntry("X11GlobalComm",false);

			// put the value of x11Comm into .mcoprc
			{
				KConfig X11CommConfig(QDir::homeDirPath()+"/.mcoprc");

				if(x11Comm)
					X11CommConfig.writeEntry("GlobalComm", "Arts::X11GlobalComm");
				else
					X11CommConfig.writeEntry("GlobalComm", "Arts::TmpGlobalComm");

				X11CommConfig.sync();
			}

			cmdline = QFile::encodeName(KStandardDirs::findExe(QString::fromLatin1("kdeinit_wrapper")));
			cmdline += " ";

			if (rt)
				cmdline += QFile::encodeName(KStandardDirs::findExe(
				QString::fromLatin1("artswrapper")));
			else
				cmdline += QFile::encodeName(KStandardDirs::findExe(
				QString::fromLatin1("artsd")));

			cmdline += " ";
			cmdline += config.readEntry("Arguments","-F 10 -S 4096 -s 60 -m artsmessage -l 3 -f").utf8();

			int status=::system(cmdline);

			if ( status!=-1 && WIFEXITED(status) )
			{
				// We could have a race-condition here.
				// The correct way to do it is to make artsd fork-and-exit
				// after starting to listen to connections (and running artsd
				// directly instead of using kdeinit), but this is better
				// than nothing.
				int time = 0;
				do
				{
					// every time it fails, we should wait a little longer
					// between tries
					::sleep(1+time/2);
					d->server = Arts::Reference("global:Arts_SoundServerV2");
				} while(++time < 5 && (d->server.isNull()));
			}
		}

		if ( !d->server.isNull() )
		{
			d->amanPlay = Arts::DynamicCast(
					d->server.createObject("Arts::Synth_AMAN_PLAY")
				);

			if (d->amanPlay.isNull())
				goto crapOut;

			d->session = Arts::DynamicCast(
				d->server.createObject("Noatun::Session"));
			if (d->session.isNull())
			{
				kdWarning() << "Couldn't instanciate artsobject Noatun::Session. "
					<< "(This is normally caused by a broken package or "
					<< "compiling kdemultimedia in a --prefix different "
					<< "from arts.  It may also be from two conflicting "
					<< "packages, so uninstall every arts/artsd package "
					<< "you have installed and try again." << endl;
				goto crapOut;
			}

			d->amanPlay.title("noatun");
			d->amanPlay.autoRestoreID("noatun");
			d->amanPlay.start();

			d->globalEffectStack=Arts::DynamicCast(
			d->server.createObject("Noatun::StereoEffectStack"));;
			d->globalEffectStack.start();
			Arts::connect(d->globalEffectStack,d->amanPlay);

			d->effectsStack=Arts::DynamicCast(
				d->server.createObject("Noatun::StereoEffectStack"));
			d->effectsStack.start();
			d->globalEffectStack.insertBottom(d->effectsStack, "Effects Stack");

			d->equalizer=Arts::DynamicCast(d->server.createObject("Noatun::Equalizer"));
			d->equalizer.start();
			d->globalEffectStack.insertBottom(d->equalizer, "Equalizer");

			if (napp->equalizer())
			{
				napp->equalizer()->update(true);
				napp->equalizer()->setPreamp(napp->equalizer()->preamp());
				napp->equalizer()->setEnabled(napp->equalizer()->isEnabled());
			}

			d->visStack=Arts::DynamicCast(
				d->server.createObject("Noatun::StereoEffectStack"));
			d->visStack.start();

			d->globalEffectStack.insertBottom(d->visStack, "Visualization Stack");
			d->volumeControl=VolumeControls::volumeControl(this);
			if (volume != -1)
				d->volumeControl->setVolume(volume);
		}
		else
		{
		crapOut:
			KMessageBox::error( 0, i18n("Connecting/starting aRts soundserver failed. Make sure that artsd is configured properly."));
			exit(1);
		}
	}

	d->playobj=0;
	emit artsError();
	return true;
}


#include "engine.moc"

