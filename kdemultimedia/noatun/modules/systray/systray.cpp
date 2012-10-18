// systray.h
//
// Copyright (C) 2000 Neil Stevens <multivac@fcmail.com>
// Copyright (C) 1999 Charles Samuels <charles@kde.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "systray.h"
#include "kitsystemtray.h"
#include "cmodule.h"
#include "yhconfig.h"

#include <noatun/app.h>
#include <noatun/pref.h>
#include <noatun/player.h>
#include <noatun/stdaction.h>

#include <kaction.h>
#include <kconfig.h>
#include <qfile.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kpassivepopup.h>
#include <kpixmapeffect.h>
#include <kstdaction.h>
#include <qbitmap.h>
#include <qhbox.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qvbox.h>

#include <qimage.h>
#include <kurl.h>
#include <kio/netaccess.h>
#include <kdebug.h>
#include <kstandarddirs.h>

#include <netwm.h>
#include <kglobalsettings.h>

// TODO: Maybe make this value configurable?
const int COVER_MAXW = 128;
const int COVER_MAXH = 128;
#define BASEICON "noatun"

// From JuK
class PassivePopup : public KPassivePopup
{
public:
	PassivePopup(QWidget *parent = 0, const char *name = 0) : KPassivePopup(parent, name) {}

protected:
	virtual void enterEvent(QEvent *)
	{
		setTimeout(3000000); // Make timeout damn near infinite
	}

	virtual void leaveEvent(QEvent *)
	{
		setTimeout(250); // Close quickly
	}
};


//NoatunSystray *NoatunSystray::self = 0;


NoatunSystray::NoatunSystray() : KMainWindow(0, "NoatunSystray"), Plugin(),
	mTray(0), trayStatus(0), trayBase(0), mPassivePopup(0L)
{
	//self = this;
	hide();

	tmpCoverPath = locateLocal("tmp", "youngHickoryCover.png");

	removeCover(); // make sure any old temp cover is gone

	KStdAction::quit(napp, SLOT(quit()), actionCollection());
	KStdAction::open(napp, SLOT(fileOpen()), actionCollection());
	KStdAction::preferences(napp, SLOT(preferences()), actionCollection());
	NoatunStdAction::back(actionCollection(), "back");
	NoatunStdAction::stop(actionCollection(), "stop");
	NoatunStdAction::playpause(actionCollection(), "play");
	NoatunStdAction::forward(actionCollection(), "forward");
	NoatunStdAction::playlist(actionCollection(), "show_playlist");
	NoatunStdAction::loop(actionCollection(), "loop_style");
	NoatunStdAction::effects(actionCollection(), "effects");
	NoatunStdAction::equalizer(actionCollection(), "equalizer");

	createGUI("systrayui.rc");

	mTray = new KitSystemTray("tray", this);
	mTray->show();

	trayBase = renderIcon(BASEICON, QString::null);
	trayStatus = renderIcon(BASEICON, "player_stop");

	mTray->changeTitle(*trayBase, i18n("Noatun"));
	showingTrayStatus = false;

	mBlinkTimer = new QTimer(this);
	connect(mBlinkTimer, SIGNAL(timeout()), this, SLOT(slotBlinkTimer()));

	connect(napp->player(), SIGNAL(playing()), this, SLOT(slotPlayPause()));
	connect(napp->player(), SIGNAL(paused()), this, SLOT(slotPlayPause()));
	connect(napp->player(), SIGNAL(stopped()), this, SLOT(slotStopped()));
	//napp->player()->handleButtons();
}


NoatunSystray::~NoatunSystray()
{
	//kdDebug(66666) << k_funcinfo << "Called." << endl;
	removeCover();
	delete trayBase;
	delete trayStatus;
	napp->showInterfaces();
}


void NoatunSystray::init()
{
	YHModule *cmod = new YHModule(this);
	connect(cmod, SIGNAL(saved()), this, SLOT(slotLoadSettings()));
	slotLoadSettings();
}


void NoatunSystray::slotLoadSettings()
{
	kdDebug(66666) << k_funcinfo << endl;

	YHConfig *c = YHConfig::self();

	if(c->stateIconDisplay() == YHConfig::FlashingIcon)
		mBlinkTimer->start(1000);
	else
		mBlinkTimer->stop();
	slotBlinkTimer();


	if(c->tip())
		QToolTip::add(mTray, tipText);
	else
		QToolTip::remove(mTray);

	if (!c->passivePopupCovers())
		removeCover();

	if(c->passivePopup())
	{
		mPassivePopup = new PassivePopup(mTray, "NoatunPassivePopup");
	}
	else
	{
		delete mPassivePopup;
		mPassivePopup = 0L;
	}
}


