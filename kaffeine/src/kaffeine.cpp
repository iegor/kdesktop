/*
 * kaffeine.cpp
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 * Copyright (C) 2005-2007 Christophe Thommeret <hftom@free.fr>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "kaffeine.h"

#include <dcopref.h>
#include <kkeydialog.h>
#include <kfiledialog.h>
#include <kdirselectdialog.h>
#include <kconfig.h>
#include <kurl.h>
#include <kedittoolbar.h>
#include <kaction.h>
#include <kaccel.h>
#include <kstdaction.h>
#include <klibloader.h>
#include <kparts/componentfactory.h>
#include <kmessagebox.h>
#include <kstatusbar.h>
#include <kmenubar.h>
#include <kcombobox.h>
#include <kpopupmenu.h>
#include <ktabbar.h>
#include <kwin.h>
#include <ktrader.h>
#include <klocale.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kio/netaccess.h>
#include <kaboutdata.h>
#include <kiconloader.h>
#include <kinputdialog.h>
#include <kcmdlineargs.h>
#include <dcopclient.h>
#include <kapplication.h>
#include <kdeversion.h>

#include <qdir.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qinputdialog.h>
#include <kurldrag.h>
#include <qsignalmapper.h>
#include <qdockarea.h>

#include "kaffeine.moc"
#include "kaffeinepart.h"
#include "playlist.h"
#include "pref.h"
#include "startwindow.h"
#include "systemtray.h"
#include "instwizard.h"
#include "version.h"
#include "kaffeineinput.h"
#ifdef HAVE_DVB
#include "dvbpanel.h"
#endif
#include "cdwidget.h"
#include "inputmanager.h"
#include "kmultitabbar.h"
#include "disc.h"

#define DEFAULT_FILTER "*.anx *.axa *.axv *.vob *.png *.y4m *.rm *.ram *.rmvb *.pva *.nsv *.ogg *.ogm *.spx *.png *.mng *.pes *.iff *.svx *.8svx *.16sv *.ilbm *.pic *.anim *.wav *.vox *.voc *.snd *.au *.ra *.nsf *.flac *.aif *.aiff *.aud *.flv *.fli *.flc *.avi *.asf *.wmv *.wma *.asx *.wvx *.wax *.mkv *.vmd *.4xm *.mjpg *.cpk *.cak *.film *.str *.iki *.ik2 *.dps *.dat *.xa *.xa1 *.xa2 *.xas *.xap *.roq *.mve *.vqa *.mve *.mv8 *.cin *.wve *.mov *.qt *.mp4 *.ts *.m2t *.trp *.mpg *.mpeg *.dv *.dif *.flac *.mp3 *.mp2 *.mpa *.mpega *.ac3 *.aac *.asc *. *.sub *.srt *.smi *.ssa *.mpv *.m4a *.m4v *.mpc *.mp+ *.iso *.ANX *.AXA *.AXV *.VOB *.PNG *.Y4M *.RM *.RAM *.RMVB *.PVA *.NSV *.OGG *.OGM *.SPX *.PNG *.MNG *.PES *.IFF *.SVX *.8SVX *.16SV *.ILBM *.PIC *.ANIM *.WAV *.VOX *.VOC *.SND *.AU *.RA *.NSF *.FLAC *.AIF *.AIFF *.AUD *.FLV *.FLI *.FLC *.AVI *.ASF *.WMV *.WMA *.ASX *.WVX *.WAX *.MKV *.VMD *.4XM *.MJPG *.CPK *.CAK *.FILM *.STR *.IKI *.IK2 *.DPS *.DAT *.XA *.XA1 *.XA2 *.XAS *.XAP *.ROQ *.MVE *.VQA *.MVE *.MV8 *.CIN *.WVE *.MOV *.QT *.MP4 *.TS *.M2T *.TRP *.MPG *.MPEG *.DV *.DIF *.FLAC *.MP3 *.MP2 *.MPA *.MPEGA *.AC3 *.AAC *.ASC *. *.SUB *.SRT *.SMI *.SSA *.MPV *.M4A *.M4V *.MPC *.MP+ *.ISO"

#define DEFAULT_PLAYER_PART "xine_part"

#include <X11/Xlib.h>

#ifdef HAVE_DPMS
#include <X11/extensions/dpms.h>
#endif

#ifdef HAVE_XTEST
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#endif

const KCmdLineOptions cmdLineOptions[] = {
	{ "p", 0, 0 },
	{ "play", I18N_NOOP("Start playing immediately"), 0 },
	{ "f", 0, 0 },
	{ "fullscreen", I18N_NOOP("Start in fullscreen mode"), 0 },
	{ "m", 0, 0 },
	{ "minimal", I18N_NOOP("Start in minimal mode"), 0 },
	{ "a", 0, 0 },
	{ "audiodriver <argument>", I18N_NOOP("Set audio driver"), "default" },
	{ "x", 0, 0 },
	{ "videodriver <argument>", I18N_NOOP("Set video driver"), "default" },
	{ "d", 0, 0 },
	{ "device <argument>", I18N_NOOP("Set Audio-CD/VCD/DVD device path."), "default" },
	{ "verbose", I18N_NOOP("Output xine debug messages"), 0 },
	{ "w", 0, 0 },
	{ "wizard", I18N_NOOP("Run installation wizard"), 0 },
	{ "tempfile", I18N_NOOP("tempfile to delete after use"), 0 },
	{ "+[file]", I18N_NOOP("File(s) to play. Can be a local file, a URL, a directory or 'DVD', 'VCD', 'AudioCD', 'DVB'."), 0 },
	KCmdLineLastOption
};


Kaffeine::Kaffeine() : DCOPObject("KaffeineIface"),
		m_mediaPart(NULL), m_playerContainer(NULL),
		m_systemTray(NULL), m_videoSize(0,0), m_noResize(false), m_autoPaused(false)
{
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	if (args->getOption("audiodriver") != "default")
		m_engineParameters.append("audioDriver=\"" + args->getOption("audiodriver") + "\"");
	if (args->getOption("videodriver") != "default")
		m_engineParameters.append("videoDriver=\"" + args->getOption("videodriver") + "\"");
	if (args->isSet("verbose"))
		m_engineParameters.append("verbose=\"True\"");

	setAcceptDrops(true);

#ifdef HAVE_DVB
	// dvb
	if ( DVBconfig::haveDvbDevice() )
	{
		kdDebug() << "Found DVB device." << endl;
		dvbPanel = new DvbPanel( 0, this, "dvbpanel" );
	}
	else
	{
		kdDebug() << "No DVB device found." << endl;
		dvbPanel = 0;
	}
#endif

	setupActions();
	setStandardToolBarMenuEnabled(true);
	//createStandardStatusBarAction();
	createGUI("kaffeineui.rc");

	//statusBar()->insertItem(i18n("Entries: %1, Playtime: %2  (Total: %3, %4)").arg("0").arg("00:00:00").arg("0").arg("00:00:00"), 9, 0, true);
	//statusBar()->insertItem(i18n("Entries: %1, Playtime: %2").arg("0").arg("0:00:00"), 9, 0, true);
	//statusBar()->insertItem(i18n("No player"), 10, 0, true);

	QString stamp =  locateLocal("appdata", "wizard_stamp_v0.7.1");
	if ((!QFile::exists(stamp)) || args->isSet("wizard"))
	{
		InstWizard::showWizard();

		KProcess process;
		process << "touch" << stamp;
		process.start(KProcess::Block, KProcess::Stderr);
		process.clearArguments();
	}

	mainbox = new QHBox( this );
	mainbox->setMouseTracking( true );
	setCentralWidget(mainbox);
	KMultiTabBar *mtBar = new KMultiTabBar( KMultiTabBar::Vertical, mainbox );
	mtBar->setPosition( KMultiTabBar::Left );
	mtBar->setStyle( KMultiTabBar::VSNET );
	QWidgetStack *stack = new QWidgetStack( mainbox );
	stack->setMouseTracking( true );
	inplug = new InputManager( this, stack, mtBar );

	m_startWindow = new StartWindow( stack );
	inplug->addStartWindow( m_startWindow, KGlobal::iconLoader()->loadIcon("kmenu", KIcon::Small), i18n("Start") );

	playerWidget = new QVBox( stack );
	playerWidget->setMouseTracking( true );
	inplug->addPlayerWidget( playerWidget, KGlobal::iconLoader()->loadIcon("kaffeine", KIcon::Small), i18n("Player Window") );

	// playlist
	m_playlist = new PlayList( stack, this );
	inplug->add( m_playlist, KGlobal::iconLoader()->loadIcon("view_text", KIcon::Small), i18n( "Playlist") );
	m_playlist->setFileFilter(DEFAULT_FILTER);
	guiFactory()->addClient( m_playlist );

	cddisc = new Disc( stack, this );
	inplug->add( cddisc, KGlobal::iconLoader()->loadIcon("cdrom_unmount", KIcon::Small), i18n( "Audio CD") );
	guiFactory()->addClient( cddisc );

	connect(m_playlist, SIGNAL(signalRequestForDVD(const QString&)), cddisc, SLOT(startDVD(const QString&)));
	connect(m_playlist, SIGNAL(signalRequestForVCD(const QString&)), cddisc, SLOT(startVCD(const QString&)));
	connect(m_playlist, SIGNAL(signalRequestForAudioCD(const QString&)), cddisc, SLOT(startCD(const QString&)));

#ifdef HAVE_DVB
	if ( dvbPanel )
	{
		inplug->add( dvbPanel, KGlobal::iconLoader()->loadIcon("tv", KIcon::Small), i18n( "Digital TV") );
		guiFactory()->addClient( dvbPanel );
		dvbPanel->checkFirstRun();
	}
#endif

	QTextStream ts(&m_filter, IO_WriteOnly);
	ts << "|" << i18n("Supported Media Formats") << "\n"
	<< "*.mp3 *.mpa *mpega *.m4a *.mpc *.mp+ *.MP3 *.MPA *.MPEGA *.M4A *.MPC *.MP+|" <<  i18n("MPEG Audio Files") << "\n"
	<< "*.mjpeg *.mpg *.mpeg *.mp2 *.mpv *.vob *.MJPEG *.MPG *.MPEG *.MP2 *.MPV *.VOB|" << i18n("MPEG Video Files") << "\n"
	<< "*.ogg *.ogm *.OGG *.OGM|" << i18n("Ogg Vorbis Files") << "\n"
	<< "*.avi *.AVI|" << i18n("AVI Files") << "\n"
	<< "*.mov *.qt *.MOV *.QT|" << i18n("Quicktime Files") << "\n"
	<< "*.rm *.ram *.ra *.rmvb *.RM *.RAM *.RA *.RMVB|" << i18n("Real Media Files") << "\n"
	<< "*.mkv *.mka *.MKV *.MKA|" << i18n("Matroska Files") << "\n"
	<< "*.flac *.flc *.FLAC *.FLC|" << i18n("FLAC Files") << "\n"
	<< "*.wmv *.wma *.asf *.asx *.wvx *.wax *.WMV *.WMA *.ASF *.ASX *.WVX *.WAX|" << i18n("Windows Media Files") << "\n"
	<< "*.wav *.WAV|" << i18n("WAV Files") << "\n"
	<< "*.m3u *.M3U *.m4u *.M4U|" << i18n("M3U Playlists") << "\n"
	<< "*.pls *.PLS|" << i18n("PLS Playlists") << "\n"
	<< "*.kaffeine *.KAFFEINE|" << i18n("Kaffeine Playlists") << "\n"
	<< "*.iso *.ISO|" << i18n("DVD ISO IMAGE") << "\n"
	<< "*.*|" << i18n("All Files");

#ifdef HAVE_XTEST
	haveXTest = false;

	int dummy_event, dummy_error, dummy_major, dummy_minor;
	if (XTestQueryExtension(x11Display(), &dummy_event, &dummy_error, &dummy_major, &dummy_minor)) {
		fakeKeycode = XKeysymToKeycode(x11Display(), XK_Shift_L);
		if (fakeKeycode != 0)
			haveXTest = true;
	}
#endif

	/** KWin are you there? **/

	m_haveKWin = KApplication::dcopClient()->isApplicationRegistered("kwin");
	if (m_haveKWin)
		kdDebug() << "Window manager: KWin found" << endl;
	else
		kdDebug() << "Window manager: not KWin - using save fullscreen mode" << endl;

	connect(&m_screensaverTimer, SIGNAL(timeout()), this, SLOT(slotFakeKeyEvent()));
	m_screensaverTimer.start( 55000 );

	connect( &m_numKeyHideTimer, SIGNAL(timeout()), this, SLOT(slotRequestForTrackNumber()) );

	KAccel* accel = new KAccel(this);
	accel->insert("Escape Fullscreen", Key_Escape, this, SLOT(slotEscapeFullscreen()));

	loadConfig();
	//slotChangeStatusbar(i18n("Kaffeine Player") + " " + KAFFEINE_VERSION);

	updateArgs();
}

