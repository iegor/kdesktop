/*
 * xine_part.cpp
 *
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
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

#include "xine_part.h"

#include <kapplication.h>
#include <kinstance.h>
#include <kiconloader.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kinputdialog.h>
#include <kxmlguifactory.h>
#include <kpopupmenu.h>
#include <kparts/genericfactory.h>
#include <kprogress.h>
#include <kio/netaccess.h>
#include <kstandarddirs.h>
#include <dcopclient.h>
#include <kprocess.h>
#include <kprotocolinfo.h>
#include <ktoolbar.h>

#include <qvbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qimage.h>
#include <qfontmetrics.h>
#include <qregexp.h>
#include <qtooltip.h>
#include <qdatetimeedit.h>

#include <xine/xineutils.h>

#include "mrl.h"
#include "kxinewidget.h"
#include "videosettings.h"
#include "equalizer.h"
#include "deinterlacequality.h"
#include "filterdialog.h"
#include "screenshotpreview.h"
#include "xineconfig.h"
#include "positionslider.h"
#include "playlistimport.h"
#include "version.h"

typedef KParts::GenericFactory<XinePart> XinePartFactory;
K_EXPORT_COMPONENT_FACTORY (libxinepart, XinePartFactory)


XinePart::XinePart(QWidget* parentWidget, const char* widgetName, QObject* parent, const char* name, const QStringList& args)
		: DCOPObject("XinePartIface"),
		KaffeinePart(parent, name ? name : "XinePart"),
		m_current(0), m_xine(NULL), m_pictureSettings(NULL), m_deinterlacerConfigWidget(NULL),
		m_filterDialog(NULL), m_embeddedContext(NULL)
{
	kdDebug() << "XinePart: Creating new XinePart..." << endl;

	/*
	 *   Parsing parameter given by kaffeine (audiodriver, videodriver, verbose)
	 *   or parameters of <embed>...</embed>
	 *
	 *   format: param="value"
	 */
	QString audioDriver = QString::null;
	QString videoDriver = QString::null;
	bool verbose = false;
	TimeShiftFilename = "";

	for (uint i=0; i<args.count(); i++)
	{
		kdDebug() << "XinePart: Argument: " << args[i] << endl;
		if (args[i].left(11).lower() == "audiodriver")
		{
			audioDriver = args[i].section( '"',1, 1 );
			kdDebug() << "XinePart: Found audiodriver parameter, value: " << audioDriver << endl;

		}
		if (args[i].left(11).lower() == "videodriver")
		{
			videoDriver = args[i].section( '"',1, 1 );
			kdDebug() << "XinePart: Found videodriver parameter, value: " << videoDriver << endl;
		}
		if (args[i].left(7).lower() == "verbose")
		{
			if (args[i].section( '"', 1, 1 ).lower() == "true")
			{
				kdDebug() << "XinePart: Found parameter verbose, set xine engine verbosity to max..." << endl;
				verbose = true;
			}
		}
	}

	// we need an instance
	setInstance(XinePartFactory::instance());

	// be careful - we may be embedded
	QString configPath = locate("data", "kaffeine/xine-config");
	QString logoPath = locate("data", "kaffeine/logo");

	kdDebug() << "XinePart: Using xine-config file: " << configPath << endl;

	m_xine = new KXineWidget(parentWidget, widgetName, configPath, logoPath,
	                         audioDriver, videoDriver, /* start xine manual*/true, verbose);
	connect(m_xine, SIGNAL(signalXineFatal(const QString&)), this, SIGNAL(canceled(const QString&)));
	connect(m_xine, SIGNAL(stopDvb()), this, SIGNAL(stopDvb()));
	connect(m_xine, SIGNAL(signalDvbOSDHidden()), this, SIGNAL(dvbOSDHide()));
	m_xine->setFocusPolicy(QWidget::ClickFocus);
	setWidget(m_xine);

	// set our XML-UI resource file
	setXMLFile("xine_part.rc");
	initActions();
	initConnections();

	QTimer::singleShot(0, this, SLOT(slotDisableAllActions()));
	m_oldPosition = m_xine->mapToGlobal(QPoint(0,0));
	m_posCheckTimer.start(333);
}

XinePart::~XinePart()
{
	kdDebug() << "XinePart: destructor" << endl;
	kdDebug() << "XinePart destructor: calling saveConfig()" << endl;
	saveConfig();
	if (m_embeddedContext)
		delete m_embeddedContext;
}

KAboutData *XinePart::createAboutData()
{
	KAboutData* aboutData = new KAboutData( "kaffeine", I18N_NOOP("XinePart"),
	                                        KAFFEINE_VERSION, I18N_NOOP("A xine based player part for Kaffeine."),
	                                        KAboutData::License_GPL,
	                                        "(c) 2003-2004, Jürgen Kofler.", 0, "http://kaffeine.sourceforge.net");
	aboutData->addAuthor("Jürgen Kofler.",0, "kaffeine@gmx.net");

	return aboutData;
}

bool XinePart::openURL(const MRL& mrl)
{
	kdDebug() << "XinePart::openURL(): " << mrl.url() << endl;

	//     if (!mrl.kurl().isValid())
	//         return false;

	m_mrl = mrl;
	m_playlist.clear();
	m_current = 0;
	bool playlist = false;

	QString ext = m_mrl.kurl().fileName();
	ext = ext.remove( 0 , ext.findRev('.')+1 ).lower();

	if (!m_mrl.mime().isNull())
	{
		KMimeType::Ptr mime = KMimeType::findByURL(m_mrl.kurl().path());
		m_mrl.setMime(mime->name());
	}

	/* is m_mrl a playlist? */
	if ((m_mrl.mime() == "text/plain") || (m_mrl.mime() == "text/xml") || (m_mrl.mime() == "application/x-kaffeine")
	        || (m_mrl.mime() == "audio/x-scpls") || (m_mrl.mime() == "audio/x-mpegurl") || (m_mrl.mime() == "audio/mpegurl")
	        || (m_mrl.mime() == "application/smil")
	        || (ext == "asx") || (ext == "asf") || (ext == "wvx") || (ext == "wax")) /* windows meta files */
	{
		kdDebug() << "XinePart: Check for kaffeine/noatun/m3u/pls/asx playlist\n";
		QString localFile;
		if (KIO::NetAccess::download(m_mrl.kurl(), localFile, widget()))
		{
			QFile file(localFile);
			file.open(IO_ReadOnly);
			QTextStream stream(&file);
			QString firstLine = stream.readLine();
			QString secondLine = stream.readLine();
			file.close();

			if (secondLine.contains("kaffeine", false))
			{
				kdDebug() << "KafeinePart: Try loading kaffeine playlist\n";
				playlist = PlaylistImport::kaffeine(localFile, m_playlist);
			}
			if (secondLine.contains("noatun", false))
			{
				kdDebug() << "XinePart: Try loading noatun playlist\n";
				playlist = PlaylistImport::noatun(localFile, m_playlist);
			}
			if (firstLine.contains("asx", false))
			{
				kdDebug() << "XinePart: Try loading asx playlist\n";
				playlist = PlaylistImport::asx(localFile, m_playlist);
			}
			if (firstLine.contains("smil", false))
			{
				kdDebug() << "XinePart: Try loading smil playlist\n";
				if (KMessageBox::warningYesNo(0, i18n("SMIL (Synchronized Multimedia Integration Language) support is rudimentary!\nXinePart can now try to playback contained video sources without any layout. Proceed?"), QString::null, KStdGuiItem::yes(), KStdGuiItem::no(), "smil_warning") == KMessageBox::Yes)
				{
					if (!PlaylistImport::smil(localFile, m_mrl, m_playlist))
					{
						emit signalTrackFinished();
						return false;
					}
				}
				else
					return false;
			}
			if (firstLine.contains("[playlist]", false))
			{
				kdDebug() << "XinePart: Try loading pls playlist\n";
				playlist = PlaylistImport::pls(localFile, m_playlist);
			}
			if (ext == "m3u")  //indentify by extension
			{
				kdDebug() << "XinePart: Try loading m3u playlist\n";
				playlist = PlaylistImport::m3u(localFile, m_playlist);
			}
		}
		else
			kdError() << "XinePart: " << KIO::NetAccess::lastErrorString() << endl;
	}
	/* check for ram playlist */
	if ( (ext == "ra") || (ext == "rm") || (ext == "ram") || (ext == "lsc") || (ext == "pl") )
	{
		kdDebug() << "XinePart: Try loading ram playlist\n";
		playlist = PlaylistImport::ram(m_mrl, m_playlist, widget());
	}
	/* urls from audiocd kio-slave */
	if (m_mrl.kurl().protocol() == "audiocd")
	{
		QString audioTrack = QString::number(m_mrl.kurl().fileName().remove( QRegExp("\\D" ) ).left(2).toUInt());
		m_mrl = MRL(audioTrack.prepend( "cdda:/" ));
	}

	if (!playlist)
	{
		kdDebug() << "XinePart: Got single track\n";
		m_playlist.append(m_mrl);
	}

	slotPlay(true);

	return true;
}

bool XinePart::closeURL()
{
	kdDebug() << "XinePart::closeURL()" << endl;
	// m_playlist.clear();
	// m_mrl = MRL();
	slotStop();

	return true;
}

