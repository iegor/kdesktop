/*
 * systemtray.cpp
 *
 * Copyright (C) 2003-2005 Jürgen Kofler <kaffeine@gmx.net>
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

#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klocale.h>
#include <kpopupmenu.h>
#include <kaction.h>

#include <qtimer.h>
#include <qlabel.h>
#include <qtooltip.h>

#include "systemtray.h"
#include "systemtray.moc"


class TitleLabel : public QLabel
{

public:
	TitleLabel() : QLabel(0, "tray_osd", WStyle_Customize | WStyle_NoBorder | WStyle_StaysOnTop | WX11BypassWM)
	{}
	virtual ~TitleLabel() {}

protected:
	void enterEvent(QEvent*)
	{
		hide();
	}
};

SystemTray::SystemTray(QWidget *parent, const char *name ) : KSystemTray(parent,name)
{
	kdDebug() << "SystemTray: Create System Tray" << endl;


	KAction* play = new KAction(i18n("Play / Pause"), "player_play", 0, this, SIGNAL(signalPlay()), actionCollection(), "trayplay");
	KAction* next = new KAction(i18n("&Next"), "next", 0, this, SIGNAL(signalNext()), actionCollection(), "traynext");
	KAction* previous = new KAction(i18n("&Previous"), "previous", 0, this, SIGNAL(signalPrevious()), actionCollection(), "trayprevious");
	KAction* stop = new KAction(i18n("Stop"), "player_stop", 0, this, SIGNAL(signalStop()), actionCollection(), "traystop");
	KAction* mute = new KAction(i18n("&Mute"), "player_mute", 0, this, SIGNAL(signalMute()), actionCollection(), "traymute");

	play->plug(contextMenu());
	next->plug(contextMenu());
	previous->plug(contextMenu());
	stop->plug(contextMenu());
	mute->plug(contextMenu());

	m_osd = new TitleLabel;
	m_osd->setFrameStyle(QFrame::Panel | QFrame::Plain);
	m_osd->setLineWidth(1);
	m_osd->setAlignment(SingleLine);
	connect(&m_hideTimer, SIGNAL(timeout()), this, SLOT(slotHideOSD()));

	setPixmap(KGlobal::iconLoader()->loadIcon("kaffeine", KIcon::Panel, 22));
	QToolTip::add(this, i18n("Kaffeine Player"));
}


SystemTray::~SystemTray()
{}

void SystemTray::wheelEvent(QWheelEvent *e)
{
	if(e->delta() < 0)
		actionCollection()->action("traynext")->activate();
	else
		actionCollection()->action("trayprevious")->activate();
	e->accept();
}

void SystemTray::mousePressEvent(QMouseEvent *e)
{
	switch(e->button()) {
	case LeftButton:
	case RightButton:
	default:
		KSystemTray::mousePressEvent(e);
		break;
	case MidButton:
		if(!rect().contains(e->pos()))
			return;
		actionCollection()->action("trayplay")->activate();
		break;
    }
}

void SystemTray::showTitle(const QString& title, uint secs)
{
	if (title.isEmpty())
		return;

	QToolTip::remove(this); // all the time remove it before add it.
	QToolTip::add(this, title);
	if (secs > 0)
	{
		m_osd->setText(QString("<nobr><b>") + title + "</b></nobr>");
		m_osd->adjustSize();
		QPoint global = mapToGlobal(QPoint(0,0));
		m_osd->move(global.x() - m_osd->width() + 10, global.y() - m_osd->height() + 15);
		m_osd->show();
		m_hideTimer.start(secs * 1000);
	}
}

void SystemTray::slotHideOSD()
{
	m_osd->hide();
	m_hideTimer.stop();
}
