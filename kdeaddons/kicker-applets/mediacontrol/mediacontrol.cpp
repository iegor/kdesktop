/***************************************************************************
                        main file of mediacontrol applet
                             -------------------
    begin                : Tue Apr 25 11:53:11 CEST 2000
    copyright            : (C) 2000-2005 by Stefan Gehn
    email                : metz {AT} gehn {DOT} net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "mediacontrol.h"
#include "mediacontrol.moc"

#include "mediacontrolconfig.h"
#include "configfrontend.h"


#include "noatunInterface.h"
#include "xmmsInterface.h"
#include "jukInterface.h"
#include "amarokInterface.h"
#include "mpdInterface.h"
#include "kscdInterface.h"

#include "mcslider.h"

#include <qfile.h>
#include <qdragobject.h>
#include <qtooltip.h>
#include <qstyle.h>
#include <qslider.h>
#include <qpainter.h>
#include <qiconset.h>
#include <kpopupmenu.h>

#include <kapplication.h>
#include <kipc.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <knotifyclient.h>
#include <kbugreport.h>
#include <dcopclient.h>

const int MC_BUTTONSIZE = 20; // TODO: Might become dynamical for bigger panels
const int NO_BUTTONS = 4;

extern "C"
{
	KDE_EXPORT KPanelApplet *init( QWidget *parent, const QString &configFile)
	{
		KGlobal::locale()->insertCatalogue("mediacontrol");
		return new MediaControl(configFile, KPanelApplet::Normal,
			KPanelApplet::About | KPanelApplet::Preferences |
			KPanelApplet::ReportBug, parent, "mediacontrol");
	}
}

// =============================================================================

class MediaControlToolTip : public QToolTip
{
	public:
		MediaControlToolTip(QWidget *widget, PlayerInterface *pl_obj) :
			QToolTip(widget), mWidget(widget), mPlayer(pl_obj) {}

	protected:
		virtual void maybeTip(const QPoint &pt)
		{
			QRect rc( mWidget->rect());
			if (rc.contains(pt))
			{
				tip ( rc, mPlayer->getTrackTitle() );
			}
		}
	private:
		QWidget *mWidget;
		PlayerInterface *mPlayer;
};

// =============================================================================

MediaControl::MediaControl(const QString &configFile, Type t, int actions,
	QWidget *parent, const char *name)
	: DCOPObject("MediaControl"),
		KPanelApplet(configFile, t, actions, parent, name),
		mInstance(new KInstance("mediacontrol")),
		mAboutData(new KAboutData("mediacontrol",
			I18N_NOOP("MediaControl"),
			MEDIACONTROL_VERSION,
			I18N_NOOP("A small control-applet for various media players"),
			KAboutData::License_GPL_V2,
			"(c) 2001-2004 Stefan Gehn",
			0,
			"http://metz.gehn.net"))
{
	setBackgroundOrigin(AncestorOrigin);
	_player = 0L;
	_prefsDialog = 0L;

	_configFrontend = new ConfigFrontend(config());
	// My own dcopclient
	_dcopClient = new DCOPClient();
	_dcopClient->registerAs("mediacontrol", false);

	mAboutData->addAuthor("Stefan Gehn", I18N_NOOP("Main Developer"),
		"metz@gehn.net", "http://metz.gehn.net");
	mAboutData->addAuthor("Robbie Ward", I18N_NOOP("Initial About-Dialog"),
		"wardy@robbieward.co.uk", "http://www.robbieward.co.uk");

	mAboutData->addCredit("Sascha Hoffman", I18N_NOOP("Button-Pixmaps"),
		"tisch.sush@gmx.de", 0);
	mAboutData->addCredit("Christian Hoffman", I18N_NOOP("Button-Pixmaps"),
		"tisch.crix@gmx.de", "http://www.crixensgfxcorner.de.vu/");
	mAboutData->addCredit("Ulrik Mikaelsson", I18N_NOOP("Fix for Noatun-Support"),
		"rawler@rsn.bth.se", 0);
	mAboutData->addCredit("Anthony J Moulen", I18N_NOOP("Fix for Vertical Slider"),
		"ajmoulen@moulen.org", 0);
	mAboutData->addCredit("Teemu Rytilahti", I18N_NOOP("Volume Control Implementation"),
		"teemu.rytilahti@kde-fi.org", 0);
	mAboutData->addCredit("Jan Spitalnik", I18N_NOOP("Fix for JuK-Support"),
		"honza@spitalnik.net", 0);
	mAboutData->addCredit("William Robinson", I18N_NOOP("mpd-Support"),
		"airbaggins@yahoo.co.uk", 0);

	setAcceptDrops(true);

	prev_button  = new TrayButton (this, "PREVIOUS");
	playpause_button  = new TrayButton (this, "PLAYPAUSE");
	stop_button  = new TrayButton (this, "STOP");
	next_button  = new TrayButton (this, "NEXT");
	time_slider  = new MCSlider (QSlider::Horizontal, this, "time_slider" );
	time_slider->setRange(0,0);
	time_slider->setValue(0);
	time_slider->setTracking( false );
	time_slider->installEventFilter(this);

	// request notification of changes in icon style
	kapp->addKipcEventMask(KIPC::IconChanged);
	connect(kapp, SIGNAL(iconChanged(int)), this, SLOT(slotIconChanged()));

	reparseConfig();

	rmbMenu = new KPopupMenu(this, "RMBMenu");
	rmbMenu->insertTitle(i18n("MediaControl"), 0, 0);
	rmbMenu->insertItem(SmallIcon("configure"), i18n("Configure MediaControl..."),
		this, SLOT(preferences()));
	rmbMenu->insertItem(i18n("About MediaControl"), this, SLOT(about()));
}

MediaControl::~MediaControl()
{
	delete _player;
	delete _configFrontend;
	delete _dcopClient;
	KGlobal::locale()->removeCatalogue("mediacontrol");
}

// Drag-n-Drop stuff ===========================================================

void MediaControl::dragEnterEvent(QDragEnterEvent* event)
{
	_player->dragEnterEvent(event);  // Just pass dnd to the playerInterface
}

void MediaControl::dropEvent(QDropEvent* event)
{
	_player->dropEvent(event);  // Just pass dnd to the playerInterface
}

// =============================================================================


void MediaControl::setSliderPosition(int len ,int time)
{
	time_slider->blockSignals(true);
	if(orientation() == Vertical)
	  time = len - time;

	if (mLastLen != len)
	  time_slider->setRange(0,len);
	mLastLen = len;

	if (mLastTime != time)
	  time_slider->setValue(time);
	mLastTime = time;

	time_slider->blockSignals(false);
}

void MediaControl::enableAll()
{
	prev_button->setDisabled(false);
	playpause_button->setDisabled(false);
        QToolTip::remove(playpause_button);
	stop_button->setDisabled(false);
	next_button->setDisabled(false);
	time_slider->setDisabled(false);
}

void MediaControl::disableAll()
{
	prev_button->setDisabled(true);
	playpause_button->setDisabled(false);
        QToolTip::add(playpause_button, i18n("Start the player"));
	stop_button->setDisabled(true);
	next_button->setDisabled(true);
	time_slider->setDisabled(true);
	if(_configFrontend->useCustomTheme()) {
		QString skindir = locate("data", "mediacontrol/"+_configFrontend->theme()+"/");
		playpause_button->setIconSet(SmallIconSet(locate("data",skindir+"play.png")));
	}
	else
		playpause_button->setIconSet(SmallIconSet("player_play"));
}

void MediaControl::slotPlayingStatusChanged(int status)
{
	if (mLastStatus == status)
		return;

	mLastStatus = status;
	QString skindir = locate("data", "mediacontrol/"+_configFrontend->theme()+"/");

	switch (status)
	{
		case (PlayerInterface::Stopped):
		case (PlayerInterface::Paused):
			if(_configFrontend->useCustomTheme())
				playpause_button->setIconSet(SmallIconSet(locate("data",skindir+"play.png")));
			else
				playpause_button->setIconSet(SmallIconSet("player_play"));
			break;
		case (PlayerInterface::Playing):
			if(_configFrontend->useCustomTheme())
				playpause_button->setIconSet(SmallIconSet(locate("data",skindir+"pause.png")));
			else
				playpause_button->setIconSet(SmallIconSet("player_pause"));
			break;
	}
}

void MediaControl::slotIconChanged()
{
	if(!_configFrontend->useCustomTheme())
	{
		prev_button->setIconSet(SmallIconSet("player_start"));
		if (_player->playingStatus() == PlayerInterface::Playing)
			playpause_button->setIconSet(SmallIconSet("player_pause"));
		else
			playpause_button->setIconSet(SmallIconSet("player_play"));
		stop_button->setIconSet(SmallIconSet("player_stop"));
		next_button->setIconSet(SmallIconSet("player_end"));
	}
}

// Dialogs =====================================================================

void MediaControl::preferences()
{
	if ( _prefsDialog )
	{
		_prefsDialog->raise();
	}
	else
	{
		_prefsDialog = new MediaControlConfig ( _configFrontend );
		connect ( _prefsDialog, SIGNAL(closing()),
			this, SLOT(slotClosePrefsDialog()) );
		connect ( _prefsDialog, SIGNAL(destroyed()),
			this, SLOT(slotPrefsDialogClosing()) );
		connect ( _prefsDialog, SIGNAL(configChanged()),
			this, SLOT(slotConfigChanged()) );
	}
}

void MediaControl::slotConfigChanged()
{
	reparseConfig();
}

void MediaControl::slotClosePrefsDialog()
{
	delete _prefsDialog;
}

void MediaControl::slotPrefsDialogClosing()
{
	if ( _prefsDialog )
		_prefsDialog = 0L;
}


void MediaControl::about()
{
	KAboutApplication aboutDlg(mAboutData);
	aboutDlg.exec();
}


void MediaControl::reportBug()
{
	KBugReport bugReport(this, true, mAboutData);
	bugReport.exec();
}


// Fixing the orientation problem in qslider.
void MediaControl::adjustTime(int time)
{
	if(orientation() == Vertical)
		emit(newJumpToTime(mLastLen - time));
	else
		emit(newJumpToTime(time));
}

// Config Stuff ================================================================

void MediaControl::reparseConfig()
{
//	kdDebug(90200) << "reparseConfig();" << endl;
	_configFrontend->reparseConfiguration();

	if (_player != 0L) // make sure there is no player-object
	{
		_player->disconnect(); // disconnect from all things

		time_slider->disconnect();
		prev_button->disconnect();
		playpause_button->disconnect();
		stop_button->disconnect();
		next_button->disconnect();

		delete slider_tooltip; // tooltip depends on _player : delete it before _player gets deleted
		slider_tooltip = 0L;

		delete _player;
		_player = 0L;
	}

	mLastLen = -1;
	mLastTime = -1;
	mLastStatus = -1;

	QString playerString = _configFrontend->player();


#ifdef HAVE_XMMS
	if (playerString == "XMMS")
	{
		_player = new XmmsInterface ();
		time_slider->setSteps((_configFrontend->mouseWheelSpeed()*1000),
			(_configFrontend->mouseWheelSpeed()*1000));
	}
	else
#endif
	if (playerString == "JuK")
	{
		_player = new JuKInterface();
		time_slider->setSteps((_configFrontend->mouseWheelSpeed()),
			(_configFrontend->mouseWheelSpeed()));
	}
	else if (playerString == "Amarok")
	{
		_player = new AmarokInterface();
		time_slider->setSteps((_configFrontend->mouseWheelSpeed()),
			(_configFrontend->mouseWheelSpeed()));
	}
	else if (playerString == "KsCD")
	{
		_player = new KsCDInterface();
		time_slider->setSteps((_configFrontend->mouseWheelSpeed()),
			(_configFrontend->mouseWheelSpeed()));
	}
	else if (playerString == "mpd")
	{
		_player = new MpdInterface();
		time_slider->setSteps((_configFrontend->mouseWheelSpeed()),
			(_configFrontend->mouseWheelSpeed()));
	}
	else // Fallback is Noatun
	{
		_player = new NoatunInterface();
		time_slider->setSteps((_configFrontend->mouseWheelSpeed()),
			(_configFrontend->mouseWheelSpeed()));
	}

	//  this signal gets emitted by a playerInterface when the player's playtime changed
	connect(_player, SIGNAL(newSliderPosition(int,int)),
		this, SLOT(setSliderPosition(int,int)));

	connect(_player, SIGNAL(playerStarted()), SLOT(enableAll()));
	connect(_player, SIGNAL(playerStopped()), SLOT(disableAll()));
	connect(_player, SIGNAL(playingStatusChanged(int)), SLOT(slotPlayingStatusChanged(int)));

	// do we use our icons or the default ones from KDE?
	if(_configFrontend->useCustomTheme())
	{
		// load theme
		QString skindir = locate("data", "mediacontrol/"+_configFrontend->theme()+"/");

		// the user has to take care if all pixmaps are there, we only check for one of them
		if (QFile(skindir+"play.png").exists())
		{
			prev_button->setIconSet(SmallIconSet(locate("data",skindir+"prev.png")));
			if (_player->playingStatus() == PlayerInterface::Playing)
				playpause_button->setIconSet(SmallIconSet(locate("data",skindir+"play.png")));
			else
				playpause_button->setIconSet(SmallIconSet(locate("data",skindir+"pause.png")));
			stop_button->setIconSet(SmallIconSet(locate("data",skindir+"stop.png")));
			next_button->setIconSet(SmallIconSet(locate("data",skindir+"next.png")));
		}
		else // icon-theme is invalid or not there
		{
			KNotifyClient::event(winId(), KNotifyClient::warning,
				i18n("There was trouble loading theme %1. Please choose" \
					" a different theme.").arg(skindir));

			// default to kde-icons, they have to be installed :)
			slotIconChanged();

			// and open prefs-dialog
			preferences();
		}
	}
	else // KDE default-icons, assuming that these icons exist!
	{
		// sets icons from kde
		slotIconChanged();
	}

	slider_tooltip = new MediaControlToolTip(time_slider, _player);

	connect(prev_button, SIGNAL(clicked()), _player, SLOT(prev()));
	connect(playpause_button, SIGNAL(clicked()), _player, SLOT(playpause()));
	connect(stop_button, SIGNAL(clicked()), _player, SLOT(stop()));
	connect(next_button, SIGNAL(clicked()), _player, SLOT(next()));

	connect(time_slider, SIGNAL(sliderPressed()), _player, SLOT(sliderStartDrag()));
	connect(time_slider, SIGNAL(sliderReleased()), _player, SLOT(sliderStopDrag()));
	connect(time_slider, SIGNAL(valueChanged(int)), this, SLOT(adjustTime(int)));
	connect(time_slider, SIGNAL(volumeUp()), _player, SLOT(volumeUp()));
	connect(time_slider, SIGNAL(volumeDown()), _player, SLOT(volumeDown()));
	connect(this, SIGNAL(newJumpToTime(int)), _player, SLOT(jumpToTime(int)));
}

// Widget Placement ===================================================================

// kicker wants to know what width we need for a given height
// (this is called when being a HORIZONTAL panel)
int MediaControl::widthForHeight(int height) const
{
//	kdDebug(90200) << "kicker height: " << height << endl;
//	kdDebug(90200) << "slider needs: " << time_slider->minimumSizeHint().height() << endl;

	// slider height + button height
	if ( height >= (time_slider->minimumSizeHint().height()+MC_BUTTONSIZE) )
	{	// slider UNDER buttons
		// (5 * button width + spaces between them);
		return (4*MC_BUTTONSIZE+10);
	}
	else
	{	// slider ASIDE buttons
		// (5 * button width + spaces between them) * 2 [size of slider = size of all buttons]
		return ((4*MC_BUTTONSIZE+10)*2);
	}
}

// kicker wants to know what height we need for a given width
// (this is called when being a VERTICAL panel)
int MediaControl::heightForWidth(int width) const
{
//	kdDebug(90200) << "kicker width: " << width << endl;

	// slider height + button height
	if ( width >= (time_slider->minimumSizeHint().width()+MC_BUTTONSIZE) )
	{	// slider ASIDE icons
		// (5 * button width + spaces between them);
		return (4*MC_BUTTONSIZE+10);
	}
	else
	{	// slider UNDER buttons
		// (5 * button width + spaces between them) * 2
		// because the size of the slider = the size of all buttons
		return ((4*MC_BUTTONSIZE+10)*2);
	}
}

void MediaControl::mousePressEvent(QMouseEvent* e)
{
	if (e->button() == QMouseEvent::RightButton)
		rmbMenu->popup(e->globalPos());
}

bool MediaControl::eventFilter(QObject *, QEvent *e)
{
	if (e->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent *me = static_cast<QMouseEvent *>(e);
		if (me->button() == QMouseEvent::RightButton)
		{
			rmbMenu->popup(me->globalPos());
			return true;
		}
	}
	return false;
}

void MediaControl::paletteChange( const QPalette& )
{
    time_slider->setBackground();
}

void MediaControl::moveEvent( QMoveEvent* )
{
    time_slider->setBackground();
}

// Danger: Weird Code ahead! ;))
void MediaControl::resizeEvent( QResizeEvent* )
{
//	kdDebug(90200) << "resizeEvent()" << endl;
	int w = width();
	int h = height();
	if ( orientation() == Vertical )
	{		// ====== VERTICAL =================================================
		time_slider->setOrientation(QSlider::Vertical);
		int slider_width = time_slider->minimumSizeHint().width();
		// some styles need more space for sliders than avilable in very small panels :(
		if ( slider_width > w ) slider_width = w;

		// that width would be needed to put the slider aside the buttons
		if ( w >= (slider_width+MC_BUTTONSIZE) )
		{	// Slider ASIDE icons
			int applet_space = (w - (slider_width+MC_BUTTONSIZE) ) / 2;
			if ( applet_space < 0 )
				applet_space = 0;

			prev_button->setGeometry  ( applet_space, 1, MC_BUTTONSIZE, MC_BUTTONSIZE );
			playpause_button->setGeometry  ( applet_space, 3+1*MC_BUTTONSIZE, MC_BUTTONSIZE, MC_BUTTONSIZE );
			stop_button->setGeometry  ( applet_space, 5+2*MC_BUTTONSIZE, MC_BUTTONSIZE, MC_BUTTONSIZE );
			next_button->setGeometry  ( applet_space, 7+3*MC_BUTTONSIZE, MC_BUTTONSIZE, MC_BUTTONSIZE );
			time_slider->setGeometry  ( applet_space+MC_BUTTONSIZE, 1, slider_width, NO_BUTTONS*MC_BUTTONSIZE+8 );
		}
		else
		{	// Slider UNDER Icons
			int slider_space = (w - slider_width)/2;
			int button_space = (w - MC_BUTTONSIZE)/2;

			prev_button->setGeometry  ( button_space, 1 , MC_BUTTONSIZE, MC_BUTTONSIZE );
			playpause_button->setGeometry  ( button_space, 3+1*MC_BUTTONSIZE, MC_BUTTONSIZE, MC_BUTTONSIZE );
			stop_button->setGeometry  ( button_space, 5+2*MC_BUTTONSIZE, MC_BUTTONSIZE, MC_BUTTONSIZE );
			next_button->setGeometry  ( button_space, 7+3*MC_BUTTONSIZE, MC_BUTTONSIZE, MC_BUTTONSIZE );
			time_slider->setGeometry  ( slider_space, 9+4*MC_BUTTONSIZE, slider_width, NO_BUTTONS*MC_BUTTONSIZE+8 );
		}
	}
	else	// ====== HORIZONTAL ===============================================
	{
		time_slider->setOrientation(QSlider::Horizontal);
		int slider_height = time_slider->minimumSizeHint().height();
		// some styles need more space for sliders than avilable in very small panels :(
		if ( slider_height > h ) slider_height = h;

		// that h would be needed to put the slider under the buttons
		if ( h >= (slider_height+MC_BUTTONSIZE) )
		{	// Slider UNDER icons
			int applet_space = (h-(slider_height+MC_BUTTONSIZE))/2;
			if ( applet_space < 0 )
				applet_space = 0;

			prev_button->setGeometry  ( 1 , applet_space, MC_BUTTONSIZE, MC_BUTTONSIZE );
			playpause_button->setGeometry  ( 3+MC_BUTTONSIZE, applet_space, MC_BUTTONSIZE, MC_BUTTONSIZE );
			stop_button->setGeometry  ( 5+2*MC_BUTTONSIZE, applet_space, MC_BUTTONSIZE, MC_BUTTONSIZE );
			next_button->setGeometry  ( 7+3*MC_BUTTONSIZE, applet_space, MC_BUTTONSIZE, MC_BUTTONSIZE );
			time_slider->setGeometry  ( 1, applet_space + MC_BUTTONSIZE, NO_BUTTONS*MC_BUTTONSIZE+8, slider_height );
		}
		else
		{	// Slider ASIDE Icons
			int slider_space = (h - slider_height)/2;
			int button_space = (h - MC_BUTTONSIZE)/2;

			prev_button->setGeometry  ( 1 , button_space, MC_BUTTONSIZE, MC_BUTTONSIZE );
			playpause_button->setGeometry  ( 3+1*MC_BUTTONSIZE, button_space, MC_BUTTONSIZE, MC_BUTTONSIZE );
			stop_button->setGeometry  ( 5+2*MC_BUTTONSIZE, button_space, MC_BUTTONSIZE, MC_BUTTONSIZE );
			next_button->setGeometry  ( 7+3*MC_BUTTONSIZE, button_space, MC_BUTTONSIZE, MC_BUTTONSIZE );
			time_slider->setGeometry  ( 9+4*MC_BUTTONSIZE, slider_space, NO_BUTTONS*MC_BUTTONSIZE+8, slider_height );
		}
	}
}

// Our Button ========================================================================

TrayButton::TrayButton(QWidget* parent, const char* name)
	: SimpleButton (parent, name)
{
	setBackgroundMode(PaletteBackground);
	setBackgroundOrigin(AncestorOrigin);
}

void TrayButton::setIconSet(const QIconSet &iconSet)
{
    setPixmap(iconSet.pixmap(QIconSet::Automatic, QIconSet::Normal, QIconSet::On));
}

