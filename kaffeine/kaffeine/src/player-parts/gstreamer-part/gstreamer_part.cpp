/*
 * gstreamer_part.cpp
 *
 * Copyright (C) 2006 Christophe Thommeret <hftom@free.fr>
 * Copyright (C) 2004-2005 Jürgen Kofler <kaffeine@gmx.net>
 *
 * based on kiss by Ronald Bultje <rbultje@ronald.bitfreak.net>
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



#include <kparts/genericfactory.h>
#include <kaction.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kxmlguifactory.h>
#include <ktoolbar.h>
#include <kio/netaccess.h>
#include <kmimetype.h>
#include <kpopupmenu.h>

#include <qtimer.h>
#include <qslider.h>
#include <qfile.h>
#include <qtooltip.h>

#include "gstreamer_part.h"
#include "gstreamer_part.moc"
#include "playlistimport.h"

#define TIMER_EVENT_PLAYBACK_FINISHED   100
#define TIMER_EVENT_ERROR               102
#define TIMER_EVENT_NEW_STATE           103
#define TIMER_EVENT_FOUND_TAG           104



typedef KParts::GenericFactory<GStreamerPart> GStreamerPartFactory;
K_EXPORT_COMPONENT_FACTORY (libgstreamerpart, GStreamerPartFactory);


GStreamerPart::GStreamerPart( QWidget* parentWidget, const char* /*widgetName*/, QObject* parent, const char* name,
                             const QStringList& /*args*/ )
		: KaffeinePart( parent, name ? name : "GStreamerPart" ),
		m_play(NULL), m_videosink(NULL), m_audiosink(NULL), m_visual(NULL), m_videoSettings(NULL),
		m_gstConfig(NULL), m_mute(false), m_logoPath(QString::null), m_posToolbar(NULL)
{
	// we need an instance
	setInstance( GStreamerPartFactory::instance() );
	parentWidget->setPaletteBackgroundColor( QColor(0,0,0) ); //black

	bus = NULL;

	loadConfig();

	if ( !initGStreamer() )
	{
		kdError() << "GStreamerPart: Initializing of GStreamer failed!" << endl;
		emit canceled( i18n("GStreamer initializing failed!") );
		return;
	}

	m_status = GST_STATE_NULL;

	kdDebug() << "GStreamerPart: Creating video window" << endl;
	m_video = new VideoWindow( parentWidget, m_videosink );
	connect( m_video, SIGNAL(signalNewFrameSize(const QSize&)), this, SIGNAL(signalNewFrameSize(const QSize&)) );
	m_video->setFocusPolicy( QWidget::ClickFocus );
	setWidget( m_video );

	setXMLFile( "gstreamer_part.rc" );
	initActions();
	stateChanged( "disable_all" );

	emit setStatusBarText( i18n("Ready") );

	// be careful - we may be embedded
	m_logoPath = locate( "data", "kaffeine/logo" );
	kdDebug() << "GStreamerPart: Found logo animation: " << m_logoPath << endl;

	connect( &busTimer, SIGNAL(timeout()), this, SLOT(slotReadBus()) );
}



GStreamerPart::~GStreamerPart()
{
	deletePlaybin();
	saveConfig();
	delete m_timer;
	kdDebug() << "GStreamerPart: destructed" << endl;
}



bool GStreamerPart::isPlaying()
{
	return ( (m_status==GST_STATE_PLAYING) && m_url != m_logoPath );
}



bool GStreamerPart::isPaused()
{
	if ( !m_play )
		return false;
	return ( GST_STATE(m_play)==GST_STATE_PAUSED );
}



bool GStreamerPart::hasVideo()
{
	//FIXME: don't work with all video codecs
	return ( !m_videoCodec.isNull() );
}



uint GStreamerPart::position() const
{
	return (uint)((1.0 / m_timer->getTotalTimeMS()) * m_timer->getCurrentTimeMS() * 100);
}



bool GStreamerPart::closeURL()
{
	slotStop();
	return true;
}



KAboutData *GStreamerPart::createAboutData()
{
	KAboutData* aboutData = new KAboutData( "gstreamerpart", I18N_NOOP("GStreamerPart"),
	                                        "0.1", "GStreamer based player part for Kaffeine.",
	                                        KAboutData::License_GPL,
	                                        "(c) 2005, Jürgen Kofler.", 0, "http://kaffeine.sourceforge.net");
	aboutData->addAuthor("Jürgen Kofler.",0, "kaffeine@gmx.net");

	return aboutData;
}