void XinePart::slotPlay(bool forcePlay)
{
	kdDebug() << "XinePart::slotPlay()" << endl;

	m_pauseButton->setChecked(false);
	if (m_xine->isPlaying())
	{
		if ( (m_xine->getSpeed() != KXineWidget::Normal) && !forcePlay )
		{
			m_xine->slotSpeedNormal();
			slotEnablePlayActions();
			return;
		}
		else
			emit stopDvb();
	}

	if (m_playlist.count() == 0)
	{
		emit signalRequestCurrentTrack();
		return;
	}

	MRL mrl = m_playlist[m_current];

	/*
	 *  is protocol supported by xine or not known by KIO?
	 */
	if ((QString(SUPPORTED_PROTOCOLS).contains(mrl.kurl().protocol()))
	        ||  (!KProtocolInfo::isKnownProtocol(mrl.kurl())))
	{
		QString sub;
		if ((!mrl.subtitleFiles().isEmpty()) && (mrl.currentSubtitle() > -1))
			sub = QString("#subtitle:%1").arg(mrl.subtitleFiles()[mrl.currentSubtitle()]);

		m_xine->clearQueue();
		m_xine->appendToQueue(mrl.url() + sub );
		if (!m_xine->isXineReady())
		{
			if (!m_xine->initXine())
				return;
		}
		else
			QTimer::singleShot(0, m_xine, SLOT(slotPlay()));
	}
	else
	{
		kdDebug() << "XinePart: Protocol not supported by xine, try to download it..." << endl;

		QString localFile;
		if (KIO::NetAccess::download(mrl.kurl(), localFile, widget()))
		{
			m_xine->clearQueue();
			m_xine->appendToQueue(localFile);
			if (!m_xine->isXineReady())
			{
				if (!m_xine->initXine())
					return;
			}
			else
				QTimer::singleShot(0, m_xine, SLOT(slotPlay()));
		}
		else
			kdError() << "XinePart: " << KIO::NetAccess::lastErrorString() << endl;
	}
}

void XinePart::slotStop()
{
	if (!m_xine->isXineReady())
		return;

	emit stopDvb();

	/* if we play a DVD we cache current title and chapter */
	if (m_playlist[m_current].url().startsWith("dvd:/"))
	{
		uint title = m_xine->currentDVDTitleNumber();
		uint chapter = m_xine->currentDVDChapterNumber();

		m_playlist[m_current] = MRL("dvd://" + QString::number(title) + "." + QString::number(chapter));
	}

	QTimer::singleShot(0, m_xine, SLOT(slotStop()));
	stateChanged("not_playing");
	m_pauseButton->setChecked(false);
	m_playTime->setText("0:00:00");
	emit setWindowCaption("");
}

void XinePart::slotNext()
{
	if (m_xine->hasChapters())
	{
		m_xine->playNextChapter();
		return;
	}

	if ((m_playlist.count() > 0) && (m_current < m_playlist.count()-1))
	{
		m_current++;
		slotPlay();
	}
	else
	{
		emit signalRequestNextTrack();
	}
}

void XinePart::slotPrevious()
{
	if (m_xine->hasChapters())
	{
		m_xine->playPreviousChapter();
		return;
	}

	if (m_current > 0)
	{
		m_current--;
		slotPlay();
	}
	else
	{
		emit signalRequestPreviousTrack();
	}
}

void XinePart::requestForOSD( const QString &text, int duration, int priority )
{
	m_xine->showOSDMessage( text, duration, priority );
}

void XinePart::setDvbCurrentNext( const QString &channelName, const QStringList &list )
{
	m_xine->setDvbCurrentNext( channelName, list );
}

void XinePart::slotDvbOpen( const QString &filename, const QString &chanName, int haveVideo )
{
	if (!m_xine->isXineReady())
		if (!m_xine->initXine())
			return;
	m_playlist.clear();
	m_xine->setDvb( filename, chanName, haveVideo );
	QTimer::singleShot(0, m_xine, SLOT(openDvb()));
	//m_xine->openDvb( filename, chanName, haveVideo );
}

void XinePart::getTimeShiftFilename( const QString &filename )
{
	TimeShiftFilename = filename;
	m_xine->TimeShiftFilename = TimeShiftFilename;
}

void XinePart::slotTogglePause( bool pauseLive )
{
	kdDebug() << "slotSpeedPause()" << endl;
	if (!m_xine->isXineReady())
		return;

	if (m_xine->getSpeed() == KXineWidget::Pause)
	{
		m_xine->slotSpeedNormal();
		slotEnablePlayActions();
		m_pauseButton->setChecked(false);
	}
	else
	{
		if ( pauseLive )
			emit playerPause();
		m_xine->slotSpeedPause();
		// kdDebug() << "XinePart: Set state to paused" << endl;
		stateChanged("paused");
		m_pauseButton->setChecked(true);
	}
}

void XinePart::speedFaster()
{
	slotFastForward();
}

void XinePart::slotFastForward()
{
	if (m_xine->getSpeed() == KXineWidget::Pause)
	{
		m_pauseButton->setChecked(false);
		slotEnablePlayActions();
	}
	m_xine->slotSpeedFaster();
}

void XinePart::speedSlower()
{
	slotSlowMotion();
}

void XinePart::slotSlowMotion()
{
	if (m_xine->getSpeed() == KXineWidget::Pause)
	{
		m_pauseButton->setChecked(false);
		slotEnablePlayActions();
	}

	m_xine->slotSpeedSlower();
}

void XinePart::slotMute()
{
	if (!m_xine->isXineReady())
		return;

	m_xine->slotToggleMute();
}

void XinePart::slotVolumeUp()
{
	int newVol = volume() + 5;
	if (newVol >100)
		newVol = 100;
	slotSetVolume(newVol);
}

void XinePart::slotVolumeDown()
{
	int newVol = volume() - 5;
	if (newVol <0)
		newVol = 0;
	slotSetVolume(newVol);
}

void XinePart::slotPosPlusSmall()
{
	slotJumpIncrement( 20 );
}

void XinePart::slotPosMinusSmall()
{
	slotJumpIncrement( -20 );
}

void XinePart::slotPosPlusMedium()
{
	slotJumpIncrement( 60 );
}

void XinePart::slotPosMinusMedium()
{
	slotJumpIncrement( -60 );
}

void XinePart::slotPosPlusLarge()
{
	slotJumpIncrement( 600 );
}

void XinePart::slotPosMinusLarge()
{
	slotJumpIncrement( -600 );
}

void XinePart::slotJumpIncrement(int increment)
{
	if (!m_xine->isSeekable())
		return;

	QTime timeNow;
	QTime projectedTime;
	QTime startTime;

	if (!m_xine->getLength().isNull())
	{
		timeNow = m_xine->getPlaytime();
		if ( increment < 0 && timeNow.msecsTo(startTime) > increment * 1000 )
		{
			m_xine->slotSeekToTime(startTime);
		}
		else
		{
			projectedTime = timeNow.addSecs(increment);
			m_xine->slotSeekToTime(projectedTime);
		}
	}
}

void XinePart::slotAdvanceSubTitle()
{
	int spuOffset;
	m_xine->getspuOffset(spuOffset);
	m_xine->slotSetSpuOffset(spuOffset+45000);
}

void XinePart::slotDelaySubTitle()
{
	int spuOffset;
	m_xine->getspuOffset(spuOffset);
	m_xine->slotSetSpuOffset(spuOffset-45000);
}

void XinePart::slotSaveStream()
{
	if (m_mrl.isEmpty())
		return;

	QString saveDir = m_xine->getStreamSaveDir();

	KURL kurl = KFileDialog::getSaveURL(saveDir + "/" + m_playlist[m_current].kurl().fileName(), QString::null, 0, i18n("Save Stream As"));
	if (!kurl.isValid())
		return;

	if ( saveDir != kurl.directory() )
		m_xine->setStreamSaveDir(kurl.directory());

	m_xine->clearQueue();
	m_xine->appendToQueue(m_playlist[m_current].url() + "#save:" + kurl.path());
	QTimer::singleShot(0, m_xine, SLOT(slotPlay()));
	m_pauseButton->setChecked(false);
}

void XinePart::slotSetSubtitle(int channel)
{
	if (m_playlist[m_current].subtitleFiles().isEmpty())
	{
		m_xine->slotSetSubtitleChannel(channel);
	}
	else
	{
		m_playlist[m_current].setCurrentSubtitle(channel - 1);
		emit signalNewMeta(m_mrl);
		m_xine->savePosition(m_xine->getPosition()-200);
		slotPlay(true); //force load of new subtitle
	}
	emit setStatusBarText(i18n("Subtitle") + ": " + m_subtitles->items()[channel]);
	m_xine->showOSDMessage(i18n("Subtitle") + ": " + m_subtitles->items()[channel], DEFAULT_OSD_DURATION);
}

void XinePart::slotAddSubtitle(void)
{
	QString subtitleURL = KFileDialog::getOpenURL(m_mrl.kurl().directory(),
                         i18n("*.smi *.srt *.sub *.txt *.ssa *.asc|Subtitle Files\n*.*|All Files"),
                         0, i18n("Select Subtitle File")).path();

	if (!(subtitleURL.isEmpty()))
	{
		if (!m_playlist[m_current].subtitleFiles().contains(subtitleURL))
		{
			m_playlist[m_current].addSubtitleFile(subtitleURL);
		}

		int subchannel = m_playlist[m_current].subtitleFiles().size() - 1;
		m_playlist[m_current].setCurrentSubtitle(subchannel);
		emit signalNewMeta(m_mrl);
		m_xine->savePosition(m_xine->getPosition()-200);
		slotPlay(true); //force load of new subtitle

		emit setStatusBarText(i18n("Subtitle") + ": " + m_subtitles->items()[subchannel]);
		m_xine->showOSDMessage(i18n("Subtitle") + ": " + m_subtitles->items()[subchannel], DEFAULT_OSD_DURATION);
	}

}

void XinePart::slotSetAudioChannel(int channel)
{
	m_xine->slotSetAudioChannel(channel);
	emit setStatusBarText(i18n("Audiochannel") + ": " + m_audioChannels->items()[channel]);
	m_xine->showOSDMessage(i18n("Audiochannel") + ": " + m_audioChannels->items()[channel], DEFAULT_OSD_DURATION);
}

