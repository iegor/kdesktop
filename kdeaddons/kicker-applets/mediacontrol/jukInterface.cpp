/***************************************************************************
                           Interface to access JuK
                             -------------------
    begin                : Mon Jan 15 21:09:00 CEST 2001
    copyright            : (C) 2001-2002 by Stefan Gehn
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

#include "jukInterface.h"
#include "jukInterface.moc"

#include <kapplication.h>
#include <kdebug.h>
#include <qstringlist.h>
#include <qstrlist.h>
#include <qprocess.h>
#include <kurldrag.h>

#define TIMER_FAST  250

JuKInterface::JuKInterface() : PlayerInterface(), mProc(0)
{
	mTimerValue = TIMER_FAST;
	mJuKTimer = new QTimer ( this, "mJukTimer" );

	connect(mJuKTimer, SIGNAL(timeout()), SLOT(updateSlider()) );
	kapp->dcopClient()->setNotifications ( true );

	connect(kapp->dcopClient(), SIGNAL(applicationRegistered(const QCString&)),
		SLOT(appRegistered(const QCString&)) );

	connect(kapp->dcopClient(), SIGNAL(applicationRemoved(const QCString&)),
		SLOT(appRemoved(const QCString&)));

	QTimer::singleShot(0, this, SLOT(myInit()));
}

JuKInterface::~JuKInterface()
{
	kapp->dcopClient()->setNotifications(false);
	delete mJuKTimer;
}

void JuKInterface::myInit()
{
	// Start the timer if juk is already running
	// Needed if user adds applet while running juk
	if ( findRunningJuK() )
	{
		emit playerStarted();
		mJuKTimer->start(mTimerValue);
	}
	else
	{
		emit playerStopped();
		emit newSliderPosition(0,0);
	}
}

void JuKInterface::appRegistered ( const QCString &appId )
{
	if(appId.contains("juk",false) )
	{
		mAppId = appId;

		// BWAHAHAHA EVIL HACK
		// JuK blocks DCOP signals on its startup, so if we try to
		// ping it now, it'll simply cause us to block, which will
		// cause kicker to block, which is bad, m'kay?
		//
		// So what we do is launch the dcop command instead, and let
		// *it* block for us.  As soon as the command exits, we know
		// that JuK is ready to go (and so are we).
		mProc = new QProcess(this, "jukdcopCheckProc");
		mProc->addArgument("dcop");
		mProc->addArgument("juk");
		mProc->addArgument("Player");
		mProc->addArgument("status()");

		connect(mProc, SIGNAL(processExited()), SLOT(jukIsReady()));
		mProc->start();
	}
}

void JuKInterface::appRemoved ( const QCString &appId )
{
	if ( appId.contains("juk",false) )
	{
	 	// is there still another juk alive?
		if ( findRunningJuK() )
			return;
		mJuKTimer->stop();
		emit playerStopped();
		emit newSliderPosition(0,0);
	}
}

/* Called when the dcop process we launch terminates */
void JuKInterface::jukIsReady()
{
	emit playerStarted();
	mJuKTimer->start(mTimerValue);

	mProc->deleteLater();
	mProc = 0;
}

void JuKInterface::updateSlider ()
{
	// length/time in msecs, -1 means "no playobject in juk"
	int len = -1;
	int time = -1;
	QByteArray data, replyData;
	QCString replyType;

	if (kapp->dcopClient()->call(mAppId, "Player", "totalTime()", data,
		replyType, replyData))
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "int")
			reply >> len;
	}

	data = 0;
	replyData = 0;
	replyType = 0;

	if (kapp->dcopClient()->call(mAppId, "Player", "currentTime()", data,
		replyType, replyData))
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "int")
			reply >> time;
	}

	if ( (time < 0) || (len < 0)) // JuK isn't playing and thus returns -1
	{
		len = 0;
		time = 0;
	}
	emit ( newSliderPosition(len,time) );
	emit playingStatusChanged(playingStatus());
}

// Drag-n-Drop stuff =================================================================