bool GStreamerPart::openURL(const MRL& mrl)
{
	kdDebug() << "GStreamerPart::openURL(): " << mrl.url() << endl;

	// FIXME: thats not the right place for that
	if ( !m_posToolbar ) {
		m_posToolbar = (KToolBar*)factory()->container( "gstPositionToolBar", this );
		if ( m_posToolbar ) {
			m_posToolbar->setItemAutoSized( m_posToolbar->idAt(0), true ); //set position slider to maximum width
		}
	}

	m_mrl = mrl;
	m_playlist.clear();
	m_current = 0;
	bool playlist = false;

	currentDevice = "";

	QString ext = m_mrl.kurl().fileName();
	ext = ext.remove( 0 , ext.findRev('.')+1 ).lower();

	if ( m_mrl.mime().isNull() ) {
		KMimeType::Ptr mime = KMimeType::findByURL( m_mrl.kurl().path() );
		m_mrl.setMime( mime->name() );
	}

	/* is m_mrl a playlist? */
	if ( (m_mrl.mime() == "text/plain") || (m_mrl.mime() == "text/xml") || (m_mrl.mime() == "application/x-kaffeine")
	        || (m_mrl.mime() == "audio/x-scpls") || (m_mrl.mime() == "audio/x-mpegurl") || (m_mrl.mime() == "audio/mpegurl")
	        || (ext == "asx") || (ext == "asf") || (ext == "wvx") || (ext == "wax") ) /* windows meta files */
	{
		kdDebug() << "GStreamerPart: Check for kaffeine/noatun/m3u/pls/asx playlist\n";
		QString localFile;
		if ( KIO::NetAccess::download(m_mrl.kurl(), localFile, widget()) ) {
			QFile file( localFile );
			file.open( IO_ReadOnly );
			QTextStream stream( &file );
			QString firstLine = stream.readLine();
			QString secondLine = stream.readLine();
			file.close();

			if ( secondLine.contains("kaffeine", false) ) {
				kdDebug() << "GStreamerPart: Try loading kaffeine playlist\n";
				playlist = PlaylistImport::kaffeine( localFile, m_playlist );
			}
			if ( secondLine.contains("noatun", false) ) {
				kdDebug() << "GStreamerPart: Try loading noatun playlist\n";
				playlist = PlaylistImport::noatun( localFile, m_playlist);
			}
			if ( firstLine.contains("asx", false) ) 	{
				kdDebug() << "GStreamerPart: Try loading asx playlist\n";
				playlist = PlaylistImport::asx( localFile, m_playlist );
			}
			if ( firstLine.contains("[playlist]", false) ) {
				kdDebug() << "GStreamerPart: Try loading pls playlist\n";
				playlist = PlaylistImport::pls( localFile, m_playlist );
			}
			if (ext == "m3u") {  //indentify by extension
				kdDebug() << "GStreamerPart: Try loading m3u playlist\n";
				playlist = PlaylistImport::m3u( localFile, m_playlist );
			}
		}
		else
			kdError() << "GStreamerPart: " << KIO::NetAccess::lastErrorString() << endl;
	}
	/* check for ram playlist */
	if ( (ext == "ra") || (ext == "rm") || (ext == "ram") || (ext == "lsc") || (ext == "pl") ) {
		kdDebug() << "GStreamerPart: Try loading ram playlist\n";
		playlist = PlaylistImport::ram( m_mrl, m_playlist, widget() );
	}

	int pos;
	QString s = mrl.url();
	if ( s.startsWith("cdda://") ) {
		s = s.remove("cdda://");
		if ( (pos=s.findRev("/"))>-1 ) {
			currentDevice=s.left( pos );
			s = s.right( s.length()-pos-1 );
		}
		else
			currentDevice=m_device;
		m_mrl.setURL( "cdda://"+s );
	}
	else if ( s.startsWith("dvd://") ) {
		s = s.remove("dvd://");
		if ( s.startsWith("/") )
			currentDevice=s;
		else
			currentDevice=m_device;
		m_mrl.setURL( "dvd://" );
	}
	else if ( s.startsWith("vcd://") ) {
		s = s.remove("vcd://");
		if ( s.startsWith("/") )
			currentDevice=s;
		else
			currentDevice=m_device;
		m_mrl.setURL( "vcd://" );
	}

	if (!playlist)
	{
		kdDebug() << "GStreamerPart: Got single track" << endl;
		m_playlist.append( m_mrl );
	}

	QTimer::singleShot(0, this, SLOT(slotPlay()));
	return true;
}



void GStreamerPart::slotPlay()
{
	if ( (m_play) && (GST_STATE(m_play)==GST_STATE_PAUSED) ) {
		gst_element_set_state( m_play, GST_STATE_PLAYING );
		return;
	}

	if ( m_playlist.count() > 0 ) {
		emit setStatusBarText( i18n("Opening...") );
		MRL curMRL = m_playlist[m_current];
		m_url = curMRL.url();
		QString subUrl = QString::null;
		if ( (!curMRL.subtitleFiles().isEmpty()) && (curMRL.currentSubtitle() > -1) )
			subUrl = curMRL.subtitleFiles()[curMRL.currentSubtitle()];

		gstPlay(m_url, subUrl);
	}
	else {
		emit signalRequestCurrentTrack();
	}
}



void GStreamerPart::slotNext()
{
	if ( (m_playlist.count() > 0) && (m_current < m_playlist.count()-1) )
	{
		m_current++;
		slotPlay();
	}
	else {
		emit signalRequestNextTrack();
	}
}



void GStreamerPart::slotPrevious()
{
	if ( m_current > 0 ) {
		m_current--;
		slotPlay();
	}
	else {
		emit signalRequestPreviousTrack();
	}
}