void XinePart::slotSetDVDTitle(const QString& titleStr)
{
	bool ok;
	uint title = titleStr.toInt(&ok);
	if (ok && title > 0 && title <= m_xine->getDVDTitleCount())
	{
		KURL url = m_mrl.kurl();
		url.addPath(QString::number(title));
		m_playlist[m_current] = MRL(url);
		slotPlay(true);
	}
}

void XinePart::slotSetDVDChapter(const QString& chapterStr)
{
	bool ok;
	uint chapter = chapterStr.toInt(&ok);
	if (ok)
		setDVDChapter(chapter);
}

void XinePart::slotSetDVDAngle(const QString& angleStr)
{
	bool ok;
	uint angle = angleStr.toInt(&ok);
	if (ok && angle > 0 && angle <= m_xine->getDVDAngleCount())
	{
		uint title = m_xine->currentDVDTitleNumber();
		uint chapter = m_xine->currentDVDChapterNumber();
		KURL url = m_mrl.kurl();

		url.addPath(QString::number(title) + "." + QString::number(chapter) + "." + QString::number(angle));
		m_playlist[m_current] = MRL(url);
		slotPlay(true);
	}
}

void XinePart::setDVDChapter(uint chapter)
{
	if (chapter > 0 && chapter <= m_xine->getDVDChapterCount())
	{
		uint title = m_xine->currentDVDTitleNumber();
		KURL url = m_mrl.kurl();

		url.addPath(QString::number(title) + "." + QString::number(chapter));
		m_playlist[m_current] = MRL(url);
		slotPlay(true);
	}
}

void XinePart::slotChannelInfo(const QStringList& audio, const QStringList& sub, int currentAudio, int currentSub)
{
	kdDebug() << "XinePart: slotChannelInfo: currentAudio="<<currentAudio<< "  currentSub="<<currentSub<<"\n";
	m_audioChannels->setItems(audio);
	m_audioChannels->setCurrentItem(currentAudio+1);

	if (m_playlist[m_current].subtitleFiles().isEmpty())
	{
		m_subtitles->setItems(sub);
		m_subtitles->setCurrentItem(currentSub+1);
	}
	else
	{
		QStringList subFiles = m_playlist[m_current].subtitleFiles();
		QStringList subs(i18n("off"));
		QString sub;
		QStringList::ConstIterator end(subFiles.end());
		for (QStringList::ConstIterator it = subFiles.begin(); it != end; ++it)
		{
			sub = (*it);
			sub = sub.remove(0 , sub.findRev('/')+1);
			subs.append(sub);
		}
		m_subtitles->setItems(subs);
		m_subtitles->setCurrentItem(m_playlist[m_current].currentSubtitle() + 1);
	}

	/* if we play a DVD enable and fill menus */
	if (m_playlist[m_current].url().startsWith("dvd:/"))
	{
		QStringList titles;
		QStringList chapters;
		QStringList angles;
		uint titlesCount = m_xine->getDVDTitleCount();
		uint chaptersCount = m_xine->getDVDChapterCount();
		uint anglesCount = m_xine->getDVDAngleCount();

		for (uint i = 1; i <= titlesCount; i++)
			titles.append(QString::number(i));
		for (uint i = 1; i <= chaptersCount; i++)
			chapters.append(QString::number(i));
		for (uint i = 1; i <= anglesCount; i++)
			angles.append(QString::number(i));

		m_dvdTitles->setItems(titles);
		m_dvdTitles->setCurrentItem(m_xine->currentDVDTitleNumber() - 1);
		m_dvdChapters->setItems(chapters);
		m_dvdChapters->setCurrentItem(m_xine->currentDVDChapterNumber() - 1);
		m_dvdAngles->setItems(angles);
		m_dvdAngles->setCurrentItem(m_xine->currentDVDAngleNumber() - 1);
		stateChanged("dvd_playback");
	}
	else
	{
		stateChanged("dvd_playback", StateReverse);
	}
}

void XinePart::slotNewPosition(int pos, const QTime& playtime)
{
	QTime length = m_xine->getLength();
	QTime calcLength;

	//if (!m_xine->isSeekable() || length.isNull() || length < playtime)
	if (!m_xine->isSeekable() )
	{
		m_position->setPosition(0,false);
		m_position->setEnabled(false);
	}
	else
	{
		m_position->setPosition(pos, false);
		m_position->setEnabled(true);
	}

	if (m_timerDirection == BACKWARD_TIMER && !length.isNull() && length >= playtime)
		calcLength = length.addSecs(-playtime.second()-playtime.minute()*60-playtime.hour()*60*60);
	else
		calcLength = playtime;

	if (m_timerDirection == BACKWARD_TIMER)
		m_playTime->setText("-" + calcLength.toString("h:mm:ss"));
	else
		m_playTime->setText(calcLength.toString("h:mm:ss"));

	QString timeMessage;
	if (m_isOsdTimer)
	{
		if (m_timerDirection == BACKWARD_TIMER || length.isNull() || length < playtime)
		{
			timeMessage = calcLength.toString("h:mm:ss");
			m_xine->showOSDMessage("-" + timeMessage, 600, OSD_MESSAGE_LOW_PRIORITY);
		}
		else
		{
			timeMessage = i18n("%1 of %2").arg(calcLength.toString("h:mm:ss")).arg(length.toString("h:mm:ss"));
			m_xine->showOSDMessage(timeMessage, 600, OSD_MESSAGE_LOW_PRIORITY);
		}
	}
	currentPosition = (playtime.hour()*3600)+(playtime.minute()*60)+playtime.second();
}

QString XinePart::screenShot()
{
	QString filename = QDir::homeDirPath()+"/kaffeinedcopshot.jpg";
	QImage shot = m_xine->getScreenshot();
	if ( shot.save( filename, "JPEG" ) )
		return filename;
	else
		return "";
}

void XinePart::slotScreenshot()
{
	QImage shot = m_xine->getScreenshot();

	KFileDialog dlg(":kaffeineMain_Screenshot", i18n("*.png|PNG-File\n*.bmp|BMP-File\n*.xbm|XBM-File"),
	                0, "save screenshot", true);
	dlg.setOperationMode(KFileDialog::Saving);
	dlg.setCaption(i18n("Save Screenshot As"));
	dlg.setSelection("screenshot.png");

	ScreenshotPreview* prev = new ScreenshotPreview(shot, &dlg);
	dlg.setPreviewWidget(prev);

	dlg.exec();
	QString fileName = dlg.selectedFile();

	if (fileName.isEmpty())
		return;

	QString type = dlg.currentFilter();
	type = (type.remove(0,2)).upper();

	kdDebug() << "XinePart: Save screenshot as " << type << "\n";
	if  (!shot.save(fileName, type.ascii()))
		kdError() << "XinePart: Screenshot not saved successfully!" << endl;
}

void XinePart::slotFilterDialog()
{
	if (!m_filterDialog)
	{
		m_filterDialog = new FilterDialog(m_xine->getAudioFilterNames(), m_xine->getVideoFilterNames());
		connect(m_filterDialog, SIGNAL(signalCreateAudioFilter(const QString&, QWidget*)),
		        m_xine, SLOT(slotCreateAudioFilter(const QString&, QWidget*)));
		connect(m_filterDialog, SIGNAL(signalCreateVideoFilter(const QString&, QWidget*)),
		        m_xine, SLOT(slotCreateVideoFilter(const QString&, QWidget*)));
		connect(m_filterDialog, SIGNAL(signalRemoveAllAudioFilters()), m_xine, SLOT(slotRemoveAllAudioFilters()));
		connect(m_filterDialog, SIGNAL(signalRemoveAllVideoFilters()), m_xine, SLOT(slotRemoveAllVideoFilters()));
		connect(m_filterDialog, SIGNAL(signalUseAudioFilters(bool)), m_xine, SLOT(slotEnableAudioFilters(bool)));
		connect(m_filterDialog, SIGNAL(signalUseVideoFilters(bool)), m_xine, SLOT(slotEnableVideoFilters(bool)));
	}
	m_filterDialog->show();
	m_filterDialog->raise();
}

void XinePart::slotDeinterlaceQuality()
{
	if (!m_deinterlacerConfigWidget)
		return;
	DeinterlaceQuality* deinterlaceQuality = new DeinterlaceQuality((QWidget*)m_deinterlacerConfigWidget);
	deinterlaceQuality->setQuality(m_lastDeinterlaceQuality);
	connect(deinterlaceQuality, SIGNAL(signalSetDeinterlaceConfig(const QString&)),
	        m_xine, SLOT(slotSetDeinterlaceConfig(const QString&)));

	deinterlaceQuality->exec();

	m_lastDeinterlaceQuality = deinterlaceQuality->getQuality();
	m_lastDeinterlacerConfig = m_xine->getDeinterlaceConfig();
	delete deinterlaceQuality;
}

void XinePart::slotSetHue( int i )
{
	m_hue = i;
	if ( i==-1 )
		return;
	m_xine->slotSetHue( i );
}

void XinePart::slotSetSaturation( int i )
{
	m_saturation = i;
	if ( i==-1 )
		return;
	m_xine->slotSetSaturation( i );
}

void XinePart::slotSetContrast( int i )
{
	m_contrast = i;
	if ( i==-1 )
		return;
	m_xine->slotSetContrast( i );
}

void XinePart::slotSetBrightness( int i )
{
	m_brightness = i;
	if ( i==-1 )
		return;
	m_xine->slotSetBrightness( i );
}