void NoatunSystray::closeEvent(QCloseEvent*)
{
	//kdDebug(66666) << k_funcinfo << "Called." << endl;
	disconnect(napp->player(), 0, 0, 0);
	unload();
}


void NoatunSystray::slotPlayPause()
{
	QString status;

	if(napp->player()->isPaused())
	{
		changeTray("player_pause");
		status = i18n("Noatun - Paused");
	}
	else
	{
		changeTray("player_play");
		status = i18n("Noatun - Playing");
	}

	const PlaylistItem item = napp->player()->current();
	QString s;

	if(!item.isProperty("title"))
	{
		// No metadata
		s = QString("<nobr>%1</nobr>").arg(item.title());
	}
	else
	{
		s = QString("<h2><nobr>%1</nobr></h2>").arg(item.property("title"));

		if(item.isProperty("author"))
			s += QString("<nobr>%1</nobr><br>").arg(item.property("author"));

		if(item.isProperty("album"))
		{
			if(item.isProperty("date"))
				s += QString("<nobr>%1 (%2)</nobr><br>").arg(item.property("album")).arg(item.property("date"));
			else
				s += QString("<nobr>%1</nobr><br>").arg(item.property("album"));
		}
	}

	// prepare cover image for display
	if (YHConfig::self()->passivePopupCovers())
		updateCover();

	if(YHConfig::self()->passivePopupCovers() && QFile::exists(tmpCoverPath))
	{
		// QT always adds an empty line after the table so we add en empty line before the
		// table to get equal spacing on top and bottom
		setTipText(QString("<qt><br><table cellspacing=0 cellpadding=0><tr>" \
			"<td align=center valign=center><h4><nobr>%1</nobr></h4>%2</td>" \
			"<td valign=center><img src='%3'></td>" \
			"</qt></tr></table>").arg(status).arg(s).arg(tmpCoverPath));
	}
	else
	{
		setTipText(QString("<qt><center><h4><nobr>%1</nobr></h4>%2</center></qt>").arg(status).arg(s));
	}
}


void NoatunSystray::slotStopped()
{
	if(!napp->player()->current())
		return;
	changeTray("player_stop");
	setTipText(QString("<qt><nobr><h4>%1</h4></nobr></qt>").arg(i18n("Noatun - Stopped")));
}



void NoatunSystray::changeTray(const QString &pm)
{
	delete trayStatus;
	trayStatus = renderIcon(BASEICON, pm);
	if(showingTrayStatus)
		slotBlinkTimer();
}


void NoatunSystray::slotBlinkTimer()
{
	switch(YHConfig::self()->stateIconDisplay())
	{
		case (YHConfig::FlashingIcon):
			showingTrayStatus ^= true;
			break;
		case (YHConfig::StaticIcon):
			showingTrayStatus = true;
			break;
		case (YHConfig::NoIcon):
			showingTrayStatus = false;
			break;
	}

	if(showingTrayStatus)
		mTray->setPixmap(*trayStatus);
	else
		mTray->setPixmap(*trayBase);
}


// taken from patched karamba xmmssensor
// modified heavily to work in this place
void NoatunSystray::updateCover()
{
	//kdDebug(66666) << k_funcinfo << endl;
	QString dir = napp->player()->current().url().directory();
	QString cover;

	// TODO: Maybe make these filenames configurable?
	if(QFile::exists(dir + "/folder.png"))
		cover = dir + "/folder.png";
	else if(QFile::exists(dir + "/.folder.png"))
		cover = dir + "/.folder.png";
	else if(QFile::exists(dir + "/cover.png"))
		cover = dir + "/cover.png";
	else if(QFile::exists(dir + "/cover.jpg"))
		cover = dir + "/cover.jpg";
	else if(QFile::exists(dir + "/cover.jpeg"))
		cover = dir + "/cover.jpeg";
	else // no cover
	{
		//kdDebug(66666) << k_funcinfo << "NO COVER" <<  endl;
		removeCover();
		return;
	}

	QString title = napp->player()->current().title();

	QImage previmg;
	previmg.load(tmpCoverPath);

	if(previmg.text("Title") != title)
	{ //Verify song change to limit CPU usage
		/*kdDebug(66666) << k_funcinfo << "Creating new temp cover for '" <<
			cover << "'" << endl;*/

		QImage src;
		QImage tmpimg;

		if(src.load(cover))
		{
			if(src.width() >= COVER_MAXW || src.height() >= COVER_MAXH)
				tmpimg = src.scale(COVER_MAXW, COVER_MAXH, QImage::ScaleMin);
			else
				tmpimg = src;

			tmpimg.setText("Title", 0, title); //add Title in the image text for cache usage
			tmpimg.save(tmpCoverPath, "PNG", 0);
		}
		else
		{
			removeCover();
		}
	}
}