void GStreamerPart::setDevice( QString device )
{
	if ( !m_play )
		return;

	GObject *source=NULL;
	g_object_get( m_play, "source", &source, NULL );
  	if ( !source ) {
  		kdDebug() << "GStreamer: NO source for 'device' " << device << endl;
    		return;
    	}

    	GObjectClass *klass = G_OBJECT_GET_CLASS( source );
 	if ( g_object_class_find_property( klass, "device" ) ) {
 		kdDebug() << "GStreamer: Set source sink property 'device' to " << device << endl;
 		g_object_set(source, "device", device.ascii(), NULL);
 	}
	g_object_unref (source);
}



void GStreamerPart::slotTogglePause( bool )
{
	if ( !m_play )
		return;

	if ( GST_STATE(m_play)==GST_STATE_PAUSED )
		gst_element_set_state( m_play, GST_STATE_PLAYING );
	else
		gst_element_set_state( m_play, GST_STATE_PAUSED );
}



void GStreamerPart::slotSetPosition( uint percent )
{
	if ( !m_play )
		return;
	m_timer->seekPercent( percent );
}



void GStreamerPart::slotStop()
{
	if ( !m_play )
		return;

	gst_element_set_state( m_play, GST_STATE_READY );
	if ( !m_logoPath.isNull() ) {
		m_url = m_logoPath;
		gstPlay( m_logoPath, QString::null );
	}
}



void GStreamerPart::slotVolume( int vol )
{
	if ( !m_play )
		return;

	emit setStatusBarText(i18n("Volume") + ": " + QString::number(vol) + "%");
	double v = vol * 0.01;
	kdDebug() << "GStreamerPart: Set volume to " << v << endl;
	g_object_set( G_OBJECT(m_play), "volume", v, NULL );
}



void GStreamerPart::slotSetVolume( uint vol )
{
	m_volume->setValue( vol );
}



uint GStreamerPart::volume() const
{
	if ( !m_play )
		return m_volume->value();

	double v;
	g_object_get( G_OBJECT(m_play), "volume", &v, NULL );

	return (uint)(v*100 );
}



void GStreamerPart::slotMute()
{
	m_mute = !m_mute;
	if ( m_mute ) {
		muteVolume = m_volume->value();
		m_volume->setValue( 0 );
		emit setStatusBarText( i18n("Mute") + ": " + i18n("On") );
	}
	else {
		m_volume->setValue( muteVolume );
		emit setStatusBarText( i18n("Mute") + ": " + i18n("Off") );
	}
}



void GStreamerPart::slotSaturation( int sat )
{
	emit setStatusBarText( i18n("Saturation") + ": " + QString::number(sat) );
	g_object_set( G_OBJECT(m_videosink), "saturation", sat, NULL );
}



void GStreamerPart::slotHue( int hue )
{
	emit setStatusBarText( i18n("Hue") + ": " + QString::number(hue) );
	g_object_set( G_OBJECT(m_videosink), "hue", hue, NULL );
}



void GStreamerPart::slotContrast( int contrast )
{
	emit setStatusBarText( i18n("Contrast") + ": " + QString::number(contrast) );
	g_object_set( G_OBJECT(m_videosink), "contrast", contrast, NULL );
}



void GStreamerPart::slotBrightness( int brightness )
{
	emit setStatusBarText( i18n("Brightness") + ": " + QString::number(brightness) );
	g_object_set( G_OBJECT(m_videosink), "brightness", brightness, NULL );
}



void GStreamerPart::gstStateChanged()
{
	if ( m_status == GST_STATE_PAUSED ) {
		kdDebug() << "GStreamerPart: New gstreamer state: PAUSE" << endl;
		emit setStatusBarText( i18n("Pause") );
	}
	else if ( m_status == GST_STATE_PLAYING ) {
		kdDebug() << "GStreamerPart: New gstreamer state: PLAYING" << endl;
		if ( m_url != m_logoPath )
			stateChanged("playing");
		else
			stateChanged("not_playing");
		QString caption = m_mrl.title();
		if ( !m_mrl.artist().isEmpty() )
			caption.append( QString(" (") + m_mrl.artist() + ")" );
		emit setWindowCaption(caption);
		emit setStatusBarText(i18n("Playing"));
	}
	else if ( m_status == GST_STATE_READY ) {
		kdDebug() << "GStreamerPart: New gstreamer state: READY" << endl;
		if ( m_playlist.count() )
			stateChanged( "not_playing" );
		else
			stateChanged( "disable_all" );
		emit setWindowCaption("");
		emit setStatusBarText( i18n("Stop") );
	}
	m_video->newState();
}