void XinePart::slotPictureSettings()
{
	if (!m_pictureSettings)
	{
		int hue, sat, contrast, bright, avOffset, spuOffset;
		m_xine->getVideoSettings(hue, sat, contrast, bright, avOffset, spuOffset);
		m_pictureSettings = new VideoSettings(hue, sat, contrast, bright, avOffset, spuOffset);
		connect(m_pictureSettings, SIGNAL(signalNewHue(int)), this, SLOT(slotSetHue(int)));
		connect(m_pictureSettings, SIGNAL(signalNewSaturation(int)), this, SLOT(slotSetSaturation(int)));
		connect(m_pictureSettings, SIGNAL(signalNewContrast(int)), this, SLOT(slotSetContrast(int)));
		connect(m_pictureSettings, SIGNAL(signalNewBrightness(int)), this, SLOT(slotSetBrightness(int)));
		connect(m_pictureSettings, SIGNAL(signalNewAVOffset(int)), m_xine, SLOT(slotSetAVOffset(int)));
		connect(m_pictureSettings, SIGNAL(signalNewSpuOffset(int)), m_xine, SLOT(slotSetSpuOffset(int)));
	}
	m_pictureSettings->show();
	m_pictureSettings->raise();
}

void XinePart::slotEqualizer()
{
	m_equalizer->show();
	m_equalizer->raise();
}

void XinePart::slotToggleBroadcastSend()
{
	bool ok = false;

	if (m_broadcastSend->isChecked())
	{
		m_broadcastPort = (uint)KInputDialog::getInteger( QString::null, i18n("Broadcasting port:"), m_broadcastPort, 0, 1000000, 1, &ok);
		if (!ok)
		{
			m_broadcastSend->setChecked(false);
			return;
		}
		m_xine->setBroadcasterPort(m_broadcastPort);
	}
	else
	{
		m_xine->setBroadcasterPort(0); /* disable */
	}
}

void XinePart::slotBroadcastReceive()
{
	if (!m_xine->isXineReady())
	{
		if (!m_xine->initXine())
			return;
	}

	KDialogBase* dialog = new KDialogBase(0, "configmaster", true, i18n("Configure Receive Broadcast Stream"), KDialogBase::Ok|KDialogBase::Cancel);
	QVBox* page = dialog->makeVBoxMainWidget();
	new QLabel(i18n("Sender address:"), page);
	KLineEdit* address = new KLineEdit(m_broadcastAddress, page);
	new QLabel(i18n("Port:"), page);
	QSpinBox* port = new QSpinBox(0, 1000000, 1, page);
	port->setValue(m_broadcastPort);

	if (dialog->exec() == KDialogBase::Accepted)
	{
		m_broadcastPort = port->value();
		m_broadcastAddress = address->text();
		openURL(MRL(QString("slave://") + m_broadcastAddress + ":" + QString::number(m_broadcastPort)));
	}
	delete dialog;
}

void XinePart::slotJumpToPosition()
{
	if (!m_xine->isSeekable())
		return;

	KDialogBase* dialog = new KDialogBase( 0, "configmaster", true, QString::null, KDialogBase::Ok|KDialogBase::Cancel );
	QVBox* page = dialog->makeVBoxMainWidget();
	page->setMargin(5);
	page->setSpacing(5);
	dialog->disableResize();
	new QLabel(i18n("Jump to position:"), page);
	QTimeEdit* timeEdit = new QTimeEdit(page);
	if (!m_xine->getLength().isNull())
	{
		timeEdit->setMaxValue(m_xine->getLength());
		timeEdit->setTime(m_xine->getPlaytime());
	}

	if (dialog->exec() == KDialogBase::Accepted)
	{
		m_xine->slotSeekToTime(timeEdit->time());
	}
	delete dialog;
}

void XinePart::slotButtonTimerPressed()
{
	m_osdTimerEnabler.start(500, true); /* Long Click is 500ms */
}

void XinePart::slotButtonTimerReleased()
{
	if (!m_osdTimerEnabler.isActive())
		return; /* If short click toggle timer Mode*/
	m_osdTimerEnabler.stop();
	//kdDebug() << "XinePart: Toggling forward/backward Timer." << endl;
	QTime length = m_xine->getLength();

	if (!length.isNull())  /* if length not available counting backwards has no meaning */
	{
		if (m_timerDirection == FORWARD_TIMER)
			m_timerDirection = BACKWARD_TIMER;
		else
			m_timerDirection = FORWARD_TIMER;
		slotNewPosition(m_xine->getPosition(),m_xine->getPlaytime());
	}
}

void XinePart::slotToggleOsdTimer()
{
	kdDebug() << "XinePart: Toggling Osd Timer." << endl;
	m_isOsdTimer = !m_isOsdTimer;
}

void XinePart::slotConfigXine()
{
	if (!m_xine->isXineReady())
	{
		if (!m_xine->initXine())
			return;
	}

	XineConfig* xineConfigDialog = new XineConfig(m_xine->getXineEngine());
	xineConfigDialog->exec();
	delete xineConfigDialog;
}

void XinePart::slotError(const QString& errMessage)
{
	if ((m_playlist.count() > 0) && (m_current < m_playlist.count() - 1))
	{
		slotNext(); // try next before aborting playback; e.g. we play a PLS playlist, primary server is full, now try secondary
	}
	else
	{
		//KMessageBox::detailedError(0, errMessage, m_xine->getXineLog(), i18n("xine Error"));
		stateChanged("not_playing");
		KMessageBox::detailedError(0, errMessage, m_xine->getXineLog(), i18n("xine Error"));
		emit signalPlaybackFailed();
	}
}

void XinePart::slotMessage(const QString& message)
{
	QString msg = message;
	if ( msg.startsWith("@") ) {
		if ( m_xine->isPlaying() && m_xine->getURL().contains("#") ) // do not warn for url containing #
			return;
		msg.remove(0,1);
	}
	KMessageBox::information(0, msg, i18n("xine Message"));
}

void XinePart::slotStatus(const QString& status)
{
	emit setStatusBarText(status);
	if ((status != i18n("Ready")) && (status != i18n("Playing")))
	{
		m_xine->showOSDMessage(status, DEFAULT_OSD_DURATION);
	}
}

void XinePart::slotTrackPlaying()
{
	QString caption;

	kdDebug() << "XinePart: xine is playing" << endl;
	m_pauseButton->setChecked(false);
	QTimer::singleShot(100, this, SLOT(slotEnablePlayActions()));

	if ( m_xine->getURL()=="DVB" )
	{
		caption = m_xine->getTitle();
		emit setWindowCaption(caption);
		m_xine->showOSDMessage(caption, DEFAULT_OSD_DURATION);
		return;
	}

	/* fill current mrl with meta info */
	MRL mrl = m_playlist[m_current];

	if (mrl.length().isNull()) /* no meta */
	{
		if ((!m_xine->getTitle().isEmpty()) && (!m_xine->getTitle().contains('/'))
		        && (m_xine->getTitle().contains(QRegExp("\\w")) > 2) && (m_xine->getTitle().left(5).lower() != "track"))
			mrl.setTitle(m_xine->getTitle());
		if ((mrl.artist().isEmpty()) && (!m_xine->getArtist().isEmpty()))
			mrl.setArtist(m_xine->getArtist());
		if ((mrl.album().isEmpty()) && (!m_xine->getAlbum().isEmpty()))
			mrl.setAlbum(m_xine->getAlbum());
		if ((mrl.year().isEmpty()) && (!m_xine->getYear().isEmpty()))
			mrl.setYear(m_xine->getYear());
		if ((mrl.genre().isEmpty()) && (!m_xine->getGenre().isEmpty()))
			mrl.setGenre(m_xine->getGenre());
		if ((mrl.comment().isEmpty()) && (!m_xine->getComment().isEmpty()))
			mrl.setComment(m_xine->getComment());
		mrl.setLength(m_xine->getLength());
		m_playlist[m_current] = mrl;
	}
	/* if we don't have a playlist emit signalNewMeta() */
	if (mrl.url() == m_mrl.url())
	{
		m_mrl = mrl;
		emit signalNewMeta(m_mrl);
	}

	caption = mrl.title();
	if (!mrl.artist().isEmpty())
		caption.append(QString(" (") + mrl.artist() + ")");
	emit setWindowCaption(caption);
	m_xine->showOSDMessage(caption, DEFAULT_OSD_DURATION);
	//emit signalNewFrameSize(m_xine->getVideoSize());
}

void XinePart::slotPlaybackFinished()
{
	if ((m_playlist.count() > 0) && (m_current < m_playlist.count()-1))
	{
		slotNext();
	}
	else
	{
		stateChanged("not_playing");
		emit signalTrackFinished();
	}
}

void XinePart::slotNewLength()
{
	m_mrl.setLength(m_xine->getLength());
	emit signalNewMeta(m_mrl);
}

void XinePart::slotNewTitle()
{
	m_mrl.setTitle(m_xine->getTitle());
	emit signalNewMeta(m_mrl);
	emit setWindowCaption(m_mrl.title());
}

void XinePart::slotNewFrameSize()
{
	kdDebug() << "XinePart: got new frame size from xine" << endl;
	emit signalNewFrameSize(m_xine->getVideoSize());
}

void XinePart::slotContextMenu(const QPoint& pos)
{
	if (factory())
	{
		KPopupMenu *pop = (KPopupMenu*)factory()->container("context_menu", this);
		if (pop)
			pop->popup(pos);
	}
	else
	{
		if (m_embeddedContext)
			m_embeddedContext->popup(pos);
	}
}

void XinePart::slotDVDMenuLeft()
{
	if (m_xine)
		m_xine->slotDVDMenuLeft();
}

void XinePart::slotDVDMenuRight()
{
	if (m_xine)
		m_xine->slotDVDMenuRight();
}