Kaffeine::~Kaffeine()
{
	kdDebug() << "Kaffeine: destructor" << endl;
	delete m_playlist;
}

void Kaffeine::updateArgs()
{
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

	QString device = args->getOption("device");
	if (device != "default") {
		if (device.startsWith("system:/media/"))
			device = device.right(14).prepend("media:/");
		KURL devicePath(device);
		DCOPRef mediamanager("kded", "mediamanager");
		DCOPReply reply = mediamanager.call("properties(QString)", devicePath.path(-1).right(1));
		if (reply.isValid()) {
			QStringList properties = reply;
			device = properties[5];
			kdDebug() << "DEVICE: " << device << "\n";
		} else
			device = QString();
	} else
		device = QString();
	m_device = device;

	QStringList urls;
	for (int i = 0; i < args->count(); i++ ) {
		QDir path;
		QString url = QFile::decodeName(args->arg(i));
		if ((url.left(1) != "/") && (!url.contains("://")) && (url.lower() != "dvd") && (url.lower() != "vcd")
			&& (url.lower() != "audiocd") && (url.lower() != "cdda") && (url.lower() != "dvb")) {
#if KDE_IS_VERSION(3,5,0)
			KURL u = KIO::NetAccess::mostLocalURL(args->url(i), 0);
			if (u.isLocalFile())
				url = path.absFilePath(u.path());
			else
				url = u.url();
#else
			url = path.absFilePath(url);
#endif
		}
		urls.append(KURL::fromPathOrURL(url).url());
	}
	loadTMP(urls);

	if (args->isSet("fullscreen")) {
		inplug->showPlayer();
		fullscreen();
	}

	if (args->isSet("minimal"))
		minimal();

	if (args->isSet("play") && urls.empty())
		slotPlaylistPlay();
	if (args->isSet("tempfile") )
		fprintf(stderr,"TEMPFILE OPTION PASSED: %s\n", urls[0].ascii() );

	show();
	KWin::activateWindow(winId());
}

void Kaffeine::unloadCurrentPart()
{
#ifdef HAVE_DVB
	if ( dvbPanel )
	{
		dvbPanel->stopLive();
		dvbPanel->enableLiveDvb( false );
	}
#endif
	if ( dvbClient )
	{
		dvbClient->stopLive();
		dvbClient->enableLive( false );
	}

	if (!m_currentPartService.isNull())
	{
		kdDebug() << "Kaffeine: Unloading player part: " << m_currentPartService << endl;
		if (m_mediaPart) {
			saveMainWindowSettings(kapp->config(), "Main Window");  // save toolbar state etc.
			guiFactory()->removeClient(m_mediaPart);
		}
		KService::Ptr service = KService::serviceByDesktopName(m_currentPartService);
		KLibLoader::self()->unloadLibrary(service->library().ascii());
		m_mediaPart = NULL;

		delete m_playerContainer;
		m_playerContainer = NULL;
		inplug->setPlayerContainer( m_playerContainer );
		//statusBar()->changeItem(i18n("No layer"), 10);
	}
}

