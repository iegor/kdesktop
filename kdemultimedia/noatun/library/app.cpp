#include "cmodule.h"
#include "downloader.h"
#include "effects.h"
#include "effectview.h"
#include "engine.h"
#include "equalizer.h"
#include "equalizerview.h"
#include "vequalizer.h"
#include "app.h"
#include "playlist.h"
#include "pref.h"
#include "player.h"
#include "plugin.h"
#include "pluginloader.h"
#include "globalvideo.h"
#include "stdaction.h"

#include <common.h>
#include <dcopobject.h>
#include <dispatcher.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmessagebox.h>
#include <knotifyclient.h>
#include <kpopupmenu.h>
#include <qfile.h>
#include <qimage.h>
#include <qiomanager.h>
#include <qsessionmanager.h>
#include <qtextstream.h>
#include <signal.h>
#include <kmimetype.h>


using std::string;
using std::vector;

// forgot the d pointer in Player TODO
static GlobalVideo *globalVideo;
struct NoatunApp::Private
{
	Effects *effects;
	VEqualizer *vequalizer;
};


NoatunApp::NoatunApp()
	: KUniqueApplication(true, true, true), mPluginMenu(0), mPluginActionMenu(0), mEqualizer(0)
{
	d = new Private;
	d->vequalizer=0;
	d->effects=0;

	Visualization::internalVis=true;

	mDownloader=new Downloader;

	Visualization::initDispatcher(); // 316

	showingInterfaces = true;

	// set the default config data
	// TODO: Maybe a first time wizard instead?
	KConfig *config=KGlobal::config(); // +
	config->setGroup(QString::null); // 1
	if (!config->readEntry("Modules").length())
	{
		QStringList modules;
		modules.append("excellent.plugin");
		modules.append("splitplaylist.plugin");
		modules.append("marquis.plugin");
		modules.append("systray.plugin");
		modules.append("metatag.plugin");
		config->writeEntry("Modules", modules);
		config->sync();
	} // 1

	mPref=new NoatunPreferences(0L); // 115
	mPref->hide(); // 1
	mLibraryLoader = new LibraryLoader; // 0

	mLibraryLoader->add("dcopiface.plugin");

	new General(this); // 25
	new Plugins(this); // 149
//	new Types(this);

	mPlayer=new Player; // 139
	d->effects=new Effects; // 1
	d->vequalizer = new VEqualizer;
	d->vequalizer->init();
	mEqualizer=new Equalizer; // 0
	mEqualizer->init();  // 41
	mEffectView=new EffectView; // 859
	mEqualizerView=new EqualizerView; // 24

	QTimer::singleShot(0, mDownloader, SLOT(start()));

	::globalVideo = new GlobalVideo;

	if(isRestored())
	{
		mLibraryLoader->add("marquis.plugin");
		static_cast<SessionManagement*>(mLibraryLoader->plugins().first())->restore();
	}
	else
	{
		loadPlugins(); //  1531
		if (!playlist())
		{
			KMessageBox::error(0,i18n("No playlist plugin was found. " \
				"Please make sure that Noatun was installed correctly."));
			KApplication::quit();
			delete this;
		}
		else
		{
			config->setGroup(QString::null); // 0
			player()->setVolume(config->readNumEntry("Volume", 100)); // 10
			player()->loop(config->readNumEntry("LoopStyle", (int)Player::None));
			mPlayer->engine()->setInitialized(); // 0

			switch (startupPlayMode())
			{
			case Restore:
				restoreEngineState();
				break;
			case Play:
				player()->play();
				break;
			case DontPlay:
			default:
				break;
			}
		}
	}
}

NoatunApp::~NoatunApp()
{
	saveEngineState();
	KConfig *config = KGlobal::config();
	config->setGroup(QString::null);
	config->writeEntry("Volume", player()->volume());
	config->writeEntry("LoopStyle", player()->loopStyle());
	// for version continuity in the future
	config->writeEntry("Version", QString(version())); // 1
	config->sync(); // 40

	mPlayer->stop();
	delete ::globalVideo;
	delete mLibraryLoader;
	delete mEffectView;
	delete mDownloader;
	delete mEqualizer;
	delete d->vequalizer;
	delete mEqualizerView;
	delete d->effects;

	delete mPlayer;
	delete mPref;
	config->sync();

	delete d;
}

