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

#ifndef AMAROKINTERFACE_H
#define AMAROKINTERFACE_H

#include "playerInterface.h"

#include <kapplication.h>
#include <dcopclient.h>

#include <qtimer.h>

class AmarokInterface : public PlayerInterface
{
   Q_OBJECT
   public:
      AmarokInterface();
      ~AmarokInterface();

   public slots:
      virtual void updateSlider(); // gets called on timer-timeout
      virtual void sliderStartDrag();
      virtual void sliderStopDrag();
      virtual void jumpToTime( int msec );
      virtual void playpause();
      virtual void stop();
      virtual void next();
      virtual void prev();
      virtual void volumeUp();
      virtual void volumeDown();
      virtual void dragEnterEvent(QDragEnterEvent* event);
      virtual void dropEvent(QDropEvent* event);
      virtual const QString getTrackTitle() const;
      virtual int playingStatus();

   private slots:
      void myInit();
      void appRegistered ( const QCString &appId );
      void appRemoved ( const QCString &appId );

   private:
      QTimer *mAmarokTimer;
      int mTimerValue;
      QCString mAppId;

      /**
       * Tries to find a DCOP registered instance of AmaroK
       * Stores the name of the first found instance in appId
       * @returns true is instance is found, false otherwise
       */
      bool findRunningAmarok();
};
#endif