void Kaffeine::slotLoadPart(const QString& desktopName)
{
	kdDebug() << "Kaffeine:: Try to load service: " << desktopName << endl;

	if (desktopName == m_currentPartService)
		return;

	KService::Ptr service = KService::serviceByDesktopName(desktopName);
	if (!service)
	{
		KMessageBox::detailedError(this, i18n("Loading of player part '%1' failed.").arg(desktopName), i18n("%1 not found in search path.").arg(QString(desktopName)+ ".desktop"));
		return;
	}

	//save state
	bool isPlaying = false;
	if (m_mediaPart)
		isPlaying = m_mediaPart->isPlaying();

	unloadCurrentPart();

	//player container widget, will contain the player part
	m_playerContainer = new PlayerContainer(playerWidget);
	connect(m_playerContainer, SIGNAL(signalURLDropEvent(const QStringList&)), this, SLOT(slotLoadURLS(const QStringList&)));

	if (service->serviceTypes().contains("KaffeinePart")) {
		kdDebug() << "This is a KaffeinePart..." << endl;
		int error = 0;
		m_mediaPart = KParts::ComponentFactory::createPartInstanceFromService<KaffeinePart>(service, m_playerContainer, service->name().ascii(), this, 0, m_engineParameters, &error);
		if (error > 0) {
			KMessageBox::detailedError(this, i18n("Loading of player part '%1' failed.").arg(service->name()), KLibLoader::self()->lastErrorMessage());
		}
		else {
			connect(m_mediaPart, SIGNAL(setWindowCaption(const QString&)), this, SLOT(slotChangeCaption(const QString&)));
			//connect(m_mediaPart, SIGNAL(setStatusBarText(const QString&)), this, SLOT(slotChangeStatusbar(const QString&)));
			connect(m_mediaPart, SIGNAL(canceled(const QString&)), this, SLOT(slotLoadingCanceled(const QString&)));
			connect(m_mediaPart, SIGNAL(signalPlaybackFailed()), this, SLOT(slotPlaybackFailed()));
			connect(m_mediaPart, SIGNAL(signalTrackFinished()), this, SLOT(slotNext()));
			connect(m_mediaPart, SIGNAL(signalNewMeta(const MRL &)), this, SLOT(slotMetaFromPlayer(const MRL &)));
			connect(m_mediaPart, SIGNAL(signalNewFrameSize(const QSize&)), this, SLOT(slotNewFrameSize(const QSize&)));
			connect(m_mediaPart, SIGNAL(signalRequestCurrentTrack()), this, SLOT(slotRequestForCurrentTrack()));
			connect(m_mediaPart, SIGNAL(signalRequestNextTrack()), this, SLOT(slotRequestForNextTrack()));
			connect(m_mediaPart, SIGNAL(signalRequestPreviousTrack()), this, SLOT(slotRequestForPreviousTrack()));
			connect(m_mediaPart, SIGNAL(signalToggleMinimalMode()), this, SLOT(slotToggleMinimalModeFromPlayer()));
			if ( service->library()=="libxinepart" )
				connect( this, SIGNAL(showOSD(const QString&,int,int)), m_mediaPart, SLOT(requestForOSD(const QString&,int,int)) );
#ifdef HAVE_DVB
			if ( service->library()=="libxinepart" && dvbPanel ) {

				connect( dvbPanel, SIGNAL(dvbPause(bool)), m_mediaPart, SLOT(slotTogglePause(bool)) );
				connect( dvbPanel, SIGNAL(dvbOpen(const QString&,const QString&,int)), m_mediaPart, SLOT(slotDvbOpen(const QString&,const QString&,int)) );
				connect( dvbPanel, SIGNAL(dvbStop()), m_mediaPart, SLOT(slotStop()) );
				connect( m_mediaPart, SIGNAL(stopDvb()), dvbPanel, SLOT(stopLive()) );
				connect( m_mediaPart, SIGNAL(playerPause()), dvbPanel, SLOT(pauseLiveTV()) );
				connect( dvbPanel, SIGNAL(setTimeShiftFilename(const QString&)), m_mediaPart, SLOT(getTimeShiftFilename(const QString&)) );
				connect( dvbPanel, SIGNAL(showOSD(const QString&,int,int)), m_mediaPart, SLOT(requestForOSD(const QString&,int,int)) );
				connect( dvbPanel, SIGNAL(showDvbOSD(const QString&, const QStringList&)), m_mediaPart, SLOT(setDvbCurrentNext(const QString&, const QStringList&)) );
				connect( m_mediaPart, SIGNAL(dvbOSDHide()), dvbPanel, SLOT(dvbOSDHide()));
				dvbPanel->enableLiveDvb( true );
			}
#endif
			if ( service->library()=="libxinepart" && dvbClient ) {
				connect( dvbClient, SIGNAL(dvbOpen(const QString&,const QString&,int)), m_mediaPart, SLOT(slotDvbOpen(const QString&,const QString&,int)) );
				connect( m_mediaPart, SIGNAL(stopDvb()), dvbClient, SLOT(stopLive()) );
				connect( dvbClient, SIGNAL(dvbStop()), m_mediaPart, SLOT(slotStop()) );
				connect( m_mediaPart, SIGNAL(playerPause()), dvbClient, SLOT(pauseLiveTV()) );
				connect( dvbClient, SIGNAL(setTimeShiftFilename(const QString&)), m_mediaPart, SLOT(getTimeShiftFilename(const QString&)) );
				dvbClient->enableLive( true );
			}
			m_currentPartService = desktopName;
			m_playlist->setFileFilter(DEFAULT_FILTER);
			//QString playerName = service->name();
			QString playerName = m_mediaPart->instance()->aboutData()->programName();
			if (playerName.length() > 20) {
				playerName.truncate(17);
				playerName = playerName + "...";
			}
			//statusBar()->changeItem(playerName, 10);
			//slotChangeStatusbar(m_mediaPart->instance()->aboutData()->shortDescription());
			guiFactory()->addClient(m_mediaPart);
			applyMainWindowSettings(kapp->config(), "Main Window"); //restore toolbar state
			stateChanged("no_media_part", StateReverse);
			if (isPlaying)
				slotPlaylistPlay();
		}
	}
	inplug->setPlayerContainer( m_playerContainer );
}

void Kaffeine::slotLoadingCanceled(const QString& message)
{
	QString name;
	if (m_mediaPart)
	{
		name = m_mediaPart->name();
		delete m_mediaPart;
		m_mediaPart = NULL;
	}
	KMessageBox::detailedError(this, i18n("Loading of player part '%1' failed.").arg(name), message);
}

void Kaffeine::slotPlaybackFailed()
{
	/*
	 * TODO: ask user for loading another part?
	 */
	//slotNext();
	m_mediaPart->closeURL();
}

void Kaffeine::load(const QString& url)
{
	load(QStringList(url));
}

void Kaffeine::load(const QStringList& urllist)
{
	if (!urllist.count())
		return;

	if (urllist[0].contains(".kaffeine", false))
	{
		m_playlist->loadPlaylist(urllist[0]);
	}
	else
	{
		m_playlist->add(urllist, NULL);
		QListViewItem* tmp = m_playlist->findByURL(urllist.first());
		if (tmp)
		{
			m_playlist->setCurrentEntry(tmp);
			slotPlaylistPlay();
		}
	}
}

void Kaffeine::slotLoadURLS(const QStringList& list)
{
	loadTMP(list);
}

void Kaffeine::slotPlay(const MRL& mrl)
{
	if (m_mediaPart)
	{
		m_mediaPart->openURL(mrl);
		if ( !mrl.mime().contains("audio") )
			QTimer::singleShot(300, this, SLOT(slotSwitchToPlayerWindow()));
	}
}

void Kaffeine::slotPlayUnPause()
{
	if (m_mediaPart)
		if (m_mediaPart->isPaused() || m_mediaPart->isPlaying())
			m_mediaPart->slotTogglePause();
		else
			slotRequestForCurrentTrack();
}

void Kaffeine::slotPlaylistPlay()
{
	MRL mrl = m_playlist->getCurrent();
	if (!mrl.isEmpty())
	{
		m_recent->addURL(mrl.kurl());
		slotPlay(mrl);
	}
}

void Kaffeine::slotNext()
{
	MRL mrl;
	if ( inplug->playbackFinished( mrl ) ) {
		if (m_sleepAfterPlay->isChecked())
		{
			stop();
			slotSleepAfterPlay();// Shut screen off
			return;
		}
		if (m_quitAfterPlay->isChecked())
		{
			stop();
			slotQuitAfterPlay();
			return;
		}
		m_recent->addURL(mrl.kurl());
		slotPlay(mrl);
	}
	else
	{
		if ((m_mediaPart) && (!m_mediaPart->isPlaying())) // playback finished, nothing more to play
		{
			m_mediaPart->closeURL();
			if (m_sleepAfterPlay->isChecked())
			{
				slotSleepAfterPlay();// Shut screen off
				return;
			}
			if (m_quitAfterPlay->isChecked() || m_quitAfterPlaylist->isChecked())
			{
				slotQuitAfterPlay();
				return;
			}
		}
	}
}

void Kaffeine::slotRequestForCurrentTrack()
{
	inplug->playCurrentTrack();
}