void JuKInterface::dragEnterEvent(QDragEnterEvent* event)
{
//	kdDebug(90200) << "JuKInterface::dragEnterEvent()" << endl;
	event->accept( KURLDrag::canDecode(event) );
}

void JuKInterface::dropEvent(QDropEvent* event)
{
//	kdDebug(90200) << "JuKInterface::dropEvent()" << endl;
	KURL::List list;
	if (KURLDrag::decode(event, list))
	{
		QByteArray data, replyData;
		QStringList fileList;
		QCString replyType;
		QDataStream arg(data, IO_WriteOnly);

		// Juk doesn't handle KURL's yet, so we need to form a list
		// that contains the local paths.
		for (KURL::List::ConstIterator it = list.begin(); it != list.end(); ++it)
			fileList += (*it).path();

		arg << fileList << false;

		// Use call instead of send to make sure the files are added
		// before we try to play.
		if (!kapp->dcopClient()->call(mAppId, "Collection", "openFile(QStringList)", data,
			      replyType, replyData, true))
		{
			kdDebug(90200) << "Couldn't send drop to juk" << endl;
		}

		// Apparently we should auto-play?
		QByteArray strData;
		QDataStream strArg(strData, IO_WriteOnly);
		strArg << *fileList.begin();

		if (!kapp->dcopClient()->send(mAppId, "Player", "play(QString)", strData))
			kdDebug(90200) << "Couldn't send play command to juk" << endl;
	}
}

// ====================================================================================

void JuKInterface::sliderStartDrag()
{
	mJuKTimer->stop();
}

void JuKInterface::sliderStopDrag()
{
	mJuKTimer->start(mTimerValue);
}

void JuKInterface::jumpToTime( int sec )
{
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << sec;
	// Used in JuK shipping with KDE < 3.3
	//kapp->dcopClient()->send(mAppId, "Player", "setTime(int)", data);
	kapp->dcopClient()->send(mAppId, "Player", "seek(int)", data);
}

void JuKInterface::playpause()
{
	if (!findRunningJuK())
		startPlayer("juk");
	QByteArray data;
	kapp->dcopClient()->send(mAppId, "Player", "playPause()", data);
}

void JuKInterface::stop()
{
	QByteArray data;
	kapp->dcopClient()->send(mAppId, "Player", "stop()", data);
}

void JuKInterface::next()
{
	QByteArray data;
	kapp->dcopClient()->send(mAppId, "Player", "forward()", data);
}

void JuKInterface::prev()
{
	QByteArray data;
	kapp->dcopClient()->send(mAppId, "Player", "back()", data);
}

void JuKInterface::volumeUp()
{
   QByteArray data;
   kapp->dcopClient()->send(mAppId, "Player", "volumeUp()", data);
}

void JuKInterface::volumeDown()
{
   QByteArray data;
   kapp->dcopClient()->send(mAppId, "Player", "volumeDown()", data);
}

const QString JuKInterface::getTrackTitle() const
{
	QString title;
	QByteArray data, replyData;
	QCString replyType;

	if (kapp->dcopClient()->call(mAppId, "Player", "playingString()",data,
		replyType, replyData))
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "QString")
		{
			reply >> title;
			return title;
		}
	}
	return QString("");
}

// FIXME: what if we have a dcop app named, let's say, 'jukfrontend'?
bool JuKInterface::findRunningJuK()
{
	QCStringList allApps = kapp->dcopClient()->registeredApplications();
	QValueList<QCString>::const_iterator iterator;

	for (iterator = allApps.constBegin(); iterator != allApps.constEnd(); ++iterator)
	{
		if ((*iterator).contains("juk",false))
		{
			mAppId = *iterator;
			return true;
		}
	}
	return false;
}

int JuKInterface::playingStatus()
{
	QByteArray data, replyData;
	QCString replyType;

	if (kapp->dcopClient()->call(mAppId, "Player", "status()", data, replyType,
		replyData))
	{
		int status = 0;
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "int")
			reply >> status;

		if (status == 2)
			return Playing;
		else if (status == 1)
			return Paused;
	}

	return Stopped;
}