void GStreamerPart::slotVideoSettings()
{
	if ( !m_videoSettings ) 	{
		int hue = 0, sat = 0, contr = 0, bright = 0;
		g_object_get( G_OBJECT(m_videosink), "hue", &hue, NULL );
		g_object_get( G_OBJECT(m_videosink), "saturation", &sat, NULL );
		g_object_get( G_OBJECT(m_videosink), "contrast", &contr, NULL );
		g_object_get( G_OBJECT(m_videosink), "brightness", &bright, NULL );

		m_videoSettings = new VideoSettings( hue, sat, contr, bright );
		connect( m_videoSettings, SIGNAL(signalNewBrightness(int)), this, SLOT(slotBrightness(int)) );
		connect( m_videoSettings, SIGNAL(signalNewContrast(int)), this, SLOT(slotContrast(int)) );
		connect( m_videoSettings, SIGNAL(signalNewHue(int)), this, SLOT(slotHue(int)) );
		connect( m_videoSettings, SIGNAL(signalNewSaturation(int)), this, SLOT(slotSaturation(int)) );
	}

	m_videoSettings->show();
}



void GStreamerPart::slotConfigDialog()
{
	if ( m_gstConfig == NULL ) {
		m_gstConfig = new GStreamerConfig( m_audioPluginList, m_videoPluginList );
	}

	m_gstConfig->setAudioDriver( m_audioSinkName );
	m_gstConfig->setVideoDriver( m_videoSinkName );
	m_gstConfig->setDrive( m_device );
	if ( m_gstConfig->exec() == KDialogBase::Accepted ) {
		kdDebug() << "GStreamerPart: Apply new configuration" << endl;
		if ( m_audioSinkName != m_gstConfig->getAudioDriver() )
			setAudioSink( m_gstConfig->getAudioDriver() );
		m_videoSinkName = m_gstConfig->getVideoDriver();
		m_device = m_gstConfig->getDrive();
	}
}



void GStreamerPart::processMetaInfo()
{
	kdDebug() << "GStreamerPart: Processing meta info" << endl;

	MRL mrl = m_playlist[m_current];

	if ( (mrl.title().contains("/") || mrl.title().contains(".") || (mrl.title().isEmpty()))
	        && !m_title.stripWhiteSpace().isEmpty() && m_title.length() > 1 )
		mrl.setTitle( m_title );
	if (mrl.artist().isEmpty() && !m_artist.stripWhiteSpace().isEmpty())
		mrl.setArtist(m_artist);
	if (mrl.album().isEmpty() && !m_album.stripWhiteSpace().isEmpty())
		mrl.setAlbum(m_album);
	if (mrl.genre().isEmpty() && !m_genre.stripWhiteSpace().isEmpty())
		mrl.setGenre(m_genre);
	if (mrl.track().isEmpty() && !m_track.stripWhiteSpace().isEmpty())
		mrl.setTrack(m_track);
	if (mrl.comment().isEmpty() && !m_comment.stripWhiteSpace().isEmpty())
		mrl.setComment(m_comment);
	if (mrl.length().isNull()) 	{
		QTime length = QTime().addMSecs(m_timer->getTotalTimeMS());
		if (!length.isNull())
			mrl.setLength(length);
	}
	m_playlist[m_current] = mrl;

	QString caption = mrl.title();
	if (!mrl.artist().isEmpty())
		caption.append(QString(" (") + mrl.artist() + ")");
	emit setWindowCaption(caption);
	/* if we don't have a playlist emit signalNewMeta() */
	if (mrl.url() == m_mrl.url()) {
		m_mrl = mrl;
		emit signalNewMeta(m_mrl);
	}
}



void GStreamerPart::slotInfo()
{
	QString info;
	QTextStream ts(&info, IO_WriteOnly);
	ts << "<qt><table width=\"90%\">";
	ts << "<tr><td colspan=\"2\"><center><b>" << m_title << "</b></center></td></tr>";
	if (!m_artist.isNull())
		ts << "<tr><td><b>" << i18n("Artist") << ":</b></td><td> " << m_artist << "</td></tr>";
	if (!m_album.isNull())
		ts << "<tr><td><b>" << i18n("Album") << ":</b></td><td> " << m_album << "</td></tr>";
	if (!m_track.isNull())
		ts << "<tr><td><b>" << i18n("Track") << ":</b></td><td> " << m_track << "</td></tr>";
	if (!m_year.isNull())
		ts << "<tr><td><b>" << i18n("Year") << ":</b></td><td> " << m_year << "</td></tr>";
	if (!m_genre.isNull())
		ts << "<tr><td><b>" << i18n("Genre") << ":</b></td><td> " << m_genre << "</td></tr>";
	if (!m_comment.isNull())
		ts << "<tr><td><b>" << i18n("Comment") << ":</b></td><td> " << m_comment << "</td></tr>";

	QTime length = QTime().addMSecs(m_timer->getTotalTimeMS());
	if (!length.isNull())
		ts << "<tr><td><b>" << i18n("Length") << ":</b></td><td> " << length.toString("h:mm:ss") << "</td></tr>";

	ts << "<br><br>";

	ts << "<tr><td><b>" << i18n("Audio") << ":</b></td><td> " << m_audioCodec << "</td></tr>";
	int width = m_video->getFrameSize().width();
	int height =  m_video->getFrameSize().height();
	if (width > 0 && height > 0)
		ts << "<tr><td><b>" << i18n("Video") << ":</b></td><td> " << m_videoCodec << " " << width << "x" << height << "</td></tr>";

	ts << "</table></qt>";

	KMessageBox::information(0, info);
}