void Kaffeine::slotRequestForNextTrack()
{
	inplug->playNextTrack();
}

void Kaffeine::slotRequestForPreviousTrack()
{
	inplug->playPreviousTrack();
}

void Kaffeine::slotNumKeyInput( int n )
{
	if ( inplug->visibleWidget()==m_startWindow ) {
		m_startWindow->execTarget( n-10 );
		return;
	}

	QString s;

	if ( m_numKeyHideTimer.isActive() ) {
		m_numKeyHideTimer.stop();
		m_numKey*=10;
		m_numKey+=( n-10 );
	}
	else {
		if ( n==10 ) {
#ifdef HAVE_DVB
			if ( dvbPanel )
				dvbPanel->recallZap();
#endif
			return;
		}
		m_captionCache = caption();
		m_numKey = n-10;
	}
	if (m_numKey > (INT_MAX/10))          // Integer Overflow
		m_numKey = n-10;
	s = s.setNum( m_numKey )+"_";
	emit showOSD( s, 1000, 3 );
	setCaption(s);
	m_numKeyHideTimer.start( 1000, true );
}

void Kaffeine::slotRequestForTrackNumber()
{
	setCaption(m_captionCache);
	if (m_mediaPart != NULL && m_mediaPart->hasChapters())
		m_mediaPart->setDVDChapter((uint)m_numKey);
	else
		inplug->playTrackNumber( m_numKey );

	m_numKey = 0;
}

void Kaffeine::slotPlayRecent(const KURL& kurl)
{
	slotPlay(MRL(kurl));
}

void Kaffeine::slotMetaFromPlayer(const MRL &mrl)
{
	inplug->mergeMeta(mrl);
}

/*
void Kaffeine::dragEnterEvent(QDragEnterEvent *dev)
{
   kdDebug() << "Kaffeine: drag enter even" << endl;
   dev->accept(QUriDrag::canDecode(dev) || QTextDrag::canDecode(dev));
}

void Kaffeine::dropEvent(QDropEvent* dev)
{
  QStringList urls;

  if (QUriDrag::decodeToUnicodeUris(dev, urls))
  {
    kdDebug() << "Kaffeine: " << urls.count() << " urls dropped..." << endl;
    load(urls);
  }
  else
  if (strcmp(dev->format(), "text/x-moz-url") == 0)    // for mozilla drops
  {
    QByteArray data = dev->encodedData("text/plain");
    QString md(data);
    load(md);
  }
}
*/

void Kaffeine::setupActions()
{
	/* file menu */
	KStdAction::open(this, SLOT(slotOpenFile()), actionCollection(), "file_open");
	new KAction(i18n("Open &URL..."), "www", CTRL|Key_U, this, SLOT(slotOpenURL()), actionCollection(), "file_open_url");
	new KAction(i18n("Open D&irectory..."), "folder_video", 0, this, SLOT(slotOpenDirectory()), actionCollection(), "file_open_directory");
	m_recent = KStdAction::openRecent(this, SLOT(slotPlayRecent(const KURL&)), actionCollection(), "file_open_recent");
	m_sleepAfterPlay = new KToggleAction(i18n("Quit && Shutoff Monitor After This Track"), 0, 0, this, SLOT(slotSleepAfterPlayMenu()), actionCollection(), "sleep_after_play");
	m_quitAfterPlay = new KToggleAction(i18n("Quit After This Track"), 0, 0, this, SLOT(slotQuitAfterPlayMenu()), actionCollection(), "quit_after_play");
	m_quitAfterPlaylist = new KToggleAction(i18n("Quit After Playlist"), 0, 0, this, SLOT(slotQuitAfterPlaylistMenu()), actionCollection(), "quit_after_playlist");
	KStdAction::quit(this, SLOT(slotQuit()), actionCollection());

	/*view menu */
	m_fullscreen = KStdAction::fullScreen(this, SLOT(slotToggleFullscreen()), actionCollection(), this, "view_fullscreen");
	m_minimal = new KToggleAction(i18n("&Minimal Mode"), 0, Key_M, this, SLOT(slotToggleMinimalMode()), actionCollection(), "view_minimal");
	new KAction(i18n("Toggle &Playlist/Player"), 0, Key_P, this, SLOT(slotTogglePlaylist()), actionCollection(), "view_toggle_tab");
	m_originalAspect = new KToggleAction(i18n("Keep &Original Aspect"), 0, 0, this, SLOT(slotOriginalAspect()), actionCollection(), "view_original_aspect");
	m_autoResizeOff = new KToggleAction(i18n("Off"), 0, ALT|Key_0, this, SLOT(slotAutoresizeOff()), actionCollection(), "view_auto_resize_off");
	m_autoResizeOriginal = new KToggleAction(i18n("Original Size"), 0, ALT|Key_1, this, SLOT(slotAutoresizeOriginal()), actionCollection(), "view_auto_resize_original");
	m_autoResizeDouble = new KToggleAction(i18n("Double Size"), 0, ALT|Key_2, this, SLOT(slotAutoresizeDouble()), actionCollection(), "view_auto_resize_double");
	m_autoResizeTriple = new KToggleAction(i18n("Triple Size"), 0, ALT|Key_3, this, SLOT(slotAutoresizeTriple()), actionCollection(), "view_auto_resize_triple");

	m_playersMenu = new KActionMenu(i18n("&Player Engine"), actionCollection(), "options_player");
	KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
	KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
	KStdAction::preferences(this, SLOT(slotPreferences()), actionCollection(), "options_preferences");

	/* fill players action menu */
	QSignalMapper* mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(const QString&)), this, SLOT(slotLoadPart(const QString&)));
	KAction* action = NULL;
	QStringList mediaParts;

	// check for kaffeine parts
	KTrader::OfferList offers = KTrader::self()->query("audio/x-mp3", "'KaffeinePart' in ServiceTypes");
	KTrader::OfferList::Iterator end(offers.end());
	for(KTrader::OfferList::Iterator it = offers.begin(); it != end; ++it)
	{
		KService::Ptr ptr = (*it);

		action = new KAction(ptr->name(), ptr->icon(), 0, mapper, SLOT(map()), actionCollection());
		if (!ptr->comment().isNull())
			action->setToolTip(ptr->comment());
		mapper->setMapping(action, ptr->desktopEntryName());
		m_playersMenu->insert(action);
		mediaParts.append(ptr->name());
	}

	QAccel *acn = new QAccel( this );
	acn->insertItem( Key_0, 10 );
	acn->insertItem( Key_1, 11 );
	acn->insertItem( Key_2, 12 );
	acn->insertItem( Key_3, 13 );
	acn->insertItem( Key_4, 14 );
	acn->insertItem( Key_5, 15 );
	acn->insertItem( Key_6, 16 );
	acn->insertItem( Key_7, 17 );
	acn->insertItem( Key_8, 18 );
	acn->insertItem( Key_9, 19 );
	connect( acn, SIGNAL(activated(int)), this, SLOT(slotNumKeyInput(int)) );
}

void Kaffeine::slotDVBNextBack(int dir)
{
#ifdef HAVE_DVB
	if ( !dvbPanel )
		return;
	switch (dir) {
	case 0:
		dvbPanel->dvbOSDPrev();
		break;
	case 1:
		dvbPanel->dvbOSDNext();
		break;
	case 2:
		dvbPanel->dvbOSDZap();
		break;
	case 3:
		dvbPanel->dvbOSDRetreat();
		break;
	case 4:
		dvbPanel->dvbOSDAdvance();
		break;
	default:
		fprintf(stderr, "Bad slotDVBNextBack param. Ignored.\n");
	}
#endif
}