void XinePart::slotDVDMenuUp()
{
	if (m_xine)
		m_xine->slotDVDMenuUp();
}

void XinePart::slotDVDMenuDown()
{
	if (m_xine)
		m_xine->slotDVDMenuDown();
}

void XinePart::slotDVDMenuSelect()
{
	if (m_xine)
		m_xine->slotDVDMenuSelect();
}

void XinePart::slotInfo()
{
	MRL mrl;
	if ( m_xine->getURL()=="DVB" )
		mrl=MRL( QString("DVB"), m_xine->getTitle() );
	else
	{
		if ((m_mrl.isEmpty()) || (m_xine->getTitle().isNull()))
			return;
		mrl = m_playlist[m_current];
	}

	QString info;
	QTextStream ts(&info, IO_WriteOnly);
	ts << "<qt><table width=\"90%\">";
	ts << "<tr><td colspan=\"2\"><center><b>" << mrl.title() << "</b></center></td></tr>";
	if (!mrl.artist().isNull())
		ts << "<tr><td><b>" << i18n("Artist") << ":</b></td><td> " << mrl.artist() << "</td></tr>";
	if (!mrl.album().isNull())
		ts << "<tr><td><b>" << i18n("Album") << ":</b></td><td> " << mrl.album() << "</td></tr>";
	if (!mrl.track().isNull())
		ts << "<tr><td><b>" << i18n("Track") << ":</b></td><td> " << mrl.track() << "</td></tr>";
	if (!mrl.year().isNull())
		ts << "<tr><td><b>" << i18n("Year") << ":</b></td><td> " << mrl.year() << "</td></tr>";
	if (!mrl.genre().isNull())
		ts << "<tr><td><b>" << i18n("Genre") << ":</b></td><td> " << mrl.genre() << "</td></tr>";
	if (!(mrl.length().isNull()))
		ts << "<tr><td><b>" << i18n("Length") << ":</b></td><td> " << mrl.length().toString("h:mm:ss") << "</td></tr>";

	ts << "<br>";
	ts << "<tr><td><b>" << i18n("Mime") << ":</b></td><td> " << mrl.mime() << "</td></tr>";
	if (m_xine->hasAudio())
		ts << "<tr><td><b>" << i18n("Audio") << ":</b></td><td> " << m_xine->getAudioCodec() << " " << QString::number(m_xine->getAudioBitrate()/1000)
		<< "kb/s</td></tr>";
	if (m_xine->hasVideo())
		ts << "<tr><td><b>" << i18n("Video") << ":</b></td><td> " << m_xine->getVideoCodec() << " " << m_xine->getVideoSize().width() << "x"
		<< m_xine->getVideoSize().height() << "(" << m_xine->getVideoWidth() << "x" << m_xine->getVideoHeight() << ")"<< "</td></tr>";

	ts << "<br>";
	if (m_xine->hasSubtitleURL())
		ts << "<tr><td><b>" << i18n("Subtitle File")  << ":</b></td><td> " << m_xine->getSubtitleURL() << "</td></tr>";
	if (m_xine->hasSaveURL())
		ts << "<tr><td><b>" << i18n("Save Stream as") << ":</b></td><td> " << m_xine->getSaveURL() << "</td></tr>";

	ts << "<tr><td></td><td></td></tr>"; // added for better layout
	ts << "</table></qt>";
	KMessageBox::information(0, info, i18n("Track info") );
}

void XinePart::slotFinalize()
{
	if (factory())
	{
		KToolBar *pos = (KToolBar*)factory()->container("positionToolBar", this);
		if (pos)
		{
			// pos->alignItemRight(pos->idAt(1), true); //align time widget right
			pos->setItemAutoSized(pos->idAt(0), true); //set position slider to maximum width
		}
		else
			kdWarning("Position toolbar not found");
	}
	else
	{
		kdDebug() << "XinePart: no xmlguifactory, will create a simple context menu..." << endl;
		KAction* action = NULL;
		m_embeddedContext = new KPopupMenu(0);
		m_embeddedContext->insertTitle(instance()->iconLoader()->loadIcon("kaffeine", KIcon::Small), i18n("Kaffeine Player"));
		actionCollection()->action("player_play")->plug(m_embeddedContext);
		actionCollection()->action("player_pause")->plug(m_embeddedContext);
		actionCollection()->action("player_stop")->plug(m_embeddedContext);
		actionCollection()->action("volume_increase")->plug(m_embeddedContext);
		actionCollection()->action("volume_decrease")->plug(m_embeddedContext);
		actionCollection()->action("audio_mute")->plug(m_embeddedContext);
		m_embeddedContext->insertSeparator();
		actionCollection()->action("player_track_info")->plug(m_embeddedContext);
		m_embeddedContext->insertSeparator();
		actionCollection()->action("file_save_screenshot")->plug(m_embeddedContext);
		actionCollection()->action("file_save_stream")->plug(m_embeddedContext);
		m_embeddedContext->insertSeparator();
		action = new KAction(i18n("Copy URL to Clipboard"), "editcopy", 0, this, SLOT(slotCopyToClipboard()), actionCollection(), "copy_to_clipboard");
		action->plug(m_embeddedContext);
		action = new KAction(i18n("Play in Kaffeine Externally"), "gear", 0, this, SLOT(slotLaunchExternally()), actionCollection(), "play_externally");
		action->plug(m_embeddedContext);
	}

	QStringList visuals = m_xine->getVisualPlugins();
	visuals.prepend("none");
	m_audioVisual->setItems(visuals);

	loadConfig();
	QTimer::singleShot(0, this, SLOT(slotEnableAllActions()));
}

void XinePart::slotCopyToClipboard()
{
	kdDebug() << "XinePart: Send URL to klipper: " << m_mrl.url() << endl;
	DCOPClient* client = KApplication::dcopClient();
	if (!client->send("klipper", "klipper", "setClipboardContents(QString)", m_mrl.url()))
		kdError() << "Can't send current URL to klipper" << endl;
}

void XinePart::slotLaunchExternally()
{
	slotStop();

	QTimer::singleShot(1000, this, SLOT(slotLaunchDelayed()));
}

void XinePart::slotLaunchDelayed()
{
	kdDebug() << "XinePart: Start Kaffeine with argument: " << m_mrl.url() << endl;
	KProcess process;
	process << "kaffeine" << m_mrl.url();
	kdDebug() << "XinePart: Launching Kaffeine externaly..." << endl;
	process.start(KProcess::DontCare);
	process.detach();
}