void GStreamerPart::slotSetVisualPlugin( const QString& name )
{
	if ( name != "none" ) {
		GstElement* visual = gst_element_factory_make( name.ascii(), "visualization" );
		if ( visual ) {
			g_object_set( G_OBJECT(m_play), "vis-plugin", visual, NULL );
			if ( m_visual )
				g_object_unref( m_visual );
			m_visual = visual;
			m_visualPluginName = name;
		}
		else
			kdWarning() << "GStreamer: Initialization of visualization plugin failed (" << name << ")" << endl;
	}
	else {
		if (m_visual) {
			g_object_set(G_OBJECT (m_play), "vis-plugin", NULL, NULL);
			g_object_unref(m_visual);
			m_visual = NULL;
			m_visualPluginName = "none";
		}
	}
}



void GStreamerPart::slotContextMenu( const QPoint& pos )
{
	if (factory()) {
		KPopupMenu *pop = (KPopupMenu*)factory()->container("context_menu", this);
		if ( pop )
			pop->popup(pos);
	}
}



void GStreamerPart::loadConfig()
{
	kdDebug() << "GStreamerPart: Load config" << endl;
	KConfig* config = instance()->config();
	config->setGroup("General Options");

	m_audioSinkName = config->readEntry("Audio Sink", "alsasink");
	m_videoSinkName = config->readEntry("Video Sink", "xvimagesink");
	m_visualPluginName = config->readEntry("Visual Plugin", "goom");
	m_savedVolume = config->readNumEntry("Volume", 25);
	m_device = config->readEntry("CD Device", "/dev/dvd");
}



void GStreamerPart::saveConfig()
{
	kdDebug() << "GStreamerPart: Save config" << endl;

	KConfig* config = instance()->config();
	config->setGroup("General Options");

	config->writeEntry( "Audio Sink", m_audioSinkName );
	config->writeEntry( "Video Sink", m_videoSinkName );
	config->writeEntry( "Visual Plugin", m_visualPluginName );
	config->writeEntry( "Volume", m_volume->value() );
	config->writeEntry( "CD Device", m_device );
}



void GStreamerPart::slotPrepareForFullscreen( bool fullscreen )
{
	if ( fullscreen )
		m_video->startMouseHideTimer();
	else
		m_video->stopMouseHideTimer();
}



void GStreamerPart::initActions()
{
	new KAction(i18n("Toggle Minimal Mode"), 0, 0, this, SIGNAL(signalToggleMinimalMode()), actionCollection(), "player_minimal_mode");
	new KAction(i18n("Play"), "player_play", 0, this, SLOT(slotPlay()), actionCollection(), "player_play");
	new KAction(i18n("Pause"), "player_pause", Key_Space, this, SLOT(slotTogglePause()), actionCollection(), "player_pause");
	new KAction(i18n("Stop"), "player_stop", Key_Backspace, this, SLOT(slotStop()), actionCollection(), "player_stop");
	new KAction(i18n("&Next"), "player_end", Key_PageDown, this, SLOT(slotNext()), actionCollection(), "player_next");
	new KAction(i18n("&Previous"), "player_start", Key_PageUp, this, SLOT(slotPrevious()), actionCollection(), "player_previous");

	m_timer = new Timer();
	new KWidgetAction(m_timer->getSlider(), i18n("Position"), 0, 0, 0, actionCollection(), "player_position");
	new KWidgetAction((QWidget*)m_timer->getLabel(), i18n("Playtime"), 0, 0, 0, actionCollection(), "player_playtime");

	m_audioVisual = new KSelectAction(i18n("Audio &Visualization"), 0, actionCollection(), "audio_visual");
	connect(m_audioVisual, SIGNAL(activated(const QString&)), this, SLOT(slotSetVisualPlugin(const QString&)));
	m_audioVisualPluginList.prepend("none");
	m_audioVisual->setItems(m_audioVisualPluginList);
	m_audioVisual->setCurrentItem(m_audioVisual->items().findIndex(m_visualPluginName));

	new KAction(i18n("&Mute"), "player_mute", Key_U, this, SLOT(slotMute()), actionCollection(), "audio_mute");
	new KAction(i18n("&Auto"), "viewmagfit", Key_F5, m_video, SLOT(slotAspectRatioAuto()), actionCollection(), "aspect_auto");
	new KAction(i18n("&4:3"), "viewmagfit", Key_F6, m_video, SLOT(slotAspectRatio4_3()), actionCollection(), "aspect_43");
	new KAction(i18n("A&namorphic"), "viewmagfit", Key_F7, m_video, SLOT(slotAspectRatioAnamorphic()), actionCollection(), "aspect_anamorphic");
	new KAction(i18n("&DVB"), "viewmagfit", Key_F8, m_video, SLOT(slotAspectRatioDVB()), actionCollection(), "aspect_dvb");
	new KAction(i18n("&Square"), "viewmagfit", Key_F9, m_video, SLOT(slotAspectRatioSquare()), actionCollection(), "aspect_square");
	new KAction(i18n("&Video Settings"), "configure", Key_V, this, SLOT(slotVideoSettings()), actionCollection(), "video_settings");
	new KAction(i18n("Track &Info"), "info", 0 , this, SLOT(slotInfo()), actionCollection(), "player_track_info");

	m_volume = new QSlider(Horizontal, 0);
	QToolTip::add(m_volume, i18n("Volume"));
	m_volume->setRange(0, 100);
	m_volume->setSteps(1, 5);
	m_volume->setFixedWidth(75);
	connect(m_volume, SIGNAL(valueChanged(int)), this, SLOT(slotVolume(int)));
	slotSetVolume(m_savedVolume);
	new KWidgetAction(m_volume, i18n("Volume"), 0, 0, 0, actionCollection(), "audio_volume");
	new KAction(i18n("&GStreamer Engine Parameters"), "edit", 0, this, SLOT(slotConfigDialog()), actionCollection(), "settings_gst_parameter");

	connect(m_video, SIGNAL(signalRightClick(const QPoint&)), this, SLOT(slotContextMenu(const QPoint&)));
}