QCString NoatunApp::version() const
{
	return aboutData()->version().ascii();
}

inline bool logicalXOR(bool A, bool B)
{
	return (A || B) && !(A && B);
}

#define READBOOLOPT_EX(name, string, def, group, reversal) \
bool NoatunApp::name() const \
{ \
	KConfig *config=KGlobal::config(); \
	config->setGroup(group); \
	return logicalXOR(config->readBoolEntry(string, def), reversal); \
}

#define READBOOLOPT(name, string, def) \
	READBOOLOPT_EX(name, string, def, "", false)


READBOOLOPT(autoPlay, "AutoPlay", false)
READBOOLOPT(loopList, "LoopList", true)
READBOOLOPT(hackUpPlaylist, "HackUpPlaylist", true)
READBOOLOPT(fastMixer, "FastMixer", false)
READBOOLOPT(displayRemaining, "DisplayRemaining", false)
READBOOLOPT(clearOnOpen, "ClearOnOpen", false)
READBOOLOPT_EX(oneInstance, "MultipleInstances", true, "KDE", true)

#undef READBOOLOPT
#undef READBOOLOPT_EX

bool NoatunApp::clearOnStart() const
{
	return clearOnOpen();
}

int NoatunApp::startupPlayMode() const
{
	KConfig *config=KGlobal::config();
	config->setGroup(QString::null);
	return config->readNumEntry("StartupPlayMode", autoPlay() ? Play : Restore);
}

void NoatunApp::setStartupPlayMode(int mode)
{
	KConfig *config=KGlobal::config();
	config->setGroup(QString::null);
	config->writeEntry("StartupPlayMode", mode);
	config->sync();
}

void NoatunApp::setHackUpPlaylist(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup(QString::null);
	config->writeEntry("HackUpPlaylist", b);
	config->sync();
}

void NoatunApp::setFastMixer(bool b)
{
	bool whatBefore=fastMixer();
	if (whatBefore!=b)
	{
		KConfig *config=KGlobal::config();
		config->setGroup(QString::null);
		config->writeEntry("FastMixer", b);
		config->sync();
		player()->engine()->useHardwareMixer(b);
	}
}

void NoatunApp::setOneInstance(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup("KDE");
	config->writeEntry("MultipleInstances", !b);
	config->sync();
}

void NoatunApp::setLoopList(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup(QString::null);
	config->writeEntry("LoopList", b);
	KGlobal::config()->sync();
}

void NoatunApp::setAutoPlay(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup(QString::null);
	config->writeEntry("AutoPlay", b);
	KGlobal::config()->sync();
}

void NoatunApp::setRememberPositions(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup(QString::null);
	config->writeEntry("RememberPositions", b);
	KGlobal::config()->sync();
}

void NoatunApp::setSaveDirectory(const QString &s)
{
	KConfig *config=KGlobal::config();
	config->setGroup(QString::null);
	config->writePathEntry("SaveDirectory", s);
	config->sync();
}

QString NoatunApp::saveDirectory() const
{
	KConfig *c=KGlobal::config();
	c->setGroup(QString::null);
	return c->readPathEntry("SaveDirectory", QString(getenv("HOME")));
}

QString NoatunApp::titleFormat() const
{
	KConfig *c=KGlobal::config();
	c->setGroup(QString::null);
	return c->readEntry("TitleFormat", "$(\"[\"author\"] - \")$(title)$(\" (\"bitrate\"kbps)\")");
}

void NoatunApp::setTitleFormat(const QString &format)
{
	KConfig *c=KGlobal::config();
	c->setGroup(QString::null);
	return c->writeEntry("TitleFormat", format);
}

void NoatunApp::setClearOnStart(bool b)
{
	setClearOnOpen(b);
}

void NoatunApp::setClearOnOpen(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup(QString::null);
	config->writeEntry("ClearOnOpen", b);
	config->sync();
}

void NoatunApp::setDisplayRemaining(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup(QString::null);
	config->writeEntry("DisplayRemaining", b);
	config->sync();
}

void NoatunApp::toggleInterfaces()
{
	showingInterfaces ^= true;

	if(showingInterfaces)
		emit showYourself();
	else
		emit hideYourself();
}

void NoatunApp::showInterfaces()
{
	showingInterfaces = true;
	emit showYourself();
}