void XinePart::initActions()
{
	KAction* action = NULL;
	/* file menu */
	m_broadcastSend = new KToggleAction(i18n("&Send Broadcast Stream..."), 0, 0, this, SLOT(slotToggleBroadcastSend()), actionCollection(), "network_send");
	new KAction(i18n("&Receive Broadcast Stream..."), "network", 0, this, SLOT(slotBroadcastReceive()), actionCollection(), "network_receive");
	new KAction(i18n("&Save Screenshot..."), "frame_image", CTRL|Key_S, this, SLOT(slotScreenshot()), actionCollection(), "file_save_screenshot");
	action = new KAction(i18n("Save Stream..."), "player_record", Key_R, this, SLOT(slotSaveStream()), actionCollection(), "file_save_stream");
	action->setWhatsThis(i18n("Saves current stream to harddisc. This feature was disabled for some formats (e.g. Real Media) to prevent potential legal problems."));

	/* player menu */

	new KAction(i18n("Toggle Minimal Mode"), 0, 0, this, SIGNAL(signalToggleMinimalMode()), actionCollection(), "player_minimal_mode");

	new KAction(i18n("Play"), "player_play", 0, this, SLOT(slotPlay()), actionCollection(), "player_play");
	m_pauseButton = new KToggleAction(i18n("Pause"), "player_pause", Key_Space, this, SLOT(slotTogglePause()), actionCollection(), "player_pause");
	new KAction(i18n("&Next"), "player_end", Key_PageDown, this, SLOT(slotNext()), actionCollection(), "player_next");
	new KAction(i18n("&Previous"), "player_start", Key_PageUp, this, SLOT(slotPrevious()), actionCollection(), "player_previous");
	new KAction(i18n("Stop"), "player_stop", Key_Backspace, this, SLOT(slotStop()), actionCollection(), "player_stop");

	new KAction(i18n("&Fast Forward"), "player_fwd", ALT|Key_Right, this, SLOT(slotFastForward()), actionCollection(), "player_ff");
	new KAction(i18n("Slow &Motion"), 0, ALT|Key_Left, this, SLOT(slotSlowMotion()), actionCollection(), "player_slowmotion");

	new KAction(i18n("Skip Forward (20s)"), NULL, Key_Right, this, SLOT(slotPosPlusSmall()), actionCollection(), "player_posplus_small");
	new KAction(i18n("Skip Backward (20s)"), NULL, Key_Left, this, SLOT(slotPosMinusSmall()), actionCollection(), "player_posminus_small");
	new KAction(i18n("Skip Forward (1m)"), NULL, CTRL|Key_PageUp, this, SLOT(slotPosPlusMedium()), actionCollection(), "player_posplus_medium");
	new KAction(i18n("Skip Backward (1m)"), NULL, CTRL|Key_PageDown, this, SLOT(slotPosMinusMedium()), actionCollection(), "player_posminus_medium");
	new KAction(i18n("Skip Forward (10m)"), NULL, ALT|Key_PageUp, this, SLOT(slotPosPlusLarge()), actionCollection(), "player_posplus_large");
	new KAction(i18n("Skip Backward (10m)"), NULL, ALT|Key_PageDown, this, SLOT(slotPosMinusLarge()), actionCollection(), "player_posminus_large");
	new KAction(i18n("Jump to Position..."), "goto", CTRL|Key_J, this, SLOT(slotJumpToPosition()), actionCollection(), "player_jump_to");

	new KAction(i18n("DVD Menu Left"), 0, CTRL|Key_Left, this, SLOT(slotDVDMenuLeft()), actionCollection(), "dvdmenuleft");
	new KAction(i18n("DVD Menu Right"), 0, CTRL|Key_Right, this, SLOT(slotDVDMenuRight()), actionCollection(), "dvdmenuright");
	new KAction(i18n("DVD Menu Up"), 0, CTRL|Key_Up, this, SLOT(slotDVDMenuUp()), actionCollection(), "dvdmenuup");
	new KAction(i18n("DVD Menu Down"), 0, CTRL|Key_Down, this, SLOT(slotDVDMenuDown()), actionCollection(), "dvdmenudown");
	new KAction(i18n("DVD Menu Select"), 0, CTRL|Key_Return, this, SLOT(slotDVDMenuSelect()), actionCollection(), "dvdmenuselect");

	m_audioChannels = new KSelectAction(i18n("Audio Channel"), 0, actionCollection(), "audio_channels");
	m_audioChannels->setToolTip(i18n("Select audio channel"));
	m_audioChannels->setComboWidth( 50 );
	connect(m_audioChannels, SIGNAL(activated(int)), this, SLOT(slotSetAudioChannel(int )));
	new KAction(i18n("&Next Audio Channel"), 0, 0, this, SLOT(slotNextAudioChannel()), actionCollection(), "next_audio_channels");
	m_audioVisual = new KSelectAction(i18n("Audio &Visualization"), 0, actionCollection(), "audio_visualization");
	connect(m_audioVisual, SIGNAL(activated(const QString&)), m_xine, SLOT(slotSetVisualPlugin(const QString&)));
	new KAction(i18n("&Mute"), "player_mute", Key_U, this, SLOT(slotMute()), actionCollection(), "audio_mute");
	new KAction(i18n("Volume Up"), NULL, Key_Plus, this, SLOT(slotVolumeUp()), actionCollection(), "volume_increase");
	new KAction(i18n("Volume Down"), NULL, Key_Minus, this, SLOT(slotVolumeDown()), actionCollection(), "volume_decrease");

	m_deinterlaceEnabled = new KToggleAction(i18n("&Deinterlace"), 0, Key_I, m_xine, SLOT(slotToggleDeinterlace()), actionCollection(), "video_deinterlace");
	m_deinterlaceEnabled->setWhatsThis(i18n("Activate this for interlaced streams, some DVD's for example."));
	new KAction(i18n("&Auto"), "viewmagfit", Key_F5, m_xine, SLOT(slotAspectRatioAuto()), actionCollection(), "aspect_auto");
	new KAction(i18n("&4:3"), "viewmagfit", Key_F6, m_xine, SLOT(slotAspectRatio4_3()), actionCollection(), "aspect_43");
	new KAction(i18n("A&namorphic"), "viewmagfit", Key_F7, m_xine, SLOT(slotAspectRatioAnamorphic()), actionCollection(), "aspect_anamorphic");
	new KAction(i18n("&DVB"), "viewmagfit", Key_F8, m_xine, SLOT(slotAspectRatioDVB()), actionCollection(), "aspect_dvb");
	new KAction(i18n("&Square"), "viewmagfit", Key_F9, m_xine, SLOT(slotAspectRatioSquare()), actionCollection(), "aspect_square");
	KStdAction::zoomIn(m_xine, SLOT(slotZoomIn()), actionCollection(), "zoom_in");
	KStdAction::zoomOut(m_xine, SLOT(slotZoomOut()), actionCollection(), "zoom_out");
	KStdAction::fitToPage(m_xine, SLOT(slotZoomOff()), actionCollection(), "zoom_off");
	new KAction(i18n("Zoom In Horizontal"), NULL, CTRL|Key_H, m_xine, SLOT(slotZoomInX()), actionCollection(), "zoom_in_x");
	new KAction(i18n("Zoom Out Horizontal"), NULL, CTRL|SHIFT|Key_H, m_xine, SLOT(slotZoomOutX()), actionCollection(), "zoom_out_x");
	new KAction(i18n("Zoom In Vertical"), NULL, CTRL|Key_V, m_xine, SLOT(slotZoomInY()), actionCollection(), "zoom_in_y");
	new KAction(i18n("Zoom Out Vertical"), NULL, CTRL|SHIFT|Key_V, m_xine, SLOT(slotZoomOutY()), actionCollection(), "zoom_out_y");
	new KAction(i18n("Deinterlace &Quality"), "blend", CTRL|Key_I, this, SLOT(slotDeinterlaceQuality()), actionCollection(), "video_deinterlace_quality");
	new KAction(i18n("&Video Settings"), "configure", Key_V, this, SLOT(slotPictureSettings()), actionCollection(), "video_picture");
	new KAction(i18n("&Equalizer"), NULL, Key_E, this, SLOT(slotEqualizer()), actionCollection(), "equalizer");


	m_subtitles = new KSelectAction(i18n("Subtitle"), 0, actionCollection(), "player_subtitles");
	m_subtitles->setToolTip(i18n("Select Subtitle"));
	m_subtitles->setComboWidth( 50 );
	connect(m_subtitles, SIGNAL(activated(int)), this, SLOT(slotSetSubtitle(int)));
	new KAction(i18n("&Next Subtitle Channel"), 0, 0, this, SLOT(slotNextSubtitleChannel()), actionCollection(), "next_player_subtitles");
	new KAction(i18n("Delay Subtitle"), 0, CTRL|ALT|Key_Left, this, SLOT(slotDelaySubTitle()), actionCollection(), "adv_sub");
	new KAction(i18n("Advance Subtitle"), 0, CTRL|ALT|Key_Right, this, SLOT(slotAdvanceSubTitle()), actionCollection(), "delay_sub");
	new KAction(i18n("Add subtitle..."), 0, 0, this, SLOT(slotAddSubtitle()), actionCollection(), "add_subtitle");

	new KAction(i18n("&Menu Toggle"), "view_detailed", Key_D, m_xine, SLOT(slotMenuToggle()), actionCollection(), "dvd_toggle");
	new KAction(i18n("&Title"), NULL, 0, m_xine, SLOT(slotMenuTitle()), actionCollection(), "dvd_title");
	new KAction(i18n("&Root"), NULL, 0, m_xine, SLOT(slotMenuRoot()), actionCollection(), "dvd_root");
	new KAction(i18n("&Subpicture"), NULL, 0, m_xine, SLOT(slotMenuSubpicture()), actionCollection(), "dvd_subpicture");
	new KAction(i18n("&Audio"), NULL, 0, m_xine, SLOT(slotMenuAudio()), actionCollection(), "dvd_audio");
	new KAction(i18n("An&gle"), NULL, 0, m_xine, SLOT(slotMenuAngle()), actionCollection(), "dvd_angle");
	new KAction(i18n("&Part"), NULL, 0, m_xine, SLOT(slotMenuPart()), actionCollection(), "dvd_part");

	m_dvdTitles = new KSelectAction(i18n("Titles"), 0, actionCollection(), "dvd_title_menu");
	connect(m_dvdTitles, SIGNAL(activated(const QString&)), this, SLOT(slotSetDVDTitle(const QString&)));
	m_dvdChapters = new KSelectAction(i18n("Chapters"), 0, actionCollection(), "dvd_chapter_menu");
	connect(m_dvdChapters, SIGNAL(activated(const QString&)), this, SLOT(slotSetDVDChapter(const QString&)));
	m_dvdAngles = new KSelectAction(i18n("Angles"), 0, actionCollection(), "dvd_angle_menu");
	connect(m_dvdAngles, SIGNAL(activated(const QString&)), this, SLOT(slotSetDVDAngle(const QString&)));

	new KAction(i18n("Track &Info"), "info", 0 , this, SLOT(slotInfo()), actionCollection(), "player_track_info");
	new KAction(i18n("Effect &Plugins..."), "filter", Key_X, this, SLOT(slotFilterDialog()), actionCollection(), "player_post_filters");

	/* settings menu */
	new KAction(i18n("&xine Engine Parameters"), "edit", 0, this, SLOT(slotConfigXine()), actionCollection(), "settings_xine_parameter");

	m_volume = new VolumeSlider();
	QToolTip::add
		(m_volume, i18n("Volume"));
	m_volume->setRange(0, 100);
	m_volume->setSteps(1, 10);
	m_volume->setFocusPolicy(QWidget::NoFocus);
	m_volume->setFixedWidth(75);
	connect(m_volume, SIGNAL(valueChanged(int)), this, SLOT(slotVolumeChanged(int)));
	connect(m_xine, SIGNAL(signalSyncVolume()), this, SLOT(slotSyncVolume()));
	new KWidgetAction(m_volume, i18n("Volume"), 0, 0, 0, actionCollection(), "audio_volume");

	m_position = new PositionSlider(Horizontal);
	QToolTip::add
		(m_position, i18n("Position"));
	m_position->setRange(0, 65535);
	m_position->setSteps(100, 1000);
	m_position->setTracking(false);
	m_position->setFocusPolicy(QWidget::NoFocus);
	m_position->setMinimumWidth(180);
	connect(m_position, SIGNAL(sliderMoved(int)), m_xine, SLOT(slotSeekToPosition(int)));
	connect(m_position, SIGNAL(sliderLastMove(int)), m_xine, SLOT(slotSeekToPositionBlocking(int)));
	connect(m_position, SIGNAL(signalStartSeeking()), m_xine, SLOT(slotStartSeeking()));
	connect(m_position, SIGNAL(signalStopSeeking()), m_xine, SLOT(slotStopSeeking()));
	new KWidgetAction(m_position, i18n("Position"), 0, 0, 0, actionCollection(), "player_position");

	m_playTime = new QPushButton(0);
	QToolTip::add
		(m_playTime, i18n("Short click: Toggle Timer Forward/Backward\nLong click: Toggle Timer OSD"));
	QFontMetrics met(KGlobalSettings::generalFont());
	m_playTime->setFixedWidth(met.width("-55:55:55") + 6);
	m_playTime->setSizePolicy(QSizePolicy (QSizePolicy::Fixed, QSizePolicy::Fixed));
	m_playTime->setFocusPolicy(QWidget::NoFocus);
	new KWidgetAction(m_playTime, i18n("Playtime"), 0, 0, 0, actionCollection(), "player_playtime");
	connect(m_playTime, SIGNAL(pressed()), this, SLOT(slotButtonTimerPressed()));
	connect(m_playTime, SIGNAL(released()), this, SLOT(slotButtonTimerReleased()));
	m_playTime->setText("0:00:00");

	m_equalizer = new Equalizer();
	connect(m_equalizer, SIGNAL(signalNewEq30(int)), m_xine, SLOT(slotSetEq30(int)));
	connect(m_equalizer, SIGNAL(signalNewEq60(int)), m_xine, SLOT(slotSetEq60(int)));
	connect(m_equalizer, SIGNAL(signalNewEq125(int)), m_xine, SLOT(slotSetEq125(int)));
	connect(m_equalizer, SIGNAL(signalNewEq250(int)), m_xine, SLOT(slotSetEq250(int)));
	connect(m_equalizer, SIGNAL(signalNewEq500(int)), m_xine, SLOT(slotSetEq500(int)));
	connect(m_equalizer, SIGNAL(signalNewEq1k(int)), m_xine, SLOT(slotSetEq1k(int)));
	connect(m_equalizer, SIGNAL(signalNewEq2k(int)), m_xine, SLOT(slotSetEq2k(int)));
	connect(m_equalizer, SIGNAL(signalNewEq4k(int)), m_xine, SLOT(slotSetEq4k(int)));
	connect(m_equalizer, SIGNAL(signalNewEq8k(int)), m_xine, SLOT(slotSetEq8k(int)));
	connect(m_equalizer, SIGNAL(signalNewEq16k(int)), m_xine, SLOT(slotSetEq16k(int)));
	connect(m_equalizer, SIGNAL(signalSetVolumeGain(bool)), m_xine, SLOT(slotSetVolumeGain(bool)));

}