//Change audio sink on the fly
void GStreamerPart::setAudioSink( QString sinkName )
{
	GstElement* sink = gst_element_factory_make(sinkName.ascii(), "audiosink");
	if ( !sink ) {
		KMessageBox::error(0, i18n("Error: Can't init new Audio Driver %1 - using %2!").arg(sinkName).arg(m_audioSinkName));
		return;
	}

	if ( m_play )
		g_object_set( G_OBJECT(m_play), "audio-sink", sink, NULL );
	m_audiosink = sink;
	m_audioSinkName = sinkName;
	kdDebug() << "GStreamerPart: Using audio driver: " << m_audioSinkName << endl;
}

/**
  *  Initialize GStreamer
  */
bool GStreamerPart::initGStreamer()
{
	if ( !gst_init_check(NULL, NULL,NULL) ) {
		KMessageBox::error(0, i18n("GStreamer could not be initialized!"));
		return false;
	}

	/* check GStreamer version */
	guint maj, min, mic, nan;
	gst_version(&maj, &min, &mic, &nan);
	kdDebug() << "GStreamerPart: Found GStreamer version "<<maj<<"."<<min<<"."<<mic<<"."<< nan <<endl<<endl;

	/* check for visualization plugins */
	GList* factories = gst_registry_get_feature_list(gst_registry_get_default (), GST_TYPE_ELEMENT_FACTORY);
	QString name, cat;
	while ( factories ) {
		name =  GST_PLUGIN_FEATURE_NAME(factories->data);
		cat = gst_element_factory_get_klass(GST_ELEMENT_FACTORY(factories->data));
		// kdDebug() << "GStreamerPart: Found plugin: " << name << " - Category: " << cat << endl;
		if ( cat == "Visualization" )
			m_audioVisualPluginList.append(name);
		else if ( cat == "Sink/Audio" )
			m_audioPluginList.append(name);
		else if ( cat == "Sink/Video" )
			m_videoPluginList.append(name);
		factories = g_list_next(factories);
	}
	g_list_free( factories );

	/*
	 * Init audio driver
	 */
	m_audiosink = gst_element_factory_make( m_audioSinkName.ascii(), "audiosink" );
	if ( !m_audiosink ) {
		KMessageBox::error(0, i18n("Can't init Audio Driver '%1' - trying another one...").arg(m_audioSinkName));
		if ( (m_audiosink = gst_element_factory_make("alsasink", "audiosink")) != NULL )
			kdDebug() << "GStreamerPart: Using audio-driver: alsasink" << endl;
		else {
			if ( (m_audiosink = gst_element_factory_make("osssink", "audiosink")) != NULL )
				kdDebug() << "GStreamerPart: Using audio-driver: osssink" << endl;
			else {
				if ( (m_audiosink = gst_element_factory_make("artsdsink", "audiosink")) != NULL )
					kdDebug() << "GStreamerPart: Using audio-driver: artsdsink" << endl;
				else {
					KMessageBox::error(0, i18n("No useable audio-driver found!") + " (" + m_audioSinkName + ")");
					return false;
				}
			}
		}
	}
	else
		kdDebug() << "GStreamerPart: Using audio driver: " << m_audioSinkName << endl;
	gst_element_set_state( m_audiosink, GST_STATE_READY );
	/*
	 * Init video driver...
	 */
	m_videosink = gst_element_factory_make( m_videoSinkName.ascii(), "videosink" );
	if ( !m_videosink ) {
		KMessageBox::error(0, i18n("Can't init Video Driver '%1' - trying another one...").arg(m_videoSinkName));
		if ((m_videosink = gst_element_factory_make("xvimagesink", "videosink")) != NULL)
			kdDebug() << "GStreamerPart: Using video-driver: xvimagesink" << endl;
		else {
			if ( (m_videosink = gst_element_factory_make( "ximagesink", "videosink") ) != NULL)
				kdDebug() << "GStreamerPart: Using video-driver: ximagesink" << endl;
			else {
				KMessageBox::error(0, i18n("No useable video-driver found!") + " (" + m_videoSinkName + ")");
				return false;
			}
		}
	}
	else
		kdDebug() << "GStreamerPart: Using video driver: " << m_videoSinkName << endl;
	gst_element_set_state( m_videosink, GST_STATE_READY );

		/*
	 * Visualization
	 */
	kdDebug() << "GStreamerPart: Using visualization plugin: " << m_visualPluginName << endl;
	if ( m_visualPluginName != "none" ) {
		m_visual = gst_element_factory_make( m_visualPluginName.ascii(), "visualization" );
		if ( !m_visual )
			kdWarning() << "GStreamer: Initialization of visualization plugin failed" << endl;
	}

	return true;
}



