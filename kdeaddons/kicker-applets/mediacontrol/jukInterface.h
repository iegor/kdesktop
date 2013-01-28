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

#ifndef JUKINTERFACE_H
#define JUKINTERFACE_H

#include "playerInterface.h"

#include <kapplication.h>
#include <dcopclient.h>

#include <qtimer.h>

class QProcess;

class JuKInterface : public PlayerInterface
{
	Q_OBJECT
	public:
		JuKInterface();
		~JuKInterface();

	public slots:
		void updateSlider();
		void sliderStartDrag();
		void sliderStopDrag();
		void jumpToTime( int sec );
		void playpause();
		void stop();
		void next();
		void prev();
		void volumeUp();
		void volumeDown();
		void dragEnterEvent(QDragEnterEvent* event);
		void dropEvent(QDropEvent* event);
		const QString getTrackTitle() const;
		int playingStatus();

	private slots:
		void myInit();
		void appRegistered ( const QCString &appId );
		void appRemoved ( const QCString &appId );
		void jukIsReady();

	private:
		QTimer *mJuKTimer;
		QProcess *mProc;
		int mTimerValue;
		QCString mAppId;

		/**
		 * Tries to find a DCOP registered instance of juk
		 * Stores the name of the first found instance in appId
		 * @returns true is instance is found, false otherwise
		 */
		bool findRunningJuK();
};
#endif