void Kaffeine::loadConfig()
{
	KConfig* config = kapp->config();
	bool b;

	config->setGroup("General Options");

	m_autoResizeFactor = config->readNumEntry("Autoresize Factor", 0);
	switch (m_autoResizeFactor)
	{
		case 0:
		m_autoResizeOff->setChecked(true);
		break;
		case 1:
		m_autoResizeOriginal->setChecked(true);
		break;
		case 2:
		m_autoResizeDouble->setChecked(true);
		break;
		case 3:
		m_autoResizeTriple->setChecked(true);
		break;
	}

	b = config->readBoolEntry("Original Aspect", false);
	m_originalAspect->setChecked(b);

	m_embedSystemTray = config->readBoolEntry("Embed in System Tray", false);
	if (m_embedSystemTray)
		slotSystemTray(m_embedSystemTray);
	m_osdDuration = config->readNumEntry("OSD Duration", 5);

	m_useAlternateEncoding = config->readBoolEntry("Use Alternate Encoding", false);
	slotUseAlternateEncoding(m_useAlternateEncoding);

	m_alternateEncoding = config->readEntry("Alternate Encoding Name", "ISO 8859-1");
	slotAlternateEncoding(m_alternateEncoding);

	m_pauseVideo = config->readBoolEntry("PauseHiddenVideo", true);
	m_dvbClientEnabled = config->readBoolEntry("DvbClientEnabled", false);
	m_dvbClientAddress = config->readEntry("DvbClientAddress", "192.168.0.255");
	m_dvbClientPort = config->readNumEntry("DvbClientPort", 1234);
	m_dvbClientInfo = config->readNumEntry("DvbClientInfo", 1235);
	m_dvbClientShiftPath = config->readEntry("DvbClientShiftPath", QDir::homeDirPath() );

	if ( m_dvbClientEnabled )
	{
		dvbClient = new CdWidget( m_dvbClientAddress, m_dvbClientPort, m_dvbClientInfo, m_dvbClientShiftPath, this, this, "cdwidget");
		inplug->add( dvbClient, KGlobal::iconLoader()->loadIcon("network", KIcon::Small), i18n( "DVB client") );
	}
	else
		dvbClient = 0;

	//inplug->setActivePlugin( config->readEntry("Active Browser", "") );

	config->setGroup("Player Part");
	QString partName = config->readEntry("Last Service Desktop Name", DEFAULT_PLAYER_PART);
	// don't load the old kaffeine_part.
	if ( partName=="kaffeine_part" )
		partName=DEFAULT_PLAYER_PART;
	slotLoadPart(partName);

	m_recent->loadEntries(config, "Recent Files");

	applyMainWindowSettings(config, "Main Window");
}

void Kaffeine::saveConfig()
{
	KConfig* config = kapp->config();

	if (!m_fullscreen->isChecked())
	{
		if (m_minimal->isChecked())
			menuBar()->show();
		saveMainWindowSettings(config, "Main Window");
	}

	config->setGroup("General Options");
	config->writeEntry("Autoresize Factor", m_autoResizeFactor);
	config->writeEntry("Original Aspect", m_originalAspect->isChecked());
	config->writeEntry("Embed in System Tray", m_embedSystemTray);
	config->writeEntry("OSD Duration", m_osdDuration);
	config->writeEntry("Use Alternate Encoding", m_useAlternateEncoding);
	config->writeEntry("Alternate Encoding Name", m_alternateEncoding);
	config->writeEntry("Active Browser", inplug->activePlugin());
	config->writeEntry("PauseHiddenVideo", m_pauseVideo);
	config->writeEntry("DvbClientEnabled", m_dvbClientEnabled);
	config->writeEntry("DvbClientAddress", m_dvbClientAddress);
	config->writeEntry("DvbClientPort", m_dvbClientPort);
	config->writeEntry("DvbClientInfo", m_dvbClientInfo);
	config->writeEntry("DvbClientShiftPath", m_dvbClientShiftPath );

	config->setGroup("Player Part");
	if (m_currentPartService.isNull())
		m_currentPartService = DEFAULT_PLAYER_PART;
	config->writeEntry("Last Service Desktop Name", m_currentPartService);

	m_recent->saveEntries(config, "Recent Files");

	inplug->saveConfig();
}

void Kaffeine::showEvent(QShowEvent*)
{
	if (m_mediaPart)
	{
		// restart playback if stream contains video
		if ( (m_mediaPart->isPlaying()) && (m_mediaPart->hasVideo()) && (m_mediaPart->isPaused()) && (m_autoPaused) )
			m_mediaPart->slotTogglePause();
	}
	m_autoPaused = false;
}

void Kaffeine::hideEvent (QHideEvent*)
{
	if (m_mediaPart && m_pauseVideo)
	{
		// pause playback if stream contains video
		if ( (m_mediaPart->isPlaying()) && (m_mediaPart->hasVideo())
		        && (!m_mediaPart->isPaused()) )
		{
			m_mediaPart->slotTogglePause();
			m_autoPaused = true;
		}
	}
}

void Kaffeine::slotPreferences()
{
	KaffeinePreferences dlg;
	dlg.setConfig(m_pauseVideo, m_embedSystemTray, m_osdDuration,  m_useAlternateEncoding, m_alternateEncoding);
	dlg.setDvbClient( m_dvbClientEnabled, m_dvbClientAddress, m_dvbClientPort, m_dvbClientInfo, m_dvbClientShiftPath );
	connect(&dlg, SIGNAL(signalClearRecent()), this, SLOT(slotClearRecent()));
	connect(&dlg, SIGNAL(signalEmbedSystemTray(bool)), this, SLOT(slotSystemTray(bool)));
	connect(&dlg, SIGNAL(signalUseAlternateEncoding(bool)), this, SLOT(slotUseAlternateEncoding(bool)));
	connect(&dlg, SIGNAL(signalAlternateEncoding(const QString&)), this, SLOT(slotAlternateEncoding(const QString&)));
	connect(&dlg, SIGNAL(signalSetOSDTimeout(uint)), this, SLOT(slotSetOSDTimeout(uint)));
	connect(&dlg, SIGNAL(signalPauseVideo(bool)), this, SLOT(slotPauseVideo(bool)));
	connect(&dlg, SIGNAL(signalDvbClient(bool,const QString&,int,int,const QString&)), this, SLOT(slotDvbClient(bool,const QString&,int,int,const QString&)));
	dlg.exec();
}

void Kaffeine::slotDvbClient( bool enabled, const QString &address, int port, int info, const QString &tspath )
{
	if ( enabled!=m_dvbClientEnabled )
	{
		if ( dvbClient )
		{
			inplug->remove( dvbClient );
			dvbClient = 0;
		}
		else
		{
			dvbClient = new CdWidget(address, port, info, tspath, this, this, "cdwidget");
			if ( m_mediaPart )
			{
				connect( dvbClient, SIGNAL(dvbOpen(const QString&,const QString&,int)), m_mediaPart, SLOT(slotDvbOpen(const QString&,const QString&,int)) );
				connect( m_mediaPart, SIGNAL(stopDvb()), dvbClient, SLOT(stopLive()) );
				connect( dvbClient, SIGNAL(dvbStop()), m_mediaPart, SLOT(slotStop()) );
				connect( m_mediaPart, SIGNAL(playerPause()), dvbClient, SLOT(pauseLiveTV()) );
				connect( dvbClient, SIGNAL(setTimeShiftFilename(const QString&)), m_mediaPart, SLOT(getTimeShiftFilename(const QString&)) );
				dvbClient->enableLive( true );
			}
			inplug->add( dvbClient, KGlobal::iconLoader()->loadIcon("network", KIcon::Small), i18n( "DVB client") );
		}
	}
	else
	{
		if ( dvbClient )
			dvbClient->setParam( address, port, info, tspath );
	}

	m_dvbClientEnabled = enabled;
	m_dvbClientAddress = address;
	m_dvbClientPort = port;
	m_dvbClientInfo = info;
	m_dvbClientShiftPath = tspath;
}

void Kaffeine::slotPauseVideo(bool b)
{
	m_pauseVideo = b;
}

void Kaffeine::slotSetOSDTimeout(uint secs)
{
	m_osdDuration = secs;
}

void Kaffeine::optionsConfigureKeys()
{
	//KKeyDialog::configure(actionCollection(), this);
	KKeyDialog* keyDialog = new KKeyDialog(true,  0);
	if (m_mediaPart)
		keyDialog->insert(m_mediaPart->actionCollection(), i18n("Player"));
	keyDialog->insert(actionCollection(), i18n("Main Window"));
	inplug->addConfigKeys( keyDialog );

	keyDialog->configure(true);

	delete keyDialog;
}

void Kaffeine::optionsConfigureToolbars()
{
	saveMainWindowSettings(KGlobal::config(), autoSaveGroup());

	// use the standard toolbar editor
	KEditToolbar dlg(factory());
	connect(&dlg, SIGNAL(newToolbarConfig()),
	        this, SLOT(applyNewToolbarConfig()));
	dlg.exec();
}

void Kaffeine::applyNewToolbarConfig()
{
	applyMainWindowSettings(KGlobal::config(), autoSaveGroup());
}