void NoatunApp::hideInterfaces()
{
	showingInterfaces = false;
	emit hideYourself();
}


int NoatunApp::pluginMenuAdd(const QString &text, const QObject *receiver, const char *member)
{
	return pluginActionMenu()->menuAdd(text, receiver, member);
}

void NoatunApp::pluginMenuRemove(int id)
{
	pluginActionMenu()->menuRemove(id);
}

NoatunStdAction::PluginActionMenu *NoatunApp::pluginActionMenu()
{
	if (!mPluginActionMenu)
		mPluginActionMenu = new NoatunStdAction::PluginActionMenu(this, "menu_actions");
	return mPluginActionMenu;
}

KPopupMenu *NoatunApp::pluginMenu()
{
	if(!mPluginMenu)
		mPluginMenu = pluginActionMenu()->popupMenu();
	return mPluginMenu;
}

int NoatunApp::newInstance()
{
	// TODO, this should call playlist()->handleArguments();
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	bool clear = clearOnOpen();
	bool playme=true;

	for (int i=0; i<args->count(); i++)
	{
		player()->openFile(args->url(i), clear, playme);
		playme=false;
		clear=false;
	}

	args->clear();
	return 0;
}

void NoatunApp::preferences()
{
	mPref->show();
	mPref->raise();
}

void NoatunApp::quit()
{
	KApplication::quit();
}

void NoatunApp::fileOpen()
{
	KURL::List files = KFileDialog::getOpenURLs(":mediadir", mimeTypes(), 0,
		i18n("Select File to Play"));

	if (files.count())
		mPlayer->openFile(files, clearOnOpen(), true);
}

void NoatunApp::effectView()
{
	mEffectView->show();
}

void NoatunApp::equalizerView()
{
	mEqualizerView->show();
}

VEqualizer * NoatunApp::vequalizer()
{
	return d->vequalizer;
}

Effects *NoatunApp::effects() const
{
	return d->effects;
}

QString NoatunApp::mimeTypes()
{
	QString mimeTypes;
	Arts::TraderQuery q;
	vector<Arts::TraderOffer> *results = q.query();
	vector<Arts::TraderOffer>::iterator i;

	for (i=results->begin(); i != results->end(); i++)
	{
		vector<string> *prop = (*i).getProperty("MimeType");

		vector<string>::iterator istr;
		for (istr = prop->begin(); istr != prop->end(); istr++)
		{
			if ( !(*istr).length())
				continue;

			const char *m = (*istr).c_str();
			if ((KServiceType::serviceType(m)) && !mimeTypes.contains(m))
			{
				mimeTypes += m;
				mimeTypes += ' ';
			}
		}
		delete prop;
	}
	delete results;
	return mimeTypes;
}

void NoatunApp::loadPlugins()
{
	mLibraryLoader->loadAll();
	// backup in case of trouble
	if(!mLibraryLoader->playlist())
	{
		kdDebug(66666) << k_funcinfo << "NO playlist plugin found!" << endl;
		//mLibraryLoader->add("splitplaylist.plugin");
	}
}

Playlist *NoatunApp::playlist() const
{
	return mLibraryLoader->playlist();
}

void NoatunApp::commitData(QSessionManager &)
{
}

void NoatunApp::saveState(QSessionManager &sm)
{
	QStringList restartCommand = sm.restartCommand();
	sm.setRestartCommand( restartCommand );

	KApplication::saveState(sm);
}

// Deprecated
QImage NoatunApp::readPNG(const QString &filename)
{
	QImageIO file(filename, "PNG");
	file.setGamma(0.0);
	file.read();
	return file.image();
}

void NoatunApp::restoreEngineState()
{
	KConfig* config = KGlobal::config();
	config->setGroup(QString::null);
	int state = config->readNumEntry("EngineState", Arts::posPlaying);
	switch (state)
	{
		case Arts::posPlaying:
			player()->play();
			break;
		case Arts::posPaused:
			if (player()->isPlaying())
				player()->playpause();
			break;
		case Arts::posIdle:
		default:
			break;
	}
}

void NoatunApp::saveEngineState()
{
	KConfig* config=KGlobal::config();
	config->setGroup(QString::null);
	config->writeEntry("EngineState", player()->engine()->state());
	// we don't sync here since it's done in the destructor afterwards anyway
}

#include "app.moc"