bool GStreamerPart::createPlaybin()
{
	m_play = gst_element_factory_make( "playbin", "play" );
	if ( !m_play ) {
		KMessageBox::error( 0, i18n("GStreamer playbin could not be initialized!") );
		return false;
	}
	if ( !m_videosink || !m_audiosink ) {
		KMessageBox::error( 0, i18n("GStreamer sink(s) missing, playbin could not be initialized!") );
		gst_object_unref( GST_OBJECT(m_play) );
		return false;
	}

	g_object_set( G_OBJECT(m_play), "video-sink", m_videosink, NULL );
	g_object_set( G_OBJECT(m_play), "audio-sink", m_audiosink, NULL );
	g_object_set( G_OBJECT(m_play), "vis-plugin", m_visual, NULL );
	gst_element_set_state( m_play, GST_STATE_READY );
	slotVolume( m_volume->value() );
	m_video->setPlaybin( m_play );
	m_timer->setPlaybin( m_play );

	bus = gst_pipeline_get_bus( GST_PIPELINE(m_play) );
	busTimer.start( 5 );
	return true;
}



void GStreamerPart::deletePlaybin()
{
	if ( bus ) {
		busTimer.stop();
		gst_object_unref( GST_OBJECT(bus) );
		bus = NULL;
	}
	if ( m_play ) {
		m_video->setPlaybin( NULL );
		m_timer->setPlaybin( NULL );
		gst_element_set_state( m_play, GST_STATE_NULL );
		gst_object_unref( GST_OBJECT(m_play) );
		m_play = NULL;
	}
}



void GStreamerPart::gstPlay( const QString& trackUrl, const QString& subtitleUrl )
{
	if ( !m_play && !createPlaybin() )
		return;

	m_title = QString::null;
	m_artist = QString::null;
	m_album = QString::null;
	m_year = QString::null;
	m_genre = QString::null;
	m_track = QString::null;
	m_comment = QString::null;
	m_audioCodec = QString::null;
	m_videoCodec = QString::null;
	QString url = trackUrl;

	if ( GST_STATE(m_play)!=GST_STATE_READY )
		gst_element_set_state( m_play, GST_STATE_READY );

	/* make sure the xoverlay know the winId */
	m_video->refresh();

	/* KDE has the habit to do "file:/location", whereas we
	       expect "file:///location" */
	if (url.left(7).lower() == "file://") {
		url.insert(6, "/");
	}
	else if (url[0] == '/') {
		url.prepend("file://");
	}
	gchar *uri = g_strdup( url.local8Bit() );
	kdDebug() << "GStreamerPart: play URL: " << uri << endl;
	g_object_set( G_OBJECT(m_play), "uri", uri, NULL );
	g_free( uri );

	//process subtitle url
	if ( !subtitleUrl.isNull() ) {
		QString sub = subtitleUrl;
		if ( sub.left(7).lower() == "file://" ) {
			sub.insert(6, "/");
		}
		else if (sub[0] == '/') {
			sub.prepend("file://");
		}
		// set sub font
		g_object_set( G_OBJECT(m_play), "subtitle-font-desc", "sans bold 18", NULL );

		gchar* suburi = g_strdup( sub.local8Bit() );
		kdDebug() << "GStreamerPart: Setting subtitle URL to " << suburi << endl;
		g_object_set( G_OBJECT(m_play), "suburi", suburi, NULL );
		g_free( suburi );
	}
	else {
		g_object_set( G_OBJECT(m_play), "suburi", NULL, NULL );
	}

	if ( !currentDevice.isEmpty() )
		setDevice( currentDevice );
	gst_element_set_state( m_play, GST_STATE_PLAYING );

	m_timer->start();
}



