/***************************************************************************
                          Interface to access XMMS
                             -------------------
    begin                : Tue Apr 25 11:53:11 CEST 2000
    copyright            : (C) 2000-2002 by Stefan Gehn
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

#ifdef HAVE_XMMS // only use if there is xmms installed on compiletime

#include "xmmsInterface.h"
#include "xmmsInterface.moc"
#include <xmms/xmmsctrl.h>
#include <kdebug.h>

#define TIMER_SLOW 1000
#define TIMER_FAST  100
#define XMMS_SESSION 0

XmmsInterface::XmmsInterface() : PlayerInterface()
{
	timervalue = TIMER_FAST;
	bStartingXMMS = false;
	xmms_timer = new QTimer ( this, "xmms_timer" );

	QObject::connect( xmms_timer, SIGNAL(timeout()), SLOT(updateSlider()) );

	// Better start the timer as late as possible in initialization
	xmms_timer->start ( timervalue );
}

XmmsInterface::~XmmsInterface()
{
	delete xmms_timer;
}

void XmmsInterface::updateSlider ( void )
{
	if ( !xmms_remote_is_running(XMMS_SESSION) )
	{  // XMMS not running
		if (timervalue == TIMER_FAST)  // timer is running fast
		{
			emit playerStopped();
			timervalue = TIMER_SLOW;  // timer does not need to run fast if XMMS is not running
			xmms_timer->changeInterval(timervalue);
			emit newSliderPosition(0,0);
		}
		return;  // as XMMS is not running we can leave now
	}

	// huh, XMMS is running :)
	if (timervalue == TIMER_SLOW)  // what? Still running slow?
	{
		emit playerStarted();
		timervalue = TIMER_FAST;  // boost the timer to have better reaction-times for the applet
		xmms_timer->changeInterval(timervalue);
	}

	int len  = xmms_remote_get_playlist_time ( XMMS_SESSION, xmms_remote_get_playlist_pos(XMMS_SESSION) );
	int time = xmms_remote_get_output_time(XMMS_SESSION);

	if (len < 0)
	{
		len = 0;
		time = 0;
	}

	emit newSliderPosition(len,time);
	emit playingStatusChanged(playingStatus());
}


// Drag-n-Drop stuff =================================================================

void XmmsInterface::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept( QTextDrag::canDecode(event) );
}

void XmmsInterface::dropEvent(QDropEvent* event)
{
	QString text;
//	kdDebug(90200) << "XmmsInterface::dropEvent()" << endl;
	if ( QTextDrag::decode(event, text) )
	{
		xmms_remote_playlist_add_url_string(XMMS_SESSION,
			(gchar *)text.local8Bit().data());
	}
}

// ====================================================================================


void XmmsInterface::sliderStartDrag()
{
	xmms_timer->stop();
}

void XmmsInterface::sliderStopDrag()
{
	xmms_timer->start( timervalue );
}

void XmmsInterface::jumpToTime( int msec )
{
	xmms_remote_jump_to_time(XMMS_SESSION, msec);
}

void XmmsInterface::playpause()
{
	if (!xmms_remote_is_running(XMMS_SESSION))
	{
		if (bStartingXMMS)
			return;
		startPlayer("xmms");
		bStartingXMMS = true;
		QTimer::singleShot(500, this, SLOT(playpause()));
	}
	else
	{
		bStartingXMMS = false;
		xmms_remote_play_pause(XMMS_SESSION);
	}
}

void XmmsInterface::stop()
{
	xmms_remote_stop(XMMS_SESSION);
}

void XmmsInterface::next()
{
	xmms_remote_playlist_next(XMMS_SESSION);
}

void XmmsInterface::prev()
{
	xmms_remote_playlist_prev(XMMS_SESSION);
}

void XmmsInterface::volumeUp()
{
	const int cur = xmms_remote_get_main_volume(XMMS_SESSION);
	xmms_remote_set_main_volume(XMMS_SESSION, cur+1);
}

void XmmsInterface::volumeDown()
{
	const int cur = xmms_remote_get_main_volume(XMMS_SESSION);
	xmms_remote_set_main_volume(XMMS_SESSION, cur-1);
}

int XmmsInterface::playingStatus()
{
	if (xmms_remote_is_paused(XMMS_SESSION))
		return Paused;

	if (xmms_remote_is_playing(XMMS_SESSION))
		return Playing;

	return Stopped;
}

const QString XmmsInterface::getTrackTitle() const
{
	return QString::fromLocal8Bit(
		xmms_remote_get_playlist_title(XMMS_SESSION,
			xmms_remote_get_playlist_pos(XMMS_SESSION)));
}
#endif // HAVE_XMMS