void XinePart::initConnections()
{
	connect(&m_posCheckTimer, SIGNAL(timeout()), this, SLOT(slotCheckMoved()));
	connect(&m_osdTimerEnabler, SIGNAL(timeout()), this, SLOT(slotToggleOsdTimer()));
	connect(m_xine, SIGNAL(signalXineReady()), this, SLOT(slotFinalize()));
	connect(m_xine, SIGNAL(signalNewChannels(const QStringList&, const QStringList&, int, int )),
	        this, SLOT(slotChannelInfo(const QStringList&, const QStringList&, int, int )));
	connect(m_xine, SIGNAL(signalXinePlaying()), this, SLOT(slotTrackPlaying()));
	connect(m_xine, SIGNAL(signalNewPosition(int, const QTime&)), this, SLOT(slotNewPosition(int, const QTime&)));
	connect(m_xine, SIGNAL(signalXineStatus(const QString&)), this, SLOT(slotStatus(const QString&)));
	connect(m_xine, SIGNAL(signalXineError(const QString&)), this, SLOT(slotError(const QString&)));
	connect(m_xine, SIGNAL(signalXineMessage(const QString&)), this, SLOT(slotMessage(const QString&)));
	connect(m_xine, SIGNAL(signalPlaybackFinished()), this, SLOT(slotPlaybackFinished()));
	connect(m_xine, SIGNAL(signalTitleChanged()), this, SLOT(slotNewTitle()));
	connect(m_xine, SIGNAL(signalLengthChanged()), this, SLOT(slotNewLength()));
	connect(m_xine, SIGNAL(signalVideoSizeChanged()), this, SLOT(slotNewFrameSize()));
	connect(m_xine, SIGNAL(signalRightClick(const QPoint&)), this, SLOT(slotContextMenu(const QPoint&)));
}

void XinePart::loadConfig()
{
	kdDebug() << "XinePart: load config" << endl;

	KConfig* config = instance()->config();

	config->setGroup("General Options");
	if (m_xine->SoftwareMixing())
	{
		int vol = config->readNumEntry("Volume", 70);
		slotSetVolume(vol);
	}
	else
		slotSyncVolume();
	m_timerDirection = config->readNumEntry("Timer Direction", FORWARD_TIMER);
	m_isOsdTimer = config->readBoolEntry("Osd Timer", false);

	config->setGroup("Visualization");
	QString visual = config->readEntry("Visual Plugin", "goom");
	m_audioVisual->setCurrentItem(m_audioVisual->items().findIndex(visual));
	m_xine->slotSetVisualPlugin(visual);

	config->setGroup("Deinterlace");
	m_lastDeinterlaceQuality = config->readNumEntry("Quality Level", 4);
	m_lastDeinterlacerConfig = config->readEntry("Config String", DEFAULT_TVTIME_CONFIG);
	DeinterlacerConfigDialog* deinterlacerConfigDialog = new DeinterlacerConfigDialog();
	m_xine->createDeinterlacePlugin(m_lastDeinterlacerConfig, deinterlacerConfigDialog->getMainWidget());
	m_deinterlacerConfigWidget = (QWidget*)deinterlacerConfigDialog;
	bool deinterlaceEnabled = config->readBoolEntry("Enabled", true);
	if (deinterlaceEnabled)
	{
		m_deinterlaceEnabled->setChecked(deinterlaceEnabled);
		m_xine->slotToggleDeinterlace();
	}

	config->setGroup("Broadcasting Options");
	m_broadcastPort = config->readNumEntry("Port", 8080);
	m_broadcastAddress = config->readEntry("Master Address", "localhost");

	config->setGroup( "Video Settings" );
	slotSetHue(config->readNumEntry( "Hue", -1));
	slotSetSaturation(config->readNumEntry( "Saturation", -1));
	slotSetContrast(config->readNumEntry( "Contrast", -1));
	slotSetBrightness(config->readNumEntry( "Brigthness", -1));

	m_equalizer->ReadValues(config);
}

void XinePart::saveConfig()
{
	if (!m_audioVisual->items().count()) // no config loaded
		return;

	kdDebug() << "XinePart: save config" << endl;

	KConfig* config = instance()->config();

	config->setGroup("General Options");
	config->writeEntry("Volume", m_volume->value());
	config->writeEntry("Timer Direction", m_timerDirection);
	config->writeEntry("Osd Timer", m_isOsdTimer);

	config->setGroup("Visualization");
	config->writeEntry("Visual Plugin", m_audioVisual->currentText());

	config->setGroup("Deinterlace");
	config->writeEntry("Quality Level", m_lastDeinterlaceQuality);
	config->writeEntry("Config String", m_lastDeinterlacerConfig);
	config->writeEntry("Enabled", m_deinterlaceEnabled->isChecked());

	config->setGroup("Broadcasting Options");
	config->writeEntry("Port", m_broadcastPort);
	config->writeEntry("Master Address", m_broadcastAddress);

	config->setGroup( "Video Settings" );
	config->writeEntry( "Hue", m_hue );
	config->writeEntry( "Saturation", m_saturation );
	config->writeEntry( "Contrast", m_contrast );
	config->writeEntry( "Brigthness", m_brightness );

	m_equalizer->SaveValues(config);
}

/* check if shell was moved, send new global position
   of the part to xine */
void XinePart::slotCheckMoved()
{
	QPoint newPos = m_xine->mapToGlobal(QPoint(0,0));
	if (newPos != m_oldPosition)
	{
		m_xine->globalPosChanged();
		m_oldPosition = newPos;
	}
}

bool XinePart::isPlaying()
{
	return m_xine->isPlaying();
}

bool XinePart::isPaused()
{
	return (m_xine->getSpeed() == KXineWidget::Pause);
}

#if 0
void XinePart::audiocdMRLS(MRL::List& mrls, bool& ok, bool& supported, const QString& device)
{
	if (!m_xine->isXineReady())
	{
		if (!m_xine->initXine())
		{
			supported = false;
			return;
		}
	}
	supported = true;

	if (!device.isNull())
		m_xine->slotSetAudiocdDevice(device);

	QStringList list;
	if (!m_xine->getAutoplayPluginURLS("CD", list))
	{
		ok = false;
		return;
	}

	MRL mrl;
	/* use xine to connect to CDDB */
	xine_stream_t* xineStreamForMeta = xine_stream_new((xine_t*)m_xine->getXineEngine(), NULL, NULL);

	KProgressDialog* progress = new KProgressDialog(0, "cddbprogress", QString::null, i18n("Looking for CDDB entries..."));
	progress->progressBar()->setTotalSteps(list.count());
	progress->show();
	QString title;
	bool cddb = true;
	for (uint i = 0; i < list.count(); i++)
	{
		mrl = MRL(list[i]);
		mrl.setTitle(i18n("AudioCD Track %1").arg(i+1));
		mrl.setTrack(QString::number(i+1));
		if (xine_open(xineStreamForMeta, QFile::encodeName(mrl.url())))
		{
			if (cddb)
			{
				title = QString::fromUtf8(xine_get_meta_info(xineStreamForMeta, XINE_META_INFO_TITLE));
				if ((!title.isNull()) && (!title.isEmpty()) )  //no meta info
				{
					mrl.setTitle(title);
					mrl.setArtist(QString::fromUtf8(xine_get_meta_info(xineStreamForMeta, XINE_META_INFO_ARTIST)));
					mrl.setAlbum(QString::fromUtf8(xine_get_meta_info(xineStreamForMeta, XINE_META_INFO_ALBUM)));
					mrl.setYear(QString::fromUtf8(xine_get_meta_info(xineStreamForMeta, XINE_META_INFO_YEAR)));
					mrl.setGenre(QString::fromUtf8(xine_get_meta_info(xineStreamForMeta, XINE_META_INFO_GENRE)));
					mrl.setTrack(QString::number(i+1));
				}
				else
					cddb = false;
			}

			int pos, time, len;
			int t = 0, ret = 0;
			while(((ret = xine_get_pos_length(xineStreamForMeta, &pos, &time, &len)) == 0) && (++t < 5))
				xine_usec_sleep(100000);
			if ( ( ret != 0 ) && (len > 0) )
				mrl.setLength(QTime().addMSecs(len));

			xine_close( xineStreamForMeta );
		}

		mrl.setMime("audio/cd");
		mrls.append(mrl);
		if (progress->wasCancelled())
			break;
		progress->progressBar()->setProgress(i+1);
		KApplication::kApplication()->processEvents();
	}

	xine_dispose(xineStreamForMeta);
	delete progress;
	if (mrls.count())
		ok = true;
}