void Kaffeine::slotSystemTray(bool embed)
{
	m_embedSystemTray = embed;
	if (embed)
	{
		if (!m_systemTray)
		{
			m_systemTray = new SystemTray(this);
			connect(m_systemTray, SIGNAL(signalPlay()), this, SLOT(slotPlayUnPause()));
			connect(m_systemTray, SIGNAL(signalNext()), this, SLOT(slotRequestForNextTrack()));
			connect(m_systemTray, SIGNAL(signalPrevious()), this, SLOT(slotRequestForPreviousTrack()));
			connect(m_systemTray, SIGNAL(signalStop()), this, SLOT(slotStop()));
			connect(m_systemTray, SIGNAL(signalMute()), this, SLOT(slotMute()));
			connect(m_systemTray, SIGNAL(quitSelected()), this, SLOT(slotQuit()));
			m_systemTray->show();
		}
	}
	else
	{
		if (m_systemTray)
		{
			delete m_systemTray;
			m_systemTray = NULL;
		}
	}
}

void Kaffeine::slotSleepAfterPlayMenu()
{
	m_quitAfterPlay->setChecked(false); //Keep from checking both quits

#ifdef HAVE_DPMS
	Display *dspl = qt_xdisplay();
	int base;
	bool hasDPMS = DPMSQueryExtension(dspl, &base, &base);
	if (!hasDPMS) // Check Xserver for DPMS
	{
		KMessageBox::error(this, i18n("DPMS Xserver extension was not found."));
		m_sleepAfterPlay->setChecked(false);
	}
	else
	{
		if (m_sleepAfterPlay->isChecked())
		{
			KMessageBox::information(this, i18n("This will quit Kaffeine and shut off the monitor's power after the file/playlist has finished. Option \"dpms\" must be in  your X config file for the monitor to power off."), QString::null, "sleep_info", 1);
		}
	}
#endif
}

void Kaffeine::slotSleepAfterPlay()
{
#ifdef HAVE_DPMS
	Display *dspl = qt_xdisplay();
	CARD16 standby;
	CARD16 suspend;
	CARD16 off;
	DPMSGetTimeouts(dspl, &standby, &suspend, &off);
	DPMSEnable(dspl);
	DPMSSetTimeouts(dspl, 0, 1, 2); // Arbitrarily chosen times
	XFlush(dspl);
	sleep(3); // Wait for DPMS to kill screen
	DPMSSetTimeouts(dspl, standby, suspend, off); // For reseting DPMS and toggle
	XFlush(dspl);
#endif

	if (m_systemTray)
	{
		hide();
		m_sleepAfterPlay->setChecked(false);
		m_quitAfterPlaylist->setChecked(false);
	}
	else
		slotQuit();
}

void Kaffeine::slotQuitAfterPlaylistMenu()
{
	m_sleepAfterPlay->setChecked(false);
	m_quitAfterPlay->setChecked(false);
}

void Kaffeine::slotQuitAfterPlayMenu()
{
	m_sleepAfterPlay->setChecked(false); //Keep from checking both quits
	m_quitAfterPlaylist->setChecked(false);
}

void Kaffeine::slotQuitAfterPlay()
{
	if (m_systemTray)
	{
		hide();
		m_quitAfterPlay->setChecked(false);
		m_quitAfterPlaylist->setChecked(false);
	}
	else
		slotQuit();
}

void Kaffeine::slotOpenFile()
{
	QString fileFilter = m_filter;
	fileFilter.prepend(" *.m3u *.pls *.kaffeine *.M3U *.PLS *.KAFFEINE");
	if (m_mediaPart)
	{
		QString extensions = m_mediaPart->supportedExtensions();
		kdDebug() << extensions << endl;
		if (extensions.isNull())
			extensions = DEFAULT_FILTER;
		fileFilter.prepend(extensions);
	}
	else
		fileFilter.prepend(DEFAULT_FILTER);

	KURL::List kurlList = KFileDialog::getOpenURLs(":kaffeine_openFile", fileFilter, 0, i18n("Open File(s)"));

	for (KURL::List::Iterator it = kurlList.begin(); it != kurlList.end(); ++it)
		if ((*it).isLocalFile() && (*it).path().endsWith(".iso", false))
			(*it).setProtocol("dvd");

	QStringList urlList = kurlList.toStringList();

	if (urlList.count() > 0)
		load(urlList);
}

void Kaffeine::slotTogglePlaylist()
{
	if (!m_playerContainer)
		return;

	if ( /* !m_fullscreen->isChecked() && */ !m_minimal->isChecked() )
		inplug->togglePlaylist();
}

void Kaffeine::slotSwitchToPlayerWindow()
{
	if ( !m_playerContainer )
		return;

	if ( m_mediaPart ) {
		if (m_mediaPart->isPlaying()) // only if playback
			inplug->showPlayer();
	}
}

void Kaffeine::slotCurrentTabChanged(QWidget*)
{
}

void Kaffeine::slotToggleMinimalModeFromPlayer()
{
	m_minimal->setChecked(!m_minimal->isChecked());
	slotToggleMinimalMode();
}

void Kaffeine::slotToggleMinimalMode()
{
	if (m_fullscreen->isChecked())
	{
		m_minimal->setChecked(!m_minimal->isChecked());
		return;
	}

	if (m_minimal->isChecked())
	{
		kdDebug() << "Kaffeine: Go to minimal mode..." << endl;
		menuBar()->hide();
		//m_statusBarVisible = statusBar()->isVisible();
		//statusBar()->hide();
		hideToolbars(true);
		inplug->fullscreen( true );
		setMouseTracking( true );
	}
	else
	{
		menuBar()->show();
		//if (m_statusBarVisible)
			//statusBar()->show();
		hideToolbars(false);
		inplug->fullscreen( false );
		setMouseTracking( false );
	}
}

void Kaffeine::slotToggleFullscreen()
{
	if (m_fullscreen->isChecked())
	{
		kdDebug() << "Kaffeine: Go to fullscreen mode..." << endl;
		if (m_mediaPart)
			m_mediaPart->slotPrepareForFullscreen(true);
		menuBar()->hide();
		if (!m_minimal->isChecked()) {
			//m_statusBarVisible = statusBar()->isVisible();
			inplug->fullscreen( true );
		}
		//statusBar()->hide();
		hideToolbars(true);

		/*
		 * uuuh, ugly :-)
		 * make sure there is no frame border around the player window
		 */
		QFrame* tabFrame = dynamic_cast<QFrame*>(m_playerContainer->parentWidget());
		if (tabFrame)
		{
			tabFrame->setFrameShape(QFrame::NoFrame);
			tabFrame->setLineWidth(0);
		}

		if (m_haveKWin)
		{
			KWin::activateWindow(winId());
			KWin::setState(winId(), NET::FullScreen);
		}
		else
			showFullScreen();
			setMouseTracking( true );
	}
	else
	{
		// restore frame style
		QFrame* tabFrame = dynamic_cast<QFrame*>(m_playerContainer->parentWidget());
		if (tabFrame)
		{
			tabFrame->setFrameStyle(QFrame::TabWidgetPanel | QFrame::Raised);
			tabFrame->setLineWidth(0);
		}

		kdDebug() << "Kaffeine: Leave fullscreen mode..." << endl;
		if (m_mediaPart)
			m_mediaPart->slotPrepareForFullscreen(false);
		if (!m_minimal->isChecked())
		{
			menuBar()->show();
			//if (m_statusBarVisible)
				//statusBar()->show();
			hideToolbars(false);
			inplug->fullscreen( false );
		}

		if (m_haveKWin)
			KWin::clearState(winId(), NET::FullScreen);
		else
			showNormal();
			setMouseTracking( false );
	}
}

void Kaffeine::slotEscapeFullscreen()
{
	if (m_fullscreen->isChecked())
	{
		m_fullscreen->setChecked(false);
		slotToggleFullscreen();
	}
	else if (m_minimal->isChecked())
	{
		m_minimal->setChecked(false);
		slotToggleMinimalMode();
	}
}

void Kaffeine::hideToolbars(bool hide)
{
	if (hide)
	{
		leftDock()->hide();
		rightDock()->hide();
		topDock()->hide();
		bottomDock()->hide();
	}
	else
	{
		leftDock()->show();
		rightDock()->show();
		topDock()->show();
		bottomDock()->show();
	}
}

void Kaffeine::mouseDoubleClickEvent(QMouseEvent*)
{
	// kdDebug() << "Kaffeine: doubleclick" << endl;
	fullscreen();
}