void GStreamerPart::slotReadBus()
{
	//kdDebug() << "readBus : " << QTime::currentTime().toString("hh:mm:ss:zzz") << endl;
	if ( !bus )
		return;

	GstMessage *msg = gst_bus_pop( bus );
	if ( !msg )
		return;

	gchar *debug = NULL;
	GError *error = NULL;
	GstState old, cur, pending;

	GstElement *src = (GstElement*)GST_MESSAGE_SRC( msg );
	switch( GST_MESSAGE_TYPE( msg ) ) {
		case GST_MESSAGE_STATE_CHANGED:
			if ( src==m_play ) {
				gst_message_parse_state_changed( msg, &old, &cur, &pending );
				if ( cur==old )
					kdDebug() << "GST_MESSAGE_STATE_UNCHANGED" << endl;
				else {
					kdDebug() << "GST_MESSAGE_STATE_CHANGED" << endl;
					m_status = cur;
					gstStateChanged();
				}
			}
			break;
		case GST_MESSAGE_ERROR:
			gst_message_parse_error( msg, &error, &debug );
			emit setStatusBarText(i18n("Error"));
			if (m_url != m_logoPath) {
				errorMessage = error->message;
				errorDetails = debug;
				QTimer::singleShot( 0, this, SLOT(slotEngineError()) );
			}
			g_error_free( error );
			g_free( debug );
			gst_element_set_state( m_play, GST_STATE_NULL );
			//gst_element_set_state( m_play, GST_STATE_READY );
			break;
		case GST_MESSAGE_EOS:
			if ( m_current < m_playlist.count()-1 ) {
				m_current ++;
				slotPlay();
			}
			else {
				kdDebug() << "GStreamerPart: Playback finished" << endl;
				m_timer->stop();
				//m_status = GST_STATE_READY;
				if (m_url != m_logoPath)
					emit signalTrackFinished();
			}
			break;
		case GST_MESSAGE_TAG:
			GstTagList *tagList;
			gst_message_parse_tag( msg, &tagList );
			foundTag( tagList );
			break;
		case GST_MESSAGE_WARNING: kdDebug() << "GST_MESSAGE_WARNING" << endl; break;
		case GST_MESSAGE_INFO: kdDebug() << "GST_MESSAGE_INFO" << endl; break;
		case GST_MESSAGE_BUFFERING: kdDebug() << "GST_MESSAGE_BUFFERING" << endl; break;
		case GST_MESSAGE_STATE_DIRTY: kdDebug() << "GST_MESSAGE_STATE_DIRTY" << endl; break;
		case GST_MESSAGE_STEP_DONE: kdDebug() << "GST_MESSAGE_STEP_DONE" << endl; break;
		case GST_MESSAGE_CLOCK_PROVIDE: kdDebug() << "GST_MESSAGE_CLOCK_PROVIDE" << endl; break;
		case GST_MESSAGE_CLOCK_LOST: kdDebug() << "GST_MESSAGE_CLOCK_LOST" << endl; break;
		case GST_MESSAGE_NEW_CLOCK: kdDebug() << "GST_MESSAGE_NEW_CLOCK" << endl; break;
		case GST_MESSAGE_STRUCTURE_CHANGE: kdDebug() << "GST_MESSAGE_STRUCTURE_CHANGE" << endl; break;
		case GST_MESSAGE_STREAM_STATUS: kdDebug() << "GST_MESSAGE_STREAM_STATUS" << endl; break;
		case GST_MESSAGE_APPLICATION: kdDebug() << "GST_MESSAGE_APPLICATION" << endl; break;
		case GST_MESSAGE_ELEMENT: kdDebug() << "GST_MESSAGE_ELEMENT" << endl; break;
		case GST_MESSAGE_SEGMENT_START: kdDebug() << "GST_MESSAGE_SEGMENT_START" << endl; break;
		case GST_MESSAGE_SEGMENT_DONE: kdDebug() << "GST_MESSAGE_SEGMENT_DONE" << endl; break;
		case GST_MESSAGE_DURATION: kdDebug() << "GST_MESSAGE_DURATION" << endl; break;
		default:
			kdDebug() << "GST_MESSAGE_OTHER" << endl;
	}
	gst_message_unref( msg );
}



void GStreamerPart::slotEngineError()
{
	KMessageBox::detailedError( NULL, errorMessage, errorDetails );
}



void GStreamerPart::foundTag( GstTagList *taglist )
{
	kdDebug() << " Received meta tags..." << endl;

	char* string;
	guint intVal = 0;
	bool success = false;

	if ( gst_tag_list_get_string(taglist, GST_TAG_TITLE, &string) && string ) {
		m_title = string;
		success = true;
	}
	if ( gst_tag_list_get_string( taglist, GST_TAG_ARTIST, &string) && string ) 	{
		m_artist = string;
		success = true;
	}
	if ( gst_tag_list_get_string(taglist, GST_TAG_ALBUM, &string) && string ) {
		m_album = string;
		success = true;
	}
	if ( gst_tag_list_get_string(taglist, GST_TAG_GENRE, &string) && string ) {
		m_genre = string;
		success = true;
	}
	if ( gst_tag_list_get_uint(taglist, GST_TAG_TRACK_NUMBER, &intVal) && intVal > 0 ) {
		m_track = QString::number(intVal);
		success = true;
	}
	if ( gst_tag_list_get_string(taglist, GST_TAG_COMMENT, &string) && string ) {
		m_comment = string;
		success = true;
	}
	if ( gst_tag_list_get_string(taglist, GST_TAG_AUDIO_CODEC, &string) && string )
		m_audioCodec = string;
	if ( gst_tag_list_get_string(taglist, GST_TAG_VIDEO_CODEC, &string) && string )
		m_videoCodec = string;

	if (success)
		processMetaInfo();
}
