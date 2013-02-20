/***************************************************************************
                           Interface to access KsCD
                             -------------------
    begin                : Sat Dec 04 13:36:00 CET 2004
    copyright            : (C) 2004 by Michal Startek
    adapted from JuK interface which is (C) 2001-2002 by Stefan Gehn
    email                : michal.startek@op.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kscdInterface.h"
#include "kscdInterface.moc"

#include <kapplication.h>
#include <kdebug.h>
#include <qstringlist.h>
#include <qstrlist.h>
#include <kurldrag.h>
#include <klocale.h>

#define TIMER_FAST  250

KsCDInterface::KsCDInterface() : PlayerInterface()
{
	mKsCDTimer = new QTimer(this, "mKsCDTimer");

	connect(mKsCDTimer, SIGNAL(timeout()), SLOT(updateSlider()) );
	kapp->dcopClient()->setNotifications ( true );

	connect(kapp->dcopClient(), SIGNAL(applicationRegistered(const QCString&)),
		SLOT(appRegistered(const QCString&)) );

	connect(kapp->dcopClient(), SIGNAL(applicationRemoved(const QCString&)),
		SLOT(appRemoved(const QCString&)));

	QTimer::singleShot(0, this, SLOT(myInit()));
}

KsCDInterface::~KsCDInterface()
{
	kapp->dcopClient()->setNotifications(false);
	delete mKsCDTimer;
}

void KsCDInterface::myInit()
{
	// Start the timer if KsCD is already running
	// Needed if user adds applet while running KsCD
	if (findRunningKsCD())
	{
		emit playerStarted();
		mKsCDTimer->start(TIMER_FAST);
	}
	else
	{
		emit playerStopped();
		emit newSliderPosition(0,0);
	}
}

void KsCDInterface::appRegistered(const QCString &appId)
{
	if((appId) == "kscd")
	{
		mAppId = appId;
		emit playerStarted();
		mKsCDTimer->start(TIMER_FAST);
	}
}

void KsCDInterface::appRemoved(const QCString &appId)
{
	if ((appId) == "kscd")
	{
		// is there still another KsCD alive?
		// Okay, KsCD does not allow multiple instances
		// of it to run at the same time, but
		// this can change.
		if (findRunningKsCD())
			return;
		mKsCDTimer->stop();
		emit playerStopped();
		emit newSliderPosition(0,0);
	}
}

void KsCDInterface::updateSlider()
{
	// length/time in secs, -1 means "no playobject in kscd"
	int len = -1;
	int time = -1;
	QByteArray data, replyData;
	QCString replyType;

	if (kapp->dcopClient()->call(mAppId, "CDPlayer", "currentTrackLength()", data,
		replyType, replyData))
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "int")
			reply >> len;
	}

	data = 0;
	replyData = 0;
	replyType = 0;

	if (kapp->dcopClient()->call(mAppId, "CDPlayer", "currentPosition()", data,
		replyType, replyData))
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "int")
			reply >> time;
	}

	if ( (time < 0) || (len < 0)) // KsCD isn't playing and thus returns -1
	{
		len = 0;
		time = 0;
	}
	emit newSliderPosition(len,time);
	emit playingStatusChanged(playingStatus());
}

// Drag-n-Drop stuff =================================================================
// As far as I know there is currently no way to drag an AudioCD track to KsCD,
// or even no application to drag AudioCD track from (not the KIO-wrapped track
// path (audiocd:/...) like from Konqueror)

void KsCDInterface::dragEnterEvent(QDragEnterEvent* event)
{
	event->ignore();
}

void KsCDInterface::dropEvent(QDropEvent* event)
{
	event->ignore();
}

// ====================================================================================

void KsCDInterface::sliderStartDrag()
{
	mKsCDTimer->stop();
}

void KsCDInterface::sliderStopDrag()
{
	mKsCDTimer->start(TIMER_FAST);
}

void KsCDInterface::jumpToTime(int sec)
{
	QByteArray data;
	QDataStream arg(data, IO_WriteOnly);
	arg << sec;
	kapp->dcopClient()->send(mAppId, "CDPlayer", "jumpTo(int)", data);
}

void KsCDInterface::playpause()
{
	if (!findRunningKsCD())
		startPlayer("kscd");
	QByteArray data;
	kapp->dcopClient()->send(mAppId, "CDPlayer", "play()", data);
}

void KsCDInterface::stop()
{
	QByteArray data;
	kapp->dcopClient()->send(mAppId, "CDPlayer", "stop()", data);
}

void KsCDInterface::next()
{
	QByteArray data;
	kapp->dcopClient()->send(mAppId, "CDPlayer", "next()", data);
}

void KsCDInterface::prev()
{
	QByteArray data;
	kapp->dcopClient()->send(mAppId, "CDPlayer", "previous()", data);
}

void KsCDInterface::volumeUp()
{
   QByteArray data;
   kapp->dcopClient()->send(mAppId, "CDPlayer", "volumeUp()", data);
}

void KsCDInterface::volumeDown()
{
   QByteArray data;
   kapp->dcopClient()->send(mAppId, "CDPlayer", "volumeDown()", data);
}

const QString KsCDInterface::getTrackTitle() const
{
	QString title, artist, album, result;
	QByteArray data, replyData;
	QCString replyType;

	// Get track title from KsCD...
	if (kapp->dcopClient()->call(mAppId, "CDPlayer", "currentTrackTitle()", data,
		replyType, replyData))
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "QString")
		{
			reply >> title;
		}
	}

	// Album...
	if (kapp->dcopClient()->call(mAppId, "CDPlayer", "currentAlbum()", data,
		replyType, replyData))
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "QString")
		{
			reply >> album;
		}
	}

	// Artist...
	if (kapp->dcopClient()->call(mAppId, "CDPlayer", "currentArtist()", data,
		replyType, replyData))
	{
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "QString")
		{
			reply >> artist;
		}
	}

	// And compose them into one string that will be displayed...
	if(album.isEmpty())
	{
		if(artist.isEmpty())
		{
			result = title;
		}
		else // artist is non-empty
		{
			if(title.isEmpty())
			{
				result = artist;
			}
			else
			{
				result = i18n("artist - trackname", "%1 - %2").arg(artist, title);
			}
		}
	}
	else // album is non-empty
	{
		if(artist.isEmpty())
		{
			if(title.isEmpty())
			{
				result = album;
			}
			else
			{
				result = i18n("(album) - trackname", "(%1) - %2").arg(artist, title);
			}
		}
		else // artist is non-empty
		{
			if(title.isEmpty())
			{
				result = i18n("artistname (albumname)", "%1 (%2)").arg(artist, album);
			}
			else
			{
				result = i18n("artistname (albumname) - trackname", "%1 (%2) - %3").arg(artist, album, title);
			}
		}
	}

	return result;
}

bool KsCDInterface::findRunningKsCD()
{
	QCStringList allApps = kapp->dcopClient()->registeredApplications();
	QValueList<QCString>::const_iterator iterator;

	for (iterator = allApps.constBegin(); iterator != allApps.constEnd(); ++iterator)
	{
		if ((*iterator) == "kscd")
		{
			mAppId = *iterator;
			return true;
		}
	}
	return false;
}

int KsCDInterface::playingStatus()
{
	QByteArray data, replyData;
	QCString replyType;

	if (kapp->dcopClient()->call(mAppId, "CDPlayer", "getStatus()", data, replyType,
		replyData))
	{
		int status = 0;
		QDataStream reply(replyData, IO_ReadOnly);
		if (replyType == "int")
			reply >> status;

		switch (status)
		{
			case 2:
				return Playing;
			case 4:
				return Paused;
			default:
				return Stopped;
		}
	}
	return Stopped;
}
