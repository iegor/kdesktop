/***************************************************************************
                          Interface to access Amarok
                             -------------------
    begin                : Tue Dec  2 23:54:53 CET 2003
    copyright            : (c) 2003 by Thomas Capricelli
    adapted from juk* (C) 2001-2002 by Stefan Gehn (metz {AT} gehn {DOT} net)
    email                : orzel@freehackers.org
 ***************************************************************************/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "amarokInterface.h"
#include "amarokInterface.moc"

#include <kdebug.h>
#include <qstringlist.h>
#include <qstrlist.h>
#include <kurldrag.h>

#define TIMER_FAST  250

AmarokInterface::AmarokInterface() : PlayerInterface()
{
   mTimerValue = TIMER_FAST;
   mAmarokTimer = new QTimer ( this, "mAmaroKTimer" );

   connect(mAmarokTimer, SIGNAL(timeout()), SLOT(updateSlider()) );
   kapp->dcopClient()->setNotifications ( true );

   connect(kapp->dcopClient(), SIGNAL(applicationRegistered(const QCString&)),
      SLOT(appRegistered(const QCString&)) );

   connect(kapp->dcopClient(), SIGNAL(applicationRemoved(const QCString&)),
      SLOT(appRemoved(const QCString&)));

   QTimer::singleShot(0, this, SLOT(myInit()));
}

AmarokInterface::~AmarokInterface()
{
   kapp->dcopClient()->setNotifications(false);
   delete mAmarokTimer;
}

void AmarokInterface::myInit()
{
   // Start the timer if amarok is already running
   // Needed if user adds applet while running amarok
   if ( findRunningAmarok() )
   {
      emit playerStarted();
      mAmarokTimer->start(mTimerValue);
   }
   else
   {
      emit playerStopped();
      emit newSliderPosition(0,0);
   }
}

void AmarokInterface::appRegistered ( const QCString &appId )
{
   if(appId.contains("amarok",false) )
   {
      mAppId = appId;
      emit playerStarted();
      mAmarokTimer->start(mTimerValue);
   }
}

void AmarokInterface::appRemoved ( const QCString &appId )
{
   if ( appId.contains("amarok",false) )
   {
       // is there still another amarok alive?
      if ( findRunningAmarok() )
         return;
      mAmarokTimer->stop();
      emit playerStopped();
      emit newSliderPosition(0,0);
   }
}

void AmarokInterface::updateSlider ( )
{
   // length/time in msecs, -1 means "no playobject in amarok"
   int len, time;
   QByteArray data, replyData;
   QCString replyType;

   if (!kapp->dcopClient()->call(mAppId, "player", "trackTotalTime()",data, replyType, replyData))
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

   if (!kapp->dcopClient()->call(mAppId, "player", "trackCurrentTime()",data, replyType, replyData))
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

   if ( (time < 0) || (len < 0)) // Amarok isn't playing and thus returns -1
   {
      len = 0;
      time = 0;
   }
   emit newSliderPosition(len,time);
   emit playingStatusChanged(playingStatus());
}

// Drag-n-Drop stuff =================================================================

void AmarokInterface::dragEnterEvent(QDragEnterEvent* event)
{
//   kdDebug(90200) << "AmarokInterface::dragEnterEvent()" << endl;
   event->accept( KURLDrag::canDecode(event) );
}

void AmarokInterface::dropEvent(QDropEvent* event)
{
   kdDebug(90200) << "AmarokInterface::dropEvent()" << endl;
   KURL::List list;
   if (KURLDrag::decode(event, list))
   {
      QByteArray data;
      QDataStream arg(data, IO_WriteOnly);
      arg << list;
      if (!kapp->dcopClient()->send(mAppId, "player", "addMediaList(KURL::List)",data))
         kdDebug(90200) << "Couldn't send drop to amarok" << endl;
   }
}

// ====================================================================================

void AmarokInterface::sliderStartDrag()
{
   mAmarokTimer->stop();
}

void AmarokInterface::sliderStopDrag()
{
   mAmarokTimer->start(mTimerValue);
}

void AmarokInterface::jumpToTime( int sec )
{
   QByteArray data;
   QDataStream arg(data, IO_WriteOnly);
   arg << sec;
   kapp->dcopClient()->send(mAppId, "player", "seek(int)", data);
}

void AmarokInterface::playpause()
{
   if (!findRunningAmarok())
       startPlayer("amarok");
   QByteArray data;
   kapp->dcopClient()->send(mAppId, "player", "playPause()", data);
}

void AmarokInterface::stop()
{
   QByteArray data;
   kapp->dcopClient()->send(mAppId, "player", "stop()", data);
}

void AmarokInterface::next()
{
   QByteArray data;
   kapp->dcopClient()->send(mAppId, "player", "next()", data);
}

void AmarokInterface::prev()
{
   QByteArray data;
   kapp->dcopClient()->send(mAppId, "player", "prev()", data);
}

void AmarokInterface::volumeUp()
{
   QByteArray data;
   kapp->dcopClient()->send(mAppId, "player", "volumeUp()", data);
}

void AmarokInterface::volumeDown()
{
   QByteArray data;
   kapp->dcopClient()->send(mAppId, "player", "volumeDown()", data);
}

const QString AmarokInterface::getTrackTitle() const
{
   QString title;
   QByteArray data, replyData;
   QCString replyType;
   if (!kapp->dcopClient()->call(mAppId, "player", "nowPlaying()",data, replyType, replyData))
   {
      //kdDebug(90200) << "mediacontrol: DCOP communication Error" << endl;
      return QString("");
   }
   else
   {
      QDataStream reply(replyData, IO_ReadOnly);
      if (replyType == "QString")
      {
         reply >> title;
         return title;
      }
      else
      {
         //kdDebug(90200) << "mediacontrol: unexpected type of DCOP-reply" << endl;
         return QString("");
      }
   }
}

bool AmarokInterface::findRunningAmarok()
{
   QCStringList allApps = kapp->dcopClient()->registeredApplications();
   QValueList<QCString>::const_iterator iterator;
   QByteArray data, replyData;
   QCString replyType;

   for (iterator = allApps.constBegin(); iterator != allApps.constEnd(); ++iterator)
   {
      if ((*iterator).contains("amarok",false))
      {
         if (kapp->dcopClient()->call((*iterator), "player", "interfaces()", data, replyType, replyData) )
    {
            if ( replyType == "QCStringList" )
            {
               QDataStream reply(replyData, IO_ReadOnly);
               QCStringList list;
               reply >> list;

               if ( list.contains("AmarokPlayerInterface") )
               {
                  kdDebug(90200) << "mediacontrol: amarok found" << endl;
                  mAppId = *iterator;
                  return true;
               }
            }
         }
      }
   }
   kdDebug(90200) << "mediacontrol: amarok not found" << endl;
   return false;
}


int AmarokInterface::playingStatus()
{
   QByteArray data, replyData;
   QCString replyType;

   if (kapp->dcopClient()->call(mAppId, "player", "status()", data, replyType,
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