void NoatunSystray::removeCover()
{
	if(QFile::exists(tmpCoverPath))
		KIO::NetAccess::del(KURL(tmpCoverPath), this);
}


void NoatunSystray::setTipText(const QString& text)
{
	if(text == tipText) // save the planet, save cpu cycles ;)
		return;
	tipText = text;

	YHConfig *c = YHConfig::self();
	if(c->passivePopup())
		QTimer::singleShot(0, this, SLOT(showPassivePopup()));

	if(c->tip())
		QToolTip::add(mTray, tipText);
}


void NoatunSystray::showPassivePopup()
{
	if (!mPassivePopup)
	{
		kdDebug(66666) << k_funcinfo << "Called but no KPassivePopup created yet!" << endl;
		return;
	}

	mPassivePopup->reparent(0L, QPoint(0,0));

	if (YHConfig::self()->passivePopupButtons() && !napp->player()->isStopped())
	{
		QVBox *widget = mPassivePopup->standardView(QString::null, tipText, QPixmap());
		QHBox *box = new QHBox(mPassivePopup, "popupbox");

		box->setSpacing(8);

		// Algorithm for determining popup location from kpassivepopup.cpp via JuK
		NETWinInfo ni(qt_xdisplay(), mTray->winId(), qt_xrootwin(),
			NET::WMIconGeometry | NET::WMKDESystemTrayWinFor);
		NETRect frame, win;
		ni.kdeGeometry(frame, win);

		QRect bounds = KGlobalSettings::desktopGeometry(QPoint(win.pos.x, win.pos.y));

		if(win.pos.x < bounds.center().x())
		{
			// Buttons to the left

			QVBox *buttonBox = new QVBox(box);
			buttonBox->setSpacing(3);

			QPushButton *forwardButton = new QPushButton(action("forward")->iconSet(), 0, buttonBox, "popup_forward");
			forwardButton->setFlat(true);
			connect(forwardButton, SIGNAL(clicked()), action("forward"), SLOT(activate()));

			QPushButton *backButton = new QPushButton(action("back")->iconSet(), 0, buttonBox, "popup_back");
			backButton->setFlat(true);
			connect(backButton, SIGNAL(clicked()), action("back"), SLOT(activate()));

			QFrame *line = new QFrame(box);
			line->setFrameShape(QFrame::VLine);

			widget->reparent(box, QPoint(0, 0));
		}
		else
		{
			// Buttons to the right
			widget->reparent(box, QPoint(0, 0));

			QFrame *line = new QFrame(box);
			line->setFrameShape(QFrame::VLine);

			QVBox *buttonBox = new QVBox(box);
			buttonBox->setSpacing(3);

			QPushButton *forwardButton = new QPushButton(action("forward")->iconSet(), 0, buttonBox, "popup_forward");
			forwardButton->setFlat(true);
			connect(forwardButton, SIGNAL(clicked()), action("forward"), SLOT(activate()));

			QPushButton *backButton = new QPushButton(action("back")->iconSet(), 0, buttonBox, "popup_back");
			backButton->setFlat(true);
			connect(backButton, SIGNAL(clicked()), action("back"), SLOT(activate()));
		}
		mPassivePopup->setView(box);
	}
	else
	{
		mPassivePopup->setView(QString::null, tipText);
	}

	mPassivePopup->setTimeout(YHConfig::self()->passivePopupTimeout()*1000);
	mPassivePopup->show();
}


QPixmap *NoatunSystray::renderIcon(const QString& baseIcon, const QString &overlayIcon) const
{
	QPixmap *base = new QPixmap(KSystemTray::loadIcon(baseIcon));

	if(!(overlayIcon.isNull())) // otherwise leave the base as-is
	{
		QPixmap overlay = KSystemTray::loadIcon(overlayIcon);
		if(!overlay.isNull())
		{
			// draw the overlay on top of it
			QPainter p(base);
			p.drawPixmap(0, 0, overlay);
		}
	}
	return base;
}

#include "systray.moc"
