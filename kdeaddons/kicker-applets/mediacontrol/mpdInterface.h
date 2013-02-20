/***************************************************************************
                   this is the class to access mpd from
                             -------------------
    begin                : Tue Apr 19 18:31:00 BST 2005
    copyright            : (C) 2005 by William Robinson
    email                : airbaggins@yahoo.co.uk
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

#ifndef  MPDINTERFACE_H
#define  MPDINTERFACE_H

#include "playerInterface.h"
#include <qtimer.h>
#include <klocale.h>
#include <qsocket.h>
#include <qmutex.h>

class MpdInterface
:   public PlayerInterface
{
    Q_OBJECT
    public:
        MpdInterface();
        ~MpdInterface();

    public slots:
        virtual void updateSlider();
        virtual void sliderStartDrag();
        virtual void sliderStopDrag();
        virtual void jumpToTime(int sec);
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

        void changeVolume(int delta);

        void connectionError(int e);
        void connected();

        void startSliderClock();
        void stopSliderClock();

        void startReconnectClock();
        void stopReconnectClock();


    protected:
        virtual void timerEvent(QTimerEvent* te);

    private:
        mutable QSocket sock;
        mutable QMutex sock_mutex;

        mutable QMutex messagebox_mutex;

        QString hostname;
        int port;

        static const int SLIDER_TIMER_INTERVAL = 500; // ms
        int slider_timer;

        static const int RECONNECT_TIMER_INTERVAL = 5000; // ms
        int reconnect_timer;

        /** starts connecting and returns, if not connected already. */
        void reconnect() const;

        /** this locks the sock sock_mutex. remember to unlock it. */
        bool dispatch(const char* cmd) const;

        /** fetches everything from the sock to the OK and unlocks the mutex.
            returns true if OK, false on anything else. */
        bool fetchOk() const;

        /** fetches a line and returns true, or false if OK or ACK (end of
            message). Will unlock the sock_mutex on the end of message. */
        bool fetchLine(QString& res) const;
};

#endif   // MPDINTERFACE_H
