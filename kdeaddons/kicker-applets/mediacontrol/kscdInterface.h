/***************************************************************************
                           Interface to access KsCD
                             -------------------
    begin                : Sat Dec 04 12:48:00 CET 2004
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

#ifndef KSCDINTERFACE_H
#define KSCDINTERFACE_H

#include "playerInterface.h"

#include <kapplication.h>
#include <dcopclient.h>

#include <qtimer.h>

class KsCDInterface : public PlayerInterface
{
	Q_OBJECT
	public:
		KsCDInterface();
		~KsCDInterface();

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

	private:
		QTimer *mKsCDTimer;
		QCString mAppId;

		/**
		 * Tries to find a DCOP registered instance of KsCD
		 * Stores the name of the first found instance in appId
		 * @returns true is instance is found, false otherwise
		 */
		bool findRunningKsCD();
};
#endif
