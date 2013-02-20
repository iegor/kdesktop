/***************************************************************************
              this is the abstract class to access a player from
                             -------------------
    begin                : Mon Jan 15 21:09:00 MEZ 2001
    copyright            : (C) 2001 by Stefan Gehn
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

#ifndef PLAYERINTERFACE_H
#define PLAYERINTERFACE_H

#include <qobject.h>
#include <qdragobject.h>

class PlayerInterface : public QObject
{
	Q_OBJECT
	public:
		PlayerInterface();
		virtual ~PlayerInterface();

		enum PlayingStatus { Stopped=0, Playing, Paused };

	public slots:
		virtual void updateSlider()=0; // gets called on timer-timeout
		virtual void sliderStartDrag()=0;
		virtual void sliderStopDrag()=0;
		virtual void jumpToTime( int msec )=0;
		virtual void playpause()=0;
		virtual void stop()=0;
		virtual void next()=0;
		virtual void prev()=0;
		virtual void volumeUp()=0;
		virtual void volumeDown()=0;
		virtual void dragEnterEvent(QDragEnterEvent* event)=0;
		virtual void dropEvent(QDropEvent* event)=0;
		virtual const QString getTrackTitle() const=0;
		virtual int playingStatus()=0;

		void startPlayer(const QString &desktopname);

	signals:
		void newSliderPosition(int, int);
		void playingStatusChanged(int);
		void playerStarted();
		void playerStopped();
};
#endif