void Kaffeine::mousePressEvent(QMouseEvent* mev)
{
	kdDebug() << "Kaffeine: Mouse press event" << endl;
	if ( m_minimal->isChecked() && !m_fullscreen->isChecked() && (mev->button() == MidButton) ) {
		if (topDock()->isVisible()) {
			hideToolbars(true);
			//menuBar()->hide;
		}
		else {
				hideToolbars(false);
				//menuBar()->show();
		}
	}
	mev->ignore();
}

void Kaffeine::mouseMoveEvent(QMouseEvent* mev)
{
	//kdDebug() << "Kaffeine: Mouse move event" << endl;
	if ( m_fullscreen->isChecked() ) {
		if ( mev->y()<60 || mev->y()>(height()-60) ) {
			if ( !topDock()->isVisible() )
				hideToolbars( false );
		}
		else {
			if ( topDock()->isVisible() )
				hideToolbars( true );
		}
	}

	mev->ignore();
}

void Kaffeine::resizeEvent(QResizeEvent* rev)
{
	/* FIXME: don't really work proper... */
	KMainWindow::resizeEvent(rev);
	if ((!m_noResize) && (!m_fullscreen->isChecked()) && (m_originalAspect->isChecked())
	        && (m_videoSize.height() != 0) && (m_videoSize.width() != 0))
	{
		QSize player = inplug->stackSize();
		double ratio = (double)m_videoSize.height() / (double)m_videoSize.width();
		int newHeight = (int)((double)player.width() * ratio);
		QSize newSize = size() - QSize(0, player.height() - newHeight);
		kdDebug() << "Kaffeine: resize to orignal aspect: old size: " << size().width() << "x" << size().height()
		<< ", video size: " << m_videoSize.width() << "x" << m_videoSize.height()
		<< ", video ratio: " << ratio
		<< ", old height: " << player.height()
		<< ", new height: " << newHeight
		<< ", resize to: " << newSize.width() << "x" << newSize.height() << endl;
		m_noResize = true;
		resize(newSize);
	}
	else
		m_noResize = false;
}

void Kaffeine::slotOriginalAspect()
{
	if (m_originalAspect->isChecked())
		Kaffeine::resizeEvent(new QResizeEvent(QSize(), QSize()));
}

void Kaffeine::autoresize()
{
	if ((!m_fullscreen->isChecked()) && (m_autoResizeFactor)
	        && (m_videoSize.height() != 0) && (m_videoSize.width() != 0))
	{
		QSize player, newSize;
		/*
		 * we do the resize twice, because after the first resize toolbars area may be much heigher
		 */
		for (uint i = 0; i < 2; i++)
		{
			player = inplug->stackSize();
			newSize = size() - QSize(player.width() - m_videoSize.width() * m_autoResizeFactor,
			                         player.height() - m_videoSize.height() * m_autoResizeFactor);
			kdDebug() << "Kaffeine: autoresize: old size: " << size().width() << "x" << size().height()
			<< ", video size: " << m_videoSize.width() << "x" << m_videoSize.height()
			<< ", player size: " << player.width() << "x" << player.height()
			<< ", resize to: " << newSize.width() << "x" << newSize.height() << endl;
			m_noResize = true;
			resize(newSize);
		}
	}
}

void Kaffeine::slotAutoresizeOff()
{
	m_autoResizeFactor = 0;
	m_autoResizeOriginal->setChecked(false);
	m_autoResizeDouble->setChecked(false);
	m_autoResizeTriple->setChecked(false);
}

void Kaffeine::slotAutoresizeOriginal()
{
	m_autoResizeFactor = 1;
	m_autoResizeOff->setChecked(false);
	m_autoResizeDouble->setChecked(false);
	m_autoResizeTriple->setChecked(false);
	autoresize();
}

void Kaffeine::slotAutoresizeDouble()
{
	m_autoResizeFactor = 2;
	m_autoResizeOriginal->setChecked(false);
	m_autoResizeOff->setChecked(false);
	m_autoResizeTriple->setChecked(false);
	autoresize();
}

void Kaffeine::slotAutoresizeTriple()
{
	m_autoResizeFactor = 3;
	m_autoResizeOriginal->setChecked(false);
	m_autoResizeDouble->setChecked(false);
	m_autoResizeOff->setChecked(false);
	autoresize();
}

void Kaffeine::slotNewFrameSize(const QSize& size)
{
	kdDebug() << "Kaffeine: new video frame size: " << size.width() << "x" << size.height() << endl;
	m_videoSize = size;
	autoresize();
}

void Kaffeine::slotOpenURL()
{
	bool ok;
	//QString url = QInputDialog::getText(i18n("Open URL"), i18n("Enter a URL:"), QLineEdit::Normal, "", &ok);
	QString url = KInputDialog::getText(i18n("Open URL"), i18n("Enter a URL:"), "", &ok);
	if (ok)
	{
		if ((!(url.left(1) == "/")) && (!url.contains(":/")))
			url.prepend("http://"); // assume http protocol
		load(url);
	}
}

void Kaffeine::slotOpenDirectory()
{
	QString s;

	if (m_mediaPart)
	{
		QString extensions =  m_mediaPart->supportedExtensions();
		if (!extensions.isNull())
			m_playlist->setFileFilter(extensions);
	}

	KURL path = KDirSelectDialog::selectDirectory(":kaffeine_openDir", false, 0, i18n("Open Folder"));
	QString checkSystemURL = path.prettyURL();
	if (path.isValid())
	{
		if (path.protocol() == "media")
			load("media:" + path.path());
		else if (checkSystemURL.startsWith("system:/media/"))
		{
			checkSystemURL = checkSystemURL.mid(14);
			checkSystemURL = checkSystemURL.prepend("media:/");
			load(checkSystemURL);
		}
		else {
			s = path.path();
			if ( QDir(s).entryList().contains("VIDEO_TS") || QDir(s).entryList().contains("video_ts") )
				s = s.prepend("dvd://");
			load( s );
		}
	}
}

/*
QString Kaffeine::askForOtherDevice(const QString& type)
{
	KDialogBase* dialog = new KDialogBase( 0, "askfordrive", true, i18n("Error"), KDialogBase::Ok|KDialogBase::Cancel);
	QVBox* page = dialog->makeVBoxMainWidget();
	page->setSpacing(5);
	page->setMargin(5);
	new QLabel(i18n("No %1 in drive, or wrong path to device.").arg(type), page);
	new QLabel(QString("\n") + i18n("Please select correct drive:"), page);
	DrivesCombo* drives = new DrivesCombo(page);

	if (dialog->exec() == KDialogBase::Accepted)
	{
		QString newDrive = drives->currentText();
		delete dialog;
		return newDrive;
	}
	else
	{
		delete dialog;
		return QString::null;
	}
}
*/

bool Kaffeine::loadTMP(const QStringList& list)
{
	QString device="";
	QStringList sublist("smi");
	sublist  << "srt" << "sub" << "txt" << "ssa" << "asc";
	if ( !m_device.isEmpty() ) {
		device = m_device;
		m_device = "";
	}

	if (!list.count())
		return false;
	QStringList::ConstIterator end(list.end());
	for (QStringList::ConstIterator it = list.begin(); it != end; ++it) {
		QString url = (*it).lower();
                QString ext = url;
                ext = ext.remove( 0 , ext.findRev('.')+1 ).lower();
		if (url == "dvd") {
			cddisc->startDVD( device );
			return true;
		}
		else if (url == "vcd") {
			cddisc->startVCD( device );
			return true;
		}
		else if ((url == "audiocd") || (url == "cdda")) {
			cddisc->startCD( device );
			return true;
		}
		else if (url == "dvb") {
			playDvb();
			return true;
		}
		else if ( sublist.contains(ext)) {
			MRL mrl = m_playlist->getCurrent();
			if (!mrl.isEmpty()) {
				if (!mrl.subtitleFiles().contains(url))	{
					mrl.addSubtitleFile(*it);
					mrl.setCurrentSubtitle(mrl.subtitleFiles().size() - 1);
					m_mediaPart->openURL(mrl);
				}
			}
			return true;
		}
	}

	m_playlist->setPlaylist(i18n("NEW"), true);
	m_playlist->add(list, NULL);
	if (!m_playlist->isQueueMode())
		slotPlaylistPlay();
	return false;
}

void Kaffeine::slotQuit()
{
	if ( !inplug->close() )
		return;

	saveConfig();

	/*if (m_systemTray)
	{
		delete m_systemTray;
		m_systemTray = NULL;
	}*/

	KApplication::exit(0);
}

