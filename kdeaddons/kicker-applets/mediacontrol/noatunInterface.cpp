/***************************************************************************
                          Interface to access Noatun
                             -------------------
    begin                : Mon Jan 15 21:09:00 CEST 2001
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

#include "noatunInterface.h"
#include "noatunInterface.moc"

#include <kapplication.h>
#include <kdebug.h>
#include <qstringlist.h>
#include <qstrlist.h>
#include <kurldrag.h>

#define TIMER_FAST  250

NoatunInterface::NoatunInterface() : PlayerInterface()
{
	mTimerValue = TIMER_FAST;
	mNoatunTimer = new QTimer(this, "mNoatunTimer");

	connect(mNoatunTimer, SIGNAL(timeout()), SLOT(updateSlider()));

	connect(kapp->dcopClient(), SIGNAL(applicationRegistered(const QCString&)),
		SLOT(appRegistered(const QCString&)) );

	connect(kapp->dcopClient(), SIGNAL(applicationRemoved(const QCString&)),
		SLOT(appRemoved(const QCString&)));

	kapp->dcopClient()->setNotifications(true);
	QTimer::singleShot(0, this, SLOT(myInit()));
}

NoatunInterface::~NoatunInterface()
{
	kapp->dcopClient()->setNotifications(false);
}

void NoatunInterface::myInit()
{
	// Start the timer if noatun is already running
	// Needed if user adds applet while running noatun
	if ( findRunningNoatun() )
	{
		emit playerStarted();
		mNoatunTimer->start(mTimerValue);
	}
	else
	{
		emit playerStopped();
		emit newSliderPosition(0,0);
	}
}

void NoatunInterface::appRegistered(const QCString &appId)
{
	if (appId.contains("noatun",false))
	{
		mAppId = appId;
		emit playerStarted();
		mNoatunTimer->start(mTimerValue);
	}
}

void NoatunInterface::appRemoved(const QCString &appId)
{
	if (appId.contains("noatun",false))
	{
	 	// is there still another noatun alive?
		if (findRunningNoatun())
			return;
		mNoatunTimer->stop();
		emit playerStopped();
		emit newSliderPosition(0,0);
	}
}

void NoatunInterface::updateSlider()
{
	// length/time in msecs, -1 means "no playobject in noatun"
	int len, time;
	QByteArray data, replyData;
	QCString replyType;

	if (!kapp->dcopClient()->call(mAppId, "Noatun", "length()", data, replyType, replyData, false, 200))
	{
		//kdDebug(90200) << "mediacontrol: DCOP communication Error" << endl;
		// -2 is an internal errornumber, might be used later
		len = -2;
	}
	else
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "int")
		{
			reply >> len;
		}
		else
		{
			//kdDebug(90200) << "mediacontrol: unexpected type of DCOP-reply" << endl;
			// -3 is an internal errornumber, might be used later
			len = -3;
		}
	}

	data = 0;
	replyData = 0;
	replyType = 0;

	if (!kapp->dcopClient()->call(mAppId, "Noatun", "position()", data,
		replyType, replyData, false, 200))
	{
		//kdDebug(90200) << "mediacontrol: DCOP communication error" << endl;
		time = -2;
	}
	else
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "int")
		{
			reply >> time;
		}
		else
		{
			//kdDebug(90200) << "mediacontrol: unexpected type of DCOP-reply" << endl;
			time = -3;
		}
	}

	if ((time < 0) || (len < 0)) // Noatun isn't playing and thus returns -1
	{
		len = 0;
		time = 0;
	}
	emit newSliderPosition(len/1000,time/1000);
	emit playingStatusChanged(playingStatus());
}

int NoatunInterface::playingStatus()
{
	QByteArray data, replyData;
	QCString replyType;

	if (!kapp->dcopClient()->call(mAppId, "Noatun", "state()", data, replyType,
		replyData, false, 200))
	{
		return Stopped;
	}
	else
	{
		int status = 0;
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "int")
			reply >> status;

		if (status == 1)
			return Paused;
		else if (status == 2)
			return Playing;
		else
			return Stopped;
	}
}


// Drag-n-Drop stuff =================================================================

void NoatunInterface::dragEnterEvent(QDragEnterEvent* event)
{
//	kdDebug(90200) << "NoatunInterface::dragEnterEvent()" << endl;
	event->accept(KURLDrag::canDecode(event));
}

void NoatunInterface::dropEvent(QDropEvent* event)
{
//	kdDebug(90200) << "NoatunInterface::dropEvent()" << endl;
	KURL::List list;
	if (KURLDrag::decode(event, list))
	{
		QByteArray data;
		QDataStream arg(data, IO_WriteOnly);
		arg << list.toStringList() << false;
		kapp->dcopClient()->send(mAppId, "Noatun", "addFile(QStringList,bool)", data);
	}
}

// ====================================================================================

void NoatunInterface::sliderStartDrag()
{
	mNoatunTimer->stop();
}

void NoatunInterface::sliderStopDrag()
{
	mNoatunTimer->start(mTimerValue);
}

void NoatunInterface::jumpToTime(int sec)
{
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << (sec*1000);  // noatun wants milliseconds
	kapp->dcopClient()->send(mAppId, "Noatun", "skipTo(int)", data);
}

void NoatunInterface::playpause()
{
	if (!findRunningNoatun())
		startPlayer("noatun");
	kapp->dcopClient()->send(mAppId, "Noatun", "playpause()", QString::null);
}

void NoatunInterface::stop()
{
	kapp->dcopClient()->send(mAppId, "Noatun", "stop()", QString::null);
}

void NoatunInterface::next()
{
	// fastForward() is noatun from kde2
	//kapp->dcopClient()->send("noatun", "Noatun", "fastForward()", QString::null);
	kapp->dcopClient()->send(mAppId, "Noatun", "forward()", QString::null);
}

void NoatunInterface::prev()
{
	kapp->dcopClient()->send(mAppId, "Noatun", "back()", QString::null);
}

void NoatunInterface::volumeUp()
{
	kapp->dcopClient()->send(mAppId, "Noatun", "volumeDown()", QString::null);
}

void NoatunInterface::volumeDown()
{
	kapp->dcopClient()->send(mAppId, "Noatun", "volumeUp()", QString::null);
}

const QString NoatunInterface::getTrackTitle() const
{
	QString title("");
	QByteArray data, replyData;
	QCString replyType;
	if (kapp->dcopClient()->call(mAppId, "Noatun", "title()", data, replyType,
		replyData, false, 200))
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "QString")
			reply >> title;
	}
	return title;
}

bool NoatunInterface::findRunningNoatun()
{
	// FIXME: what if we have a dcop app named, let's say, 'noatunfrontend'?
	QCStringList allApps = kapp->dcopClient()->registeredApplications();
	QValueList<QCString>::const_iterator iterator;

	for (iterator = allApps.constBegin(); iterator != allApps.constEnd(); ++iterator)
	{
		if ((*iterator).contains("noatun", false))
		{
			mAppId = *iterator;
			return true;
		}
	}
	return false;
}