void XinePart::vcdMRLS(MRL::List& mrls, bool& ok, bool& supported, const QString& device)
{
	if (!m_xine->isXineReady())
	{
		if (!m_xine->initXine())
		{
			supported = false;
			return;
		}
	}
	supported = true;

	if (!device.isNull())
		m_xine->slotSetVcdDevice(device);

	QStringList list;
	if (!m_xine->getAutoplayPluginURLS("VCD", list))
	{
		if (!m_xine->getAutoplayPluginURLS("VCDO", list))
		{
			ok = false;
			return;
		}
	}

	MRL mrl;
	for (uint i = 0; i < list.count(); i++)
	{
		mrl = MRL(list[i]);
		mrl.setMime("video/vcd");
		mrl.setTrack(QString::number(i+1));
		mrl.setTitle(i18n("VCD Track %1").arg(i+1));
		mrls.append(mrl);
	}
	if (mrls.count())
		ok = true;
}

void XinePart::dvdMRLS(MRL::List& mrls, bool& ok, bool& supported, const QString& device)
{
	if (!m_xine->isXineReady())
	{
		if (!m_xine->initXine())
		{
			supported = false;
			return;
		}
	}
	supported = true;

	if (!device.isNull())
		m_xine->slotSetDvdDevice(device);

	QStringList list;
	if (!m_xine->getAutoplayPluginURLS("DVD", list))
	{
		ok = false;
		return;
	}

	MRL mrl;
	for (uint i = 0; i < list.count(); i++)
	{
		mrl = MRL(list[i]);
		mrl.setMime("video/dvd");
		mrl.setTitle("DVD");
		mrl.setTrack(QString::number(i+1));
		mrls.append(mrl);
	}
	if (mrls.count())
		ok = true;
}
#endif

bool XinePart::hasChapters()
{
	if (m_xine->isXineReady())
		return m_xine->hasChapters();
	else
		return false;
}

bool XinePart::hasVideo()
{
	return m_xine->hasVideo();
}

void XinePart::playNextChapter()
{
	if (m_xine->isXineReady())
		m_xine->playNextChapter();
}

void XinePart::playPreviousChapter()
{
	if (m_xine->isXineReady())
		m_xine->playPreviousChapter();
}

void XinePart::slotPrepareForFullscreen(bool fullscreen)
{
	if (fullscreen)
		m_xine->startMouseHideTimer();
	else
		m_xine->stopMouseHideTimer();
}

uint XinePart::volume() const
{
	if (!m_xine->isXineReady())
		return 0;

	return m_xine->getVolume();
}

uint XinePart::position() const
{
	if (!m_xine->isXineReady())
		return 0;

	if ( m_xine->isPlaying() )
		return currentPosition;
	else
		return 0;
}

void XinePart::slotSetVolume(uint vol)
{
	if (!m_xine->isXineReady())
		return;

	kdDebug() << "Set volume to: " << vol << endl;
	m_volume->setValue(vol);
}

void XinePart::slotVolumeChanged(int vol)
{
	m_xine->slotSetVolume(vol);
}

void XinePart::slotSyncVolume()
{
	if (!m_xine->isXineReady())
		return;

	uint vol = volume();
	slotSetVolume(vol);
}

void XinePart::slotSetPosition(uint pos)
{
	if (!m_xine->isXineReady())
		return;

	m_xine->slotSeekToPosition((int)(pos * 655.35));
}

QString XinePart::supportedExtensions()
{
	if (!m_xine->isXineReady())
		return QString::null;

	QString ext  = m_xine->getSupportedExtensions();
	ext = ext.remove("txt");
	ext = "*." + ext;
	ext.append(" smil");
	ext = ext.replace( ' ', " *." );
	ext = ext + " " + ext.upper();

	return ext;
}

void* XinePart::engine()
{
	if (!m_xine->isXineReady())
		return NULL;

	return (void*)m_xine->getXineEngine();
}

void XinePart::slotDisableAllActions()
{
	stateChanged("xine_not_ready");
}

void XinePart::slotEnableAllActions()
{
	stateChanged("xine_not_ready", StateReverse);
	stateChanged("not_playing");
}

void XinePart::slotEnablePlayActions()
{
	if ((m_playlist.count() > 1) || (m_xine->hasChapters())) // we need next/previous buttons
		stateChanged("play_multiple_tracks");
	else
		stateChanged("play_single_track");
}

void XinePart::slotNextAudioChannel()
{
	nextAudioChannel();
}

void XinePart::slotNextSubtitleChannel()
{
	nextSubtitleChannel();
}

/********* DCOP INTERFACE *********/

void XinePart::nextAudioChannel()
{
	int num = m_audioChannels->items().count();
	int index = m_audioChannels->currentItem()+1;
	if ( index>=num )
		index = 0;
	m_audioChannels->setCurrentItem( index );
	slotSetAudioChannel( index );
}

void XinePart::nextSubtitleChannel()
{
	int num = m_subtitles->items().count();
	int index = m_subtitles->currentItem()+1;
	if ( index>=num )
		index = 0;
	m_subtitles->setCurrentItem( index );
	slotSetSubtitle( index );
}

int XinePart::getContrast()
{
	int hue, sat, contrast, bright, avOffset, spuOffset;
	if (!m_xine->isXineReady())
		return -1;
	m_xine->getVideoSettings(hue, sat, contrast, bright, avOffset, spuOffset);
	return contrast;
}

void XinePart::setContrast(int c)
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotSetContrast(c);
}

int XinePart::getBrightness()
{
	int hue, sat, contrast, bright, avOffset, spuOffset;
	if (!m_xine->isXineReady())
		return -1;
	m_xine->getVideoSettings(hue, sat, contrast, bright, avOffset, spuOffset);
	return bright;
}

void XinePart::setBrightness(int b)
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotSetBrightness(b);
}

void XinePart::dvdMenuUp()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotDVDMenuUp();
}

void XinePart::dvdMenuDown()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotDVDMenuDown();
}

void XinePart::dvdMenuLeft()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotDVDMenuLeft();
}

void XinePart::dvdMenuRight()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotDVDMenuRight();
}

void XinePart::dvdMenuSelect()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotDVDMenuSelect();
}

void XinePart::dvdMenuToggle()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotMenuToggle();
}

void XinePart::aspectRatioAuto()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotAspectRatioAuto();
}

void XinePart::aspectRatio4_3()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotAspectRatio4_3();
}

void XinePart::aspectRatioAnamorphic()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotAspectRatioAnamorphic();
}

void XinePart::aspectRatioSquare()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotAspectRatioSquare();
}

void XinePart::aspectRatioDVB()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotAspectRatioDVB();
}

void XinePart::zoomInX()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotZoomInX();
}

void XinePart::zoomOutX()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotZoomOutX();
}

void XinePart::zoomInY()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotZoomInY();
}

void XinePart::zoomOutY()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotZoomOutY();
}

void XinePart::zoomIn()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotZoomIn();
}

void XinePart::zoomOut()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotZoomOut();
}

void XinePart::zoomOff()
{
	if (!m_xine->isXineReady())
		return;
	m_xine->slotZoomOff();
}



/********** volume slider ****************/


VolumeSlider::VolumeSlider() : QSlider(Horizontal, 0)
{
	installEventFilter(this);
}

VolumeSlider::~VolumeSlider()
{}

void VolumeSlider::wheelEvent(QWheelEvent* e)
{
	int newVal = value();
	if (e->delta() > 0)
		newVal -= 5;
	else if (e->delta() < 0)
		newVal += 5;
	setValue(newVal);
	e->accept();
}

/*bool VolumeSlider::eventFilter(QObject *obj, QEvent *ev)
{
	if( obj == this && (ev->type() == QEvent::MouseButtonPress ||
	                    ev->type() == QEvent::MouseButtonDblClick) )
	{
		QMouseEvent *e = (QMouseEvent *)ev;
		QRect r = sliderRect();

		if( r.contains( e->pos() ) || e->button() != LeftButton )
			return FALSE;

		int range = maxValue() - minValue();
		int pos = (orientation() == Horizontal) ? e->pos().x() : e->pos().y();
		int maxpos = (orientation() == Horizontal) ? width() : height();
		int value = pos * range / maxpos + minValue();

		if (QApplication::reverseLayout())
			value = maxValue() - (value - minValue());

		setValue(value);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}*/

#include "xine_part.moc"