void Kaffeine::closeEvent(QCloseEvent* e)
{
	if (m_systemTray && !kapp->sessionSaving())
	{
		//KMessageBox::information(this, i18n("Closing the main window will keep Kaffeine running in the system tray. Use Quit from the File menu to quit the application."), QString::null, "system_tray_info");
		hide();
		e->ignore();
	}
	else
	{
		slotQuit();
		if (kapp->sessionSaving())
			e->accept();
		else
			e->ignore();
	}
}

void Kaffeine::slotClearRecent()
{
	m_recent->clear();
	m_recent->setItems(QStringList());
}

void Kaffeine::slotChangeStatusbar(const QString& )
{
	// display the text on the statusbar
	//statusBar()->message(text, 5000); /* show for 5 seconds */
}


void Kaffeine::slotChangePlaylistStatus(const QString& )
{
	//statusBar()->changeItem(text, 9);
}

void Kaffeine::slotChangeCaption(const QString& text)
{
	// display the text on the caption
	setCaption(text);

	if ((m_systemTray) && (!m_fullscreen->isChecked()))
		m_systemTray->showTitle(text, m_osdDuration);
}

void Kaffeine::slotFakeKeyEvent()
{
	if ( m_mediaPart && m_mediaPart->isPlaying() && !m_mediaPart->isPaused() && haveXTest)
		if ( m_fullscreen->isChecked() || m_mediaPart->hasVideo() ) {
			if (m_haveKWin) {
				// use a better method if we're in a kde environment
				kdDebug() << "Kaffeine: Fake mouse movement\n";
				XWarpPointer(x11Display(), None, None, 0, 0, 0, 0, 0, 0);
				XFlush(x11Display());
			} else {
#ifdef HAVE_XTEST
				kdDebug() << "Kaffeine: Fake key press\n";
				XTestFakeKeyEvent(x11Display(), fakeKeycode, true, 0);
				XTestFakeKeyEvent(x11Display(), fakeKeycode, false, 0);
				XFlush(x11Display());
#endif
			}
		}
}

/** slots for meta to unicode encoding **/

void Kaffeine::slotUseAlternateEncoding(bool useEncoding)
{
	m_useAlternateEncoding = useEncoding;
	m_playlist->useAlternateEncoding(useEncoding);
}

void Kaffeine::slotAlternateEncoding(const QString& encoding)
{
	m_alternateEncoding = encoding;
	m_playlist->setAlternateEncoding(encoding);
}

/** slots for system tray **/

void Kaffeine::slotStop()
{
	stop();
}

void Kaffeine::slotMute()
{
	mute();
}

/********* DCOP INTERFACE *********/

void Kaffeine::openURL(QString url)
{
	loadTMP(QStringList(url));
}

void Kaffeine::appendURL(QString url)
{
	if (!m_playlist) return;
	m_playlist->add
	(url, m_playlist->getLast());
}

void Kaffeine::play()
{
	slotRequestForCurrentTrack();
}

void Kaffeine::playAudioCD()
{
	cddisc->startCD();
}

void Kaffeine::playVCD()
{
	cddisc->startVCD();
}

void Kaffeine::playDVD()
{
	cddisc->startDVD();
}

void Kaffeine::pause()
{
	if (m_mediaPart)
		m_mediaPart->slotTogglePause();
}

void Kaffeine::stop()
{
	if (m_mediaPart)
		m_mediaPart->slotStop();
}

void Kaffeine::next()
{
	if ((m_mediaPart) && (m_mediaPart->hasChapters()))
		m_mediaPart->playNextChapter();
	else
		slotRequestForNextTrack();
}

void Kaffeine::previous()
{
	if ((m_mediaPart) && (m_mediaPart->hasChapters()))
		m_mediaPart->playPreviousChapter();
	else
		slotRequestForPreviousTrack();
}

void Kaffeine::changePlaylist()
{
	if (!m_playlist) return;
	m_playlist->nextPlaylist();
}

bool Kaffeine::isPlaying()
{
	if (m_mediaPart)
		return m_mediaPart->isPlaying();
	else
		return false;
}

bool Kaffeine::isVideo()
{
	if (m_mediaPart)
	{
		if (m_mediaPart->isPlaying())
			return m_playlist->getCurrent().mime().contains("video");
		else
			return false;
	}
	else
		return m_playlist->getCurrent().mime().contains("video");
}

QString Kaffeine::title()
{
	if (!m_playlist)
		return QString();
	return m_playlist->getCurrent().title();
}

QString Kaffeine::artist()
{
	if (!m_playlist)
		return QString();
	return m_playlist->getCurrent().artist();
}

QString Kaffeine::album()
{
	if (!m_playlist)
		return QString();
	return m_playlist->getCurrent().album();
}

QString Kaffeine::track()
{
	if (!m_playlist)
		return QString();
	return m_playlist->getCurrent().track();
}

QString Kaffeine::getFileName()
{
	if (!m_playlist)
		return QString();
	return m_playlist->getCurrent().url();
}

void Kaffeine::random()
{
	m_playlist->slotToggleShuffle();
}

void Kaffeine::fullscreen()
{
	m_fullscreen->setChecked(!m_fullscreen->isChecked());
	slotToggleFullscreen();
}

void Kaffeine::minimal()
{
	m_minimal->setChecked(!m_minimal->isChecked());
	slotToggleMinimalMode();
}

int Kaffeine::getLength()
{
	if (!m_playlist)
		return 0;
	QTime length = m_playlist->getCurrent().length();
	return length.hour() * 3600 + length.minute() * 60 + length.second();
}

int Kaffeine::getTimePos()
{
	if (m_mediaPart)
		return m_mediaPart->position();
	else
		return 0;
}

void Kaffeine::posPlus()
{
	if (m_mediaPart)
		m_mediaPart->slotPosPlusSmall();
}

void Kaffeine::posMinus()
{
	if (m_mediaPart)
		m_mediaPart->slotPosMinusSmall();
}

void Kaffeine::volUp()
{
	if (m_mediaPart)
	{
		int newVol = m_mediaPart->volume() + 5;
		if (newVol >100)
			newVol = 100;
		m_mediaPart->slotSetVolume(newVol);
	}
}

void Kaffeine::volDown()
{
	if (m_mediaPart)
	{
		int newVol = m_mediaPart->volume() - 5;
		if (newVol <0)
			newVol = 0;
		m_mediaPart-> slotSetVolume(newVol);
	}
}

void Kaffeine::mute()
{
	if (m_mediaPart)
		m_mediaPart->slotMute();
}

void Kaffeine::quit()
{
	slotQuit();
}

void Kaffeine::setNumber( int num )
{
	slotNumKeyInput( num+10 );
}

void Kaffeine::dvbOSD()
{
#ifdef HAVE_DVB
	if ( dvbPanel )
		dvbPanel->dvbOSD();
#endif
}

void Kaffeine::dvbOSDNextChannel()
{
#ifdef HAVE_DVB
	if ( dvbPanel )
		dvbPanel->dvbOSDNext();
#endif
}

void Kaffeine::dvbOSDPreviousChannel()
{
#ifdef HAVE_DVB
	if ( dvbPanel )
		dvbPanel->dvbOSDPrev();
#endif
}

void Kaffeine::dvbOSDNextProgram()
{
#ifdef HAVE_DVB
	if ( dvbPanel )
		dvbPanel->dvbOSDAdvance();
#endif
}

void Kaffeine::dvbOSDPreviousProgram()
{
#ifdef HAVE_DVB
	if ( dvbPanel )
		dvbPanel->dvbOSDRetreat();
#endif
}

void Kaffeine::dvbOSDZap()
{
#ifdef HAVE_DVB
	if ( dvbPanel )
		dvbPanel->dvbOSDZap();
#endif
}

void Kaffeine::playDvb()
{
#ifdef HAVE_DVB
	if ( dvbPanel ) {
		dvbPanel->recallZap();
	}
#endif
}

void Kaffeine::dvbNewTimer( QString name, QString channel, QString datetime, QString duration )
{
#ifdef HAVE_DVB
	if ( dvbPanel ) {
		dvbPanel->dvbNewTimer( name, channel, datetime, duration );
	}
#endif
}

int Kaffeine::dvbSNR( int device )
{
#ifdef HAVE_DVB
	if ( dvbPanel ) {
		return dvbPanel->getSNR( device );
	}
#endif
	return -1;
}
